/*

https://github.com/hilch/TrakDiag

Helper Library for ACOPOStrak diagnosis

MIT License

Copyright (c) 2022 https://github.com/hilch

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#include <bur/plctypes.h>
#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "TrakDiag.h"
#ifdef __cplusplus
	};
#endif

#include <cstring>
#include <cstdio>
#include <algorithm>

#include "utils.h"

/* initialize  */
void InitalizeInstance(struct TD_WebServices* inst){
	/* */
	inst->fbRtInfo.enable = true;
	RTInfo( &inst->fbRtInfo );

	/* */
	MoVerStruc_typ moverstruc;
	STRING module[] = {"McAcpTrak"};
	MO_ver( module, 0, &moverstruc); 
	std::strncpy( (char*) inst->acpTrakVersion, (char*) moverstruc.version, sizeof(inst->acpTrakVersion)-1 );

	/* */
	inst->fbSegmentsInfo.Assembly = inst->pAssembly;
	inst->fbSegmentsInfo.MaxCount = sizeof(inst->SegInfo.segment)/sizeof(inst->SegInfo.segment[0]);
	inst->fbSegmentsInfo.Segments = &inst->SegInfo.segment[0];
	inst->fbSegmentsInfo.SegmentsInfo = &inst->SegInfo.segmentInfo[0];
	inst->fbSegmentsInfo.Execute = false; /* reset fb */
	TD_SegmentsInfo( &inst->fbSegmentsInfo );
	inst->fbSegmentsInfo.Execute = true;

	/* */
	inst->fbCopySegmentData.Assembly = inst->pAssembly;
	inst->fbCopySegmentData.Command =  mcACPTRAK_SEG_DATA_ALL;
	inst->fbCopySegmentData.AdvancedParameters.Trigger = mcACPTRAK_SEG_DATA_TRIGGER_IMM;
	inst->fbCopySegmentData.AdvancedParameters.SegmentID = 0;
	inst->fbCopySegmentData.AdvancedParameters.DataAddress = (UDINT) &inst->SegInfo.segmentData;
	inst->fbCopySegmentData.AdvancedParameters.DataSize = 0;

	/* */
	inst->fbAsmReadInfo.Enable = false;
	inst->fbAsmReadInfo.Assembly = inst->pAssembly;
	MC_BR_AsmReadInfo_AcpTrak( &inst->fbAsmReadInfo ); /* reset fb */
	inst->fbAsmReadInfo.Enable = true;

	/* */
	std::strcpy( inst->webData.serviceName, "TrakWebApi/" );
	std::strcat( inst->webData.serviceName, inst->AssemblyName );
	inst->webData.fbHttpService.enable = false; /* reset fb */
	inst->webData.fbHttpService.option = httpOPTION_HTTP_11 + httpOPTION_SERVICE_TYPE_ROOT;
	inst->webData.fbHttpService.pServiceName = (UDINT) &inst->webData.serviceName;
	inst->webData.fbHttpService.pUri = (UDINT) &inst->webData.uri,
	inst->webData.fbHttpService.uriSize = sizeof(inst->webData.uri),
	inst->webData.fbHttpService.pRequestHeader = (UDINT) &inst->webData.requestHeader;
	inst->webData.fbHttpService.pRequestData = (UDINT) &inst->webData.requestData;
	inst->webData.fbHttpService.requestDataSize = sizeof(inst->webData.requestData),
	inst->webData.fbHttpService.pResponseHeader = (UDINT) &inst->webData.responseHeader;
	inst->webData.fbHttpService.pResponseData = (UDINT) &inst->webData.responseData;
	inst->webData.fbHttpService.responseDataLen = sizeof(inst->webData.responseData),
	inst->webData.fbHttpService.pStatistics = (UDINT) &inst->webData.statistics;

	/* */
	inst->fbShuttleErrorTexts.Assembly = inst->pAssembly;

	/* create data object for dynamic memory allocation */
	inst->fbDatObjInfo.enable = false; /* reset fb */
	if( std::strlen(inst->DataObjectName) == 0 ){
		char name[256]{0};
		std::sprintf( name, "tdws%s", inst->AssemblyName );
		std::sprintf( inst->DataObjectName, "$$%8lx", Djb2( (USINT*) name ) );	
	}
	inst->fbDatObjInfo.pName = (UDINT) inst->DataObjectName;
	DatObjInfo( &inst->fbDatObjInfo );
	inst->fbDatObjDelete.enable = false; /* reset fb */
	DatObjDelete( &inst->fbDatObjDelete );
	inst->fbDatObjCreate.enable = false; /* reset fb */
	inst->fbDatObjCreate.grp = 0;
	inst->fbDatObjCreate.pName = (UDINT) inst->DataObjectName;
	inst->fbDatObjCreate.MemType = doTEMP;
	inst->fbDatObjCreate.Option = doNO_CS;
	inst->fbDatObjCreate.pCpyData = 0;
	DatObjCreate( &inst->fbDatObjCreate );


	/* */
	std::strcpy( inst->svgFileName, inst->AssemblyName );
	std::strcat( inst->svgFileName, ".svg" );
	inst->fbFileInfo.pDevice = (UDINT)"SvgData";
	inst->fbFileInfo.pName = (UDINT) &inst->svgFileName;
	inst->fbFileInfo.pInfo = (UDINT) &inst->infoSvgFile;
	inst->fbFileInfo.enable = false; /* reset fb */
	FileInfo( &inst->fbFileInfo );
	inst->fbFileOpen.pDevice = inst->fbFileInfo.pDevice;
	inst->fbFileOpen.pFile = inst->fbFileInfo.pName;
	inst->fbFileOpen.mode = fiREAD_ONLY;
	inst->fbFileOpen.enable = false; /* reset fb */
	FileOpen( &inst->fbFileOpen );
	FileReadEx( &inst->fbFileRead );
}



