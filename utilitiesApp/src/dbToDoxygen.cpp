/// @file dbToDoxygen.cpp Read EPICS dbd and format for doxygen parsing
/// @author Freddie Akeroyd
///
/// based on dbToRecordtypeH.c from EPICS base
///
/// run on base.dbd to create doxygen parable code

#include <stdlib.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "dbDefs.h"
#include "epicsPrint.h"
#include "errMdef.h"
#include "dbStaticLib.h"
#include "dbStaticPvt.h"
#include "dbBase.h"
#include "gpHash.h"
#include "osiFileName.h"

DBBASE *pdbbase = NULL;

int main(int argc,char **argv)
{
	int		i;
	char	*outFilename;
	char	*pext;
	FILE	*outFile;
	dbMenu	*pdbMenu;
	dbRecordType	*pdbRecordType;
	dbFldDes	*pdbFldDes;
	char	*plastSlash;
	int		strip;
	char	*path = NULL;
	char	*sub = NULL;
	int		pathLength = 0;
	int		subLength = 0;
	char	**pstr;
	char	*psep;
	int		*len;
	long	status;
	static char *pathSep = OSI_PATH_LIST_SEPARATOR;
	static char *subSep = ",";

	/*Look for options*/
	if(argc<2) {
		fprintf(stderr,"usage: dbToDoxygen -Idir -Idir file.dbd [outfile]\n");
		exit(0);
	}
	while((strncmp(argv[1],"-I",2)==0)||(strncmp(argv[1],"-S",2)==0)) {
		if(strncmp(argv[1],"-I",2)==0) {
			pstr = &path;
			psep = pathSep;
			len = &pathLength;
		} else {
			pstr = &sub;
			psep = subSep;
			len = &subLength;
		}
		if(strlen(argv[1])==2) {
			dbCatString(pstr,len,argv[2],psep);
			strip = 2;
		} else {
			dbCatString(pstr,len,argv[1]+2,psep);
			strip = 1;
		}
		argc -= strip;
		for(i=1; i<argc; i++) argv[i] = argv[i + strip];
	}
	if(argc<2 || (strncmp(argv[1],"-",1)==0)) {
		fprintf(stderr,"usage: dbToRecordtypeH -Idir -Idir file.dbd [outfile]\n");
		exit(0);
	}
	if(argc==2){
		/*remove path so that outFile is created where program is executed*/
		plastSlash = strrchr(argv[1],'/');
		if(!plastSlash)  plastSlash = strrchr(argv[1],'\\');
		plastSlash = (plastSlash ? plastSlash+1 : argv[1]);
		outFilename = (char*)dbCalloc(1,strlen(plastSlash)+1);
		strcpy(outFilename,plastSlash);
		pext = strstr(outFilename,".dbd");
		if(!pext) {
			fprintf(stderr,"Input file MUST have .dbd extension\n");
			exit(-1);
		}
		strcpy(pext,".h");
	}else {
		outFilename = (char*)dbCalloc(1,strlen(argv[2])+1);
		strcpy(outFilename,argv[2]);
	}
	pdbbase = dbAllocBase();
	pdbbase->ignoreMissingMenus = TRUE;
	pdbbase->loadCdefs = TRUE;
	status = dbReadDatabase(&pdbbase,argv[1],path,sub);
	if(status)  {
		errlogFlush();
		fprintf(stderr, "dbToMenuH: Input errors, no output generated\n");
		exit(1);
	}
	outFile = fopen(outFilename,"wt");
	if(!outFile) {
		epicsPrintf("Error creating output file \"%s\"\n", outFilename);
		exit(1);
	}
	fprintf(outFile,"/// @file\n/// EPICS DB record definitions\n\n");

	pdbMenu = (dbMenu *)ellFirst(&pdbbase->menuList);
	while(pdbMenu) {
		fprintf(outFile,"/// %s EPICS DB menu\n",pdbMenu->name);
		fprintf(outFile,"enum %s\n{\n", pdbMenu->name);
		for(i=0; i<pdbMenu->nChoice; i++) {
			fprintf(outFile,"\t%s",pdbMenu->papChoiceName[i]);
			if(i < (pdbMenu->nChoice - 1)) fprintf(outFile,",");
			fprintf(outFile,"\t///< %s", pdbMenu->papChoiceValue[i]);
			fprintf(outFile,"\n");
		}
		fprintf(outFile,"};\n\n");
		pdbMenu = (dbMenu *)ellNext(&pdbMenu->node);
	}
	pdbRecordType = (dbRecordType *)ellFirst(&pdbbase->recordTypeList);
	while(pdbRecordType) {
		fprintf(outFile,"/// %s EPICS DB record\n",pdbRecordType->name);
		fprintf(outFile,"struct %s\n{\n",pdbRecordType->name);
		for(i=0; i<pdbRecordType->no_fields; i++) {
			char	name[256];
			int		j;

			pdbFldDes = pdbRecordType->papFldDes[i];
			for(j=0; j< (int)strlen(pdbFldDes->name); j++)
				name[j] = pdbFldDes->name[j];
			name[strlen(pdbFldDes->name)] = 0;
			switch(pdbFldDes->field_type) {
			case DBF_STRING :
				fprintf(outFile, "\tchar\t\t%s[%d];\t///< %s\n",
					name, pdbFldDes->size, pdbFldDes->prompt);
				break;
			case DBF_CHAR :
				fprintf(outFile, "\tepicsInt8\t%s;\t///< %s\n",
					name, pdbFldDes->prompt);
				break;
			case DBF_UCHAR :
				fprintf(outFile, "\tepicsUInt8\t%s;\t///< %s\n",
					name, pdbFldDes->prompt);
				break;
			case DBF_SHORT :
				fprintf(outFile, "\tepicsInt16\t%s;\t///< %s\n",
					name, pdbFldDes->prompt);
				break;
			case DBF_USHORT :
				fprintf(outFile, "\tepicsUInt16\t%s;\t///< %s\n",
					name, pdbFldDes->prompt);
				break;
			case DBF_LONG :
				fprintf(outFile, "\tepicsInt32\t%s;\t///< %s\n",
					name, pdbFldDes->prompt);
				break;
			case DBF_ULONG :
				fprintf(outFile, "\tepicsUInt32\t%s;\t///< %s\n",
					name, pdbFldDes->prompt);
				break;
			case DBF_FLOAT :
				fprintf(outFile, "\tepicsFloat32\t%s;\t///< %s\n",
					name, pdbFldDes->prompt);
				break;
			case DBF_DOUBLE :
				fprintf(outFile, "\tepicsFloat64\t%s;\t///< %s\n",
					name, pdbFldDes->prompt);
				break;
			case DBF_MENU :
				pdbMenu = (dbMenu*)pdbFldDes->ftPvt;
				if (pdbMenu != NULL)
				{
				    fprintf(outFile, "\t%s\t%s;\t///< %s\n",
					      pdbMenu->name, name, pdbFldDes->prompt);
				}
				else
				{
					fprintf(outFile, "\tepicsEnum16\t%s;\t///< %s\n",
						name, pdbFldDes->prompt);
				}
				break;
			case DBF_ENUM :
			case DBF_DEVICE :
				fprintf(outFile, "\tepicsEnum16\t%s;\t///< %s\n",
					name, pdbFldDes->prompt);
				break;
			case DBF_INLINK :
			case DBF_OUTLINK :
			case DBF_FWDLINK :
				fprintf(outFile, "\tDBLINK\t\t%s;\t///< %s\n",
					name, pdbFldDes->prompt);
				break;
			case DBF_NOACCESS:
				break;
			default:
				fprintf(outFile,"ILLEGAL FIELD TYPE\n");
			}
		}
		fprintf(outFile,"};\n");
		pdbRecordType = (dbRecordType *)ellNext(&pdbRecordType->node);
		if(pdbRecordType) fprintf(outFile,"\n");
	}
	fclose(outFile);
	free((void *)outFilename);
	return(0);
}
