#include <cstdio>
#include <cstring>
#include "Record.h"





/* class Record */
Record::Record(){

}

/* store timestamp of cyclic shuttle data in record */
void Record::packTimeStamp( McAcpTrakDateTimeType timestamp ){
	m_timestamp = timestamp;
}

/* store cyclic shuttle data in record */
void Record::packShuttleData( UDINT dataAddress, UDINT dataSize, USINT sizeofUserData ){
	unsigned maxShuttles = dataSize / (sizeof(McAcpTrakShuttleData) + sizeofUserData);

	auto source = reinterpret_cast<McAcpTrakShuttleData*>(dataAddress);

	for( unsigned n = 0; n < maxShuttles; ++n, ++source ){
		auto &dest = m_shuttles[n];
		dest.index = 0;
		dest.PlcOpenState = mcAXIS_STARTUP;
		dest.segmentID = 0;
		dest.bits.active = false;
		dest.bits._virtual = false;
		dest.bits.controlled = false;

		if( source->Index != 0 and source->SegmentID != 0xffff ) {		
			dest.index = source->Index;
			dest.PlcOpenState = source->PLCopenState;
			dest.segmentID = source->SegmentID;
			dest.segmentPosition = source->SegmentPosition;
			dest.bits.active = source->Active;
			dest.bits._virtual = source->Virtual;
			dest.bits.controlled = source->Controlled;
		}
	}
}


/* store cyclic segment data in record */
void Record::packSegmentData( McAcpTrakSegmentData * segmentData, UINT numberOfSegments ){	
	for( unsigned n = 0; n < numberOfSegments; ++n ){
		auto &dest = m_segmentData[n];
		auto &source = segmentData[n];
		dest.id = source.SegmentID;
		dest.PlcOpenState = source.PLCopenState;
		dest.errorReason = source.ErrorReason;
		dest.bits.communicationReady = source.CommunicationReady;
		dest.bits.readyForPowerOn = source.ReadyForPowerOn;
		dest.bits.powerOn = source.PowerOn;
		dest.bits.segmentEnable = source.SegmentEnable;
		dest.bits.movementDetected = source.MovementDetected;
		dest.bits.errorInitiator = source.ErrorInitiator;
	}
	m_segmentData[numberOfSegments].id = 0xffff; /* end */
}


size_t Record::toJavascript( char* s, const size_t maxLength, const unsigned recordNumber  ){
	char buffer[256]{};
	const char head[]{"%snew Record( new Date(%d, %d, %d, %d, %d, %d, %d), \n\t\t\t\t[ "};

	int totalLength = std::snprintf( buffer, sizeof(buffer), head, recordNumber == 0 ? "\n\t\t\t" : "\n\t\t\t,", 
							m_timestamp.Year, m_timestamp.Month, m_timestamp.Day, 
							m_timestamp.Hour, m_timestamp.Minute, m_timestamp.Second,  
							m_timestamp.Millisec );

	if( static_cast<size_t>(totalLength) <= maxLength )
		std::strcpy( s, buffer );
	else
		return 0;

	for( unsigned n = 0; n < TD_MAX_SUPPORTED_SHUTTLES_ASM; ++n ){
		if( m_shuttles[n].index != 0 ){

			int len = std::snprintf( buffer, sizeof(buffer), "%s new Shuttle( %lu, %d, 0x%x, %i, %.3lf )", 
											n == 0 ? "" : "\n\t\t\t\t,", 
											m_shuttles[n].index,
											m_shuttles[n].bits.active,
											m_shuttles[n].PlcOpenState,
											m_shuttles[n].segmentID,
											m_shuttles[n].segmentPosition );

			if( (static_cast<size_t>(totalLength) + len) <= maxLength ){
				totalLength += len;
				std::strcat( s, buffer );
			}
			else {
				return 0;
			}
		}
	}

	const char separator[]{" ],\n\t\t\t\t[ "};
	if( (totalLength + strlen(separator)) <= maxLength ){
		std::strcat( s, separator );
		totalLength += strlen(separator);
	}

	for( unsigned n = 0; n < TD_MAX_SUPPORTED_SEGMENTS_ASM; ++n ){
		auto &sd = m_segmentData[n];
		if( sd.id != 0xffff ){
			UINT flags{};
			flags |= sd.bits.communicationReady ? 0x01 : 0;
			flags |= sd.bits.readyForPowerOn ? 0x02 : 0;
			flags |= sd.bits.powerOn ? 0x04 : 0;
			flags |= sd.bits.segmentEnable ? 0x08 : 0;
			flags |= sd.bits.errorInitiator ? 0x10 : 0;
			flags |= sd.bits.movementDetected ? 0x20 : 0;
			flags |= sd.PlcOpenState == mcACPTRAK_ERRORSTOP ? 0x80 : 0;
			int len = std::snprintf( buffer, sizeof(buffer), "%s new SegmentData( %u, 0x%x, 0x%x, 0x%x )",
											n == 0 ? "" : "\n\t\t\t\t,", 
											sd.id,
											sd.PlcOpenState,
											sd.errorReason,
											flags );
			if( (static_cast<size_t>(totalLength) + len) <= maxLength ){
				totalLength += len;
				std::strcat( s, buffer );
			}
			else {
				return 0;
			}
		}
		else break;
	}

	const char tail[]{" ] )"};
	if( (totalLength + strlen(tail)) <= maxLength ){
		std::strcat( s, tail );
		totalLength += strlen(tail);
		return totalLength;
	}
	return totalLength;
}

