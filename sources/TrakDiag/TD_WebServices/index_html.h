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

const char* INDEX_HTML= "\xef\xbb\xbf"
"<!DOCTYPE html><html>\n"
"<head><!-- created by https://github.com/hilch/TrakDiag V " STRINGIFY(_TrakDiag_VERSION) "- TD_WebServices.cpp -->\n"
"\t<title>Assembly</title>\n"
"\t<link rel=\"stylesheet\" href=\"index.css\">\n"
"\t<script src=\"./panzoom.js\"></script>\n"
"\t<script src=\"./index.js\"></script>\n"
"</head>\n"
"\n"
"<body>\n"
"\t<h2 id=\"hoverInfo\" style=\"position:relative;z-index:100\";></h2>\t\n"
"\t<div id=\"svgParent\"></div>\n"
"\t<div id=\"timeoutBox\"><h2>Error: Server is not responding !</h2></div>\n"
"\t<dialog id=\"modalDialog\">\n"
"\t\t<h4 id=\"modalDialogTitle\"></h4>\n"
"\t\t<div id=\"modalDialogContent\"></div>\n"
"\t\t<form method=\"dialog\">\n"
"\t\t\t<button>OK</button>\n"
"\t\t</form>\n"
"\t</dialog>\n"
"</body>\n"
"\n"
"</html>\n"
;