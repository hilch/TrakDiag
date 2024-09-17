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
#include <regex>

#include "TD_WebServices.h"

std::regex rxUri(R"((TrakWebApi)\x2f(\w+)\x2f([\w\.]*)\x3f?(\w*)=?(\w*))" );

/* Web services for Trak components */
void TD_WebServices(struct TD_WebServices* inst)
{
	if( inst->Enable ){
		switch( inst->step ){
			case STARTUP:
				inst->Valid = false;
				inst->Busy = false;
				inst->Error = false;
				/* get pointer to assembly's PV */
				inst->ErrorID = PV_xgetadr( (char*) inst->AssemblyName, (UDINT*) &inst->pAssembly, (UDINT*) &inst->tempudint );
				if( inst->ErrorID == 0 ){					
					inst->Busy = true;
					InitalizeInstance( inst );
					inst->step = GET_SEGMENT_INFO;
				}
				else {
					inst->Error = true;
					inst->step = INTERNAL_ERROR_PVXGETADR;
				}
			break;


			case GET_SEGMENT_INFO:
			if( inst->fbSegmentsInfo.Done ){
				inst->SegInfo.numberOfSegments = inst->fbSegmentsInfo.Count;
				inst->fbCopySegmentData.AdvancedParameters.DataSize = inst->SegInfo.numberOfSegments * sizeof(McAcpTrakSegmentData);
				inst->fbCopySegmentData.Execute = true;
				inst->fbSegmentsInfo.Execute = false; /* reset fb */
				TD_SegmentsInfo( &inst->fbSegmentsInfo );
				inst->webData.fbHttpService.enable = true; /* switch on web services */
				inst->stepTimeout.PT = 10000;
				inst->stepTimeout.IN = false;
				TON( &inst->stepTimeout );
				for( int n = 0; n < inst->SegInfo.numberOfSegments; ++n ){
					inst->fbSegReadInfo[n].Segment = &inst->SegInfo.segment[n];
					inst->fbSegReadInfo[n].Enable = true;
				}
				inst->step = W_HTTP_REQUESTS;
			}
			else if( inst->fbSegmentsInfo.Error ){
				inst->Error = true;
				inst->ErrorID = inst->fbSegmentsInfo.ErrorID;
				inst->step = INTERNAL_ERROR_SEGMENTINFO;				
				inst->fbSegmentsInfo.Execute = false; /* reset fb */
				TD_SegmentsInfo( &inst->fbSegmentsInfo );
			}
			else { /* busy */
				TD_SegmentsInfo( &inst->fbSegmentsInfo );
			}
			break;


			case W_HTTP_REQUESTS: /* waiting for http requests */
			if( (inst->refreshTimer += inst->fbRtInfo.cycle_time) > 100000 ){
				inst->refreshTimer = 0;
				CollectShuttleInfo(inst);	
			}

			httpService( &inst->webData.fbHttpService); 
			if( inst->webData.fbHttpService.status == 0 ){
				if( inst->webData.fbHttpService.method == httpMETHOD_GET && inst->webData.fbHttpService.phase == httpPHASE_RECEIVED ){

					/* prepare responses */
					std::memset( &inst->webData.responseHeader, 0, sizeof(inst->webData.responseHeader) );
					std::strcpy( (char*) inst->webData.responseHeader.protocol, "HTTP/1.1" );
					std::strcpy( (char*) inst->webData.responseHeader.status, "200 OK" );
					inst->webData.fbHttpService.pResponseData = (UDINT) &inst->webData.responseData;

					/* break the uri down into its components */
					std::cmatch m;
					std::memset( inst->webData.uri_part, 0, sizeof(inst->webData.uri_part) );
					bool result = std::regex_search( (char*) inst->webData.uri, m, rxUri );
					if( result ){
						size_t n = 0;
						for( auto sm : m ){
							std::copy( sm.first, sm.second, inst->webData.uri_part[n] );
							if( ++n >= sizeof(inst->webData.uri_part)/sizeof(inst->webData.uri_part[0]) ) break;
						}
					}

					/* check requests and send appripriate responses */
					for( unsigned n = 0; n < sizeof(responses)/sizeof(responses[0]); ++n ){
						if( responses[n].request == 0 ){
							SendResponse_404(inst); /* 404 - not found */
							break;
						}
						else {
							if( std::strcmp( responses[n].request, inst->webData.uri_part[3]  ) == 0 ){
								responses[n].send_response( inst );
								break;
							}
						}
					}
				}
			}
			else if( inst->webData.fbHttpService.status != 65535 && inst->webData.fbHttpService.status != httpERR_SYSTEM){
				inst->ErrorID = inst->webData.fbHttpService.status;
				inst->Error = true;
				inst->webData.lastError = inst->webData.fbHttpService.status;
				inst->webData.fbHttpService.enable = false;
				inst->step = INTERNAL_ERROR_HTTP;
			}
			else { /* busy */
				if( inst->webData.fbHttpService.method == httpMETHOD_GET && inst->webData.fbHttpService.phase == httpPHASE_RECEIVED ){
					inst->stepTimeout.IN = true;
				}
				if( inst->stepTimeout.Q ) { /* busy */
					AbortResponse( inst );
				}
			}			
			break;

			
			case HTTP_SERV_RESPONSE: /* send http response */
			TON( &inst->stepTimeout );
			httpService( &inst->webData.fbHttpService); 
			if( inst->webData.fbHttpService.status == 0 ){
				if( inst->webData.fbHttpService.phase == httpPHASE_WAITING ){
					inst->webData.fbHttpService.send = false;
					httpService( &inst->webData.fbHttpService);
					inst->stepTimeout.IN = false; /* reset fb */
					TON( &inst->stepTimeout );
					inst->step = W_HTTP_REQUESTS;	
				}
			}
			else if( inst->webData.fbHttpService.status != 65535 && inst->webData.fbHttpService.status != httpERR_SYSTEM ){
				inst->webData.lastError = inst->webData.fbHttpService.status;
				inst->webData.fbHttpService.enable = false;
				inst->step = INTERNAL_ERROR_HTTP;
			}
			else if( inst->stepTimeout.Q ) { /* busy */
				inst->webData.fbHttpService.send = false;
				inst->webData.fbHttpService.abort = true;
				inst->stepTimeout.PT = 30000;
				inst->stepTimeout.IN = false;
				TON( &inst->stepTimeout );
				inst->stepTimeout.IN = true;
				inst->step = ABORT_RESPONSE;	
			}			
			break;


			case ABORT_RESPONSE: /* abort Http- Response */
			TON( &inst->stepTimeout );
			httpService( &inst->webData.fbHttpService);
			if( inst->webData.fbHttpService.status == 0 || inst->webData.fbHttpService.status == httpERR_SYSTEM ||
					( inst->webData.fbHttpService.status == 65535 && inst->stepTimeout.Q )	){
					inst->stepTimeout.PT = 30000;
					inst->stepTimeout.IN = false;
					TON( &inst->stepTimeout );
					inst->webData.fbHttpService.send = false;
					inst->webData.fbHttpService.abort = false;
					httpService( &inst->webData.fbHttpService);
					inst->step = W_HTTP_REQUESTS;	
			}
			else if( inst->webData.fbHttpService.status != 65535 ){
				inst->Error = true;
				inst->webData.lastError = inst->webData.fbHttpService.status;
				inst->webData.fbHttpService.enable = false;
				inst->webData.fbHttpService.send = false;
				inst->webData.fbHttpService.abort = false;
				inst->step = INTERNAL_ERROR_HTTP;
			}
			break;


			case INFO_SVG_FILE:
			if( inst->fbFileInfo.status == 0 ){  /* done */
				inst->fbFileInfo.enable = false; /* reset fb */
				FileInfo( &inst->fbFileInfo );
				inst->fbDatObjInfo.enable = true; /* open svg file */
				DatObjInfo( &inst->fbDatObjInfo );
				inst->step = CHECK_DATA_OBJ;
			}
			else if( inst->fbFileInfo.status != 65535 ){
				inst->ErrorID = inst->fbFileInfo.status;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileInfo.enable = false; /* reset fb */
				FileInfo( &inst->fbFileInfo );
				inst->step = INTERNAL_ERROR_FILEINFO_SVG;	
			}
			else {
				FileInfo( &inst->fbFileInfo );
			}
			break;


			case CHECK_DATA_OBJ: /* check if data object already exist */		
			if( inst->fbDatObjInfo.status == 0 ){  /* data object exists - delete this */
				inst->fbDatObjDelete.ident = inst->fbDatObjInfo.ident;
				inst->fbDatObjInfo.enable = false; /* reset fb */
				DatObjInfo( &inst->fbDatObjInfo );
				inst->fbDatObjDelete.enable = true; /* start fb */
				DatObjDelete( &inst->fbDatObjDelete );
				inst->step = DELETE_DATA_OBJ;
			}
			else if( inst->fbDatObjInfo.status == doERR_MODULNOTFOUND ){ /* module not found */
				inst->fbDatObjInfo.enable = false; /* reset fb */
				DatObjInfo( &inst->fbDatObjInfo );
				inst->fbDatObjCreate.len = inst->infoSvgFile.size + 1024;
				inst->fbDatObjCreate.enable = true; /* start fb */
				DatObjCreate( &inst->fbDatObjCreate );
				inst->step = CREATE_DATA_OBJ;
			}
			else if( inst->fbDatObjInfo.status != 65535 ){ /* other errors */
				inst->ErrorID = inst->fbDatObjInfo.status;
				inst->Error = true;
				inst->fbDatObjInfo.enable = false; /* reset fb */
				DatObjInfo( &inst->fbDatObjInfo );
				inst->step = INTERNAL_ERROR_DATOBJINFO;
			}
			else { /* busy */
				DatObjInfo( &inst->fbDatObjInfo );
			}
			break;


			case DELETE_DATA_OBJ: /* delete an already existing data object */
			if( inst->fbDatObjDelete.status == 0 ){  /* delete successful */
				inst->fbDatObjDelete.enable = false; /* reset fb */
				DatObjDelete( &inst->fbDatObjDelete );
				inst->fbDatObjCreate.len = inst->infoSvgFile.size + 1024;
				inst->fbDatObjCreate.enable = true; /* start fb */
				DatObjCreate( &inst->fbDatObjCreate );
				inst->step = CREATE_DATA_OBJ;
			}
			else if( inst->fbDatObjDelete.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbDatObjDelete.status;
				inst->Error = true;
				inst->fbDatObjDelete.enable = false; /* reset fb */
				DatObjDelete( &inst->fbDatObjDelete );
				inst->step = INTERNAL_ERROR_DATOBJDELETE;
			}
			else { /* busy */
				DatObjDelete( &inst->fbDatObjDelete );
			}
			break;


			case CREATE_DATA_OBJ: /* create the data object for dynamic memory */
			if( inst->fbDatObjCreate.status == 0 ){ /* create successful */
				inst->pDataObject = inst->fbDatObjCreate.pDatObjMem; /* pointer to memory */
				inst->fbDatObjCreate.enable = false; /* reset fb */
				DatObjCreate( &inst->fbDatObjCreate );	
				inst->fbDatObjDelete.ident = inst->fbDatObjCreate.ident;
				inst->fbFileOpen.enable = true;			
				FileOpen( &inst->fbFileOpen );
				inst->step = OPEN_SVG_FILE;
			}
			else if( inst->fbDatObjCreate.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbDatObjCreate.status;
				inst->Error = true;
				inst->fbDatObjCreate.enable = false; /* reset fb */
				DatObjCreate( &inst->fbDatObjCreate );
				inst->step = INTERNAL_ERROR_DATOBJCREATE;
			}
			else { /* busy */
				DatObjCreate( &inst->fbDatObjCreate );
			}
			break;


			case OPEN_SVG_FILE:
			if( inst->fbFileOpen.status == 0 ){ /* done */
				inst->fbFileRead.offset = 0;
				inst->fbFileRead.pDest = inst->pDataObject;			
				inst->fbFileRead.len = inst->infoSvgFile.size;				
				inst->fbFileRead.ident = inst->fbFileOpen.ident;
				inst->fbFileRead.enable = true; /* start fb */
				FileReadEx( &inst->fbFileRead );
				inst->fbFileOpen.enable = false; /* reset fb */
				FileOpen( &inst->fbFileOpen );
				inst->step = READ_SVG_DATA;
			}
			else if( inst->fbFileOpen.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbFileOpen.status;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileOpen.enable = false; /* reset fb */
				FileOpen( &inst->fbFileOpen );
				inst->step = INTERNAL_ERROR_FILEOPEN_SVG;	
			}
			else { /* busy */
				FileOpen( &inst->fbFileOpen );
			}
			break;


			case READ_SVG_DATA:
			if( inst->fbFileRead.status == 0 ){ /* done */
				inst->fbFileClose.ident = inst->fbFileRead.ident;
				inst->fbFileRead.enable = false; /* reset fb */
				FileReadEx( &inst->fbFileRead );
				inst->fbFileClose.enable = true;  /* start fb */
				FileClose( &inst->fbFileClose );
				inst->step = CLOSE_SVG_FILE;
			}
			else if( inst->fbFileRead.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbFileRead.status;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileRead.enable = false; /* reset fb */
				FileReadEx( &inst->fbFileRead );
				inst->step = INTERNAL_ERROR_FILEREAD_SVG;
			}
			else { /* busy */
				FileReadEx( &inst->fbFileRead );
			}
			break;


			case CLOSE_SVG_FILE: /* close the svg file */
			if( inst->fbFileClose.status == 0 ){ /* successful */
				inst->fbFileClose.enable = false; /* reset fb */
				FileClose( &inst->fbFileClose );
				SendResponse_Svgdata( inst );
				inst->step = SEND_SVGDATA;
			}
			else if( inst->fbFileClose.status != 65535 ) {  /* error */
				inst->ErrorID = inst->fbFileClose.status ;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileClose.enable = false; /* reset fb */
				FileClose( &inst->fbFileClose );
				inst->step = INTERNAL_ERROR_FILECLOSE_SVG;	
			}
			else { /* busy */
				FileClose( &inst->fbFileClose );
			}
			break;


			case SEND_SVGDATA: /* send http response */
			httpService( &inst->webData.fbHttpService); 
			if( inst->webData.fbHttpService.status == 0 ){
				if( inst->webData.fbHttpService.phase == httpPHASE_WAITING ){
					inst->webData.fbHttpService.send = false;
					inst->fbDatObjDelete.enable = true; /* start fb */
					DatObjDelete( &inst->fbDatObjDelete );
					inst->step = FREE_UP_MEMORY;	
				}
			}
			else if( inst->webData.fbHttpService.status != 65535 ){
				inst->webData.lastError = inst->webData.fbHttpService.status;
				inst->webData.fbHttpService.enable = false;
				inst->step = INTERNAL_ERROR_HTTP;
			}			
			break;


			case FREE_UP_MEMORY: /* delete data object */
			if( inst->fbDatObjDelete.status == 0 ){  /* delete successful */
				inst->fbDatObjDelete.enable = false; /* reset fb */
				DatObjDelete( &inst->fbDatObjDelete );
				inst->step = W_HTTP_REQUESTS;;
			}
			else if( inst->fbDatObjDelete.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbDatObjDelete.status;
				inst->Error = true;
				inst->fbDatObjDelete.enable = false; /* reset fb */
				DatObjDelete( &inst->fbDatObjDelete );
				inst->step = INTERNAL_ERROR_FREEMEMORY;
			}
			else { /* busy */
				DatObjDelete( &inst->fbDatObjDelete );
			}
			break;


			case SHUTTLE_INFO:  /* send info about a single shuttle */
			inst->webData.fbGetParamUrl.enable = false; /* reset fb */
			inst->webData.fbGetParamUrl.pSrc = (UDINT) &inst->webData.uri;
			inst->webData.fbGetParamUrl.pParam = (UDINT) "index";
			inst->webData.fbGetParamUrl.pValue = (UDINT) inst->webData.urlParamBuffer;
			inst->webData.fbGetParamUrl.valueSize = sizeof(inst->webData.urlParamBuffer);
			httpGetParamUrl( &inst->webData.fbGetParamUrl );
			inst->webData.fbGetParamUrl.enable = true;
			do {
				httpGetParamUrl( &inst->webData.fbGetParamUrl );
			} while( inst->webData.fbGetParamUrl.status == ERR_FUB_BUSY );
		
			if( inst->webData.fbGetParamUrl.status == 0 ){
				inst->webData.shuttle.Index = std::atoi( (char*) inst->webData.urlParamBuffer ); /* save shuttle index */
				bool found = false;
		
				McAcpTrakShuttleData * pShuttleData = (McAcpTrakShuttleData*) inst->DataAddress;
				size_t numShuttleData = inst->DataSize / ( sizeof(McAcpTrakShuttleData) + inst->UserDataSize );
				for( unsigned n = 0; n < numShuttleData; ++n ){
					if( inst->webData.shuttle.Index == pShuttleData[n].Index ){
						std::memcpy( &inst->webData.shuttle, &pShuttleData[n], sizeof(inst->webData.shuttle) );
						found = true;
						break;
					}
				}
				if( found ){ /* shuttle in array found */
					if( !inst->webData.shuttle.Active || (inst->webData.shuttle.PLCopenState != mcAXIS_ERRORSTOP) ){ /* active shuttle or shuttle not in error stop */
						size_t sizeOfResponse = std::sprintf( inst->webData.responseData, 
						    "{ \"result\" : \"ok\", \"index\":%lu, \"userID\":\"%s\", \"active\": %s, \"controlled\": %s, \"virtual\": %s,"
								" \"segmentName\":\"%s\", \"segmentPosition\":%f,"
								" \"sectorName\":\"%s\", \"sectorPosition\":%f,"
								" \"PLCopen\":\"%s\"", 
						    UDINT(inst->webData.shuttle.Index), inst->webData.shuttle.UserID,
							inst->webData.shuttle.Active ? "true" : "false",
							inst->webData.shuttle.Controlled ? "true" : "false",  
							inst->webData.shuttle.Virtual ? "true" : "false", 
						    inst->webData.shuttle.SegmentName, inst->webData.shuttle.SegmentPosition,
							inst->webData.shuttle.SectorName, inst->webData.shuttle.SectorPosition, 
							GetAxisPlcOpenStateString(inst->webData.shuttle.PLCopenState) );
						const char* tail = "}";
						sizeOfResponse += std::strlen(tail);
						if( sizeOfResponse < sizeof(inst->webData.responseData) ){
							std::strcat( inst->webData.responseData, tail );
						}
						SendResponse( inst, "application/json; charset=iso-8859-1" );
					} else {  /* active shuttle in error stop */
						inst->fbShuttleErrorTexts.Axis = 0;
						inst->fbShuttleErrorTexts.Index = inst->webData.shuttle.Index;
						inst->fbShuttleErrorTexts.Execute = false; /* reset fb */
						TD_ShuttleErrorTexts( &inst->fbShuttleErrorTexts );
						inst->fbShuttleErrorTexts.Execute = true;
						TD_ShuttleErrorTexts( &inst->fbShuttleErrorTexts );
						inst->step = SHUTTLE_ERROR_TEXTS;
					}
				}
				else { /* shuttle with given index not found */
					std::sprintf( inst->webData.responseData, "{ \"result\" : \"shuttle with given index not found\", \"index\":%lu }", UDINT(inst->webData.shuttle.Index) );
					SendResponse( inst, "application/json; charset=iso-8859-1" );
				}
			}
			else if( inst->webData.fbGetParamUrl.status == httpERR_NOT_FOUND ){  /* GET parameter invalid */
				std::sprintf( inst->webData.responseData, "{ \"result\" : \"GET parameter invalid\" }" );
				SendResponse( inst, "application/json; charset=iso-8859-1" );	
			}
			else { /* internal error */
				std::sprintf( inst->webData.responseData, "{ \"result\" : \"%d\" }", inst->webData.fbGetParamUrl.status  );
				SendResponse( inst, "application/json; charset=iso-8859-1" );
			}
			inst->webData.fbGetParamUrl.enable = false; /* reset fb */
			httpGetParamUrl( &inst->webData.fbGetParamUrl );
			break;


			case SHUTTLE_ERROR_TEXTS:
			if( inst->fbShuttleErrorTexts.Done ){ /* done */
				size_t sizeOfResponse = std::sprintf( inst->webData.responseData, 
				    "{ \"result\" : \"ok\", \"index\":%lu, \"userID\":\"%s\", \"active\": %s, \"controlled\": %s, \"virtual\": %s,"
						" \"segmentName\":\"%s\", \"segmentPosition\":%f,"
						" \"sectorName\":\"%s\", \"sectorPosition\":%f,"
						" \"PLCopen\":\"%s\", \"errorTexts\":[ ", 
				    UDINT(inst->webData.shuttle.Index), inst->webData.shuttle.UserID,
					inst->webData.shuttle.Active ? "true" : "false", 
					inst->webData.shuttle.Controlled ? "true" : "false", 
					inst->webData.shuttle.Virtual ? "true" : "false", 
				    inst->webData.shuttle.SegmentName, inst->webData.shuttle.SegmentPosition, 
				    inst->webData.shuttle.SectorName, inst->webData.shuttle.SectorPosition, 
					GetAxisPlcOpenStateString(inst->webData.shuttle.PLCopenState) );
				for( unsigned n = 0; n < inst->fbShuttleErrorTexts.NumberOfRecords; ++n ){
					char record[300];
					double timestamp = double(inst->fbShuttleErrorTexts.Records.Record[n].TimeStamp.Seconds) * 1000 + 
										double(inst->fbShuttleErrorTexts.Records.Record[n].TimeStamp.Nanoseconds) / 1000;
					sizeOfResponse += std::snprintf( record, sizeof(record), "%s{\"t\":%lf, \"txt\":\"%s\"}", n==0 ? "" : ",", timestamp, inst->fbShuttleErrorTexts.Records.Record[n].Text );
					if( sizeOfResponse < sizeof(inst->webData.responseData) ){
						std::strcat( inst->webData.responseData, record );
					}
				}
				const char* tail = " ] }";
				sizeOfResponse += std::strlen(tail);
				if( sizeOfResponse < sizeof(inst->webData.responseData) ){
					std::strcat( inst->webData.responseData, tail );
				}
				inst->fbShuttleErrorTexts.Execute = false; /* reset fb */
				TD_ShuttleErrorTexts( &inst->fbShuttleErrorTexts );
				SendResponse( inst, "application/json; charset=iso-8859-1" );
			}
			else if( inst->fbShuttleErrorTexts.Error ) { /* error */
				std::sprintf( inst->webData.responseData, "{ \"result\" : \"%ld\" }", inst->fbShuttleErrorTexts.ErrorID  );
				inst->fbShuttleErrorTexts.Execute = false; /* reset fb */
				TD_ShuttleErrorTexts( &inst->fbShuttleErrorTexts );
				SendResponse( inst, "application/json; charset=iso-8859-1" );
			}
			else { /* busy */
				TD_ShuttleErrorTexts( &inst->fbShuttleErrorTexts );
			}
			break;


			case INTERNAL_ERROR:
			case INTERNAL_ERROR_SEGMENTINFO:
			case INTERNAL_ERROR_PVXGETADR:
			case INTERNAL_ERROR_HTTP:
			case INTERNAL_ERROR_FILEINFO_SVG:
			case INTERNAL_ERROR_FILEOPEN_SVG:
			case INTERNAL_ERROR_FILEREAD_SVG:
			case INTERNAL_ERROR_FILECLOSE_SVG:
			case INTERNAL_ERROR_DATOBJINFO:
			case INTERNAL_ERROR_DATOBJDELETE:
			case INTERNAL_ERROR_DATOBJCREATE:
			case INTERNAL_ERROR_FREEMEMORY:
			break;

		}

		/* collect some information */
		CollectAssemblyInformation( inst );

		/* copy cyclic segment data */
		MC_BR_AsmCopySegmentData_AcpTrak( &inst->fbCopySegmentData );
		if( inst->fbCopySegmentData.Done ){
			inst->fbCopySegmentData.Execute = false,
			MC_BR_AsmCopySegmentData_AcpTrak( &inst->fbCopySegmentData );
			inst->fbCopySegmentData.Execute = true;
		}

	}
	else {
		inst->step = STARTUP;
		inst->webData.fbHttpService.enable = false; /* reset fb */
		httpService( &inst->webData.fbHttpService); 
		inst->ErrorID = 0;
		inst->Error = false;
		inst->Valid = false;
		inst->Busy = false;
	}
}

