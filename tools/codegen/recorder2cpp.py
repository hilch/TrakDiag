import argparse


header = '''/*

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

'''


def convert( source : str, output : str ):
    skip = False
    with open( output, "w+") as outfile:
        outfile.write(header)

        with open( source, encoding="utf-8" ) as infile:
            for line in infile:
                if line.startswith('<!-- ### C_COMMENT('):
                    line = line.replace('<!-- ### C_COMMENT(','/* ')
                    line = line.replace(') -->',' */')
                    outfile.write(line)
                    continue
                if line.startswith('<!-- ### HTML_HEADER -->'):
                    outfile.write( 'const char* HTML_HEADER = ""' + '\n')
                    skip = False
                    continue
                if line.startswith('<!-- ### HTML_HEADER_END -->'):
                    outfile.write( 'const char* HTML_HEADER_END = ""' + '\n')
                    skip = False
                    continue
                if line.startswith('<!-- ### HTML_FOOTER -->'):
                    outfile.write( 'const char* HTML_FOOTER = ""' + '\n')
                    skip = False
                    continue
                if line.startswith('<!-- ### SKIP -->'):
                    outfile.write(';' + '\r\n')                    
                    skip = True
                    continue                
                if skip:
                    continue                
                line = line.replace('\n', '') # remove end of line
                line = line.replace( '\\', '\\\\') ## convert backslash
                line = line.replace('"', r'\"') # escape double quotes
                line = line.replace('%', '%%') # escape percentage symbol
                line = line.replace('\t', r'\t') # convert tabulators to tab symbol
                line = line.replace('### TITLE', '%s')
                line = line.replace('### CREATED BY', 'created by https://github.com/hilch/TrakDiag V " STRINGIFY(_TrakDiag_VERSION) " - TD_Recorder.cpp')
                outfile.write( '"' + line + '\\n"\n') # pack into double qoutes (c-string) with newline
        outfile.write(';')


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("-s", "--source", help = "web file (*.*)", required= True )
    parser.add_argument("-o", "--output", help = "output file (*.h)", required = True)
    try:
        args = parser.parse_args()
        convert( args.source, args.output)

    except argparse.ArgumentError:
        print("wrong or missing arguments")
    except SystemExit:
        pass