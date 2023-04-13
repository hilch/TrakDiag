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

bool isValidEntry( struct TD_Logger* inst ){
			bool validObjectName = true;

			if( std::strstr( inst->fbReadEntry.Entry.ObjectID, "Shuttle") )
				validObjectName = false;

			if( strlen( inst->objectStartsWith1 ) )
				if( !std::strstr( inst->fbReadEntry.Entry.ObjectID, inst->objectStartsWith1) )
					validObjectName = false;

			if( strlen( inst->objectStartsWith2 ) )
				if( !std::strstr( inst->fbReadEntry.Entry.ObjectID, inst->objectStartsWith2) )
					validObjectName = false;

			bool validSeverity = (inst->fbReadEntry.Entry.Severity >= inst->thrSeverity);

			return( validObjectName && validSeverity );
}



/* read motion logger cyclically */
void TD_Logger(struct TD_Logger* inst) {


	if( !inst->Enable ){
		inst->step = 0;
	}
	
	BOOL refreshRequired = false;
	TIME ms = clock_ms();
	if( ms > inst->msOld ){
		if( (ms - inst->msOld) > 5000 ) {
			refreshRequired = true;
			inst->msOld = ms;
		}
	}


	/* cyclically check if there are new entries */
	if( inst->fbGetIdent.Done ) {
		inst->fbGetLatestRecordID.Ident = inst->fbGetIdent.Ident;
		inst->fbGetLatestRecordID.Execute = 1;
		if( inst->fbGetLatestRecordID.Done ) {
			inst->LatestRecordID = inst->fbGetLatestRecordID.RecordID;
			inst->fbGetLatestRecordID.Execute = 0;
		}	
		ArEventLogGetLatestRecordID( &inst->fbGetLatestRecordID );
	}

	switch( inst->step ) {

		case 0:  /* Enable := FALSE */
		inst->Busy = 0;
		inst->Valid = 0;
		inst->Error = 0;
		inst->StatusID = 0;
		inst->fbGetIdent.Execute = 0;
		std::strcpy( (char*) &inst->fbGetIdent.Name, "$motion" );

		inst->fbGetLatestRecordID.Execute = 0;
		inst->fbGetPreviousRecordID.Execute = 0;
		inst->fbReadEntry.Execute = 0;
		if( inst->Enable && inst->pLoggerEntries && (inst->SizeLoggerEntries >= sizeof(TD_LoggerEntryType) ) ) {
			inst->MaxEntries = inst->SizeLoggerEntries / sizeof(TD_LoggerEntryType);
			inst->NbrOfEntries = 0;
			std::memset( reinterpret_cast<TD_LoggerEntryType*>(inst->pLoggerEntries),0, inst->SizeLoggerEntries );  /* clear logger entries */
			inst->fbGetIdent.Execute = 1;
			inst->LatestRecordID_Old = 0;
			inst->Busy = 1;
			inst->step = 10;
		}
		ArEventLogGetIdent( &inst->fbGetIdent );
		ArEventLogGetLatestRecordID( &inst->fbGetLatestRecordID );
		ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
		TD_LoggerReadEntry( &inst->fbReadEntry );
		break;

		case 10: /* Start */
		if( inst->fbGetIdent.Done ) {
			inst->fbGetPreviousRecordID.Ident = inst->fbGetIdent.Ident;
			inst->fbReadEntry.Ident = inst->fbGetIdent.Ident;
			inst->step = 20;
		}
		else if( inst->fbGetIdent.Error ) {
			inst->StatusID = inst->fbGetIdent.StatusID;
			inst->step = 9999;
		}
		break;

		case 20:  /* wait for something happen */
		{
			if( inst->LatestRecordID_Old == 0 && inst->LatestRecordID != 0 )  {  /* no entries ==> first run */
				inst->LatestRecordID_Old = inst->LatestRecordID;
				inst->fbReadEntry.RecordID = inst->LatestRecordID;
				inst->fbReadEntry.Execute = 1;
				TD_LoggerReadEntry( &inst->fbReadEntry );
				inst->NbrOfEntries = 0;
				inst->step = 30;								
			}
			else { /* wait for new entries */
				if( inst->LatestRecordID != inst->LatestRecordID_Old &&  refreshRequired ){
					inst->LatestRecordID_Old = inst->LatestRecordID;
						inst->fbReadEntry.RecordID = inst->LatestRecordID;
						inst->fbReadEntry.Execute = 1;
						TD_LoggerReadEntry( &inst->fbReadEntry );
						inst->index = 0;
						inst->step = 30;				
				}
			}
		}
		break;

		case 30: /* read out all entries from logger which could take some minutes on first run */
		if( inst->fbReadEntry.Done ) {				
			if( isValidEntry(inst) ){
				TD_LoggerEntryType *entry = reinterpret_cast<TD_LoggerEntryType*> (inst->pLoggerEntries); 
				for( int i = (inst->MaxEntries-1); i > inst->index; --i ){  /* shift list and remove oldest entry */
					std::memcpy( &entry[i], &entry[i-1], sizeof(TD_LoggerEntryType) );
				} 
				std::memcpy( &entry[inst->index], &inst->fbReadEntry.Entry, sizeof(TD_LoggerEntryType) );  /* read latest entry */
				if( ++inst->NbrOfEntries > inst->MaxEntries )
					inst->NbrOfEntries = inst->MaxEntries;
				inst->index = 1;
			}
			else { /* entry doesn't fit to the filter -> ignore */
				inst->index = 0;
			}
			inst->fbGetPreviousRecordID.RecordID = inst->LatestRecordID;
			inst->fbGetPreviousRecordID.Execute = 1;
			ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );	
			inst->fbReadEntry.Execute = 0;
			inst->step = 40;
		}
		else if( inst->fbReadEntry.Error ) {
			inst->fbGetPreviousRecordID.RecordID = inst->LatestRecordID;
			inst->fbGetPreviousRecordID.Execute = 1;
			ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );			
			inst->fbReadEntry.Execute = 0;
			inst->step = 40;
		}
		TD_LoggerReadEntry( &inst->fbReadEntry );
		break;

		case 40: /* read previous entry */
		if( inst->index < inst->MaxEntries ) {  /* do we have enough space in entries array ? */
			if( inst->fbGetPreviousRecordID.Done ) {
				inst->fbReadEntry.RecordID = inst->fbGetPreviousRecordID.PrevRecordID;
				inst->fbReadEntry.Execute = 1;
				TD_LoggerReadEntry( &inst->fbReadEntry );
				inst->fbGetPreviousRecordID.Execute = 0;
				ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
				inst->step = 50;
			}
			else if( inst->fbGetPreviousRecordID.Error ) {
				if( inst->fbGetPreviousRecordID.StatusID == arEVENTLOG_ERR_RECORDID_INVALID ) {  /* end of logger ? */
					inst->Valid = 1;
					inst->step = 20;
					inst->fbGetPreviousRecordID.Execute = 0;
					ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
				}
				else {
					inst->StatusID = inst->fbGetPreviousRecordID.StatusID;
					inst->step = 9999;
				}
			}
		}
		else { /* maximum number of allowed entries reached */
			inst->Valid = 1;
			inst->step = 20;
			inst->fbGetPreviousRecordID.Execute = 0;
			ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
		}
		break;

		case 50: /* read all data from entry */		
		if( inst->fbReadEntry.Done ) {
			if( inst->fbReadEntry.Entry.LocalTime < inst->eventTimeMinimum ){ /* is message out of time scope ? */
				inst->Valid = 1;
				inst->step = 20;
				inst->fbGetPreviousRecordID.Execute = 0;
				ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
			}
			else if( isValidEntry(inst) ){ /* does entry fit to the filter ? */		
				TD_LoggerEntryType* entry = reinterpret_cast<TD_LoggerEntryType*>(inst->pLoggerEntries);				
				for( int i = (inst->MaxEntries-1); i > inst->index; --i ){  /* shift list and remove oldest entry */
					std::memcpy( &entry[i], &entry[i-1], sizeof(TD_LoggerEntryType) );
				} 
				std::memcpy( &entry[inst->index], &inst->fbReadEntry.Entry, sizeof(TD_LoggerEntryType) );  /* read entry */
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
			inst->step = 40;
		}
		else if( inst->fbReadEntry.Error ) {
			inst->fbGetPreviousRecordID.RecordID = inst->fbReadEntry.RecordID; /* prepare for next call */
			inst->fbGetPreviousRecordID.Execute = 1;
			ArEventLogGetPreviousRecordID( &inst->fbGetPreviousRecordID );
			inst->fbReadEntry.Execute = 0;
			inst->step = 40;
		}
		TD_LoggerReadEntry( &inst->fbReadEntry );
		break;


		case 9999: /* error */
		inst->Error = 1;
		inst->Busy = 0;
		inst->Valid = 0;
		break;

	}
}


