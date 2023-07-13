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

#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "TrakDiag.h"
#ifdef __cplusplus
	};
#endif

#define STR(x) #x
#define STRINGIFY(x) STR(x)

const char* INDEX_CSS= "\xef\xbb\xbf"
"/* created by https://github.com/hilch/TrakDiag V " STRINGIFY(_TrakDiag_VERSION) "- TD_WebServices.cpp */\n"
"created by https://github.com/hilch/TrakDiag V " STRINGIFY(_TrakDiag_VERSION) "- TD_WebServices.cppkeyframes blinkerOffline { 50%% { fill: red; } }\n"
"created by https://github.com/hilch/TrakDiag V " STRINGIFY(_TrakDiag_VERSION) "- TD_WebServices.cppkeyframes blinkerNotReady { 50%% { fill: lightgray; } }\n"
"\n"
".shuttleDisabled {\n"
"\tfill: grey;\n"
"\tstroke: black;\n"
"\tstroke-width: 0.01;\n"
"}\n"
"\n"
".shuttleReady {\n"
"\tfill: white;\n"
"\tstroke: black;\n"
"\tstroke-width: 0.01;\n"
"}\n"
"\n"
".shuttleErrorStop {\n"
"\tfill: red;\n"
"\tstroke: black;\n"
"\tstroke-width: 0.01;\n"
"}\n"
"\n"
".shuttleDeleted {\n"
"\tstroke-width: 0.001;\n"
"\topacity: 10;\n"
"}\n"
"\n"
".highlightShuttle {\n"
"\tstroke: orange !important;\n"
"\tfill: orange !important;\n"
"}\n"
"\n"
".segReady {\n"
"\tfill: lightgray;\n"
"}\n"
"\n"
".segNotReadyForPowerOn {\n"
"\tanimation: blinkerNotReady 2s step-start 0s infinite;\n"
"}\n"
"\n"
".segDisabled {\n"
"\tfill: dimgray;\n"
"}\n"
"\n"
".segOffline {\n"
"\tanimation: blinkerOffline 2s step-start 0s infinite;\n"
"}\n"
"\n"
".highlightSegment {\n"
"\tfill: orange !important;\n"
"}\n"
"\n"
"#hoverInfo { padding: 0px 1em 0px 1em; margin: auto; border: none; background-color: orange;}\n"
"#timeoutBox { position : fixed; top: 0; left : 0; width: 100%%; height: 100%%; border: none; background-color: rgba(0,0,0,0.4); display : none }\n"
"#timeoutBox h2 { padding: 0px 1em 0px 1em; margin: auto; color: red }\n"
"h1,h2,h3,h4,h5,h6 { font-family: \"Segoe UI\",Arial,sans-serif; font-weight: 400; margin: 10px 0 }\n"
"h1 { font-size: 30px }\n"
"h2 { font-size: 24px }\n"
"\n"
;