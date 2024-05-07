#include <bur/plctypes.h>

#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "TrakDiag.h"
#ifdef __cplusplus
	};
#endif

#include <cstddef>

struct ShuttleDataRecordType {
	UDINT index;
	McAxisPLCopenStateEnum PlcOpenState;
	UINT  segmentID;
	LREAL segmentPosition;
	struct {
		bool active : 1;
		bool _virtual : 1;
		bool controlled : 1;
	} bits;
};

struct SegmentDataRecordType {
	UINT id;
	McAcpTrakPLCopenStateEnum PlcOpenState;
	McAcpTrakSegErrorReasonEnum errorReason;
	struct {
		bool communicationReady : 1;
		bool readyForPowerOn : 1;
		bool powerOn : 1;
		bool segmentEnable : 1;
		bool movementDetected : 1;
		bool errorInitiator : 1;
	} bits;
};

class Record {
	public:
	Record( void );
	void packTimeStamp( McAcpTrakDateTimeType timestamp );
	void packShuttleData( UDINT dataAddress, UDINT dataSize, USINT sizeofUserData );
	void packSegmentData( McAcpTrakSegmentData * segmentData, UINT numberOfSegments );
	size_t toJavascript( char* s, const size_t maxLength, const unsigned recordNumber );

	private:
	McAcpTrakDateTimeType m_timestamp;
	ShuttleDataRecordType m_shuttles[TD_MAX_SUPPORTED_SHUTTLES_ASM];
	SegmentDataRecordType m_segmentData[TD_MAX_SUPPORTED_SEGMENTS_ASM];
};