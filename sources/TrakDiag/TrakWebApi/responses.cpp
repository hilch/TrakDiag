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

#include <cstring>
#include <cstdio>
#include <algorithm>

#include "responses.h"
#include "index_css.h"
#include "index_js.h"
#include "panzoom_js.h"
#include "index_html.h"
#include "utils.h"



const char* JavascriptBoolean[] = { "false", "true" };

void SendResponse( struct TD_WebServices* inst, const char *contentType, const char *connection ){
	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
	std::strcpy( (char*) inst->webData.responseHeader.contentType, contentType );
	inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
	std::strcpy( (char*) inst->webData.responseHeader.connection, connection );
	std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
	inst->webData.fbHttpService.send = true;
}


void SendResponse_GeneralInformation(struct TD_WebServices* inst){
	sprintf( (char*) inst->webData.responseData, "{\"TrakDiag\" : \"" STRINGIFY(_TrakDiag_VERSION) "\","
												 "\"McAcpTrak\" : \"%s\""
												 "}", inst->acpTrakVersion );
	SendResponse( inst, "application/json; charset=iso-8859-1" );
}



void SendResponse_AssemblyInformation(struct TD_WebServices* inst){
	sprintf( (char*) inst->webData.responseData, 
			"{"
			"\"CommunicationReady\" : %s,"
			"\"ReadyForPowerOn\" : %s,"
			"\"PowerOn\" : %s,"
			"\"SegmentsInDisabledCount\" : %d,"
			"\"SegmentsInStoppingCount\" : %d,"
			"\"SegmentsInErrorStopCount\" : %d,"
			"\"ShuttlesCount\" : %d,"
			"\"ShuttlesInErrorStopCount\" : %d"
			"}", 
			JavascriptBoolean[inst->asmInfo.CommunicationReady],
			JavascriptBoolean[inst->asmInfo.ReadyForPowerOn],
			JavascriptBoolean[inst->asmInfo.PowerOn],
			inst->asmInfo.SegmentsInDisabledCount,
			inst->asmInfo.SegmentsInStoppingCount,
			inst->asmInfo.SegmentsInErrorStopCount,
			inst->asmInfo.ShuttlesCount,
			inst->asmInfo.ShuttlesInErrorStopCount
	);
	SendResponse( inst, "application/json; charset=iso-8859-1" );
}

void SendResponse_ShuttlePositions(struct TD_WebServices* inst){
	std::strcpy( (char*) inst->webData.responseData, "[" ); /* start JSON */
	size_t lengthOfJSON = 2; 
	char s[256];
	for( unsigned n = 0; n < inst->ShuttleInfo.numberOfEntries; ++n ){
		TD_ServicesShuttleType *shuttle = &inst->ShuttleInfo.shuttle[n];
		/* we return an array of pairs of two UDINT:
			[0] Bit 0-31  : shuttle index
			[1] Bit 0-7   : PLCopen state
			[1] Bit 8     : active shuttle
			[1] Bit 9     : virtual shuttle
			[1] Bit 10-20 : segment index (11 Bit)
			[1] Bit 21-27 : segment position in % (7 Bit)
		*/
		UDINT first = shuttle->index;
		UDINT second = static_cast<UDINT>(shuttle->plcOpenState)
						+ (static_cast<UDINT>(shuttle->flags &0x3) << 8)
						+ (static_cast<UDINT>(shuttle->segmentID & 0x7ff) << 10)
					 	+ (static_cast<UDINT>(shuttle->segmentPosition & 0x7f) << 21); 
		if( !inst->fbAsmReadInfo.AssemblyInfo.PowerOn || (shuttle->flags & 0x01) ){ /* don't return deleted shuttles when assembly is powered on */
			lengthOfJSON += sprintf( s, "%s[%ld,%ld]" , n == 0 ? "" : ",", first, second );
			if( lengthOfJSON < sizeof(inst->webData.responseData) ){
				std::strcat( (char*) inst->webData.responseData, s );
			}
			else break;
		}
	}
	std::strcat( (char*) inst->webData.responseData, "]" ); /* close JSON */
	SendResponse( inst, "application/json; charset=iso-8859-1", "keep-alive" );
}


