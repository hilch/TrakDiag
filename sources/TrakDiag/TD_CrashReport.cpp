/*

https://github.com/hilch/TrackDiag

Helper Library for ACOOStrak diagnosis

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

#ifndef _REPLACE_CONST
#define _REPLACE_CONST
#endif

#include <bur/plctypes.h>
#include <cstring>
#include <cstdio>
#include <ctime>
#include <algorithm>

#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "TrakDiag.h"
#ifdef __cplusplus
	};
#endif


/* check if we want this entry */

bool isValidEntry( struct TD_CrashReport* inst ){
			bool validSeverity = (inst->fbReadEntry.Entry.Severity >= inst->thrSeverity);

			if( strlen( inst->objectStartsWith1 ) )
				if( std::strstr( inst->fbReadEntry.Entry.ObjectID, inst->objectStartsWith1) )
					return validSeverity;

			if( strlen( inst->objectStartsWith2 ) )
				if( std::strstr( inst->fbReadEntry.Entry.ObjectID, inst->objectStartsWith2) )
					return validSeverity;

			return false;

}


#define W_COMMAND 0
#define GET_IDENT 10
#define W_ENTRIES_WRITTEN 20
#define READ_LATEST_RECORD 30
#define READ_PREVIOUS_ID 40
#define COPY_ENTRY 50
#define READY 100
#define ERROR_GET_IDENT 9001
#define ERROR_GET_PREVIOUS_RECORD_ID 9002
#define ERROR_GET_LATEST_RECORD_ID 9003
#define ERROR_READ_FIRST_ENTRY 9004
#define ERROR_READ_ENTRY 9005



