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

#define STARTUP 0
#define CHECK_SIGNAL 10
#define W_DONE 20
#define ERROR 9999


/* Write contents of STRING variable into log */
void TD_LogWatchSTRING(struct TD_LogWatchSTRING* inst)
{
	if( inst->Enable ){
		for(;;){
			switch( inst->step ){
				case STARTUP: /* startup */
				inst->Busy = false;
				inst->Valid = false;
				inst->Error = false;
				inst->ErrorID = 0;
				std::memset( &inst->fbLogWrite, 0, sizeof(inst->fbLogWrite) );
				std::strcpy( (char*) inst->fbLogWrite.Name, (char*) inst->LogName );
				TD_LogWrite( &inst->fbLogWrite ); /* reset fb */
				std::strcpy( (char*) inst->signalOld , "!" ); /* force write when enabling the fb */
				std::strcat( (char*) inst->signalOld , (char*) inst->Signal );
				inst->step = CHECK_SIGNAL;
				continue; /* save one cycle */
	
	
				case CHECK_SIGNAL: /* check signal */
				inst->Valid = 1;
				if( std::strcmp( (char*) inst->Signal, (char*) inst->signalOld ) ){
					std::strcpy( (char*) inst->signalOld ,(char*) inst->Signal );
					std::strcpy( (char*) inst->fbLogWrite.ObjectID, (char*) inst->SignalName );
					std::strcpy( (char*) inst->fbLogWrite.Ascii, (char*) inst->Signal );
					inst->fbLogWrite.EventID = inst->EventID;
					inst->fbLogWrite.Execute = true;
					TD_LogWrite( &inst->fbLogWrite );
					inst->Busy = true;
					inst->Valid = false;
					inst->step = W_DONE;
				}			
				break;
	
				
				case W_DONE:
				if( inst->fbLogWrite.Done ){
					inst->Busy = false;
					inst->Valid = true;
					inst->fbLogWrite.Execute = false;
					TD_LogWrite( &inst->fbLogWrite );
					inst->step = CHECK_SIGNAL;
					continue; /* save one cycle */
				}
				else if( inst->fbLogWrite.Error ){
					inst->Busy = false;
					inst->Error = true;
					inst->ErrorID = inst->fbLogWrite.ErrorID;
					inst->fbLogWrite.Execute = false;
					TD_LogWrite( &inst->fbLogWrite );
					inst->step = ERROR;				
				}
				else { /* busy */
					TD_LogWrite( &inst->fbLogWrite );
				}
				break;  
	
	
				case ERROR: /* error */
				break;
	
			}

			break; /* leave endless loop */
		} /* end for(;;) */
	}
	else {
		inst->Busy = false;
		inst->Valid = false;
		inst->ErrorID = 0;
		inst->Error = false;
		inst->step = STARTUP;
	}
}
