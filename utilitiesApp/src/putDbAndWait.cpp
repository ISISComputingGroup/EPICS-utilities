#include <dbCommon.h>
#include <dbAccess.h>
#include <dbChannel.h>
#include <dbNotify.h>
#include <recGbl.h>
#include <errlog.h>
#include <alarm.h>

#include <epicsExport.h>

#include "utilities.h"

// based on example of dbtpn in EPICS base dbNotify.c

struct notifyCallbackInfo {
    epicsEventId callbackDone;
    DBADDR* addr;
    const void* value;
};

static int putDbAndWaitPutCallback(processNotify *ppn, notifyPutType type)
{
    notifyCallbackInfo *pInfo = (notifyCallbackInfo *) ppn->usrPvt;
    int status = 0;

    if (ppn->status == notifyCanceled)
        return 0;
    ppn->status = notifyOK;
    switch (type) {
    case putDisabledType:
        ppn->status = notifyError;
        return 0;
    case putFieldType:
        status = dbChannelPutField(ppn->chan, pInfo->addr->dbr_field_type, pInfo->value, 1);
        break;
    case putType:
        status = dbChannelPut(ppn->chan, pInfo->addr->dbr_field_type, pInfo->value, 1);
        break;
    }
    if (status)
        ppn->status = notifyError;
    return 1;
}

static void putDbAndWaitDoneCallback(processNotify *ppn)
{
    notifyCallbackInfo *pInfo = (notifyCallbackInfo *) ppn->usrPvt;
    if (ppn->status == notifyCanceled)
        return;
    epicsEventSignal(pInfo->callbackDone);
}

/// Set a PV to a value and wait \a timeout seconds for a completion callback
/// Currently do not use this to set PVs that call back into this driver as it will just block itself and timeout
/// releasing/reacquiring the asyn port lock around epicsEventWaitWithTimeout() did not resolve this
/// return 0 on success and -1 on error
int putDbAndWait(const std::string& pvName, const void *value, double timeout) {
    DBADDR addr;
    const char* pvname = pvName.c_str();

    if (dbNameToAddr(pvname, &addr)) {
        errlogSevPrintf(errlogMajor, "Invalid PV (dbaddr) for putDbAndWait: %s", pvname);
        return -1;
    }
    struct dbChannel *chan = dbChannelCreate(pvname);
    if (!chan) {
        errlogSevPrintf(errlogMajor, "Invalid PV (chan) for putDbAndWait: %s", pvname);
        return -1;
    }

    notifyCallbackInfo notifyInfo;
    memset(&notifyInfo, 0, sizeof(notifyCallbackInfo));
    notifyInfo.callbackDone = epicsEventCreate(epicsEventEmpty);
    notifyInfo.addr = &addr;
    notifyInfo.value = value;

    processNotify procNotify;
    memset(&procNotify, 0, sizeof(processNotify));
    procNotify.requestType = putProcessRequest;
    procNotify.chan = chan;
    procNotify.putCallback = putDbAndWaitPutCallback;
    procNotify.doneCallback = putDbAndWaitDoneCallback;
    procNotify.usrPvt = &notifyInfo;

    dbProcessNotify(&procNotify);
    epicsEventStatus event_status = epicsEventWaitWithTimeout(notifyInfo.callbackDone, timeout);
    int was_processed = procNotify.wasProcessed; // was record processed as result of write e.g. due to it being a PP field
    notifyStatus notify_status = procNotify.status;
    dbNotifyCancel(&procNotify); // this sets status to notifyCancelled hence need copy taken above
    epicsEventDestroy(notifyInfo.callbackDone);
    dbChannelDelete(procNotify.chan);

    if (event_status == epicsEventOK && notify_status == notifyOK) {
        //errlogSevPrintf(errlogInfo,"putDbAndWait: successfully wrote to PV \"%s\" was_processed=%d", pvname, was_processed);
        return 0;
    }

    dbCommon *precord = addr.precord;
    recGblSetSevr(precord, WRITE_ALARM, INVALID_ALARM);

    if (event_status != epicsEventOK) {
        errlogSevPrintf(errlogMajor, "Timeout after %f seconds for putDbAndWait when attempting to set %s", timeout, pvname);
    }
    if (notify_status != notifyOK) {
        errlogSevPrintf(errlogMajor, "Notify error %d for putDbAndWait when attempting to set %s", (int)notify_status, pvname);
    }
    return -1;
}
