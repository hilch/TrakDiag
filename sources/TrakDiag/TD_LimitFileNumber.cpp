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

#include <bur/plctypes.h>

#ifdef __cplusplus
	extern "C"
	{
#endif
	#include "TrakDiag.h"
#ifdef __cplusplus
	};
#endif

#include <cstring>
#include <cstdio>
#include <regex>
#include <algorithm>

#define STARTUP 						0
#define DIR_OPEN 						10
#define DIR_READ						20
#define CHECK_FILE						30
#define DIR_CLOSE						40
#define DELETE_FILE						50
#define DONE							100

#define INTERNAL_ERROR					9000
#define INTERNAL_ERROR_DIR_OPEN			9001
#define INTERNAL_ERROR_DIR_READ			9002
#define INTERNAL_ERROR_DIR_CLOSE		9003
#define INTERNAL_ERROR_TOO_MANY_FILES   9005
#define INTERNAL_ERROR_BAD_REGEX		9006
#define INTERNAL_ERROR_FILE_DELETE		9007

#define PASS_SEARCH_FILES				1
#define PASS_DELETE_FILES				2


/* Recorder */
void TD_LimitFileNumber(struct TD_LimitFileNumber* inst)
{

	if( inst->Execute ){

		switch( inst->step ){
			case STARTUP:
			if( inst->MaxCount == 0 ){ /* nothing to do */
				inst->Done = true;
				inst->step = DONE;
			}
			else {
				inst->Busy = true;
				inst->Done = false;
				inst->Error = false;
				inst->ErrorID = 0;
				std::strcpy( inst->ErrorFileNamePattern, "" );
	
				inst->fileCount = 0;
				inst->pass = PASS_SEARCH_FILES;			
	
				inst->fbDirOpen.enable = false; /* reset fb */
				DirOpen( &inst->fbDirOpen );
				inst->fbDirOpen.pDevice = (UDINT) inst->FileDeviceName;
				inst->fbDirOpen.pName = (UDINT) inst->DirectoryName;
				inst->fbDirOpen.enable = true;
				DirOpen( &inst->fbDirOpen );
	
				inst->fbDirClose.enable = false; /* reset fb */
				DirClose( &inst->fbDirClose );
	
				inst->fbDirRead.enable = false; /* reset fb */
				DirReadEx( &inst->fbDirRead );
				inst->fbDirRead.pData = (UDINT) &inst->dirEntry;
				inst->fbDirRead.data_len = sizeof( inst->dirEntry );
	
				inst->fbFileDelete.enable = false; /* reset fb */
				inst->fbFileDelete.pDevice = (UDINT) inst->FileDeviceName;
				FileDelete( &inst->fbFileDelete );
	
				inst->step = DIR_OPEN;
			}
			break;

			case DIR_OPEN:
			if( inst->fbDirOpen.status == ERR_OK ){  /* done */
				inst->fbDirRead.ident = inst->fbDirOpen.ident;
				inst->fbDirRead.enable = true;
				inst->fbDirClose.ident = inst->fbDirOpen.ident;
				inst->fbDirOpen.enable = false; /* reset fb */
				DirOpen( &inst->fbDirOpen );
				inst->step = DIR_READ;
			}
			else if( inst->fbDirOpen.status != ERR_FUB_BUSY ){  /* error */
				inst->ErrorID = inst->fbDirOpen.status;
				inst->Error = true;
				inst->fbDirOpen.enable = false; /* reset fb */
				DirOpen( &inst->fbDirOpen );
				inst->step = INTERNAL_ERROR_DIR_OPEN;
			}
			else { /* busy */
				DirOpen( &inst->fbDirOpen );
			}
			break;


			case DIR_READ:
			DirReadEx( &inst->fbDirRead ); /* call fb until fiERR_NO_MORE_ENTRIES is returned */
			if( inst->fbDirRead.status == ERR_OK ){ /* done */
				if( inst->dirEntry.Mode == fiFILE ){

					try {
						std::regex filePattern(inst->FileNamePattern);
						bool match = std::regex_match( (char*) inst->dirEntry.Filename, filePattern );

						if( inst->pass == PASS_SEARCH_FILES && match ){ /* pass1 : collect timestamps */
							inst->timeStamps[inst->fileCount] = inst->dirEntry.Date;
							++inst->fileCount;		
						}
						else if(inst->pass == PASS_DELETE_FILES && match ) { /* pass2 : delete old files */
							++inst->fileCount;
							if( inst->MaxCount > 0 ){
								if( inst->dirEntry.Date < inst->timeStamps[inst->MaxCount-1] ){
									inst->fbFileDelete.pName = (UDINT) inst->dirEntry.Filename;
									inst->fbFileDelete.enable = true; /* delete this file */
									FileDelete( &inst->fbFileDelete );
									inst->step = DELETE_FILE;
								}
							}
						}
						if( inst->fileCount >= (sizeof(inst->timeStamps)/sizeof(DATE_AND_TIME)) ){
							inst->ErrorID = TD_ERR_INTERNAL;
							inst->Error = true;
							inst->fbDirRead.enable = false; /* reset fb */
							inst->step = INTERNAL_ERROR_DIR_READ;
						}
					}
					catch( const std::regex_error& e ){
						inst->ErrorID = TD_ERR_WRONG_PARAM;
						std::strncpy( inst->ErrorFileNamePattern, e.what(), sizeof(inst->ErrorFileNamePattern)-1 );
						inst->Error = true;
						inst->step = INTERNAL_ERROR_BAD_REGEX;
					}					
				}	
			}
			else if( inst->fbDirRead.status == fiERR_NO_MORE_ENTRIES ){
				inst->fbDirRead.enable = false; /* reset fb */
				DirReadEx( &inst->fbDirRead );
				inst->fbDirClose.enable = true;
				DirClose( &inst->fbDirClose );
				inst->step = DIR_CLOSE;
			}
			else if( inst->fbDirRead.status != ERR_FUB_BUSY ){ /* error */
				inst->ErrorID = inst->fbDirRead.status;
				inst->Error = true;
				inst->fbDirRead.enable = false; /* reset fb */
				DirReadEx( &inst->fbDirRead );
				inst->step = INTERNAL_ERROR_DIR_READ;
			}
			break;


			case DIR_CLOSE:
			if( inst->fbDirClose.status == ERR_OK ){ /* done */				
				inst->fbDirClose.enable = false;
				DirClose( &inst->fbDirClose );
				if( inst->pass == PASS_SEARCH_FILES ){
					if( inst->fileCount <= inst->MaxCount ){ /* below MaxCount ==> nothing to do */
						inst->Busy = false;
						inst->Done = true;
						inst->step = DONE;
					}
					else { /* too many files */
						std::sort( inst->timeStamps, inst->timeStamps + sizeof(inst->timeStamps)/sizeof(inst->timeStamps[0]), std::greater<DATE_AND_TIME>() );
						inst->pass = PASS_DELETE_FILES;
						inst->fbDirOpen.enable = true;
						DirOpen( &inst->fbDirOpen );
						inst->fileCount = 0;
						inst->step = DIR_OPEN;			
					}
				}
				else { /* old files were deleted */
					inst->Busy = false;
					inst->Done = true;
					inst->step = DONE;
				}
			}
			else if( inst->fbDirClose.status != ERR_FUB_BUSY ){ /* error */
				inst->ErrorID = inst->fbDirClose.status;
				inst->Error = true;
				inst->fbDirClose.enable = false;
				DirClose( &inst->fbDirClose );
				inst->step = INTERNAL_ERROR_DIR_CLOSE;
			}
			else { /* busy */
				DirClose( &inst->fbDirClose );
			}
			break;

			
			case DELETE_FILE:
			if( inst->fbFileDelete.status == 0 ){ /* done */
				inst->fbFileDelete.enable = false; /* reset fb */
				FileDelete( &inst->fbFileDelete );
				inst->step = DIR_READ;
			}
			else if( inst->fbFileDelete.status != ERR_FUB_BUSY ){ /* error */
				inst->ErrorID = inst->fbFileDelete.status;
				inst->Error = true;
				inst->fbFileDelete.enable = false; /* reset fb */
				FileDelete( &inst->fbFileDelete );
				inst->step = INTERNAL_ERROR_FILE_DELETE;
			}
			else { /* busy */
				FileDelete( &inst->fbFileDelete );
			}
			break;



			case DONE:
			break;
			

			case INTERNAL_ERROR:
			case INTERNAL_ERROR_DIR_OPEN:
			case INTERNAL_ERROR_DIR_READ:
			case INTERNAL_ERROR_DIR_CLOSE:
			case INTERNAL_ERROR_TOO_MANY_FILES:
			case INTERNAL_ERROR_BAD_REGEX:
			case INTERNAL_ERROR_FILE_DELETE:
			break;


		}
	}
	else {
		inst->Busy = false;
		inst->Done = false;
		inst->Error = false;
		inst->ErrorID = 0;
		inst->step = STARTUP;
	}
}
