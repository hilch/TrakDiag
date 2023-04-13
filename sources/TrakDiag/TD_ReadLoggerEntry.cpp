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

#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "TrakDiag.h"
#ifdef __cplusplus
	};
#endif

unsigned long bur_heap_size = 0xFFFF; 

/* read all data from a logger entry */
void TD_LoggerReadEntry( struct TD_LoggerReadEntry* inst ){
	
	inst->fbRead.RecordID = inst->fbReadObjectID.RecordID = inst->fbReadDescription.RecordID = inst->RecordID;
	inst->fbRead.Ident = inst->fbReadObjectID.Ident = inst->fbReadDescription.Ident = inst->Ident;
	inst->fbRead.Execute = inst->fbReadObjectID.Execute = inst->fbReadDescription.Execute = inst->Execute;

	ArEventLogRead( &inst->fbRead );
	ArEventLogReadObjectID( &inst->fbReadObjectID );
	inst->fbReadDescription.TextBuffer = (UDINT) &inst->Entry.Description;
	inst->fbReadDescription.TextBufferSize = sizeof(inst->Entry.Description);
	ArEventLogReadDescription( &inst->fbReadDescription );

	inst->Error = inst->fbRead.Error || inst->fbReadObjectID.Error || inst->fbReadDescription.Error;
	if( inst->fbRead.Error )
		inst->StatusID = inst->fbRead.StatusID;
	else if( inst->fbReadObjectID.Error )
		inst->StatusID = inst->fbReadObjectID.StatusID;
	else if( inst->fbReadDescription.Error ) {
		if( inst->fbReadDescription.StatusID == arEVENTLOG_ERR_NO_DESCRIPTION ){  /* no description found, but that's ok */
			inst->StatusID = 0;
			inst->Error = 0; 
			inst->fbReadDescription.Done = 1;  /* force done */
			std::strcpy( inst->Entry.Description, "(no description)" );
		}
		else {
			inst->StatusID = inst->fbReadDescription.StatusID;
		}
	}
	else
		inst->StatusID = 0;

	/* convert TimeStamp which is UTC to local time */
	if( inst->fbRead.Done )
	{
		inst->fbUtcToLocalTime.enable = 1;
		inst->fbUtcToLocalTime.DT1 = inst->fbRead.TimeStamp.sec;
		inst->fbUtcToLocalTime.pDTStructure = reinterpret_cast<UDINT>(&inst->LocalTimeDTStructure);	
		UtcDT_TO_LocalDTStructure(&inst->fbUtcToLocalTime);
		if( inst->fbUtcToLocalTime.status == 0 ) {
			inst->fbUtcToLocalTime.enable = 0;
			inst->Entry.LocalTime = DTStructure_TO_DT(inst->fbUtcToLocalTime.pDTStructure);
			inst->UtcToLocalTime_Done = 1;	
		}
	}

	inst->Done = inst->fbRead.Done && inst->fbReadObjectID.Done && inst->fbReadDescription.Done && inst->UtcToLocalTime_Done;
	if( inst->Done ) {
		inst->Entry.RecordID = inst->fbRead.RecordID;
		inst->Entry.OriginRecordID = inst->fbRead.OriginRecordID;
		inst->Entry.EventID = inst->fbRead.EventID;
		inst->Entry.Severity = (inst->fbRead.EventID >> 30 & 0x03);
		inst->Entry.TimeStamp.sec = inst->fbRead.TimeStamp.sec;
		inst->Entry.TimeStamp.nsec = inst->fbRead.TimeStamp.nsec;
		std::strcpy( (char*) inst->Entry.ObjectID, (char*) inst->fbReadObjectID.ObjectID );
	}

	if( !inst->Execute ) {
		inst->Done = 0;
		inst->Error = 0;
		inst->StatusID = 0;
		std::memset( &inst->Entry, 0, sizeof(inst->Entry) );
		inst->UtcToLocalTime_Done = 0;
	}
}