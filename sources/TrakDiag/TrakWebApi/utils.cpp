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
	inst->fbGetSegment.Assembly = inst->pAssembly;
	inst->fbGetSegment.AdvancedParameters.SelectionMode = mcACPTRAK_GET_SEG_ALL;
	inst->fbGetSegment.Next = false;
	inst->fbGetSegment.Enable = false; /* reset fb */
	MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	

	/* */
	inst->fbAsmReadInfo.Enable = false;
	inst->fbAsmReadInfo.Assembly = inst->pAssembly;
	MC_BR_AsmReadInfo_AcpTrak( &inst->fbAsmReadInfo ); /* reset fb */
	inst->fbAsmReadInfo.Enable = true;


	/* */
	inst->fbSegGetInfo.Execute = false; /* reset fb */
	MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );

	/* */
	std::strcpy( inst->webData.serviceName, "TrakWebApi" );
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
	inst->fbGetSegment.Enable = true; /* start fb */
	MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	


	/* */
	inst->fbDatObjInfo.enable = false; /* reset fb */
	if( std::strlen(inst->DataObjectName) == 0 ){
		std::strcpy( inst->DataObjectName, "$$tdwmem" );
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



/* calculates djb2 hash */
UDINT Djb2(USINT *str, size_t length ) {
    UDINT hash = 5381;

    while (length--) {
        hash = ((hash << 5) + hash) + *str++; /* hash * 33 + c */
    }
    return hash;
}

/* gets length and ID of segment or -1 if not found */
bool GetSegmentData(struct TD_WebServices* inst, char* segmentName, UINT* ID, LREAL* length ){
	*ID = -1; /* not found */
	*length = -1.0; /* invalid */
	for( int i = 0; i < inst->SegInfo.numberOfSegments; ++i ){
		if( std::strcmp( (char*) inst->SegInfo.segmentInfo[i].Name, segmentName ) == 0 ){
			*ID = i;
			*length = inst->SegInfo.segmentInfo[i].Length;
			return true;
		}
	} 
	return false; /* not found */
}


/* retrieve shuttle data */
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

/* collect segment information */
void CollectSegmentInformation(struct TD_WebServices* inst){
	for( int n = 0; n < inst->SegInfo.numberOfSegments; ++n ){
		MC_BR_SegReadInfo_AcpTrak( &inst->fbSegReadInfo[n] );
	}
}