
void SendResponse_GeneralInformation(struct TD_WebServices* inst);
void SendResponse_AssemblyInformation(struct TD_WebServices* inst);
void SendResponse_ShuttlePositions(struct TD_WebServices* inst);
void SendResponse_SegmentList(struct TD_WebServices* inst);
void SendResponse_SegmentStatus(struct TD_WebServices* inst);
void SendResponse_IndexCss(struct TD_WebServices* inst);
void SendResponse_IndexJs(struct TD_WebServices* inst);
void SendResponse_IndexHtml(struct TD_WebServices* inst);
void SendResponse_Svgdata(struct TD_WebServices *inst);
void SendResponse_404(struct TD_WebServices* inst);

const struct ResponseType {
	const char* request;
	void (*send_response)(struct TD_WebServices* inst);
} responses[] = {
	{ "TrakWebApi/info", SendResponse_GeneralInformation },
	{ "TrakWebApi/assembly", SendResponse_AssemblyInformation },
	{ "TrakWebApi/positions", SendResponse_ShuttlePositions },
	{ "TrakWebApi/segments", SendResponse_SegmentList },
	{ "TrakWebApi/segment_status", SendResponse_SegmentStatus },
	{ "TrakWebApi/index.html", SendResponse_IndexHtml },
	{ "TrakWebApi/index.css", SendResponse_IndexCss },
	{ "TrakWebApi/index.js", SendResponse_IndexJs },
	{ 0, SendResponse_404 }
};