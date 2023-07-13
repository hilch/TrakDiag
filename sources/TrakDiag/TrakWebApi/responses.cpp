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
#include "index_html.h"



const char* JavascriptBoolean[] = { "false", "true" };

void SendResponse_GeneralInformation(struct TD_WebServices* inst){
	sprintf( (char*) inst->webData.responseData, "{\"TrakDiag\" : \"" STRINGIFY(_TrakDiag_VERSION) "\","
												 "\"McAcpTrak\" : \"%s\""
												 "}", inst->acpTrakVersion );
	
	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
	std::strcpy( (char*) inst->webData.responseHeader.contentType, "application/json; charset=iso-8859-1");
	inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
	std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
	std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
}



void SendResponse_AssemblyInformation(struct TD_WebServices* inst){
// not available in 5.21
//						int plcOpen = inst->asmInfo.PLCopenState;
//						if( plcOpen > mcACPTRAK_INVALID_CONFIGURATION ){
//							plcOpen = mcACPTRAK_INVALID_CONFIGURATION;
//						}
	sprintf( (char*) inst->webData.responseData, 
			"{"
//								"\"PLCopen\" : \"%s\"," // not available in 5.21
			"\"CommunicationReady\" : %s,"
			"\"ReadyForPowerOn\" : %s,"
			"\"PowerOn\" : %s,"
			"\"SegmentsInDisabledCount\" : %d,"
			"\"SegmentsInStoppingCount\" : %d,"
			"\"SegmentsInErrorStopCount\" : %d,"
			"\"ShuttlesCount\" : %d,"
			"\"ShuttlesInErrorStopCount\" : %d"
			"}", 
//								AcpTrakPLCopenState[plcOpen], // not available in 5.21
			JavascriptBoolean[inst->asmInfo.CommunicationReady],
			JavascriptBoolean[inst->asmInfo.ReadyForPowerOn],
			JavascriptBoolean[inst->asmInfo.PowerOn],
			inst->asmInfo.SegmentsInDisabledCount,
			inst->asmInfo.SegmentsInStoppingCount,
			inst->asmInfo.SegmentsInErrorStopCount,
			inst->asmInfo.ShuttlesCount,
			inst->asmInfo.ShuttlesInErrorStopCount
	);
	
	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
	std::strcpy( (char*) inst->webData.responseHeader.contentType, "application/json; charset=iso-8859-1");
	inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
	std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
	std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
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

	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
	std::strcpy( (char*) inst->webData.responseHeader.contentType, "application/json; charset=iso-8859-1");
	inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
	std::strcpy( (char*) inst->webData.responseHeader.connection, "keep-Alive" );
	std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
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
	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
	std::strcpy( (char*) inst->webData.responseHeader.contentType, "application/json; charset=iso-8859-1");
	inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
	std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
	std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
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
	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
	std::strcpy( (char*) inst->webData.responseHeader.contentType, "application/json; charset=iso-8859-1");
	inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
	std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
	std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
}


void SendResponse_IndexCss(struct TD_WebServices* inst){
	std::sprintf( (char*) inst->webData.responseData, INDEX_CSS, "Assembly" );
	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
	std::strcpy( (char*) inst->webData.responseHeader.contentType, "text/css; charset=iso-8859-1");
	inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
	std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
	std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
}

	
void SendResponse_IndexJs(struct TD_WebServices* inst){
	std::sprintf( (char*) inst->webData.responseData, INDEX_JS, "Assembly" );
	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
	std::strcpy( (char*) inst->webData.responseHeader.contentType, "text/javascript; charset=iso-8859-1");
	inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
	std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
	std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
}


void SendResponse_IndexHtml(struct TD_WebServices* inst){
	std::sprintf( (char*) inst->webData.responseData, INDEX_HTML, "Assembly" );
	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->webData.responseData );
	std::strcpy( (char*) inst->webData.responseHeader.contentType, "text/html; charset=iso-8859-1");
	inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
	std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
	std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
}


void SendResponse_Svgdata(struct TD_WebServices *inst){
	inst->webData.fbHttpService.pResponseData = inst->pDataObject;
	inst->webData.fbHttpService.responseDataLen = std::strlen( (char*) inst->pDataObject );
	std::strcpy( (char*) inst->webData.responseHeader.contentType, "image/svg+xml");
	inst->webData.responseHeader.contentLength = inst->webData.fbHttpService.responseDataLen;
	std::strcpy( (char*) inst->webData.responseHeader.connection, "close" );
	std::strcpy( (char*) inst->webData.responseHeader.keepAlive, "timeout=20, max=5" );
}


void SendResponse_404(struct TD_WebServices* inst){
	std::strcpy( (char*) inst->webData.responseData, "404 - Not Found" );
	std::strcpy( inst->webData.responseHeader.status, "404 Not Found" );
	std::strcpy( inst->webData.responseHeader.contentType, "text/plain; charset=utf-8" );
	inst->webData.responseHeader.contentLength = std::strlen( (char*) inst->webData.responseData );
	inst->webData.fbHttpService.responseDataLen = inst->webData.responseHeader.contentLength;
	std::strcpy( inst->webData.responseHeader.connection, "close" );
	std::strcpy( inst->webData.responseHeader.keepAlive, "" );
}



