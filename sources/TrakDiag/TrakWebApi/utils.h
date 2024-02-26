#ifndef UTILS_H
#define UTILS_H

void InitalizeInstance(struct TD_WebServices* inst);
UDINT Djb2(USINT *str, size_t length );
bool GetSegmentData(struct TD_WebServices* inst, char* segmentName, UINT* ID, LREAL* position );
void CollectShuttleInfo(struct TD_WebServices* inst);
void CollectAssemblyInformation(struct TD_WebServices* inst);
void CollectSegmentInformation(struct TD_WebServices* inst);
const char *GetAxisPlcOpenStateString( McAxisPLCopenStateEnum state );

#endif

