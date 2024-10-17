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
#include <algorithm>

#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "TrakDiag.h"
#ifdef __cplusplus
	};
#endif

#define START 0
#define GET_SEGMENT_LIST 10
#define GET_SEGMENT_INFO 20
#define DONE 100
#define INTERNAL_ERROR_GETSEGMENT 9000
#define INTERNAL_ERROR_SEGGETINFO 9001

/* help structure to sort McSegmentType */
struct Segment { 
	McSegmentType segment;
	UINT id;
};

/* compare function for help structure */
bool compareSegments ( Segment seg1, Segment seg2 ){
	return( seg1.id < seg2.id );
}

/* compare function for McAcpTrakSegGetInfoType */
bool compareSegmentInfo( McAcpTrakSegGetInfoType info1, McAcpTrakSegGetInfoType info2 ){
	return( info1.ID < info2.ID );
}

/* sort Segments and SegmentsInfo */
void sortSegments( struct TD_SegmentsInfo* inst ){
	Segment segments[inst->Count];

	/* copy McSegmentType to help structure */
	for( unsigned n = 0; n < inst->Count; ++n ){
		std::memcpy( &segments[n].segment, &inst->Segments[n], sizeof(McSegmentType) );
		segments[n].id = inst->SegmentsInfo[n].ID;
	}

	/* sort McSegmentTyp by ID */
	std::sort( &segments[0], &segments[inst->Count], compareSegments );

	/* store sorted McSegmentType back in output array */
	for( unsigned n = 0; n < inst->Count; ++n ){
		std::memcpy( &inst->Segments[n], &segments[n].segment, sizeof(McSegmentType) );
	}
	
	/* sort McAcpTrakSegGetInfoType by ID */
	std::sort( &inst->SegmentsInfo[0], &inst->SegmentsInfo[inst->Count], compareSegmentInfo );

}


/* Gets information about all segments in assembly */
void TD_SegmentsInfo(struct TD_SegmentsInfo* inst)
{
	if( inst->Execute ){
		switch( inst->step ){
			case START: /* start */
			inst->Busy = 1;
			inst->ErrorID = 0;
			inst->n = 0;
			inst->fbGetSegment.Assembly = inst->Assembly;
			inst->fbGetSegment.AdvancedParameters.SelectionMode = mcACPTRAK_GET_SEG_ALL;
			inst->fbGetSegment.Next = false;
			inst->fbGetSegment.Enable = false; /* reset fb */
			MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	
			inst->fbSegGetInfo.Execute = false; /* reset fb */
			MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
			inst->fbGetSegment.Enable = true; 
			MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	
			std::memset( (void*) inst->Segments, 0, inst->MaxCount * sizeof(McSectorType) );
			std::memset( (void*) inst->SegmentsInfo, 0, inst->MaxCount * sizeof(McAcpTrakSegGetInfoType) );
			inst->step = GET_SEGMENT_LIST;
			break;


			case GET_SEGMENT_LIST: /* get a list of all segments in this assembly */
			if( inst->fbGetSegment.Valid ){
				inst->Count = inst->fbGetSegment.TotalCount; /* save the number of segments */
				std::memcpy( &inst->Segments[inst->n], &inst->fbGetSegment.Segment, sizeof(McSegmentType) ); /* save segment handle */
				++inst->n;
				if( inst->fbGetSegment.RemainingCount == 0 || /* all segments found */
					 inst->n >= inst->MaxCount || inst->n >= TD_MAX_SUPPORTED_SEGMENTS_ASM ){ // max. permitted number reached 
					inst->fbGetSegment.Next = false;
					inst->fbGetSegment.Enable = false; /* reset fb */
					MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	
					inst->n = 0;
					inst->fbSegGetInfo.Execute = true; /* start fb */
					inst->fbSegGetInfo.Segment = &inst->Segments[inst->n];
					MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
					inst->step = GET_SEGMENT_INFO;
				}
				else { /* find next segment */
						inst->fbGetSegment.Next = false;
						MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	
						inst->fbGetSegment.Next = true;
						MC_BR_AsmGetSegment_AcpTrak( &inst->fbGetSegment );	
				}
			}
			else if( inst->fbGetSegment.Error ){ /* internal error */
				inst->ErrorID = inst->fbGetSegment.ErrorID;
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
				std::memcpy( &inst->SegmentsInfo[inst->n], &inst->fbSegGetInfo.SegmentInfo, sizeof(McAcpTrakSegGetInfoType) ); /* save segment infos */
				inst->fbSegGetInfo.Execute = false; /* reset fb */
				MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
				++inst->n;
				if( inst->n >= inst->Count  || /* there are still some segments left */
					 inst->n >= inst->MaxCount || inst->n >= TD_MAX_SUPPORTED_SEGMENTS_ASM ){ // max. permitted number reached
					inst->fbSegGetInfo.Execute = false; /* reset fb */
					MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
					sortSegments(inst);
					inst->step = DONE;
				}
				else { /* there are still some segments left */
					inst->fbSegGetInfo.Execute = true; /* start fb */
					inst->fbSegGetInfo.Segment = &inst->Segments[inst->n];
					MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
				}
			}
			else if( inst->fbSegGetInfo.Error ){ /* internal error */
				inst->ErrorID = inst->fbSegGetInfo.ErrorID;
				inst->fbSegGetInfo.Execute = false; /* reset fb */
				MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
				inst->step = INTERNAL_ERROR_SEGGETINFO;
			}
			else { /* busy */
				MC_BR_SegGetInfo_AcpTrak( &inst->fbSegGetInfo );
			}
			break;

			case DONE:
			inst->Busy = false;
			inst->Done = true;
			break;

			
			case INTERNAL_ERROR_GETSEGMENT:
			case INTERNAL_ERROR_SEGGETINFO:
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
