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




#include "TrakWebApi/responses.h"
#include "TrakWebApi/utils.h"

#define STARTUP					0
#define GET_SEGMENT_INFO		20
#define W_HTTP_REQUESTS			100
#define HTTP_SERV_RESPONSE		110
#define ABORT_RESPONSE			105
#define INFO_SVG_FILE			120
#define CHECK_DATA_OBJ			130
#define CREATE_DATA_OBJ			140
#define DELETE_DATA_OBJ			150
#define OPEN_SVG_FILE			160
#define READ_SVG_DATA			170
#define CLOSE_SVG_FILE			180
#define SEND_SVGDATA			190			
#define FREE_UP_MEMORY			200
#define SHUTTLE_INFO			300
#define SHUTTLE_ERROR_TEXTS		310
#define SEND_SHUTTLE_INFO		320


#define INTERNAL_ERROR						9000
#define INTERNAL_ERROR_SEGMENTINFO			9001
#define INTERNAL_ERROR_PVXGETADR			9002
#define INTERNAL_ERROR_HTTP					9003
#define INTERNAL_ERROR_FILEINFO_SVG			9004
#define INTERNAL_ERROR_FILEOPEN_SVG			9005
#define INTERNAL_ERROR_FILEREAD_SVG			9006
#define INTERNAL_ERROR_FILECLOSE_SVG		9007
#define INTERNAL_ERROR_DATOBJINFO			9008
#define INTERNAL_ERROR_DATOBJDELETE			9009	
#define INTERNAL_ERROR_DATOBJCREATE			9010
#define INTERNAL_ERROR_FREEMEMORY			9011




