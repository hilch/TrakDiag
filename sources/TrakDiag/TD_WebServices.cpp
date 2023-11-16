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

#include <TD_WebServices.h>



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
					inst->n = 0;
					inst->step = GET_SEGMENT_LIST;
				}
				else {
					inst->Error = true;
					inst->step = INTERNAL_ERROR_PVXGETADR;
				}
			break;


			case GET_SEGMENT_LIST: /* get a list of all segments in this assembly */
			if( inst->fbGetSegment.Valid ){
				inst->SegInfo.numberOfSegments = inst->fbGetSegment.TotalCount; /* save the number of segments */
				inst->SegInfo.segment[inst->n] = inst->fbGetSegment.Segment; /* save segment handle */
				if( inst->fbGetSegment.RemainingCount == 0 ){
					inst->fbGetSegment.Next = false;
					inst->fbGetSegment.Enable = false; /* reset fb */
					MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	
					inst->n = 0;
					inst->fbSegGetInfo.Execute = true; /* start fb */
					inst->fbSegGetInfo.Segment = &inst->SegInfo.segment[inst->n];
					inst->fbSegReadInfo[inst->n].Segment = &inst->SegInfo.segment[inst->n];
					MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
					inst->step = GET_SEGMENT_INFO;
				}
				else {
					++inst->n;
					inst->fbGetSegment.Next = false;
					MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	
					inst->fbGetSegment.Next = true;
					MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	
				}
			}
			else if( inst->fbGetSegment.Error ){
				inst->ErrorID = inst->fbGetSegment.ErrorID;
				inst->Error = true;
				inst->fbGetSegment.Next = false;
				inst->fbGetSegment.Enable = false; /* reset fb */
				MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );
				inst->step = INTERNAL_ERROR_GETSEGMENT;	
			}
			else { /* busy */
				MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	
			}
			break;


			case GET_SEGMENT_INFO:
			if( inst->fbSegGetInfo.Done ){
				std::memcpy( &inst->SegInfo.segmentInfo[inst->n], &inst->fbSegGetInfo.SegmentInfo, sizeof(McAcpTrakSegGetInfoType) ); /* save segment infos */
				inst->fbSegGetInfo.Execute = false; /* reset fb */
				inst->fbSegReadInfo[inst->n].Enable = true; /* enable fb */
				MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
				++inst->n;
				if( inst->n < inst->SegInfo.numberOfSegments ){ /* there are still some segments left */
					inst->fbSegGetInfo.Execute = true; /* start fb */
					inst->fbSegGetInfo.Segment = &inst->SegInfo.segment[inst->n];
					inst->fbSegReadInfo[inst->n].Segment = &inst->SegInfo.segment[inst->n];
					MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
				}
				else { /* all segments processed. */
					inst->webData.fbHttpService.enable = true; /* switch on web services */
					inst->stepTimeout.PT = 10000;
					inst->stepTimeout.IN = false;
					TON( &inst->stepTimeout );
					inst->stepTimeout.IN = true;
					inst->step = W_HTTP_REQUESTS;
				}
			}
			else if( inst->fbSegGetInfo.Error ){
				inst->fbSegGetInfo.Execute = false; /* reset fb */
				MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );

			}
			else { /* busy */
				MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
			}
			break;


			case W_HTTP_REQUESTS: /* waiting for http requests */
			TON( &inst->stepTimeout );
			if( (inst->refreshTimer += inst->fbRtInfo.cycle_time) > 100000 ){
//				UDINT hash = Djb2( (USINT*) &inst->ShuttleInfo, sizeof(inst->ShuttleInfo) );
//				if( hash != inst->hashShuttleInfo ){
//					inst->hashShuttleInfo = hash;
					inst->refreshTimer = 0;
					CollectShuttleInfo(inst);	
//				}
			}

			httpService( &inst->webData.fbHttpService); 
			if( inst->webData.fbHttpService.status == 0 ){
				if( inst->webData.fbHttpService.method == httpMETHOD_GET && inst->webData.fbHttpService.phase == httpPHASE_RECEIVED ){
					/* prepare responses */
					std::memset( &inst->webData.responseHeader, 0, sizeof(inst->webData.responseHeader) );
					std::strcpy( (char*) inst->webData.responseHeader.protocol, "HTTP/1.1" );
					std::strcpy( (char*) inst->webData.responseHeader.status, "200 OK" );
					inst->webData.fbHttpService.pResponseData = (UDINT) &inst->webData.responseData;

					/* request for svgdata */
					if( std::strcmp( (char*) inst->webData.uri, "TrakWebApi/svgdata") == 0 ){
						inst->fbFileInfo.enable = true;
						FileInfo( &inst->fbFileInfo );
						inst->step = INFO_SVG_FILE; /* svgdata is stored in file generated by mappTrak */
					}
					else { /* all other requests */
						for( unsigned n = 0; n < sizeof(responses)/sizeof(responses[0]); ++n ){
							if( responses[n].request == 0 ){
								SendResponse_404(inst); /* 404 - not found */
								break;
							}
							else if( std::strcmp( (char*) inst->webData.uri, responses[n].request ) == 0 ){
								responses[n].send_response( inst );
								inst->step = HTTP_SERV_RESPONSE;
								break;
							}
						}
						inst->webData.fbHttpService.send = true;
						inst->stepTimeout.PT = 10000;
						inst->stepTimeout.IN = false;
						TON( &inst->stepTimeout );
						inst->stepTimeout.IN = true;
						inst->step = HTTP_SERV_RESPONSE;	
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
			else if( inst->stepTimeout.Q ) { /* busy */
				inst->webData.fbHttpService.send = false;
				inst->webData.fbHttpService.abort = true;
				inst->step = ABORT_RESPONSE;	
			}			
			break;

			
			case HTTP_SERV_RESPONSE: /* send http response */
			TON( &inst->stepTimeout );
			httpService( &inst->webData.fbHttpService); 
			if( inst->webData.fbHttpService.status == 0 ){
				if( inst->webData.fbHttpService.phase == httpPHASE_WAITING ){
					inst->webData.fbHttpService.send = false;
					httpService( &inst->webData.fbHttpService);
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
				inst->step = ABORT_RESPONSE;	
			}			
			break;


			case ABORT_RESPONSE: /* abort Http- Response */
			httpService( &inst->webData.fbHttpService);
			if( inst->webData.fbHttpService.status == 0 || inst->webData.fbHttpService.status == httpERR_SYSTEM ){
					inst->stepTimeout.PT = 10000;
					inst->stepTimeout.IN = false;
					TON( &inst->stepTimeout );
					inst->stepTimeout.IN = true;
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
				inst->webData.fbHttpService.send = true;
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


			case INTERNAL_ERROR:
			case INTERNAL_ERROR_GETSEGMENT:
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
		CollectSegmentInformation(inst);

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

