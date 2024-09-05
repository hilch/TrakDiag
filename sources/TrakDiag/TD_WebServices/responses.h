
void SendResponse( struct TD_WebServices*, const char *contentType, const char *connection = "close" );
void SendResponse_SvgData(struct TD_WebServices* inst);
void SendResponse_GeneralInformation(struct TD_WebServices* inst);
void SendResponse_AssemblyInformation(struct TD_WebServices* inst);
void SendResponse_ShuttleInformation(struct TD_WebServices* inst);
void SendResponse_ShuttlePositions(struct TD_WebServices* inst);
void SendResponse_SegmentList(struct TD_WebServices* inst);
void SendResponse_SegmentStatus(struct TD_WebServices* inst);
void SendResponse_IndexCss(struct TD_WebServices* inst);
void SendResponse_IndexJs(struct TD_WebServices* inst);
void SendResponse_PanZoomJs(struct TD_WebServices* inst);
void SendResponse_IndexHtml(struct TD_WebServices* inst);
void SendResponse_Svgdata(struct TD_WebServices *inst);
void SendResponse_404(struct TD_WebServices* inst);
void AbortResponse(struct TD_WebServices* inst);


const struct ResponseType {
	const char* request;
	void (*send_response)(struct TD_WebServices* inst);
} responses[] = {
	{ "svgdata", SendResponse_SvgData },
	{ "info", SendResponse_GeneralInformation },
	{ "assembly", SendResponse_AssemblyInformation },
	{ "shuttle", SendResponse_ShuttleInformation },
	{ "positions", SendResponse_ShuttlePositions },
	{ "segments", SendResponse_SegmentList },
	{ "segment_status", SendResponse_SegmentStatus },
	{ "index.html", SendResponse_IndexHtml },
	{ "index.css", SendResponse_IndexCss },
	{ "index.js", SendResponse_IndexJs },
	{ "panzoom.js", SendResponse_PanZoomJs },
	{ 0, SendResponse_404 }
};