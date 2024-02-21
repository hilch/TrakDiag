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

#define RECORDING_TIME_US		5000000
#define DEFAULT_CYCLE_US		50000
#define BUFFER_SIZE				16384

#define STARTUP					0
#define CHECK_DATA_OBJ			10
#define CREATE_DATA_OBJ			20
#define DELETE_DATA_OBJ			30
#define GET_SEGMENT_LIST		40
#define GET_SEGMENT_INFO		50
#define START_RECORDING			60
#define RECORDING				100
#define CREATE_FILE				200
#define WRITE_HTML_HEADER		210
#define WRITE_SEGMENT_INFO		220
#define COLLECT_SHUTTLE_DATA	230
#define WRITE_SHUTTLE_DATA		240
#define WRITE_HTML_HEADER_END	250
#define OPEN_SVG_FILE			260
#define READ_SVG_DATA			270
#define WRITE_SVG_DATA			280
#define CLOSE_SVG_FILE			290
#define WRITE_HTML_FOOTER		300
#define CLOSE_DEST_FILE			310
#define W_SYSTEMDUMP			320
#define DELETE_OLD_RECORDINGS   330
#define W_RESTART				400


#define INTERNAL_ERROR						9000
#define INTERNAL_ERROR_BUFFER_SIZE			9001
#define INTERNAL_ERROR_PVXGETADR			9002
#define INTERNAL_ERROR_DATOBJINFO			9003
#define INTERNAL_ERROR_DATOBJDELETE			9004	
#define INTERNAL_ERROR_DATOBJCREATE			9005
#define INTERNAL_ERROR_GETSEGMENT			9006
#define INTERNAL_ERROR_COPYSHUTTLEDATA		9007
#define INTERNAL_ERROR_FILECREATE			9008
#define INTERNAL_ERROR_FILEWRITE			9009
#define INTERNAL_ERROR_FILECLOSE_SVG		9010
#define INTERNAL_ERROR_FILECLOSE_DST		9011
#define INTERNAL_ERROR_FILEOPEN				9012
#define INTERNAL_ERROR_FILEREAD				9013
#define INTERNAL_ERROR_SYSTEMDUMP			9014
#define INTERNAL_ERROR_TD_LIMIT_FILE_NUMBER 9015