/* read motion logger cyclically */
void TD_CrashReport(struct TD_CrashReport* inst) {


	if( !inst->Execute ){
		inst->step = W_COMMAND;
	}
	

	for(;;) /* use 'continue' instead of 'break' to save one cycle */
	{
		switch( inst->step ) {
	
			case W_COMMAND:  /* Execute := FALSE */
			inst->Busy = 0;
			inst->Done = 0;
			inst->Error = 0;
			inst->StatusID = 0;
	
			if( inst->Execute && inst->pLoggerEntries && (inst->SizeLoggerEntries >= sizeof(TD_LoggerEntryType) ) ) {
	
				std::memset( (void*) &inst->fbGetIdent, 0, sizeof(inst->fbGetIdent));
				ArEventLogGetIdent( &inst->fbGetIdent );	/* reset fb */
	
				std::memset( (void*) &inst->fbGetLatestRecordID, 0, sizeof(inst->fbGetLatestRecordID));
				ArEventLogGetLatestRecordID( &inst->fbGetLatestRecordID ); /* reset fb */
	
				std::memset( (void*) &inst->fbGetPreviousRecordID, 0, sizeof(inst->fbGetPreviousRecordID));
				ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID ); /* reset fb */
	
				std::memset( (void*) &inst->fbReadEntry, 0, sizeof(inst->fbReadEntry));
				TD_LoggerReadEntry( &inst->fbReadEntry ); /* reset fb */
	
				std::memset( (void*) &inst->tonDelayRead, 0, sizeof(inst->tonDelayRead));
				TON( &inst->tonDelayRead ); /* reset fb */
				
	
				inst->MaxEntries = inst->SizeLoggerEntries / sizeof(TD_LoggerEntryType);
				inst->NbrOfEntries = 0;
				inst->counterEntries = 0;
				std::memset( reinterpret_cast<TD_LoggerEntryType*>(inst->pLoggerEntries),0, inst->SizeLoggerEntries );  /* clear logger entries */
	
				inst->fbDTGetTime.enable = 1;
				DTGetTime( &inst->fbDTGetTime );
				if( inst->observationPeriod == 0 )
					inst->observationPeriod = 3000; /* 3 seconds */
				inst->eventTimeMinimum = inst->fbDTGetTime.DT1 - inst->observationPeriod/1000;
									
				inst->Busy = 1;
				inst->index = 0;
				inst->fbGetIdent.Execute = 1;
				std::strcpy( (char*) &inst->fbGetIdent.Name, "$motion" );
				ArEventLogGetIdent( &inst->fbGetIdent );
				inst->step = GET_IDENT;
	
			}
			break;
	
	
			case GET_IDENT: /* get ident of logger module */
			if( inst->fbGetIdent.Done ) {
				inst->fbGetPreviousRecordID.Ident = inst->fbGetIdent.Ident;
				inst->fbReadEntry.Ident = inst->fbGetIdent.Ident;
				inst->fbGetLatestRecordID.Ident = inst->fbGetIdent.Ident;
				inst->tonDelayRead.PT = 1000;
				inst->tonDelayRead.IN = 1;
				inst->step = W_ENTRIES_WRITTEN;
			}
			else if( inst->fbGetIdent.Error ) { /* error getting logger ident for module copy */
				inst->StatusID = inst->fbGetIdent.StatusID;
				inst->step = ERROR_GET_IDENT;
			}
			else { /* busy */
				ArEventLogGetIdent( &inst->fbGetIdent );
			}
			break;
	
	
			case W_ENTRIES_WRITTEN: /* wait until entries have been written to logger */
			TON( &inst->tonDelayRead );
			if( inst->tonDelayRead.Q ){
				inst->fbGetLatestRecordID.Execute = 1;
				ArEventLogGetLatestRecordID( &inst->fbGetLatestRecordID );
				inst->step = READ_LATEST_RECORD;
			}
			break;
	
	
			case READ_LATEST_RECORD:  /* read lates record id */
			if( inst->fbGetLatestRecordID.Done )
			{
				inst->counterEntries++;
				inst->fbReadEntry.RecordID = inst->fbGetLatestRecordID.RecordID;
				inst->fbReadEntry.Execute = 1;
				TD_LoggerReadEntry( &inst->fbReadEntry );
				inst->NbrOfEntries = 0;
				inst->fbGetLatestRecordID.Execute = 0;
				ArEventLogGetLatestRecordID( &inst->fbGetLatestRecordID );
				inst->step = COPY_ENTRY;
				continue;								
			}
			else if( inst->fbGetLatestRecordID.Error ){
				inst->StatusID = inst->fbGetLatestRecordID.StatusID;
				inst->step = ERROR_GET_LATEST_RECORD_ID;
			}
			else {
				ArEventLogGetLatestRecordID( &inst->fbGetLatestRecordID );	
			}
			break;
	
	
			case READ_PREVIOUS_ID: /* read previous entry */
			if( inst->index < inst->MaxEntries ) {  /* do we have enough space in entries array ? */
				if( inst->fbGetPreviousRecordID.Done ) {
					inst->counterEntries++;
					inst->fbReadEntry.RecordID = inst->fbGetPreviousRecordID.PrevRecordID;
					inst->fbReadEntry.Execute = 1;
					TD_LoggerReadEntry( &inst->fbReadEntry );
					inst->fbGetPreviousRecordID.Execute = 0;
					ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
					inst->step = COPY_ENTRY;
					continue;
				}
				else if( inst->fbGetPreviousRecordID.Error ) {
					if( inst->fbGetPreviousRecordID.StatusID == arEVENTLOG_ERR_RECORDID_INVALID ) {  /* end of logger ? */
						inst->step = READY;
						inst->fbGetPreviousRecordID.Execute = 0;
						ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
					}
					else {
						inst->StatusID = inst->fbGetPreviousRecordID.StatusID;
						inst->step = ERROR_GET_PREVIOUS_RECORD_ID;
					}
				}
			}
			else { /* maximum number of allowed entries reached */
				inst->step = READY;
				inst->fbGetPreviousRecordID.Execute = 0;
				ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
			}
			break;
	
	
			case COPY_ENTRY: /* read all data from entry */		
			if( inst->fbReadEntry.Done ) {
				if( inst->fbReadEntry.Entry.LocalTime < inst->eventTimeMinimum ){ /* is message out of time scope ? */
					inst->fbGetPreviousRecordID.Execute = 0;
					ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
					inst->step = READY;
				}
				else {
					if( isValidEntry(inst) ){ /* does entry fit to the filter ? */
						TD_LoggerEntryType* entry = reinterpret_cast<TD_LoggerEntryType*>(inst->pLoggerEntries);
						if( inst->index > 0 ){ /* only valid if we have two or more entries already */						
							for( int i = (inst->MaxEntries-1); i > inst->index; --i ){  /* shift list and remove oldest entry */
								entry[i] = entry[i-1];
							} 
						}
						entry[inst->index] = inst->fbReadEntry.Entry;
						++inst->index;
						if( ++inst->NbrOfEntries > inst->MaxEntries )
							inst->NbrOfEntries = inst->MaxEntries;
					}
					else  { /* entry does not fit to filter */
						inst->index = inst->index;
					}
					inst->fbGetPreviousRecordID.RecordID = inst->fbReadEntry.RecordID; /* prepare for next call */
					inst->fbGetPreviousRecordID.Execute = 1;
					ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
					inst->fbReadEntry.Execute = 0;
					inst->step = READ_PREVIOUS_ID;
				}
			}
			else if( inst->fbReadEntry.Error ) {
				inst->StatusID = inst->fbReadEntry.StatusID;
				inst->step = ERROR_READ_ENTRY;
			}
			TD_LoggerReadEntry( &inst->fbReadEntry );
			break;
	
	
			case READY: /* done */
			inst->Error = 0;
			inst->Busy = 0;
			inst->Done = 1;
			break;
	
	
			case ERROR_GET_IDENT: /* internal errors */
			case ERROR_GET_PREVIOUS_RECORD_ID:
			case ERROR_GET_LATEST_RECORD_ID:
			case ERROR_READ_ENTRY:
			case ERROR_READ_FIRST_ENTRY:
	
			inst->Error = 1;
			inst->Busy = 0;
			inst->Done = 0;
			break;
	
		} /* end of switch() */
		break; /* exit for */
	} /* end of for() */
}


