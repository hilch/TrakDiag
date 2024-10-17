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
#include <cstring>

#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "TrakDiag.h"
#ifdef __cplusplus
	};
#endif


/* Simulate Segment Error */
void TD_SegCommandError(struct TD_SegCommandError* inst)
{

	if( !inst->Execute )
		inst->step = 0;


	switch( inst->step ){
		case 0:
		inst->Busy = 0;
		inst->Done = 0;
		inst->Error = 0;
		inst->ErrorID = 0;
		if( inst->Execute ){
			inst->Busy = 1;
			std::memset( &inst->fbSegPowerOn, 0, sizeof(inst->fbSegPowerOn) );
			MC_BR_SegPowerOn_AcpTrak( &inst->fbSegPowerOn ); /* reset fb */

			std::memset( &inst->fbSegCommandError, 0, sizeof(inst->fbSegCommandError) );
			MC_BR_SegCommandError_AcpTrak( &inst->fbSegCommandError ); /* reset fb */
			inst->fbSegCommandError.Segment = inst->Segment;
			switch( inst->Type ){
				case TD_SEGERRCMD_SEGMENT: inst->fbSegCommandError.Command = mcERROR_CMD; break;
				case TD_SEGERRCMD_CHANNEL: inst->fbSegCommandError.Command = mcERROR_CHANNEL_CMD; break;
				case TD_SEGERRCMD_ENCODER: inst->fbSegCommandError.Command = mcERROR_ENCODER_CMD; break;
			}
			inst->fbSegCommandError.Execute = 1;
			MC_BR_SegCommandError_AcpTrak( &inst->fbSegCommandError ); 
			inst->step = 10;
		}
		break;

		
		case 10: /* send error command to segment */
		if( inst->fbSegCommandError.Done ){
			if( inst->fbSegCommandError.Command == mcERROR_CMD ){ /* mcERROR_CMD requires a subsequent command */
				inst->fbSegPowerOn.Segment = inst->Segment;
				inst->fbSegPowerOn.Execute = 1;
				MC_BR_SegPowerOn_AcpTrak( &inst->fbSegPowerOn ); 
				inst->step = 20;	
			}
			else {
				inst->step = 100;
			}
		}
		else if( inst->fbSegCommandError.Error ){
			inst->ErrorID = inst->fbSegCommandError.ErrorID;
			inst->step = 9001;
		}
		else { /* busy */
			MC_BR_SegCommandError_AcpTrak( &inst->fbSegCommandError ); 
		}
		break;


		case 20: /* try to power on segment in errorstop (which will fail!) */
		if( inst->fbSegPowerOn.Done || inst->fbSegPowerOn.Error ){
			inst->step = 100;
		}
		else {  /* busy */
			MC_BR_SegPowerOn_AcpTrak( &inst->fbSegPowerOn ); 
		}
		

		case 100: /* Done */
		inst->Busy = 0;
		inst->Done = 1;
		break;


		case 9001: /* internal errors */
		inst->Busy = 0;
		inst->Error = 1;
		break;


	}
}
