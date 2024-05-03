#include <cstdio>
#include <cstring>
#include "Record.h"

/* class Record */
void Record::packTimeStamp( McAcpTrakDateTimeType timestamp ){
	this->timestamp = timestamp;
}

void Record::packShuttleData( UDINT dataAddress, UDINT dataSize, USINT sizeofUserData ){
	unsigned maxShuttles = dataSize / (sizeof(McAcpTrakShuttleData) + sizeofUserData);

	for( unsigned n = 0; n < maxShuttles; ++n ){
		auto *dest = &this->shuttles[n];
		auto source = reinterpret_cast<McAcpTrakShuttleData*>(dataAddress);

		dest->index = source->Index;
		dest->PlcOpenState = source->PLCopenState;
		dest->segmentID = source->SegmentID;
		dest->segmentPosition = source->SegmentPosition;
		dest->bits.active = source->Active;
		dest->bits._virtual = source->Virtual;
		dest->bits.controlled = source->Controlled;

		dataAddress = dataAddress + sizeof(McAcpTrakShuttleData) + sizeofUserData;
	}
}

void Record::packSegmentData( McAcpTrakSegmentData * segmentData, UINT numberOfSegments ){
}

size_t Record::toJavascript( char* s, const size_t maxLength, const unsigned recordNumber  ){
	char buffer[256];
	const char head[]{"%snew Record( new Date(%d, %d, %d, %d, %d, %d, %d), \n\t\t\t\t[ "};

	int totalLength = std::snprintf( buffer, sizeof(buffer), head, recordNumber == 0 ? "\n\t\t\t" : "\n\t\t\t,", 
							this->timestamp.Year, this->timestamp.Month, this->timestamp.Day, 
							this->timestamp.Hour, this->timestamp.Minute, this->timestamp.Second,  
							this->timestamp.Millisec );

	if( static_cast<size_t>(totalLength) <= maxLength )
		std::strcpy( s, buffer );
	else
		return 0;

	for( unsigned n = 0; n < TD_MAX_SUPPORTED_SHUTTLES_ASM; ++n ){
		if( this->shuttles[n].index != 0 ){
			int len = std::snprintf( buffer, sizeof(buffer), "%s new Shuttle( %lu, %d, 0x%x, %i, %lf )", 
											n == 0 ? "" : "\n\t\t\t\t,", 
											this->shuttles[n].index,
											this->shuttles[n].bits.active,
											this->shuttles[n].PlcOpenState,
											this->shuttles[n].segmentID,
											this->shuttles[n].segmentPosition );

			if( (static_cast<size_t>(totalLength) + len) <= maxLength ){
				totalLength += len;
				std::strcat( s, buffer );
			}
			else {
				return 0;
			}
		}
	}

	const char tail[]{" ] )"};
	if( (totalLength + strlen(tail)) <= maxLength ){
		std::strcat( s, tail );
		totalLength += strlen(tail);
		return totalLength;
	}
	return totalLength;
}
