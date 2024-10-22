#ifndef UTILS_H
#define UTILS_H

void InitalizeInstance(struct TD_WebServices* inst);
unsigned long Djb2(unsigned char *);
bool GetSegmentData(struct TD_WebServices* inst, char* segmentName, UINT* ID, LREAL* position );
void CollectShuttleInfo(struct TD_WebServices* inst);
void CollectAssemblyInformation(struct TD_WebServices* inst);
const char *GetAxisPlcOpenStateString( McAxisPLCopenStateEnum state );

#endif

