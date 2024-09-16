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

#include <bur/plctypes.h>
#include <cstring>
#include <cstdio>
#include <algorithm>


#define STARTUP 0
#define CREATE_NEW_LOG 10
#define DONE 100
#define ERROR_LOGGETIDENT 9000
#define ERROR_LOGWRITE 9001
#define ERROR_LOGCREATE 9002


/* Write into Logger */
void TD_LogWrite(struct TD_LogWrite* inst)
{
	if( inst->Execute ){
		switch( inst->step ){
			case STARTUP: /* start */
			inst->Busy = true;
			inst->ErrorID = 0;
			std::memset( &inst->fbLogGetIdent, 0, sizeof(inst->fbLogGetIdent) );
			std::strcpy( (char*) &inst->fbLogGetIdent.Name, inst->Name );
			inst->fbLogGetIdent.Execute = true;
			ArEventLogGetIdent( &inst->fbLogGetIdent );
			if( inst->fbLogGetIdent.Done ){ /* ArEventLogGetIdent is executed synchronously ! */
				std::memset( &inst->fbLogWrite, 0, sizeof(inst->fbLogWrite) );
				ArEventLogWrite( &inst->fbLogWrite ); /* reset fb */
				inst->fbLogWrite.Ident = inst->fbLogGetIdent.Ident;
				inst->fbLogWrite.EventID = inst->EventID;
				inst->fbLogWrite.OriginRecordID = 0;
				inst->fbLogWrite.AddDataSize = std::min( sizeof(inst->Ascii)-1, 1+std::strlen((char*)&inst->Ascii) );
				inst->fbLogWrite.AddData = (UDINT) &inst->Ascii;
				inst->fbLogWrite.AddDataFormat = arEVENTLOG_ADDFORMAT_TEXT;
				std::strcpy( (char*) &inst->fbLogWrite.ObjectID, (char*) &inst->ObjectID );
				inst->fbLogWrite.TimeStamp = 0;
				inst->fbLogWrite.Execute = true;

				inst->fbLogGetIdent.Execute = 0; /* reset fb */
				ArEventLogGetIdent( &inst->fbLogGetIdent );	

				ArEventLogWrite( &inst->fbLogWrite );
				if( inst->fbLogWrite.Done ){ /* ArEventLogWrite is executed synchronously !  */
					inst->fbLogWrite.Execute = false; /* reset fb */
					ArEventLogWrite( &inst->fbLogWrite );
					inst->Busy = false;
					inst->Done = true;
					inst->step = DONE;
				}
				else if( inst->fbLogWrite.Error ){
					inst->ErrorID = inst->fbLogWrite.StatusID;
					inst->fbLogWrite.Execute = false; /* reset fb */
					ArEventLogWrite( &inst->fbLogWrite );
					inst->step = ERROR_LOGWRITE;
				}
			}
			else if (inst->fbLogGetIdent.Error ) {
				if( inst->fbLogGetIdent.StatusID == arEVENTLOG_ERR_LOGBOOK_NOT_FOUND ){
					std::memset( &inst->fbLogCreate, 0, sizeof(inst->fbLogCreate) );
					inst->fbLogCreate.Execute = true;
					std::strcpy( (char*) inst->fbLogCreate.Name, (char*) inst->Name );
					inst->fbLogCreate.Size = 200 * 1024;
					inst->fbLogCreate.Persistence = arEVENTLOG_PERSISTENCE_PERSIST;
					ArEventLogCreate( &inst->fbLogCreate );
					inst->step = CREATE_NEW_LOG;
				}
				else {
					inst->ErrorID = inst->fbLogGetIdent.StatusID;
					inst->step = ERROR_LOGGETIDENT;
				}
				inst->fbLogGetIdent.Execute = false; /* reset fb */
				ArEventLogGetIdent( &inst->fbLogGetIdent );
			}
			break;


			case CREATE_NEW_LOG: /* create a new log */
			if( inst->fbLogCreate.Done ){
				inst->fbLogCreate.Execute = false;
				ArEventLogCreate( &inst->fbLogCreate );
				inst->step = STARTUP;
			}
			else if( inst->fbLogCreate.Error ){
				inst->ErrorID = inst->fbLogCreate.StatusID;
				inst->fbLogCreate.Execute = false;
				ArEventLogCreate( &inst->fbLogCreate );
				inst->step = ERROR_LOGCREATE;
			}
			else {
				ArEventLogCreate( &inst->fbLogCreate );
			}
			break;
			

			case DONE: /* done */

			break;


			case ERROR_LOGGETIDENT: /* error */
			case ERROR_LOGWRITE:
			case ERROR_LOGCREATE:
			inst->Busy = false;
			inst->Error = true;				
			break;

		}
	}
	else {
		inst->step = STARTUP;
		inst->Error = false;
		inst->Busy = false;
		inst->Done = false;
	}
}
