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


UDINT djb2(USINT *str, size_t length );
bool GetSegmentData(struct TD_WebServices* inst, char* segmentName, UINT* ID, LREAL* position );
void CollectShuttleInfo(struct TD_WebServices* inst);

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
			if( (inst->refreshTimer += inst->fbRtInfo.cycle_time) > 100000 ){
//				UDINT hash = djb2( (USINT*) &inst->ShuttleInfo, sizeof(inst->ShuttleInfo) );
//				if( hash != inst->hashShuttleInfo ){
//					inst->hashShuttleInfo = hash;
					inst->refreshTimer = 0;
					CollectShuttleInfo(inst);	
//				}
			}

			httpService( &inst->webData.fbHttpService); 
			if( inst->webData.fbHttpService.status == 0 ){
				if( inst->webData.fbHttpService.method == httpMETHOD_GET && inst->webData.fbHttpService.phase == httpPHASE_RECEIVED ){
					std::memset( &inst->webData.responseHeader, 0, sizeof(inst->webData.responseHeader) );
					std::strcpy( (char*) inst->webData.responseHeader.protocol, "HTTP/1.1" );
					std::strcpy( (char*) inst->webData.responseHeader.status, "200 OK" );
					inst->webData.fbHttpService.pResponseData = (UDINT) &inst->webData.responseData;

					/* deliver summarized information */		
					if( std::strcmp( (char*) inst->webData.uri, "TrakWebApi/info") == 0 ){
						sprintf( (char*) inst->webData.responseData, "{\"TrakDiag\" : \"" STRINGIFY(_TrakDiag_VERSION) "\","
																	 "\"McAcpTrak\" : \"%s\""
																	 "}", inst->acpTrakVersion );
						
						inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
						std::strcpy( (char*) inst->webData.responseHeader.contentType, "application/json; charset=iso-8859-1");
						inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
						std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
						std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
						inst->webData.fbHttpService.send = true;
						inst->step = HTTP_SERV_RESPONSE;
					}

					/* deliver assembly information */		
					else if( std::strcmp( (char*) inst->webData.uri, "TrakWebApi/assembly") == 0 ){
// not available in 5.21
//						int plcOpen = inst->asmInfo.PLCopenState;
//						if( plcOpen > mcACPTRAK_INVALID_CONFIGURATION ){
//							plcOpen = mcACPTRAK_INVALID_CONFIGURATION;
//						}
						sprintf( (char*) inst->webData.responseData, 
								"{"
//								"\"PLCopen\" : \"%s\"," // not available in 5.21
								"\"CommunicationReady\" : %s,"
								"\"ReadyForPowerOn\" : %s,"
								"\"PowerOn\" : %s,"
								"\"SegmentsInDisabledCount\" : %d,"
								"\"SegmentsInStoppingCount\" : %d,"
								"\"SegmentsInErrorStopCount\" : %d,"
								"\"ShuttlesCount\" : %d,"
								"\"ShuttlesInErrorStopCount\" : %d"
								"}", 
//								AcpTrakPLCopenState[plcOpen], // not available in 5.21
								JavascriptBoolean[inst->asmInfo.CommunicationReady],
								JavascriptBoolean[inst->asmInfo.ReadyForPowerOn],
								JavascriptBoolean[inst->asmInfo.PowerOn],
								inst->asmInfo.SegmentsInDisabledCount,
								inst->asmInfo.SegmentsInStoppingCount,
								inst->asmInfo.SegmentsInErrorStopCount,
								inst->asmInfo.ShuttlesCount,
								inst->asmInfo.ShuttlesInErrorStopCount
						);
						
						inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
						std::strcpy( (char*) inst->webData.responseHeader.contentType, "application/json; charset=iso-8859-1");
						inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
						std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
						std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
						inst->webData.fbHttpService.send = true;
						inst->step = HTTP_SERV_RESPONSE;
					}

					/* deliver shuttle positions */
					else if( std::strcmp( (char*) inst->webData.uri, "TrakWebApi/positions") == 0 ){

						std::strcpy( (char*) inst->webData.responseData, "[" ); /* start JSON */
						size_t lengthOfJSON = 2; 
						char s[256];
						for( unsigned n = 0; n < inst->ShuttleInfo.numberOfEntries; ++n ){
							TD_ServicesShuttleType *shuttle = &inst->ShuttleInfo.shuttle[n];
							/* we return an array of pairs of two UDINT:
								[0] Bit 0-31  : shuttle index
								[1] Bit 0-7   : PLCopen state
								[1] Bit 8     : active shuttle
								[1] Bit 9     : virtual shuttle
								[1] Bit 10-20 : segment index (11 Bit)
								[1] Bit 21-27 : segment position in % (7 Bit)
							*/
							UDINT first = shuttle->index;
							UDINT second = static_cast<UDINT>(shuttle->plcOpenState)
											+ (static_cast<UDINT>(shuttle->flags &0x3) << 8)
											+ (static_cast<UDINT>(shuttle->segmentID & 0x7ff) << 10)
										 	+ (static_cast<UDINT>(shuttle->segmentPosition & 0x7f) << 21); 
							if( !inst->fbAsmReadInfo.AssemblyInfo.PowerOn || (shuttle->flags & 0x01) ){ /* don't return deleted shuttles when assembly is powered on */
								lengthOfJSON += sprintf( s, "%s[%ld,%ld]" , n == 0 ? "" : ",", first, second );
								if( lengthOfJSON < sizeof(inst->webData.responseData) ){
									std::strcat( (char*) inst->webData.responseData, s );
								}
								else break;
							}
						}
						std::strcat( (char*) inst->webData.responseData, "]" ); /* close JSON */

						inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
						std::strcpy( (char*) inst->webData.responseHeader.contentType, "application/json; charset=iso-8859-1");
						inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
						std::strcpy( (char*) inst->webData.responseHeader.connection, "keep-Alive" );
						std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
						inst->webData.fbHttpService.send = true;
						inst->step = HTTP_SERV_RESPONSE;
					}

					/* deliver list of segments */
					else if( std::strcmp( (char*) inst->webData.uri, "TrakWebApi/segments") == 0 ){
						std::strcpy( (char*) inst->webData.responseData, "[ " ); /* start JSON */
						size_t length = 2;
						for( int n = 0; n < inst->SegInfo.numberOfSegments; ++n ){
							char s[256];
							length += std::sprintf( s, "%s \"name\": \"%s\", \"ID\": %d, \"length\": %01.15lf }", n == 0 ? "{" : ",{", 
									inst->SegInfo.segmentInfo[n].Name,
									inst->SegInfo.segmentInfo[n].ID,
									inst->SegInfo.segmentInfo[n].Length	);
							if( length < sizeof(inst->webData.responseData) ){
								std::strcat( (char*) inst->webData.responseData, s ); /* copy segment info */
							}
							else
								break;
						} 
						std::strcat( (char*) inst->webData.responseData, "]" ); /* close JSON */
						inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
						std::strcpy( (char*) inst->webData.responseHeader.contentType, "application/json; charset=iso-8859-1");
						inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
						std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
						std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
						inst->webData.fbHttpService.send = true;
						inst->step = HTTP_SERV_RESPONSE;
					}


					/* cyclic segment information */		
					else if( std::strcmp( (char*) inst->webData.uri, "TrakWebApi/segment_status") == 0 ){
						std::strcpy( (char*) inst->webData.responseData, "[ " ); /* start JSON */
						size_t length = 2;
						for( int n = 0; n < inst->SegInfo.numberOfSegments; ++n ){
							char s[256];
							unsigned int flags =  0;
							if( inst->fbSegReadInfo[n].Valid ){
								McAcpTrakSegInfoType * i = &inst->fbSegReadInfo[n].SegmentInfo;
								flags =  (!!i->CommunicationReady)
										 | (!!i->ReadyForPowerOn << 1)
										 | (!!i->PowerOn << 2)
										 | (!!i->SegmentEnable << 3); 
							}
 							length += static_cast<size_t>(std::sprintf( s, "%s%d", n == 0 ? "" : ",", flags	));
							if( length < sizeof(inst->webData.responseData) ){
								std::strcat( (char*) inst->webData.responseData, s ); /* copy segment info */
							}
							else
								break;
						} 
						std::strcat( (char*) inst->webData.responseData, "]" ); /* close JSON */
						inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
						std::strcpy( (char*) inst->webData.responseHeader.contentType, "application/json; charset=iso-8859-1");
						inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
						std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
						std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
						inst->webData.fbHttpService.send = true;
						inst->step = HTTP_SERV_RESPONSE;
					}


					/* deliver svgdata */
					else if( std::strcmp( (char*) inst->webData.uri, "TrakWebApi/svgdata") == 0 ){
						inst->fbFileInfo.enable = true;
						FileInfo( &inst->fbFileInfo );
						inst->step = INFO_SVG_FILE;
					}

					/* deliver index page */
					else if( std::strcmp( (char*) inst->webData.uri, "TrakWebApi/index.html") == 0 ){
						std::sprintf( (char*) inst->webData.responseData, INDEX_HTML, "Assembly" );
						inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
						std::strcpy( (char*) inst->webData.responseHeader.contentType, "text/html; charset=iso-8859-1");
						inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
						std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
						std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
						inst->webData.fbHttpService.send = true;
						inst->step = HTTP_SERV_RESPONSE;
					}

					else {	
						std::strcpy( (char*) inst->webData.responseData, "no data" );
						std::strcpy( inst->webData.responseHeader.status, "404 Not Found" );
						std::strcpy( inst->webData.responseHeader.contentType, "text/plain; charset=utf-8" );
						inst->webData.responseHeader.contentLength = std::strlen( (char*) inst->webData.responseData );
						inst->webData.fbHttpService.responseDataLen = inst->webData.responseHeader.contentLength;
						std::strcpy( inst->webData.responseHeader.connection, "close" );
						std::strcpy( inst->webData.responseHeader.keepAlive, "" );
						inst->webData.fbHttpService.send = true;
						inst->step = HTTP_SERV_RESPONSE;	
					}
				}
			}
			else if( inst->webData.fbHttpService.status != 65535 ){
				inst->webData.lastError = inst->webData.fbHttpService.status;
				inst->webData.fbHttpService.enable = false;
				inst->step = INTERNAL_ERROR_HTTP;
			}
			break;

			
			case HTTP_SERV_RESPONSE: /* send http response */
			httpService( &inst->webData.fbHttpService); 
			if( inst->webData.fbHttpService.status == 0 ){
				if( inst->webData.fbHttpService.phase == httpPHASE_WAITING ){
					inst->webData.fbHttpService.send = false;
					inst->step = W_HTTP_REQUESTS;	
				}
			}
			else if( inst->webData.fbHttpService.status != 65535 ){
				inst->webData.lastError = inst->webData.fbHttpService.status;
				inst->webData.fbHttpService.enable = false;
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
				inst->webData.fbHttpService.pResponseData = inst->pDataObject;
				inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->pDataObject );
				std::strcpy( (char*) inst->webData.responseHeader.contentType, "image/svg+xml");
				inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
				std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
				std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
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

		/* assembly information */
		MC_BR_AsmReadInfo_AcpTrak( &inst->fbAsmReadInfo );
		if( inst->fbAsmReadInfo.Valid ){
			std::memcpy( &inst->asmInfo, &inst->fbAsmReadInfo.AssemblyInfo, sizeof(inst->asmInfo) );
		}
		else {
			std::memset( &inst->asmInfo, 0, sizeof(inst->asmInfo ));	
		}

		/* segment information */
		for( int n = 0; n < inst->SegInfo.numberOfSegments; ++n ){
			MC_BR_SegReadInfo_AcpTrak( &inst->fbSegReadInfo[n] );
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

/* calculates djb2 hash */
UDINT djb2(USINT *str, size_t length ) {
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