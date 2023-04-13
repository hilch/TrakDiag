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

#include <bur/plctypes.h>
#include <cstring>
#include <algorithm>

#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "TrakDiag.h"
#ifdef __cplusplus
	};
#endif

/* Write into Logger */
void TD_LogWrite(struct TD_LogWrite* inst)
{
	if( inst->Execute ){
		switch( inst->step ){
			case 0: /* start */
			inst->Busy = 1;
			inst->StatusID = 0;
			std::memset( &inst->fbLogGetIdent, 0, sizeof(inst->fbLogGetIdent) );
			std::strcpy( (char*) &inst->fbLogGetIdent.Name, inst->Name );
			inst->fbLogGetIdent.Execute = 1;
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
				inst->fbLogWrite.Execute = 1;

				inst->fbLogGetIdent.Execute = 0; /* reset fb */
				ArEventLogGetIdent( &inst->fbLogGetIdent );	

				ArEventLogWrite( &inst->fbLogWrite );
				if( inst->fbLogWrite.Done ){ /* ArEventLogWrite is executed synchronously !  */
					inst->fbLogWrite.Execute = 0; /* reset fb */
					ArEventLogWrite( &inst->fbLogWrite );
					inst->Busy = 0;
					inst->Done = 1;
					inst->step = 100;
				}
				else if( inst->fbLogWrite.Error ){
					inst->StatusID = inst->fbLogWrite.StatusID;
					inst->fbLogWrite.Execute = 0; /* reset fb */
					ArEventLogWrite( &inst->fbLogWrite );
					inst->step = 9001;
				}
			}
			else if (inst->fbLogGetIdent.Error ) {
				if( inst->fbLogGetIdent.StatusID == arEVENTLOG_ERR_LOGBOOK_NOT_FOUND ){
					std::memset( &inst->fbLogCreate, 0, sizeof(inst->fbLogCreate) );
					inst->fbLogCreate.Execute = 1;
					std::strcpy( (char*) inst->fbLogCreate.Name, (char*) inst->Name );
					inst->fbLogCreate.Size = 200 * 1024;
					inst->fbLogCreate.Persistence = arEVENTLOG_PERSISTENCE_PERSIST;
					ArEventLogCreate( &inst->fbLogCreate );
					inst->step = 10;
				}
				else {
					inst->StatusID = inst->fbLogGetIdent.StatusID;
					inst->step = 9000;
				}
				inst->fbLogGetIdent.Execute = 0; /* reset fb */
				ArEventLogGetIdent( &inst->fbLogGetIdent );
			}
			break;


			case 10: /* create a new log */
			if( inst->fbLogCreate.Done ){
				inst->fbLogCreate.Execute = 0;
				ArEventLogCreate( &inst->fbLogCreate );
				inst->step = 0;
			}
			else if( inst->fbLogCreate.Error ){
				inst->StatusID = inst->fbLogCreate.StatusID;
				inst->fbLogCreate.Execute = 0;
				ArEventLogCreate( &inst->fbLogCreate );
				inst->step = 9002;
			}
			else {
				ArEventLogCreate( &inst->fbLogCreate );
			}
			break;
			

			case 100: /* done */

			break;


			case 9000: /* error */
			case 9001:
			case 9002:
			inst->Busy = 0;
			inst->Error = 1;				
			break;

		}
	}
	else {
		inst->step = 0;
		inst->Error = 0;
		inst->Busy = 0;
		inst->Done = 0;
	}
}