/* calculates djb2 hash http://www.cse.yorku.ca/~oz/hash.html */
unsigned long Djb2(unsigned char *str) {
	unsigned long hash = 5381;
	int c;

	while ( (c = *str++) )
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */
	return hash;
}

/* gets length and ID of segment or -1 if not found */
bool GetSegmentData(struct TD_WebServices* inst, char* segmentName, UINT* ID, LREAL* length ){
	*ID = -1; /* not found */
	*length = -1.0; /* invalid */
	for( int i = 0; i < inst->SegInfo.numberOfSegments; ++i ){
		if( std::strcmp( (char*) inst->SegInfo.segmentInfo[i].Name, segmentName ) == 0 ){
			*ID = inst->SegInfo.segmentInfo[i].ID;
			*length = inst->SegInfo.segmentInfo[i].Length;
			return true;
		}
	} 
	return false; /* not found */
}


/* collect shuttle data */
void CollectShuttleInfo(struct TD_WebServices* inst){
	USINT * pShuttleData = (USINT*) inst->DataAddress;
	size_t numShuttleData = inst->DataSize / ( sizeof(McAcpTrakShuttleData) + inst->UserDataSize );
	std::memset( &inst->ShuttleInfo, 0, sizeof(inst->ShuttleInfo) );

	unsigned count = 0;
	unsigned countActive = 0;
	for( unsigned n = 0; n < numShuttleData; ++n ){
		McAcpTrakShuttleData *sd = (McAcpTrakShuttleData*) pShuttleData;
		TD_ServicesShuttleType shuttle{0};
	
		LREAL segmentLength;
		if( sd->Index && GetSegmentData( inst, sd->SegmentName, &shuttle.segmentID, &segmentLength ) ){
			/* a shuttle can be located on up to four segments. we just use the first segment listed */
			bool found = false;
			for( unsigned m = 0; m < count; ++m ){
				if( inst->ShuttleInfo.shuttle[m].index == sd->Index ){
					found = true;
					break;
				}
			}
			if( !found ){ /* shuttle data was not entered yet */
				shuttle.index = sd->Index;	
				if( sd->Active ){
					shuttle.flags |= 1;
					++countActive;
				}
				if( sd->Virtual ){
					shuttle.flags |= 2;
				}
				shuttle.plcOpenState = sd->PLCopenState;
				shuttle.segmentPosition = sd->SegmentPosition / segmentLength * 100;
				inst->ShuttleInfo.shuttle[count++] = shuttle;
			}
		}
		pShuttleData += ( sizeof(McAcpTrakShuttleData) + inst->UserDataSize );
	}
	inst->ShuttleInfo.numberOfEntries = count;
	inst->ShuttleInfo.numberOfActiveShuttles = countActive;

}

/* collect assembly information */
void CollectAssemblyInformation(struct TD_WebServices* inst){
	MC_BR_AsmReadInfo_AcpTrak( &inst->fbAsmReadInfo );
	if( inst->fbAsmReadInfo.Valid ){
		std::memcpy( &inst->asmInfo, &inst->fbAsmReadInfo.AssemblyInfo, sizeof(inst->asmInfo) );
	}
	else {
		std::memset( &inst->asmInfo, 0, sizeof(inst->asmInfo ));	
	}
}


/* returns a string describing PLCopen state for an axis */
const char* GetAxisPlcOpenStateString( McAxisPLCopenStateEnum state ){
	switch( state ){
		case mcAXIS_DISABLED: return "disabled"; break;
		case mcAXIS_STANDSTILL: return "standstill"; break;
		case mcAXIS_HOMING: return "homing"; break;
		case mcAXIS_STOPPING: return "stopping"; break;
		case mcAXIS_DISCRETE_MOTION: return "discrete motion"; break;
		case mcAXIS_CONTINUOUS_MOTION: return "continuous motion"; break;
		case mcAXIS_SYNCHRONIZED_MOTION: return "synchronized motion"; break;
		case mcAXIS_ERRORSTOP: return "errorstop"; break;
		case mcAXIS_STARTUP: return "startup"; break;
		case mcAXIS_INVALID_CONFIGURATION: return "invalid configuration"; break;
		default: return "unknonw"; break;
	}
}