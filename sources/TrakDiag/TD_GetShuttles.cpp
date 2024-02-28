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

#define START 0
#define GET_SHUTTLE 10
#define DONE 100
#define INTERNAL_ERROR 9000

/* Returns the axis references of all shuttles of the assembly that corresponds to the selected mode */
void TD_GetShuttles(struct TD_GetShuttles* inst)
{
	if( inst->Execute ){
		switch( inst->step ){
			case START: /* start */
			inst->Busy = 1;
			inst->ErrorID = 0;
			inst->Count = 0;
			std::memset( &inst->Shuttle, 0, sizeof(inst->Shuttle));
			inst->fbGetShuttle.Enable = false; /* reset fb */
			inst->fbGetShuttle.Assembly = inst->Assembly;
			inst->fbGetShuttle.AdvancedParameters.VirtualSelectionMode = mcACPTRAK_GET_SH_VIRT_NONVIRTUAL; /* only non-virtual shuttles */
			inst->fbGetShuttle.AdvancedParameters.SelectionMode = inst->SelectionMode;
			inst->fbGetShuttle.Next = false;
			MC_BR_AsmGetShuttleSel_AcpTrak( &inst->fbGetShuttle );
			inst->fbGetShuttle.Enable = true;
			MC_BR_AsmGetShuttleSel_AcpTrak( &inst->fbGetShuttle );
			inst->step = GET_SHUTTLE;
			break;

			case GET_SHUTTLE:
			if( inst->fbGetShuttle.Valid ){
				if( inst->fbGetShuttle.TotalCount > 0 ) {
					std::memcpy( &inst->Shuttle[inst->Count].Axis, &inst->fbGetShuttle.Axis, sizeof(McAxisType));
					inst->Shuttle[inst->Count].ShuttleID = inst->fbGetShuttle.AdditionalInfo.ShuttleID;
					++inst->Count;
				}
				inst->fbGetShuttle.Next = false;
				MC_BR_AsmGetShuttleSel_AcpTrak( &inst->fbGetShuttle );
				if( inst->fbGetShuttle.RemainingCount > 0 ){
					inst->fbGetShuttle.Next = true; /* get next shuttle */
					MC_BR_AsmGetShuttleSel_AcpTrak( &inst->fbGetShuttle );
				}
				else { /* no more shuttles */
					inst->fbGetShuttle.Enable = false;
					MC_BR_AsmGetShuttleSel_AcpTrak( &inst->fbGetShuttle );
					inst->step = DONE;
				}
			}
			else if( inst->fbGetShuttle.Error ){
				inst->ErrorID = inst->fbGetShuttle.ErrorID;
				inst->fbGetShuttle.Enable = false; /* reset fb */
				inst->fbGetShuttle.Assembly = inst->Assembly;
				inst->step = INTERNAL_ERROR;
			}
			else { /* busy */
				MC_BR_AsmGetShuttleSel_AcpTrak( &inst->fbGetShuttle );
			}
			break;


			case DONE: /* done */
			inst->Busy = false;
			inst->Done = true;
			break;


			case INTERNAL_ERROR: /* error */
			inst->Busy = false;
			inst->Error = true;				
			break;

		}
	}
	else {
		inst->step = START;
		inst->Count = 0;
		inst->Error = false;
		inst->ErrorID = 0;
		inst->Busy = false;
		inst->Done = false;
	}
}
