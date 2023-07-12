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
#include "TD_Recorder.h"




/* Recorder */
void TD_Recorder(struct TD_Recorder* inst)
{
	if( inst->Enable ){

		switch( inst->step ){
			case STARTUP:
				inst->Valid = false;
				inst->Busy = false;
				inst->Error = false;
				inst->Saved = false;
				/* get pointer to assembly's PV */
				inst->ErrorID = PV_xgetadr( (char*) inst->AssemblyName, (UDINT*) &inst->pAssembly, (UDINT*) &inst->tempudint );
				if( inst->ErrorID == 0 ){					
					inst->Busy = true;
					/* */
					inst->fbRtInfo.enable = true;
					RTInfo( &inst->fbRtInfo );
					if( inst->NumberOfCycles == 0 ){
						inst->refreshCycles = DEFAULT_CYCLE_US / inst->fbRtInfo.cycle_time;
					}
					else {
						inst->refreshCycles = inst->NumberOfCycles;
					}
					inst->maxRecords = RECORDING_TIME_US / (inst->refreshCycles * inst->fbRtInfo.cycle_time); /* max. number of records */
	
					/* */
					inst->fbGetSegment.Assembly = inst->pAssembly;
					inst->fbGetSegment.AdvancedParameters.SelectionMode = mcACPTRAK_GET_SEG_ALL;
					inst->fbGetSegment.Next = false;
					inst->fbGetSegment.Enable = false; /* reset fb */
					MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );		
	
					/* */
					inst->fbSegGetInfo.Execute = false; /* reset fb */
					MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
	
					/* */
					inst->fbDatObjInfo.enable = false; /* reset fb */
					if( std::strlen(inst->DataObjectName) == 0 ){
						std::strcpy( inst->DataObjectName, "$$tdrec" );
					}
					inst->fbDatObjInfo.pName = (UDINT) inst->DataObjectName;
					DatObjInfo( &inst->fbDatObjInfo );
					inst->fbDatObjDelete.enable = false; /* reset fb */
					DatObjDelete( &inst->fbDatObjDelete );
					inst->fbDatObjCreate.enable = false; /* reset fb */
					inst->fbDatObjCreate.grp = 0;
					inst->fbDatObjCreate.pName = (UDINT) inst->DataObjectName;
					inst->fbDatObjCreate.len = inst->maxRecords * ( inst->DataSize + sizeof(McAcpTrakDateTimeType) ) + BUFFER_SIZE;
					inst->fbDatObjCreate.MemType = doTEMP;
					inst->fbDatObjCreate.Option = doNO_CS;
					inst->fbDatObjCreate.pCpyData = 0;
					DatObjCreate( &inst->fbDatObjCreate );
	
					/*  */
					inst->fbCopyShuttleData.Assembly = inst->pAssembly;
					inst->fbCopyShuttleData.Abort = false;
					inst->fbCopyShuttleData.Command = mcACPTRAK_SH_DATA_ALL;
					inst->fbCopyShuttleData.AdvancedParameters.ShuttleIndex = 0;
					inst->fbCopyShuttleData.AdvancedParameters.DataAddress = inst->DataAddress;
					inst->fbCopyShuttleData.AdvancedParameters.DataSize = inst->DataSize;
					inst->fbCopyShuttleData.AdvancedParameters.Trigger = mcACPTRAK_TRIGGER_IMMEDIATELY;
					inst->fbCopyShuttleData.Execute = false; /* reset fb */
					MC_BR_AsmCopyShuttleData_AcpTrak( &inst->fbCopyShuttleData );
	
					/* */
					inst->fbFileCreate.pDevice = (UDINT) &inst->FileDeviceName;
					inst->fbFileCreate.pFile = (UDINT) &inst->OutputFileName;
					inst->fbFileCreate.enable = false; /* reset fb */
					FileCreate( &inst->fbFileCreate );
					inst->fbFileRead.enable = false; /* reset fb */
					FileWriteEx( &inst->fbFileWrite );
					
					/* */
					std::strcpy( inst->svgFileName, inst->AssemblyName );
					std::strcat( inst->svgFileName, ".svg" );
					inst->fbFileOpen.pDevice = (UDINT)"SvgData";
					inst->fbFileOpen.pFile = (UDINT) &inst->svgFileName;
					inst->fbFileOpen.mode = fiREAD_ONLY;
					inst->fbFileOpen.enable = false; /* reset fb */
					FileOpen( &inst->fbFileOpen );
					FileReadEx( &inst->fbFileRead );
					inst->fbFileWrite.enable = false; /* reset fb */
	
					/* */
					inst->fbDatObjInfo.enable = true; /* true fb */
					DatObjInfo( &inst->fbDatObjInfo );
					inst->step = CHECK_DATA_OBJ;

					/*  */
					inst->fbSystemDump.enable = false; 
					inst->fbSystemDump.configuration = sdm_SYSTEMDUMP_DATA;
					inst->fbSystemDump.pDevice = (UDINT) &inst->FileDeviceName;
					inst->fbSystemDump.pFile = (UDINT) &inst->DumpFileName;
					inst->fbSystemDump.pParam = 0;
					SdmSystemDump( &inst->fbSystemDump ); /* reset fb */

					inst->tonTriggerDelay.PT = 200;
				}
				else {
					inst->Error = true;
					inst->step = INTERNAL_ERROR_PVXGETADR;
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
				inst->pTimestamps = inst->pDataObject + inst->maxRecords * inst->DataSize; /* pointer to record timestamps */
				inst->pBuffer = inst->pTimestamps + inst->maxRecords * sizeof(McAcpTrakDateTimeType); /* pointer to buffer */
				inst->fbDatObjCreate.enable = false; /* reset fb */
				DatObjCreate( &inst->fbDatObjCreate );				
				inst->fbGetSegment.Enable = true; /* start fb */
				MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	
				inst->n = 0;
				inst->step = GET_SEGMENT_LIST;
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
				MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
				++inst->n;
				if( inst->n < inst->SegInfo.numberOfSegments ){ /* there are still some segments left */
					inst->fbSegGetInfo.Execute = true; /* start fb */
					inst->fbSegGetInfo.Segment = &inst->SegInfo.segment[inst->n];
					MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
				}
				else { /* all segments processed. We can start recording */
					inst->step = START_RECORDING;
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


			case START_RECORDING:
				inst->fbCopyShuttleData.AdvancedParameters.DataAddress = inst->pDataObject; 
				inst->fbCopyShuttleData.AdvancedParameters.DataSize = inst->DataSize;
				inst->currentRecord = 0; /* point to first record in data object */
				inst->fbCopyShuttleData.Execute = true; /* start fb */
				MC_BR_AsmCopyShuttleData_AcpTrak( &inst->fbCopyShuttleData );
				inst->tonTriggerDelay.IN = false; /* reset fb */
				TON( &inst->tonTriggerDelay );
				inst->Busy = false;
				inst->step = RECORDING;
			break;

			
			case RECORDING:  /* recording and waiting for a trigger */	
			if( (inst->cycleCounter >= inst->refreshCycles) && ! inst->fbCopyShuttleData.Execute ){
				inst->cycleCounter = 0;
				inst->fbCopyShuttleData.Execute = true; /* start fb */
				MC_BR_AsmCopyShuttleData_AcpTrak( &inst->fbCopyShuttleData );
			}
			inst->tonTriggerDelay.IN = inst->Trigger;
			TON( &inst->tonTriggerDelay );
			if( inst->fbCopyShuttleData.Done ){
				std::memcpy( (void*) inst->DataAddress, 
							(void*) inst->fbCopyShuttleData.AdvancedParameters.DataAddress, inst->DataSize ); /* copy shuttle data to output structure */
				((McAcpTrakDateTimeType*) inst->pTimestamps)[inst->currentRecord] = inst->fbCopyShuttleData.Info.TimeStamp; /* copy timestamp */
				inst->fbCopyShuttleData.Execute = false; /* reset fb */
				MC_BR_AsmCopyShuttleData_AcpTrak( &inst->fbCopyShuttleData );

				if( !inst->tonTriggerDelay.Q ){ /* we don't get a trigger so we continue to record */
					inst->currentRecord += 1; /* use next record in data object */
					inst->fbCopyShuttleData.AdvancedParameters.DataAddress += inst->DataSize;
					if( inst->currentRecord >= inst->maxRecords ){
						inst->currentRecord = 0;
						inst->Valid = true;
						inst->fbCopyShuttleData.AdvancedParameters.DataAddress = inst->pDataObject;
					}
				}
				else if( inst->tonTriggerDelay.Q and inst->Valid ) { /* we got a trigger and now we save everything */
					TD_filenameDT( (UDINT) "TD_Recorder_", (UDINT) &inst->OutputFileName, sizeof(inst->OutputFileName)-1 );
					std::strcat( (char*) &inst->OutputFileName, (char*) ".html" );
					TD_filenameDT( (UDINT) "TD_Recorder_Dump_", (UDINT) &inst->DumpFileName, sizeof(inst->DumpFileName)-1 );
					std::strcat( (char*) &inst->DumpFileName, (char*) ".tar.gz" );
					inst->Valid = false;
					inst->Busy = true;
					inst->Saved = false;
					inst->fbFileCreate.enable = true; /* start fb */
					FileCreate( &inst->fbFileCreate );
					inst->step = CREATE_FILE;
				}
			}
			else if( inst->fbCopyShuttleData.Error ){
				inst->Valid = false;
				inst->ErrorID = inst->fbCopyShuttleData.ErrorID;
				inst->Error = true;
				inst->fbCopyShuttleData.Execute = false; /* reset fb */
				MC_BR_AsmCopyShuttleData_AcpTrak( &inst->fbCopyShuttleData );
				inst->step = INTERNAL_ERROR_COPYSHUTTLEDATA;
			}
			else { /* busy */
				MC_BR_AsmCopyShuttleData_AcpTrak( &inst->fbCopyShuttleData );
			}
			++inst->cycleCounter;
			break;

			
			case CREATE_FILE:
			if( inst->fbFileCreate.status == 0 ){
				inst->fbFileWrite.ident = inst->fbFileCreate.ident;
				inst->fbFileCreate.enable = false; /* reset fb */
				FileCreate( &inst->fbFileCreate );
				inst->n = 0;
				inst->fbFileWrite.offset = 0;
				inst->fbFileWrite.enable = true;
				int len = sprintf( (char*) inst->pBuffer, HTML_HEADER, inst->OutputFileName );
				inst->fbFileWrite.pSrc = inst->pBuffer;
				inst->fbFileWrite.len = len;
				FileWriteEx( &inst->fbFileWrite );
				inst->step = WRITE_HTML_HEADER;
			}
			else if( inst->fbFileCreate.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbFileCreate.status;
				inst->Error = true;
				inst->fbFileCreate.enable = false; /* reset fb */
				FileCreate( &inst->fbFileCreate );
				inst->step = INTERNAL_ERROR_FILECREATE;
			}
			else { /* busy */
				FileCreate( &inst->fbFileCreate );
			}
			break;


			case WRITE_HTML_HEADER:
			if( inst->fbFileWrite.status == 0 ){ /* done */
				inst->fbFileWrite.offset += inst->fbFileWrite.len;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );	
				/* write segment information to buffer */
				const char head[] = "\t\tconst segment = [\n\t\t\t/* name, length */\n";
				const char tail[] = "\n\t\t];\n\n";
				int totalLength = std::strlen(head) + std::strlen(tail);
				char temp[256];
				std::strcpy( (char*) inst->pBuffer, head );
				for( int i = 0; i < inst->SegInfo.numberOfSegments; ++i ){
					int len = std::sprintf( temp, "%s new Segment( %d, \"%s\", %1.8f, \"%s.ST%lu\" )  /* [%d] */", i ? "\n\t\t\t," : "\t\t\t ", 
								inst->SegInfo.segmentInfo[i].ID, 
								inst->SegInfo.segmentInfo[i].Name, 
								inst->SegInfo.segmentInfo[i].Length,
								inst->SegInfo.segmentInfo[i].PlkInterface,
								inst->SegInfo.segmentInfo[i].NodeNumber, i );
					if( (totalLength + len ) < BUFFER_SIZE ){
						totalLength += len;
						std::strcat( (char*) inst->pBuffer, temp );
					}
					else {
						inst->ErrorID = fiERR_DATA; /* error : buffer to small */
						inst->Error = true;
						inst->Busy = false;
						break;
					}
				}
				if(inst->Error ){
					inst->step = INTERNAL_ERROR_BUFFER_SIZE;
				}
				else {
					std::strcat( (char*) inst->pBuffer, tail );
					inst->fbFileWrite.pSrc = inst->pBuffer;	
					inst->fbFileWrite.len = totalLength;						
					inst->fbFileWrite.enable = true; /* start fb */
					FileWriteEx( &inst->fbFileWrite );
					inst->step = WRITE_SEGMENT_INFO;					
				}
			}
			else if( inst->fbFileWrite.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbFileWrite.status;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );
				inst->step = INTERNAL_ERROR_FILEWRITE;				
			}
			else { /* busy */
				FileWriteEx( &inst->fbFileWrite );
			}
			break;


			case WRITE_SEGMENT_INFO:
			if( inst->fbFileWrite.status == 0 ){
				inst->fbFileWrite.offset += inst->fbFileWrite.len;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );	
				inst->n = 0;
				inst->step = COLLECT_SHUTTLE_DATA;
			}
			else if( inst->fbFileWrite.status != 65535 ){
				inst->ErrorID = inst->fbFileWrite.status;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );
				inst->step = INTERNAL_ERROR_FILEWRITE;				
			}
			else { /* busy */
				FileWriteEx( &inst->fbFileWrite );				
			}
			break;


			case COLLECT_SHUTTLE_DATA:
			{
				int entrySize = sizeof(McAcpTrakShuttleData) + inst->UserDataSize;
				int numberOfEntries = inst->DataSize / entrySize;
				McAcpTrakDateTimeType timeStamp = ((McAcpTrakDateTimeType*)inst->pTimestamps)[inst->currentRecord];
				USINT *pBase = (USINT*) (inst->pDataObject + inst->currentRecord * inst->DataSize);
				const char head1[] = "\t\tconst record = [\n" // head for first record
									"\t\t\t/* index, active, PLCopen state, segment index, segment position */"
									"\n\t\t\t/* record: %d = latest */\n\t\t\t  new Record( new Date(%d, %d, %d, %d, %d, %d, %d), \n\t\t\t\t[";
				const char headx[] = " ] )\n\t\t\t/* record: %d */\n\t\t\t, new Record( new Date(%d, %d, %d, %d, %d, %d, %d), \n\t\t\t\t["; // head for other records
				const char tail[] = " ] )\n\t\t];\n\n"; /* to close declaration */
				int totalLength = sprintf( (char*) inst->pBuffer, inst->n == 0 ? head1 : headx, inst->n, timeStamp.Year, timeStamp.Month, timeStamp.Day, 
												timeStamp.Hour, timeStamp.Minute, timeStamp.Second, timeStamp.Millisec );
				char temp[256];
				/* write shuttle data of one record into buffer */
				bool validEntriesFound = false;
				for( int i = 0; i <numberOfEntries; ++i, pBase += entrySize ){
					McAcpTrakShuttleData *pEntry = (McAcpTrakShuttleData *) pBase;
					if( pEntry->Index ){ /* valid entry ? */
						/* find index of segment */
						int indexOfSegment = -1;
						for( int i = 0; i < inst->SegInfo.numberOfSegments; ++i ){
							if( !std::strcmp( pEntry->SegmentName, inst->SegInfo.segmentInfo[i].Name ) ){
								indexOfSegment = i;
								break;
							}
						}
						if( indexOfSegment >= 0 ){ 
							int len = sprintf( temp, "%s new Shuttle( %d, %d, 0x%x, %i, %lf )", !validEntriesFound ? "" : "\n\t\t\t\t,", 
																			pEntry->Index,
																			pEntry->Active,
																			pEntry->PLCopenState,
																			indexOfSegment, pEntry->SegmentPosition );
							validEntriesFound = true;
							if( (totalLength + len + 256 ) < BUFFER_SIZE ){
								totalLength += len;
								std::strcat( (char*) inst->pBuffer, temp );
							}
							else {
								inst->ErrorID = fiERR_DATA; /* error : buffer to small */
								inst->Error = true;
								inst->Busy = false;
								break;
							}
						} // if( indexOfSegment >= 0 )...
					}
				}
				if(inst->Error ){
					inst->step = INTERNAL_ERROR_BUFFER_SIZE;
				}
				else {
					if( inst->n == (inst->maxRecords -1) ){
						std::strcat( (char*) inst->pBuffer, tail );
						totalLength += strlen(tail);
					}
					inst->fbFileWrite.pSrc = inst->pBuffer;	
					inst->fbFileWrite.len = totalLength;						
					inst->fbFileWrite.enable = true; /* start fb */
					FileWriteEx( &inst->fbFileWrite );
					inst->step = WRITE_SHUTTLE_DATA;	
				}
			}
			break;


			case WRITE_SHUTTLE_DATA:
			if( inst->fbFileWrite.status == 0 ){ /* done */
				inst->fbFileWrite.offset += inst->fbFileWrite.len;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );
				++inst->n;
				if( inst->n == inst->maxRecords ) { /* no more records to write */
					inst->fbFileWrite.pSrc = (UDINT) HTML_HEADER_END;
					inst->fbFileWrite.len = std::strlen( HTML_HEADER_END );
					inst->fbFileWrite.enable = true; /* start fb */
					FileWriteEx( &inst->fbFileWrite );	
					inst->step = WRITE_HTML_HEADER_END;
				}
				else {
					if( --inst->currentRecord < 0 )
						inst->currentRecord = inst->maxRecords - 1;
					inst->step = COLLECT_SHUTTLE_DATA;
				}
			}
			else if( inst->fbFileWrite.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbFileWrite.status;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );
				inst->step = INTERNAL_ERROR_FILEWRITE;				
			}
			else { /* busy */
				FileWriteEx( &inst->fbFileWrite );
			}
			break;


			case WRITE_HTML_HEADER_END:
			if( inst->fbFileWrite.status == 0 ){ /* done */
				inst->fbFileWrite.enable = false; /* reset fb */
				inst->fbFileWrite.offset += inst->fbFileWrite.len;
				FileWriteEx( &inst->fbFileWrite );
				inst->fbFileOpen.enable = true; /* open svg file */
				FileOpen( &inst->fbFileOpen );
				inst->step = OPEN_SVG_FILE;
			}
			else if( inst->fbFileWrite.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbFileWrite.status;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );
				inst->step = INTERNAL_ERROR_FILEWRITE;				
			}
			else { /* busy */
				FileWriteEx( &inst->fbFileWrite );
			}
			break;


			case OPEN_SVG_FILE:
			if( inst->fbFileOpen.status == 0 ){ /* done */
				inst->fbFileRead.offset = 0;
				inst->fbFileRead.pDest = inst->pBuffer;			
				inst->fbFileRead.len = BUFFER_SIZE;				
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
				inst->step = INTERNAL_ERROR_FILEOPEN;	
			}
			else { /* busy */
				FileOpen( &inst->fbFileOpen );
			}
			break;


			case READ_SVG_DATA:
			if( inst->fbFileRead.status == 0 ){ /* done */
				if( inst->fbFileRead.bytesread > 0 ){
					inst->fbFileWrite.pSrc = inst->pBuffer;							
					inst->fbFileWrite.len = inst->fbFileRead.bytesread;
					inst->fbFileRead.offset += inst->fbFileRead.bytesread;
					inst->fbFileRead.enable = false; /* reset fb */
					FileReadEx( &inst->fbFileRead );
					inst->fbFileWrite.enable = true; /* start fb */
					FileWriteEx( &inst->fbFileWrite );
					inst->step = WRITE_SVG_DATA;
				}
				else { /* end of file */
					inst->fbFileClose.ident = inst->fbFileRead.ident;
					inst->fbFileRead.enable = false; /* reset fb */
					FileReadEx( &inst->fbFileRead );
					inst->fbFileClose.enable = false; /* reset fb */
					FileClose( &inst->fbFileClose );
					inst->fbFileClose.enable = true;  /* start fb */
					FileClose( &inst->fbFileClose );
					inst->step = CLOSE_SVG_FILE;
				}
			}
			//else if( inst->fbFileRead.status == EOF )
			else if( inst->fbFileRead.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbFileRead.status;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileRead.enable = false; /* reset fb */
				FileReadEx( &inst->fbFileRead );
				inst->step = INTERNAL_ERROR_FILEREAD;
			}
			else { /* busy */
				FileReadEx( &inst->fbFileRead );
			}
			break;


			case WRITE_SVG_DATA:
			if( inst->fbFileWrite.status == 0 ){ /* done */
				inst->fbFileWrite.offset += inst->fbFileWrite.len;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );
				inst->fbFileRead.enable = true; /* start fb */
				FileReadEx( &inst->fbFileRead );	
				inst->step = READ_SVG_DATA;
			}
			else if( inst->fbFileWrite.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbFileWrite.status;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );
				inst->step = INTERNAL_ERROR_FILEWRITE;				
			}
			else { /* busy */
				FileWriteEx( &inst->fbFileWrite );
			}
			break;


			case CLOSE_SVG_FILE: /* close the svg file */
			if( inst->fbFileClose.status == 0 ){ /* successful */
				inst->fbFileClose.enable = false; /* reset fb */
				FileClose( &inst->fbFileClose );
				inst->fbFileWrite.pSrc = (UDINT) HTML_FOOTER;
				inst->fbFileWrite.len = std::strlen( HTML_FOOTER );
				inst->fbFileWrite.enable = true; /* start fb */
				FileWriteEx( &inst->fbFileWrite );	
				inst->step = WRITE_HTML_FOOTER;
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



			case WRITE_HTML_FOOTER:
			if( inst->fbFileWrite.status == 0 ){ /* done */
				inst->fbFileClose.ident = inst->fbFileWrite.ident;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );	
				inst->fbFileClose.enable = false; /* reset fb */
				FileClose( &inst->fbFileClose );
				inst->fbFileClose.enable = true;  /* start fb */
				FileClose( &inst->fbFileClose );
				inst->step = CLOSE_DEST_FILE;
			}
			else if( inst->fbFileWrite.status != 65535 ){ /* error */
				inst->ErrorID = inst->fbFileWrite.status;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileWrite.enable = false; /* reset fb */
				FileWriteEx( &inst->fbFileWrite );
				inst->step = INTERNAL_ERROR_FILEWRITE;				
			}
			else { /* busy */
				FileWriteEx( &inst->fbFileWrite );
			}
			break;


			case CLOSE_DEST_FILE: /* close the destination file */
			if( inst->fbFileClose.status == 0 ){ /* successful */
				inst->fbFileClose.enable = false; /* reset fb */
				FileClose( &inst->fbFileClose );
				inst->fbSystemDump.enable = true;
				SdmSystemDump( &inst->fbSystemDump );
				inst->step = W_SYSTEMDUMP;
			}
			else if( inst->fbFileClose.status != 65535 ) {  /* error */
				inst->ErrorID = inst->fbFileClose.status ;
				inst->Busy = false;
				inst->Error = true;
				inst->fbFileClose.enable = false; /* reset fb */
				FileClose( &inst->fbFileClose );
				inst->step = INTERNAL_ERROR_FILECLOSE_DST;	
			}
			else { /* busy */
				FileClose( &inst->fbFileClose );
			}
			break;


			case W_SYSTEMDUMP:
			if( inst->fbSystemDump.status == 0 ){ /* successful */
				inst->fbSystemDump.enable = false; /* reset fb */
				SdmSystemDump( &inst->fbSystemDump );
				inst->step = W_RESTART;
			}
			else if( inst->fbSystemDump.status != 65535 ){  /* busy */
				inst->fbSystemDump.enable = false; /* reset fb */
				SdmSystemDump( &inst->fbSystemDump );
				inst->step = INTERNAL_ERROR_SYSTEMDUMP;
			}
			else {  /* busy */
				SdmSystemDump( &inst->fbSystemDump );
			}
			break;


			case W_RESTART:
			inst->Busy = false;
			inst->Saved = true;
			if( inst->Trigger == false ){
				inst->Saved = false;
				inst->currentRecord = 0;
				std::memset( (void*) inst->pDataObject, 0, inst->fbDatObjCreate.len );  /* delete old data */
				inst->step = START_RECORDING; 
			}

			case INTERNAL_ERROR:
			case INTERNAL_ERROR_BUFFER_SIZE:
			case INTERNAL_ERROR_PVXGETADR:
			case INTERNAL_ERROR_DATOBJINFO:
			case INTERNAL_ERROR_DATOBJDELETE:
			case INTERNAL_ERROR_DATOBJCREATE:
			case INTERNAL_ERROR_GETSEGMENT:
			case INTERNAL_ERROR_COPYSHUTTLEDATA:
			case INTERNAL_ERROR_FILECREATE:
			case INTERNAL_ERROR_FILEWRITE:
			case INTERNAL_ERROR_FILECLOSE_SVG:
			case INTERNAL_ERROR_FILECLOSE_DST:
			case INTERNAL_ERROR_FILEOPEN:
			case INTERNAL_ERROR_FILEREAD:
			break;

		} /* end of switch() */
	}
	else {
		inst->step = STARTUP;
		inst->ErrorID = 0;
		inst->Error = false;
		inst->Valid = false;
		inst->Busy = false;
	} 
}
