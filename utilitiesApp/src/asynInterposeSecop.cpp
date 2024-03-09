#include <cstddef>
#include <cstring>
#include <cstdlib>
#include <cstdio>
#include <vector>
#include <string>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <iostream>
#include <list>
#include <cantProceed.h>
#include <epicsAssert.h>
#include <epicsStdio.h>
#include <epicsString.h>
#include <osiSock.h>
#include <iocsh.h>

#include <epicsThread.h>
#include "asynDriver.h"
#include "asynOctet.h"
#include "asynInterposeSecop.h"

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <epicsExport.h>

static const char *driver="secopInterpose";

class Json2XML : public json::json_sax_t
{
private:
    std::stringstream output_;
    std::list<std::string> nodes_;
    std::string key_;
    bool in_array_;

public:

    Json2XML(const std::string& input) : in_array_(false)
    {
        bool result = json::sax_parse(input, this);
    }
    
    std::string getXML() const {
        return output_.str();
    }

    bool null()
    {
        output_ << "<null />";
        return true;
    }

    bool boolean(bool val)
    {
        output_ << "<bool>" << (val ? "true" : "false") << "</bool>";
        return true;
    }

    bool number_integer(number_integer_t val)
    {
        output_ << "<int>" << std::to_string(val) << "</int>";
        return true;
    }

    bool number_unsigned(number_unsigned_t val)
    {
        output_ << "<uint>" << std::to_string(val) << "</uint>";
        return true;
    }

    bool number_float(number_float_t val, const string_t& s)
    {
        output_ << "<float>" << s << "</float>";
        return true;
    }

    bool string(string_t& val)
    {
        output_ << "<string>" << std::quoted(val) << "</string>";
        return true;
    }

    bool start_object(std::size_t elements)
    {
        nodes_.push_back(key_);
        if (key_.size() > 0) {
            output_ << "<" << key_ << ">";
        }
        return true;
    }

    bool end_object()
    {
        if (nodes_.back().size() > 0) {
            output_ << "</" << nodes_.back() << ">";
        }
        nodes_.pop_back();
        return true;
    }

    bool start_array(std::size_t elements)
    {
        in_array_ = true;
        output_ << "<array>";
        return true;
    }

    bool end_array()
    {
        in_array_ = false;
        output_ << "</array>";
        return true;
    }

    bool key(string_t& val)
    {
        key_ = val;
        return true;
    }

    bool binary(binary_t& val)
    {
        output_ << "<binary />";
        return true;
    }

    bool parse_error(std::size_t position, const std::string& last_token, const nlohmann::json::exception& ex) override
    {
        output_ << std::string(ex.what());
        return false;
    }
};

int secopParse(const std::string& secop_input, std::string& action, std::string& specifier, std::string& xml_data)
{
    std::string json_data;
    size_t first_space = secop_input.find(" "), second_space = std::string::npos;
    if (first_space != std::string::npos) {
        action = secop_input.substr(0, first_space);
        second_space = secop_input.find(" ", first_space + 1);
    } else {
        action = secop_input;
    }
    if (second_space != std::string::npos) {
        specifier = secop_input.substr(first_space + 1, second_space - first_space - 1);
        json_data = secop_input.substr(second_space + 1);
    } else if (first_space != std::string::npos) {
        specifier = secop_input.substr(first_space + 1);
    } else {
        specifier = "";
    }
    if (json_data.size() > 0) {
        Json2XML jxml(json_data);
        xml_data = jxml.getXML();
    } else {
        xml_data = "";
    }
    return 0;
}

struct secopPvt {
    std::string    portName;
    int addr;
    asynInterface  secopInterface;
    asynOctet      *poctet;           /* low level driver */
    void           *octetPvt;
    asynUser       *pasynUser;  /* For connect/disconnect reporting */
};

/* asynOctet methods */
static asynStatus writeIt(void *ppvt,asynUser *pasynUser,
    const char *data,size_t numchars,size_t *nbytesTransfered);
