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
#define READ_ERRORS 20
#define READ_RECORDS 30
#define DONE 100

/* Read last error records of a shuttle given by ID or Axis reference */
void TD_ShuttleErrorTexts(struct TD_ShuttleErrorTexts* inst)
{
	if( inst->Execute ){
		switch( inst->step ){
			case START: /* start */
			inst->Busy = 1;
			inst->StatusID = 0;
			inst->NumberOfRecords = 0;
			inst->fbReadErrorText.Component = 0;
			inst->fbReadErrorText.Enable = false;
			inst->fbReadErrorText.AdvancedParameters.Language = inst->Language;
			inst->fbReadErrorText.AdvancedParameters.ShowInfoSeverity = mcSWITCH_OFF;
			inst->fbReadErrorText.ReadNext = false;
			MC_BR_ReadErrorText( &inst->fbReadErrorText ); /* reset fb */
			if( inst->Axis ){ /* is Axis reference given ? */
				std::memcpy( &inst->axis, inst->Axis, sizeof(McAxisType) );
				inst->fbReadErrorText.Component = reinterpret_cast<McComponentType>(&inst->axis);
				inst->fbReadErrorText.Enable = true;
				MC_BR_ReadErrorText( &inst->fbReadErrorText );
				inst->step = READ_ERRORS;
			}
			else{ /* no Axis reference so use ID instead */
				std::memset( &inst->Records, 0, sizeof(inst->Records) );
				inst->fbAsmGetShuttle.Assembly = inst->Assembly;
				inst->fbAsmGetShuttle.Enable = false;
				inst->fbAsmGetShuttle.Next = false;
				MC_BR_AsmGetShuttle_AcpTrak( &inst->fbAsmGetShuttle ); /* reset fb */
				inst->fbAsmGetShuttle.Enable = true;
				MC_BR_AsmGetShuttle_AcpTrak( &inst->fbAsmGetShuttle ); 
				inst->step = GET_SHUTTLE;
			}		
			break;

			
			case GET_SHUTTLE: /* wait for first shuttle found */
			if( inst->fbAsmGetShuttle.Valid ){

				if( inst->fbAsmGetShuttle.AdditionalInfo.ShuttleID == inst->Index ){ /* shuttle with given index found */
					std::memcpy( &inst->axis, &inst->fbAsmGetShuttle.Axis, sizeof(McAxisType) );
					inst->fbReadErrorText.Component = reinterpret_cast<McComponentType>(&inst->axis);
					inst->fbAsmGetShuttle.Next = false;
					inst->fbAsmGetShuttle.Enable = false;
					MC_BR_AsmGetShuttle_AcpTrak( &inst->fbAsmGetShuttle ); /* reset fb */
					inst->fbReadErrorText.Enable = false; /* reset fb */
					inst->fbReadErrorText.ReadNext = false;
					MC_BR_ReadErrorText( &inst->fbReadErrorText );
					inst->fbReadErrorText.Enable = true;
					MC_BR_ReadErrorText( &inst->fbReadErrorText );
					inst->step = READ_ERRORS;
				}
				else { /*not yet found */
					if( inst->fbAsmGetShuttle.RemainingCount == 0 ){ /* no more shuttles */
						inst->fbReadErrorText.Component = 0;
						inst->fbAsmGetShuttle.Next = false;
						inst->fbAsmGetShuttle.Enable = false;
						MC_BR_AsmGetShuttle_AcpTrak( &inst->fbAsmGetShuttle ); /* reset fb */
						inst->fbReadErrorText.Enable = true;
						MC_BR_ReadErrorText( &inst->fbReadErrorText );
						inst->step = READ_ERRORS;
					}
					else { /* get next shuttle */
						inst->fbAsmGetShuttle.Next = false;
						MC_BR_AsmGetShuttle_AcpTrak( &inst->fbAsmGetShuttle );
						inst->fbAsmGetShuttle.Next = true; 
						MC_BR_AsmGetShuttle_AcpTrak( &inst->fbAsmGetShuttle );
						break;
					}
				}
			}
			else if( inst->fbAsmGetShuttle.Error ){
				inst->StatusID = inst->fbAsmGetShuttle.ErrorID;
				inst->fbAsmGetShuttle.Enable = false;
				MC_BR_AsmGetShuttle_AcpTrak( &inst->fbAsmGetShuttle ); /* reset fb */
				inst->step = 9001;
			}
			else if( inst->fbAsmGetShuttle.Busy ){ /* busy */
				MC_BR_AsmGetShuttle_AcpTrak( &inst->fbAsmGetShuttle );	
			}
			break;


			case READ_ERRORS: /* read error texts */
			if( inst->fbReadErrorText.Valid ){
				inst->NumberOfRecords = inst->fbReadErrorText.NumberOfRecords;
				if( inst->NumberOfRecords > 0 ){
					inst->fbReadErrorText.ReadNext = true;
					MC_BR_ReadErrorText( &inst->fbReadErrorText );
					inst->step = READ_RECORDS;
				}
				else { /* no records */
					inst->fbReadErrorText.Enable = false;
					MC_BR_ReadErrorText( &inst->fbReadErrorText ); /* reset fb */
					inst->step = DONE;
				}
			}
			else if( inst->fbReadErrorText.Error ){
				inst->StatusID = inst->fbReadErrorText.ErrorID;
				inst->fbReadErrorText.Enable = false;
				MC_BR_ReadErrorText( &inst->fbReadErrorText ); /* reset fb */				
				inst->step = 9000;
			}
			else if( inst->fbReadErrorText.Busy ){ /* busy */
				MC_BR_ReadErrorText( &inst->fbReadErrorText );
			}

			break;


			case READ_RECORDS:
			if( inst->fbReadErrorText.ReadDone ){ /* all text read */
				std::memcpy( &inst->Records, &inst->fbReadErrorText.ErrorRecords, sizeof(inst->Records) );
				inst->fbReadErrorText.Enable = false;
				MC_BR_ReadErrorText( &inst->fbReadErrorText ); /* reset fb */
				inst->step = DONE;
			}
			else if( inst->fbReadErrorText.Error ){
				inst->StatusID = inst->fbReadErrorText.ErrorID;
				inst->fbReadErrorText.Enable = false;
				MC_BR_ReadErrorText( &inst->fbReadErrorText ); /* reset fb */				
				inst->step = 9000;
			}
			else if( inst->fbReadErrorText.Busy ){ /* busy */
				MC_BR_ReadErrorText( &inst->fbReadErrorText );
			}
			break;


			case DONE: /* done */
			inst->Busy = false;
			inst->Done = true;
			break;


			case 9000: /* error */
			case 9001:
			inst->Busy = false;
			inst->Error = true;				
			break;

		}
	}
	else {
		inst->step = START;
		inst->Error = false;
		inst->StatusID = 0;
		inst->Busy = false;
		inst->Done = false;
	}
}