void SendResponse_SingleShuttleInfo(struct TD_WebServices* inst){
	inst->webData.fbGetParamUrl.enable = false; /* reset fb */
	inst->webData.fbGetParamUrl.pSrc = (UDINT) &inst->webData.uri;
	inst->webData.fbGetParamUrl.pParam = (UDINT) "index";
	inst->webData.fbGetParamUrl.pValue = (UDINT) inst->webData.urlParamBuffer;
	inst->webData.fbGetParamUrl.valueSize = sizeof(inst->webData.urlParamBuffer);
	httpGetParamUrl( &inst->webData.fbGetParamUrl );
	inst->webData.fbGetParamUrl.enable = true;
	do {
		httpGetParamUrl( &inst->webData.fbGetParamUrl );
	} while( inst->webData.fbGetParamUrl.status == ERR_FUB_BUSY );

	if( inst->webData.fbGetParamUrl.status == 0 ){
		unsigned shuttleIndex = std::atoi( (char*) inst->webData.urlParamBuffer );
		TD_ServicesShuttleType *shuttle = 0;
		bool found = false;

		for( unsigned n = 0; n < inst->ShuttleInfo.numberOfEntries; ++n ){
			shuttle = &inst->ShuttleInfo.shuttle[n]; 
			if( shuttleIndex == shuttle->index ){
				found = true;
				break;
			}
		}
		if( found ){ /* shuttle found */

			LREAL segmentLength = 0.0;
			STRING segmentName[32] = {0};
			for( int n = 0; n < inst->SegInfo.numberOfSegments; ++n ){
				if( shuttle->segmentID == inst->SegInfo.segmentInfo[n].ID ){
					segmentLength = inst->SegInfo.segmentInfo[n].Length;
					std::strcpy( segmentName, inst->SegInfo.segmentInfo[n].Name );
					break;
				}
			}

			std::sprintf( inst->webData.responseData, 
				"{ \"result\" : \"ok\", \"index\":%d, \"active\": %s, \"virtual\": %s, \"segmentID\":%d, \"segmentName\":\"%s\", \"segmentPosition\":%f, \"PLCopen\":\"%s\" }", 
				shuttleIndex, shuttle->flags&0x01 ? "true" : "false", shuttle->flags&0x02 ? "true" : "false", shuttle->segmentID, 
				segmentName, shuttle->segmentPosition/100.0*segmentLength, GetAxisPlcOpenStateString(shuttle->plcOpenState) );
		}
		else {
			std::sprintf( inst->webData.responseData, "{ \"result\" : \"shuttle with given index not found\", \"index\":%d }", shuttleIndex );
		}
	}
	else if( inst->webData.fbGetParamUrl.status == httpERR_NOT_FOUND ){
		std::sprintf( inst->webData.responseData, "{ \"result\" : \"GET parameter invalid\" }" );	
	}
	else { /* internal error */
		std::sprintf( inst->webData.responseData, "{ \"result\" : \"%d\" }", inst->webData.fbGetParamUrl.status  );
	}
	inst->webData.fbGetParamUrl.enable = false; /* reset fb */
	httpGetParamUrl( &inst->webData.fbGetParamUrl );

	SendResponse( inst, "application/json; charset=iso-8859-1" );
}



void SendResponse_SegmentList(struct TD_WebServices* inst){
	std::strcpy( (char*) inst->webData.responseData, "[ " ); /* start JSON */
	size_t length = 2;
	for( int n = 0; n < inst->SegInfo.numberOfSegments; ++n ){
		char s[256];
		length += std::sprintf( s, "%s \"name\": \"%s\", \"ID\": %d, \"length\": %01.15lf }", n == 0 ? "{" : ",{", 
				inst->SegInfo.segmentInfo[n].Name,
				inst->SegInfo.segmentInfo[n].ID,
				inst->SegInfo.segmentInfo[n].Length	);
		if( length < sizeof(inst->webData.responseData) ){
			std::strcat( (char*) inst->webData.responseData, s ); /* copy segment info */
		}
		else
			break;
	} 
	std::strcat( (char*) inst->webData.responseData, "]" ); /* close JSON */
	SendResponse( inst, "application/json; charset=iso-8859-1" );
}


void SendResponse_SegmentStatus(struct TD_WebServices* inst){
	std::strcpy( (char*) inst->webData.responseData, "[ " ); /* start JSON */
	size_t length = 2;
	for( int n = 0; n < inst->SegInfo.numberOfSegments; ++n ){
		char s[256];
		unsigned int flags =  0;
		if( inst->fbSegReadInfo[n].Valid ){
			McAcpTrakSegInfoType * i = &inst->fbSegReadInfo[n].SegmentInfo;
			flags =  (!!i->CommunicationReady)
					 | (!!i->ReadyForPowerOn << 1)
					 | (!!i->PowerOn << 2)
					 | (!!i->SegmentEnable << 3); 
		}
			length += static_cast<size_t>(std::sprintf( s, "%s%d", n == 0 ? "" : ",", flags	));
		if( length < sizeof(inst->webData.responseData) ){
			std::strcat( (char*) inst->webData.responseData, s ); /* copy segment info */
		}
		else
			break;
	} 
	std::strcat( (char*) inst->webData.responseData, "]" ); /* close JSON */
	SendResponse( inst, "application/json; charset=iso-8859-1" );
}


void SendResponse_IndexCss(struct TD_WebServices* inst){
	std::sprintf( (char*) inst->webData.responseData, INDEX_CSS, "Assembly" );
	SendResponse( inst, "text/css; charset=iso-8859-1" );
}

	
void SendResponse_IndexJs(struct TD_WebServices* inst){
	std::sprintf( (char*) inst->webData.responseData, INDEX_JS, "Assembly" );
	SendResponse( inst, "text/javascript; charset=iso-8859-1" );
}


void SendResponse_PanZoomJs(struct TD_WebServices* inst){
	std::sprintf( (char*) inst->webData.responseData, PANZOOM_JS, "Assembly" );
	SendResponse( inst, "text/javascript; charset=iso-8859-1" );
}


void SendResponse_IndexHtml(struct TD_WebServices* inst){
	std::sprintf( (char*) inst->webData.responseData, INDEX_HTML, "Assembly" );
	SendResponse( inst, "text/html; charset=iso-8859-1" );
}


void SendResponse_Svgdata(struct TD_WebServices *inst){
	SendResponse( inst, "image/svg+xml" );
	inst->webData.fbHttpService.pResponseData = inst->pDataObject;
	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->pDataObject );
}


void SendResponse_404(struct TD_WebServices* inst){
	std::strcpy( (char*) inst->webData.responseData, "404 - Not Found" );
	std::strcpy( inst->webData.responseHeader.status, "404 Not Found" );
	SendResponse( inst, "text/plain; charset=utf-8" );
}