static asynStatus readIt(void *ppvt,asynUser *pasynUser,
    char *data,size_t maxchars,size_t *nbytesTransfered,int *eomReason);
static asynStatus flushIt(void *ppvt,asynUser *pasynUser);
static asynStatus registerInterruptUser(void *ppvt,asynUser *pasynUser,
    interruptCallbackOctet callback, void *userPvt,void **registrarPvt);
static asynStatus cancelInterruptUser(void *drvPvt,asynUser *pasynUser,
     void *registrarPvt);
static asynStatus setInputEos(void *ppvt,asynUser *pasynUser,
    const char *eos,int eoslen);
static asynStatus getInputEos(void *ppvt,asynUser *pasynUser,
    char *eos,int eossize ,int *eoslen);
static asynStatus setOutputEos(void *ppvt,asynUser *pasynUser,
    const char *eos,int eoslen);
static asynStatus getOutputEos(void *ppvt,asynUser *pasynUser,
    char *eos,int eossize,int *eoslen);

static asynOctet octet = {
    writeIt,readIt,flushIt,
    registerInterruptUser, cancelInterruptUser,
    setInputEos,getInputEos,setOutputEos,getOutputEos
};

int asynInterposeSecopConfig(const char *portName, int addr, const char* options) 
{
    asynInterface *pasynInterface;
    asynStatus    status;
    asynUser      *pasynUser;

    if (portName == NULL) {
        printf("asynInterposeSecopConfig: no port specified\n");
        return -1;
    }

    secopPvt     *pPvt = new secopPvt;
    pPvt->portName = portName;
    pPvt->addr = addr;
    
    pPvt->secopInterface.interfaceType = asynOctetType;
    pPvt->secopInterface.pinterface = &octet;
    pPvt->secopInterface.drvPvt = pPvt;
    pasynUser = pasynManager->createAsynUser(0,0);
    pPvt->pasynUser = pasynUser;
    pPvt->pasynUser->userPvt = pPvt;
    status = pasynManager->connectDevice(pasynUser,portName,addr);
    if(status!=asynSuccess) {
        printf("%s connectDevice failed\n",portName);
        pasynManager->freeAsynUser(pasynUser);
        delete pPvt;
        return -1;
    }
    /* Find the asynOctet interface */
    pasynInterface = pasynManager->findInterface(pasynUser, asynOctetType, 1);
    if (!pasynInterface) {
        printf("%s findInterface error for asynOctetType %s\n",
               portName, pasynUser->errorMessage);
        pasynManager->freeAsynUser(pasynUser);
        delete pPvt;
        return -1;
    }
    
    status = pasynManager->interposeInterface(portName, addr,
       &pPvt->secopInterface, &pasynInterface);
    if(status!=asynSuccess) {
        printf("%s interposeInterface failed\n", portName);
        pasynManager->freeAsynUser(pasynUser);
        delete pPvt;
        return -1;
    }
    pPvt->poctet = (asynOctet *)pasynInterface->pinterface;
    pPvt->octetPvt = pasynInterface->drvPvt;
    return(0);
}

/* asynOctet methods */
static asynStatus writeIt(void *ppvt,asynUser *pasynUser,
    const char *data,size_t numchars,size_t *nbytesTransfered)
{
    secopPvt     *pPvt = (secopPvt *)ppvt;
    return pPvt->poctet->write(pPvt->octetPvt,
                pasynUser,data,numchars,nbytesTransfered);    
}

