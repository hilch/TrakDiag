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

#include <ctime>
#include <cstring>

/* returns a filename with DT included */
unsigned short TD_filenameDT( unsigned long prefix, unsigned long pString, unsigned long maxSize)
{
	struct DTGetTime fbGetTime;

	if( prefix == 0 || pString == 0 || maxSize == 0 )
		return 0; /* error */

	fbGetTime.enable = 1;
	DTGetTime( &fbGetTime );
	if( fbGetTime.status == 0 ) {
		char temp[80];
		std::strcpy( reinterpret_cast<char*>(pString), reinterpret_cast<char*>(prefix) ); 
		std::time_t rawtime = fbGetTime.DT1;
		strftime( temp, maxSize, "%Y-%m-%dT%H_%M_%S", std::localtime(&rawtime) );
		std::strcat( reinterpret_cast<char*>(pString), temp );
		return 0; /* ok */
	}
	else
		return fbGetTime.status; 
}