static asynStatus readIt(void *ppvt,asynUser *pasynUser,
    char *data,size_t maxchars,size_t *nbytesTransfered,int *eomReason)
{
    secopPvt *pPvt = (secopPvt *)ppvt;
    asynStatus status = pPvt->poctet->read(pPvt->octetPvt,
            pasynUser,data,maxchars,nbytesTransfered,eomReason);
    std::string secop_input(data, *nbytesTransfered);
    std::string action, specifier, xml_data;
    secopParse(secop_input, action, specifier, xml_data);
//    if (n != *nbytesTransfered)
//        asynPrintIO(pasynUser,ASYN_TRACEIO_FILTER,
//            data,n,"asynInterposeSecop:readIt %s secopped %llu characters\n",
//               pPvt->portName.c_str(), (epicsUInt64)(*nbytesTransfered - n));
//        *nbytesTransfered = n;
//        if (n < maxchars) {
//            data[n] = 0;
//        }
//    }
    return status;
}

static asynStatus flushIt(void *ppvt,asynUser *pasynUser)
{
    secopPvt *pPvt = (secopPvt *)ppvt;

    return pPvt->poctet->flush(pPvt->octetPvt,pasynUser);
}

static asynStatus registerInterruptUser(void *ppvt,asynUser *pasynUser,
    interruptCallbackOctet callback, void *userPvt,void **registrarPvt)
{
    secopPvt *pPvt = (secopPvt *)ppvt;

    return pPvt->poctet->registerInterruptUser(pPvt->octetPvt,
        pasynUser,callback,userPvt,registrarPvt);
}

static asynStatus cancelInterruptUser(void *ppvt,asynUser *pasynUser,
     void *registrarPvt)
{
    secopPvt *pPvt = (secopPvt *)ppvt;

    return pPvt->poctet->cancelInterruptUser(pPvt->octetPvt,
        pasynUser,registrarPvt);
}

static asynStatus setInputEos(void *ppvt,asynUser *pasynUser,
    const char *eos,int eoslen)
{
    secopPvt *pPvt = (secopPvt *)ppvt;

    return pPvt->poctet->setInputEos(pPvt->octetPvt,pasynUser,
           eos,eoslen);
}

static asynStatus getInputEos(void *ppvt,asynUser *pasynUser,
    char *eos,int eossize,int *eoslen)
{
    secopPvt *pPvt = (secopPvt *)ppvt;

        return pPvt->poctet->getInputEos(pPvt->octetPvt,pasynUser,
           eos,eossize,eoslen);
}

static asynStatus setOutputEos(void *ppvt,asynUser *pasynUser,
    const char *eos, int eoslen)
{
    secopPvt *pPvt = (secopPvt *)ppvt;
    
    return pPvt->poctet->setOutputEos(pPvt->octetPvt, pasynUser, eos, eoslen);
}

static asynStatus getOutputEos(void *ppvt,asynUser *pasynUser,
    char *eos,int eossize,int *eoslen)
{
    secopPvt *pPvt = (secopPvt *)ppvt;
    return pPvt->poctet->getOutputEos(pPvt->octetPvt, pasynUser, eos, eossize, eoslen);
}

/* register asynInterposeSecopConfig*/
static const iocshArg asynInterposeSecopConfigArg0 =
    { "portName", iocshArgString };
static const iocshArg asynInterposeSecopConfigArg1 =
    { "addr", iocshArgInt };
static const iocshArg asynInterposeSecopConfigArg2 =
    { "SecopInChars", iocshArgString };
static const iocshArg *asynInterposeSecopConfigArgs[] =
    {&asynInterposeSecopConfigArg0,&asynInterposeSecopConfigArg1,
     &asynInterposeSecopConfigArg2};
static const iocshFuncDef asynInterposeSecopConfigFuncDef =
    {"asynInterposeSecopConfig", 3, asynInterposeSecopConfigArgs};
static void asynInterposeSecopConfigCallFunc(const iocshArgBuf *args)
{
    asynInterposeSecopConfig(args[0].sval,args[1].ival,
          args[2].sval);
}

extern "C" {

static void asynInterposeSecopRegister(void)
{
    static int firstTime = 1;
    if (firstTime) {
        firstTime = 0;
        iocshRegister(&asynInterposeSecopConfigFuncDef, asynInterposeSecopConfigCallFunc);
    }
}
epicsExportRegistrar(asynInterposeSecopRegister);

}
