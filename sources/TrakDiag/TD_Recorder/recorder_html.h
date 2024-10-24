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

const char* HTML_HEADER = ""
"<!DOCTYPE html><html>\n"
"<head><!-- created by https://github.com/hilch/TrakDiag V " STRINGIFY(_TrakDiag_VERSION) " - TD_Recorder.cpp -->\n"
"\t<title>%s</title>\n"
"\t<meta name=\"viewport\" content=\"width=device-width, user-scalable=no\">\n"
/*  styles  */
"\t<style type=\"text/css\" media=\"screen\">\n"
"\t\tbody { overflow: hidden; }\n"
"\t\tbutton { width: 40px; height: 40px; border-radius: 5px;}\n"
"\t\t.shuttleDisabled { fill: grey; stroke: black; stroke-width: 0.01; }\n"
"\t\t.shuttleReady {fill: white; stroke: black; stroke-width: 0.01; }\n"
"\t\t.shuttleErrorStop { fill: red; stroke: black; stroke-width: 0.01; }\n"
"\t\t.shuttleDeleted { stroke-width: 0.001; opacity: 10; }\n"
"\t\t.segReady { fill: lightgray; }\n"
"\t\t.segErrorInitiator { fill: red; }\n"
"\t\t.segErrorStop {\tfill: darkred;}\n"
"\t\t.segNotReadyForPowerOn { fill: darkgray; }\n"
"\t\t.segNotEnabled { fill: yellow; }\n"
"\t\t.segDisabled { \tfill: dimgray; }\n"
"\t\t.segOffline { \tfill: magenta; }\n"
"\t\t.segMovementDetected { stroke: green !important; }\t\n"
"\t\t.highlightShuttle { stroke-width: 0.02; }\n"
"\t\t.highlightSegment { fill: #ff8f0f  !important;}\n"
"\t\t#controlContainer { border: 2px solid grey; background-color: lightgray; position: fixed; padding: 5px; border-radius: 5px; bottom: 10px; right: 10px; opacity:70%%;}\n"
"\t\t#sliderTime { width: 100%%; accent-color: #ff8f0f; }\n"
"\t\t#currentTime { display: block; text-align: center;}\n"
"\t\t#dlgHoverInfo { border: 2px solid grey; background-color: lightgray; position: absolute; margin: 0px; opacity:70%%; z-index: 2147483647; }\n"
"\t</style>\n"
/*  scripting  */	
"\t<script>\n"
"\t\t\"use strict\";\n"
/*  class Segment  */		
"\t\tclass Segment {\n"
/*  Segment.constructor  */		
"\t\t\tconstructor( ID, name, length, plcAddress ){\n"
"\t\t\t\tthis.ID = ID;\n"
"\t\t\t\tthis.name = name;\n"
"\t\t\t\tthis.length = length;\n"
"\t\t\t\tthis.plcAddress = plcAddress;\n"
"\t\t\t\tthis.segmentBody = undefined;\n"
"\t\t\t\tthis.segmentBody2 = undefined;\n"
"\t\t\t\tthis.segmentParent = undefined;\n"
"\t\t\t\tthis.arrowElement = undefined;\n"
"\t\t\t\tthis.PLCopen = 5; // startup \n"
"\t\t\t\tthis.errReason = 0; // none\n"
"\t\t\t\tthis.errInitiator = false;\n"
"\t\t\t}\n"
/*  Segment.plcOpenStatus  */			
"\t\t\tplcOpenText() {\n"
"\t\t\t\treturn ['disabled', 'homing', 'ready', 'stopping', 'errorstop', 'startup', 'invalid configuration'][this.PLCopen];\n"
"\t\t\t}\n"
/*  Segment.errorReason  */			
"\t\t\terrorReasonText() {\n"
"\t\t\t\treturn ['none', 'unspecified', 'no gripper', 'command', 'segment', 'assembly', 'channel', 'shuttle unobservable', 'encoder difference'][this.errReason];\n"
"\t\t\t}\n"
/*  Segment.addEvents  */
"\t\t\taddEvents(){\n"
"\t\t\t\tthis.segmentParent.addEventListener('mouseover', (event) => {\n"
"\t\t\t\t\tconst dlg = document.getElementById('dlgHoverInfo');\n"
"\t\t\t\t\tdlg.style.left = event.pageX + 10 + 'px';\n"
"\t\t\t\t\tdlg.style.top = event.pageY + 10 + 'px';\n"
"\t\t\t\t\tlet content = `name: ${this.name}`\n"
"\t\t\t\t\t\t+ '<br />ID: ' + this.ID\n"
"\t\t\t\t\t\t+ '<br />PLC address: ' + this.plcAddress\n"
"\t\t\t\t\t\t+ `<br />length: ${this.length.toPrecision(3)} m`;\n"
"\t\t\t\t\t\t+ '<br />PLCopen: ' + this.plcOpenText();\t\t\t\t\t\t\n"
"\t\t\t\t\tif( this.PLCopen == 4){ // error stop ?\n"
"\t\t\t\t\t\tcontent = content + '<br />Error Reason: ' + this.errorReasonText();\n"
"\t\t\t\t\t\tcontent = content + `<br />Error Initiator: ${this.errInitiator ? 'yes' : 'no'}`;\n"
"\t\t\t\t\t}\n"
"\t\t\t\t\tdlg.innerHTML = content;\n"
"\t\t\t\t\tthis.segmentBody.classList.add('highlightSegment');\n"
"\t\t\t\t\tthis.segmentBody2.classList.add('highlightSegment');\t\t\t\t\t\n"
"\t\t\t\t\tdlg.show();\n"
"\t\t\t\t\tconst svgRect = svgData.getBoundingClientRect();\n"
"\t\t\t\t\tconst dlgRect = dlg.getBoundingClientRect();\n"
"\t\t\t\t\tif( (dlgRect.x + dlgRect.width) > (svgRect.x + svgRect.width/2) ){\n"
"\t\t\t\t\t\tdlg.style.left = (event.pageX - 10 - dlgRect.width) + 'px';\n"
"\t\t\t\t\t}\n"
"\t\t\t\t\tif( (dlgRect.y + dlgRect.height) > (svgRect.y + svgRect.height/2) ){\n"
"\t\t\t\t\t\tdlg.style.top = (event.pageY - 10 - dlgRect.height) + 'px';\n"
"\t\t\t\t\t}\n"
"\t\t\t\t});\n"
"\t\t\t\tthis.segmentParent.addEventListener('mouseout', (event) => {\n"
"\t\t\t\t\tconst dlg = document.getElementById('dlgHoverInfo');\n"
"\t\t\t\t\tthis.segmentBody.classList.remove('highlightSegment');\n"
"\t\t\t\t\tthis.segmentBody2.classList.remove('highlightSegment');\t\t\t\t\t\n"
"\t\t\t\t\tdlg.close();\n"
"\t\t\t\t});\n"
"\t\t\t}\n"
/*  Segment.findElements  */
"\t\t\tfindElements( container ){ /* >= 5.26 */\n"
"\t\t\t\tthis.segmentBody = container.querySelector(`#pgsg_${this.name}`);\n"
"\t\t\t\tthis.segmentBody.style.fill = '';\n"
"\t\t\t\tthis.segmentBody2 = this.segmentBody.nextElementSibling;\n"
"\t\t\t\tthis.segmentBody2.style.fill = '';\n"
"\t\t\t\tthis.segmentParent = container.querySelector(`#gsg_${this.name}`);\n"
"\t\t\t\tthis.pathElement = container.querySelector(`#plsg_${this.name}`);\n"
"\t\t\t\tthis.addEvents();\n"
"\t\t\t}\n"
/*  Segment.xy  */
"\t\t\txy( segmentPosition ){ /* get coords */\n"
"\t\t\t\treturn this.pathElement.getPointAtLength(this.linePosition( segmentPosition ) );\n"
"\t\t\t}\n"
/*  Segment.linePosition  */
"\t\t\tlinePosition( segmentPosition ){ /* calc pos on line */\n"
"\t\t\t\tconst relpos = segmentPosition / this.length;\n"
"\t\t\t\treturn relpos * this.pathElement.getTotalLength();\n"
"\t\t\t}\n"
/*  Segment.setStatus  */
"\t\t\tsetStatus( flags, po, er ){\n"
"\t\t\t\tthis.PLCopen = po;\n"
"\t\t\t\tthis.errReason = er;\n"
"\t\t\t\tconst commReady = !!(flags &0x01);\n"
"\t\t\t\tconst readyForPowerOn = !!(flags & 0x02);\n"
"\t\t\t\tconst power = !!(flags & 0x04);\n"
"\t\t\t\tconst enable = !!(flags & 0x08);\n"
"\t\t\t\tthis.errInitiator = !!(flags & 0x10);\n"
"\t\t\t\tconst movementDetected = !!(flags & 0x20);\t\t\n"
"\t\t\t\tconst errorStop = !!(flags & 0x80);\n"
"\t\t\t\tthis.segmentBody.classList.remove('segReady', 'segNotReadyForPowerOn', 'segDisabled', 'segOffline', 'segErrorStop', 'segErrorInitiator', 'segNotEnabled' );\n"
"\t\t\t\tthis.segmentBody2.classList.remove('segReady', 'segNotReadyForPowerOn', 'segDisabled', 'segOffline', 'segErrorStop', 'segErrorInitiator', 'segNotEnabled' );\n"
"\t\t\t\tif( commReady ){\n"
"\t\t\t\t\tif( errorStop ){\n"
"\t\t\t\t\t\tif( this.errInitiator) {\n"
"\t\t\t\t\t\t\tthis.segmentBody.classList.add('segErrorInitiator');\n"
"\t\t\t\t\t\t\tthis.segmentBody2.classList.add('segErrorInitiator');\n"
"\t\t\t\t\t\t}\n"
"\t\t\t\t\t\telse {\n"
"\t\t\t\t\t\t\tthis.segmentBody.classList.add('segErrorStop');\n"
"\t\t\t\t\t\t\tthis.segmentBody2.classList.add('segErrorStop');\n"
"\t\t\t\t\t\t}\n"
"\t\t\t\t\t}\n"
"\t\t\t\t\telse if( readyForPowerOn ){\n"
"\t\t\t\t\t\tif( power ){\n"
"\t\t\t\t\t\t\tthis.segmentBody.classList.add('segReady');\n"
"\t\t\t\t\t\t\tthis.segmentBody2.classList.add('segReady');\n"
"\t\t\t\t\t\t}\n"
"\t\t\t\t\t\telse {\n"
"\t\t\t\t\t\t\tthis.segmentBody.classList.add('segDisabled');\n"
"\t\t\t\t\t\t\tthis.segmentBody2.classList.add('segDisabled');\n"
"\t\t\t\t\t\t}\n"
"\t\t\t\t\t}\n"
"\t\t\t\t\telse { /* not ready for power on */\n"
"\t\t\t\t\t\tif( enable ){\n"
"\t\t\t\t\t\t\tthis.segmentBody.classList.add('segNotReadyForPowerOn');\n"
"\t\t\t\t\t\t\tthis.segmentBody2.classList.add('segNotReadyForPowerOn');\n"
"\t\t\t\t\t\t}\n"
"\t\t\t\t\t\telse {\n"
"\t\t\t\t\t\t\tthis.segmentBody.classList.add('segNotEnabled');\n"
"\t\t\t\t\t\t\tthis.segmentBody2.classList.add('segNotEnabled');\n"
"\t\t\t\t\t\t}\n"
"\t\t\t\t\t}\n"
"\t\t\t\t}\n"
"\t\t\t\telse {\n"
"\t\t\t\t\tthis.segmentBody.classList.add('segOffline');\n"
"\t\t\t\t\tthis.segmentBody2.classList.add('segOffline');\n"
"\t\t\t\t}\n"
"\t\t\t\tif( movementDetected ){\n"
"\t\t\t\t\tthis.pathElement.classList.add('segMovementDetected');\n"
"\t\t\t\t}\n"
"\t\t\t\telse {\n"
"\t\t\t\t\tthis.pathElement.classList.remove('segMovementDetected');\t\t\t\t\t\n"
"\t\t\t\t}\n"
"\t\t\t}\t\t\t\t\n"
/*  Segment.paintShuttle  */
"\t\t\tpaintShuttle( svg ){\n"
"\t\t\t\tdocument.getElementById('shuttles').appendChild( svg );\n"
"\t\t\t}\t\t\n"
"\t\t}\n"
"\n"
/*  class SegmentData  */		
"\t\tclass SegmentData {\n"
/*  SegmentData.constructor  */
"\t\t\tconstructor( id, PLCopen, errReason, flags ){\n"
"\t\t\t\tthis.id = id;\n"
"\t\t\t\tthis.PLCopen = PLCopen;\n"
"\t\t\t\tthis.errReason = errReason;\n"
"\t\t\t\tthis.flags = flags;\n"
"\t\t\t}\n"
/*  SegmentData.paint  */
"\t\t\tpaint(){\n"
"\t\t\t\tsegment[this.id].setStatus( this.flags, this.PLCopen, this.errReason );\n"
"\t\t\t}\n"
"\t\t}\n"
/*  class Shuttle  */
"\t\tclass Shuttle {\n"
/*  Shuttle.constructor  */
"\t\t\tconstructor( index, active, PLCopen, segmentIndex, segmentPosition ){\n"
"\t\t\t\tthis.index = index;\n"
"\t\t\t\tthis.active = active;\n"
"\t\t\t\tthis.PLCopen = PLCopen;\n"
"\t\t\t\tthis.segmentIndex = segmentIndex;\n"
"\t\t\t\tthis.segmentPosition = segmentPosition;\n"
"\t\t\t}\n"
/*  Shuttle.plcOpenStatus  */			
"\t\t\tplcOpenStatus() {\n"
"\t\t\t\treturn ['disabled', 'standstill', 'homing', 'stopping', 'discrete motion', 'continous motion',\n"
"\t\t\t\t\t'synchronized motion', 'error stop', 'startup', 'invalid configuration'][this.PLCopen];\n"
"\t\t\t}\n"
/*  Shuttle.createSVG  */			
"\t\t\tcreateSVG(){\n"
"\t\t\t\tconst { x, y }= segment[this.segmentIndex].xy(this.segmentPosition);\n"
"\t\t\t\tthis.svg = document.createElementNS('http://www.w3.org/2000/svg', 'circle');\n"
"\t\t\t\tthis.svg.classList.add('shuttle');\n"
"\t\t\t\tthis.svg.classList.add('shuttleId_' + this.index);\n"
"\t\t\t\tthis.svg.setAttribute('shuttle-id', this.index);\n"
"\t\t\t\tthis.svg.setAttribute('r', '0.024');\n"
"\t\t\t\tthis.svg.setAttribute('cx', x);\n"
"\t\t\t\tthis.svg.setAttribute('cy', y);\n"
"\t\t\t\tswitch (this.PLCopen) {\n"
"\t\t\t\t\tcase 0: /* disabled */\n"
"\t\t\t\t\tcase 8: /* startup */\n"
"\t\t\t\t\t\tsvg.classList.add('shuttleDisabled');\n"
"\t\t\t\t\t\tbreak;\n"
"\n"
"\t\t\t\t\tcase 1: /* standstill */\n"
"\t\t\t\t\tcase 2: /* homing */\n"
"\t\t\t\t\tcase 3: /* stopping */\n"
"\t\t\t\t\tcase 4: /* discrete motion */\n"
"\t\t\t\t\tcase 5: /* continuous motion */\n"
"\t\t\t\t\tcase 6: /* synchronized motion */\n"
"\t\t\t\t\t\tthis.svg.classList.add('shuttleReady');\n"
"\t\t\t\t\t\tbreak;\n"
"\n"
"\t\t\t\t\tcase 7: /* errorstop */\n"
"\t\t\t\t\tcase 9: /* invalid configuration */\n"
"\t\t\t\t\t\tthis.svg.classList.add('shuttleErrorStop');\n"
"\t\t\t\t\t\tbreak;\n"
"\t\t\t\t}\n"
"\t\t\t\tif( !this.active ) this.svg.classList.add('shuttleDeleted');\n"
"\t\t\t\tthis.svg.addEventListener('mouseover', (event) => {\n"
"\t\t\t\t\tconst dlg = document.getElementById('dlgHoverInfo');\n"
"\t\t\t\t\tdlg.style.left = event.pageX + 10 + 'px';\n"
"\t\t\t\t\tdlg.style.top = event.pageY + 10 + 'px';\n"
"\t\t\t\t\tconst deleted = !this.active ? 'deleted shuttle !' + '<br />' : '';\n"
"\t\t\t\t\tdlg.innerHTML = deleted + 'index: ' + this.index + '<br />' + 'PLCopen: ' + this.plcOpenStatus() + '<br />'\n"
"\t\t\t\t\t\t+ 'segment: ' + segment[this.segmentIndex].name + '<br />'\n"
"\t\t\t\t\t\t+ 'segment-position: ' + this.segmentPosition.toFixed(3);\n"
"\t\t\t\t\tthis.svg.classList.add('highlightShuttle');\n"
"\t\t\t\t\tdlg.show();\n"
"\t\t\t\t\tconst svgRect = svgData.getBoundingClientRect();\n"
"\t\t\t\t\tconst dlgRect = dlg.getBoundingClientRect();\n"
"\t\t\t\t\tif( (dlgRect.x + dlgRect.width) > (svgRect.x + svgRect.width/2) ){\n"
"\t\t\t\t\t\tdlg.style.left = (event.pageX - 10 - dlgRect.width) + 'px';\n"
"\t\t\t\t\t}\n"
"\t\t\t\t\tif( (dlgRect.y + dlgRect.height) > (svgRect.y + svgRect.height/2) ){\n"
"\t\t\t\t\t\tdlg.style.top = (event.pageY - 10 - dlgRect.height) + 'px';\n"
"\t\t\t\t\t}\n"
"\t\t\t\t\tevent.stopPropagation();\n"
"\t\t\t\t});\n"
"\t\t\t\tthis.svg.addEventListener('mouseout', (event) => {\n"
"\t\t\t\t\tconst dlg = document.getElementById('dlgHoverInfo');\n"
"\t\t\t\t\tthis.svg.classList.remove('highlightShuttle');\n"
"\t\t\t\t\tdlg.close();\n"
"\t\t\t\t});\n"
"\t\t\t}\n"
/*  Shuttle.paint  */			
"\t\t\tpaint(){\n"
"\t\t\t\tsegment[this.segmentIndex].paintShuttle(this.svg);\n"
"\t\t\t}\n"
"\t\t}\n"
"\n"
/*  class Record  */
"\t\tclass Record {\n"
/*  Record.constructor  */
"\t\t\tconstructor( timestamp, shuttles, segmentData ){\n"
"\t\t\t\tthis.timestamp = timestamp;\n"
"\t\t\t\tthis.shuttles = shuttles;\n"
"\t\t\t\tthis.segmentData = segmentData;\n"
"\t\t\t}\n"
/*  Record.getTime  */
"\t\t\tgetTime() {\n"
"\t\t\t\treturn this.timestamp.toISOString();\n"
"\t\t\t}\n"
/*  Record.createSVG  */
"\t\t\tcreateSVG() {\n"
"\t\t\t\tthis.shuttles.forEach(s=>s.createSVG());\n"
"\t\t\t}\n"
/*  Record.paint  */
"\t\t\tpaint(){\n"
"\t\t\t\tthis.shuttles.forEach( s => s.paint() );\n"
"\t\t\t\tthis.segmentData.forEach( d => d.paint() );\n"
"\t\t\t}\n"
"\t\t}\n"
/*  class Transformation  */
"\t\tclass Transformation {\n"
/*  Transformation.constructor  */
"\t\t\tconstructor() {\n"
"\t\t\t\tthis.svg = document.getElementById(\"svgData\").querySelector(\"svg\");\n"
"\t\t\t\tthis.svg.onwheel = this.mouseWheel;\t\t\t\t\n"
"\t\t\t\tdocument.getElementById(\"btnLeft\").onclick = () => this.pan(\"left\");\t\t\t\t\n"
"\t\t\t\tdocument.getElementById(\"btnRight\").onclick = () => this.pan(\"right\");\n"
"\t\t\t\tdocument.getElementById(\"btnUp\").onclick = () => this.pan(\"up\");\n"
"\t\t\t\tdocument.getElementById(\"btnDown\").onclick = () => this.pan(\"down\");\n"
"\t\t\t\tdocument.getElementById(\"btnRotate\").onclick = () => this.rotate();\n"
"\t\t\t\tdocument.getElementById(\"btnZoomIn\").onclick = () => this.zoom(\"in\");\n"
"\t\t\t\tdocument.getElementById(\"btnZoomOut\").onclick = () => this.zoom(\"out\");\n"
"\t\t\t}\n"
"\n"
/*  Transformation.mouseWheel  */
"\t\t\tmouseWheel = ( event ) => {\n"
"\t\t\t\tlet { scale, x, y, rotate } = this.getTransformParameters();\n"
"\t\t\t\tif( !event.ctrlKey ){\n"
"\t\t\t\t\tif( event.shiftKey ){\n"
"\t\t\t\t\t\tif ( event.deltaY > 0 ){\n"
"\t\t\t\t\t\t\ty += 3;\n"
"\t\t\t\t\t\t} else if( event.deltaY < 0 && scale > 0.1 ){\n"
"\t\t\t\t\t\t\ty -= 3;\n"
"\t\t\t\t\t\t}\t\t\t\t\n"
"\t\t\t\t\t}\n"
"\t\t\t\t\telse if( event.altKey ){\n"
"\t\t\t\t\t\tif ( event.deltaY > 0 ){\n"
"\t\t\t\t\t\t\tx += 3;\n"
"\t\t\t\t\t\t} else if( event.deltaY < 0 && scale > 0.1 ){\n"
"\t\t\t\t\t\t\tx -= 3;\n"
"\t\t\t\t\t\t}\t\t\t\t\t\t\t\t\t\n"
"\t\t\t\t\t}\n"
"\t\t\t\t\telse {\n"
"\t\t\t\t\t\tif ( event.deltaY > 0 ){\n"
"\t\t\t\t\t\t\tscale += 0.1;\n"
"\t\t\t\t\t\t} else if( event.deltaY < 0 && scale > 0.1 ){\n"
"\t\t\t\t\t\t\tscale -= 0.1;\n"
"\t\t\t\t\t\t}\n"
"\t\t\t\t\t}\n"
"\t\t\t\t}\n"
"\t\t\t\tthis.svg.style.transform = this.getTransformString(scale, x, y, rotate );\n"
"\t\t\t}\n"
"\t\t\t\n"
/*  Transformation.getTransformParameter  */
"\t\t\tgetTransformParameters = () => {\n"
"\t\t\t\tconst transform = this.svg.style.transform;\n"
"\t\t\t\tlet scale = 1, x = 0, y = 0, rotate = 0;\n"
"\n"
"\t\t\t\tif (transform.includes(\"scale\"))\n"
"\t\t\t\t\tscale = parseFloat(transform.slice(transform.indexOf(\"scale\") + 6));\n"
"\t\t\t\tif (transform.includes(\"translateX\"))\n"
"\t\t\t\t\tx = parseInt(transform.slice(transform.indexOf(\"translateX\") + 11));\n"
"\t\t\t\tif (transform.includes(\"translateY\"))\n"
"\t\t\t\t\ty = parseInt(transform.slice(transform.indexOf(\"translateY\") + 11));\n"
"\t\t\t\tif (transform.includes(\"rotate\"))\n"
"\t\t\t\t\trotate = parseFloat(transform.slice(transform.indexOf(\"rotate\") + 7))                                                                                                                                                                                                                                                                                                                                                                       ;\n"
"\n"
"\t\t\t\treturn { scale, x, y, rotate };\n"
"\t\t\t};\n"
/*  Transformation.getTransformString  */
"\t\t\tgetTransformString = (scale, x, y, rotate ) =>\n"
"\t\t\t\t\"scale(\" + scale + \") \" + \"translateX(\" + x + \"%%) translateY(\" + y + \"%%) rotate(\" + rotate + \"deg)\";\n"
/*  Transformation.pan  */
"\t\t\tpan = (direction) => {\n"
"\t\t\t  const { scale, x, y, rotate } = this.getTransformParameters();\n"
"\t\t\t  let dx = 0,\n"
"\t\t\t\tdy = 0;\n"
"\t\t\t  switch (direction) {\n"
"\t\t\t\tcase \"left\":\n"
"\t\t\t\t  dx = -3;\n"
"\t\t\t\t  break;\n"
"\t\t\t\tcase \"right\":\n"
"\t\t\t\t  dx = 3;\n"
"\t\t\t\t  break;\n"
"\t\t\t\tcase \"up\":\n"
"\t\t\t\t  dy = -3;\n"
"\t\t\t\t  break;\n"
"\t\t\t\tcase \"down\":\n"
"\t\t\t\t  dy = 3;\n"
"\t\t\t\t  break;\n"
"\t\t\t  }\n"
"\t\t\t  this.svg.style.transform = this.getTransformString(scale, x + dx, y + dy, rotate );\n"
"\t\t\t};\n"
/*  Transformation.zoom  */
"\t\t\tzoom = (direction) => {\n"
"\t\t\t  const { scale, x, y, rotate } = this.getTransformParameters();\n"
"\t\t\t  let dScale = 0.1;\n"
"\t\t\t  if (direction == \"out\") dScale *= -1;\n"
"\t\t\t  if (scale == 0.1 && direction == \"out\") dScale = 0;\n"
"\t\t\t  this.svg.style.transform = this.getTransformString(scale + dScale, x, y, rotate );\n"
"\t\t\t};\n"
/*  Transformation.zoomByWheel  */
"\t\t\tzoomByWheel = ( event ) => {\n"
"\t\t\t\tlet { scale, x, y, rotate } = this.getTransformParameters();\n"
"\t\t\t\tif( !event.ctrlKey ){\n"
"\t\t\t\t\tif( event.deltaY > 0 ){\n"
"\t\t\t\t\t\tscale += 0.1;\n"
"\t\t\t\t\t} else if( event.deltaY < 0 && scale > 0.1 ){\n"
"\t\t\t\t\t\tscale -= 0.1;\n"
"\t\t\t\t\t}\n"
"\t\t\t\t}\n"
"\t\t\t\tthis.svg.style.transform = this.getTransformString(scale, x, y, rotate );\n"
"\t\t\t}\t\t\t\n"
/*  Transformation.rotate  */
"\t\t\trotate = () => {\n"
"\t\t\t\tconst { scale, x, y, rotate } = this.getTransformParameters();\n"
"\t\t\t\tthis.svg.style.transform = this.getTransformString(scale, x, y, rotate + 90);\n"
"\t\t\t}\n"
"\t\t\n"
"\t\t}\n"
;
const char* HTML_HEADER_END = ""
/*  remove shuttles  */
"\t\tconst removeAllShuttles = () => {\n"
"\t\t\tconst shuttles = this.shuttles.querySelectorAll('.shuttle');\n"
"\t\t\tif( shuttles.length != 0 ){\n"
"\t\t\t\tshuttles.forEach((s) => s.parentNode.removeChild(s));\n"
"\t\t\t}\t\t\n"
"\t\t}\n"
/*  slider event  */
"\t\tconst sliderChanged = ( event ) => {\n"
"\t\t\tremoveAllShuttles();\n"
"\t\t\tconst rec = record[record.length- 1 - parseInt(event.target.value)];\n"
"\t\t\trec.paint();\n"
"\t\t\tthis.currentTime.innerHTML = rec.getTime();\n"
"\t\t}\t\n"
/*  play  */
"\t\tconst play = () => {\n"
"\t\t\tconst interval = record[0].timestamp - record[1].timestamp;\n"
"\t\t\tlet r = record.length -1;\n"
"\t\t\tthis.sliderTime.value = this.sliderTime.min;\n"
"\t\t\tconst paint = () => {\n"
"\t\t\t\tremoveAllShuttles();\n"
"\t\t\t\tconst rec = record[r];\n"
"\t\t\t\tthis.sliderTime.value = parseInt(this.sliderTime.max) - r;\n"
"\t\t\t\trec.paint()\n"
"\t\t\t\tthis.currentTime.innerHTML = rec.getTime();\n"
"\t\t\t\tif( r ){\n"
"\t\t\t\t\t--r;\n"
"\t\t\t\t\tsetTimeout( paint, interval);\n"
"\t\t\t\t}\n"
"\t\t\t}\n"
"\t\t\tsetTimeout( paint, interval);\n"
"\t\t}\n"
/*  load document  */
"\t\tconst load = () => {\n"
"\t\t\tconst container = workspace.parentElement;\n"
"\t\t\tconst segmentObjects = container.querySelector('#segments');\n"
"\t\t\tsegmentObjects.querySelector('#sg_legend').remove();\n"
"\t\t\tcontainer.querySelector('#sectors').remove();\n"
"\t\t\tcontainer.querySelector('#barriers').remove();\n"
"\t\t\tcontainer.querySelector('#processpoints').remove();\n"
"\t\t\tcontainer.querySelector('#workspace').remove();\n"
"\t\t\tsegment.forEach( s => s.findElements(segmentObjects.querySelector('#sg_layout')) );\n"
"\n"
/*  adjust view box  */			
"\t\t\tconst svg = document.querySelector('#svgData svg');\n"
"\t\t\tsvg.viewBox.baseVal.x = svg.getBBox().x-0.05;\n"
"\t\t\tsvg.viewBox.baseVal.width = svg.getBBox().width+0.1;\n"
"\t\t\tsvg.viewBox.baseVal.y = svg.getBBox().y-0.05;\n"
"\t\t\tsvg.viewBox.baseVal.height = svg.getBBox().height+0.1;\n"
"\t\t\trecord.forEach( r => r.createSVG());\n"
"\t\t\tconst shuttleParent = document.createElementNS('http://www.w3.org/2000/svg', 'g');\n"
"\t\t\tshuttleParent.setAttribute('id', 'shuttles');\n"
"\t\t\tsvg.appendChild(shuttleParent);\n"
"\t\t\tthis.sliderTime.min = 0;\n"
"\t\t\tthis.sliderTime.max = record.length-1;\n"
"\t\t\tthis.sliderTime.value = this.sliderTime.max;\n"
"\t\t\tthis.sliderTime.addEventListener('input', sliderChanged );\n"
"\t\t\trecord[0].paint();\n"
"\t\t\tthis.currentTime.innerHTML = record[0].getTime();\n"
"\t\t\tlet transformation = new Transformation();\n"
"\t\t\tthis.btnPlay.addEventListener('click', play );\n"
"\n"
"\t\t}\n"
/*  document load  */		
"\t\tdocument.addEventListener('DOMContentLoaded', load );\n"
"\t</script>\n"
"</head>\n"
"<body>\n"
/*  hover dialog  */
"\t<dialog id=\"dlgHoverInfo\"></dialog>\n"
/*  start of SVG  */	
"\t<div id=\"svgData\">\n"
;
/*  end of SVG  */
const char* HTML_FOOTER = ""
"\t</div>\n"
"\t<div id=\"controlContainer\">\n"
"\t<input type=\"range\" min=\"1\" max=\"100\" value=\"0\" class=\"slider\" id=\"sliderTime\"><br />\n"
"\t<label id=\"currentTime\" for=\"sliderTime\"></label><br />\n"
"\t<button id=\"btnPlay\"><svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><polygon points=\"5 3 19 12 5 21 5 3\"></polygon></svg></button>\n"
"\t<button id=\"btnLeft\"><svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" ><line x1=\"19\" y1=\"12\" x2=\"5\" y2=\"12\"></line><polyline points=\"12 19 5 12 12 5\"></polyline></svg></button>\n"
"\t<button id=\"btnRight\"><svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\" ><line x1=\"5\" y1=\"12\" x2=\"19\" y2=\"12\"></line><polyline points=\"12 5 19 12 12 19\"></polyline></svg></button>\n"
"\t<button id=\"btnUp\"><svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"12\" y1=\"19\" x2=\"12\" y2=\"5\"></line><polyline points=\"5 12 12 5 19 12\"></polyline></svg></button>\n"
"\t<button id=\"btnDown\"><svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><line x1=\"12\" y1=\"5\" x2=\"12\" y2=\"19\"></line><polyline points=\"19 12 12 19 5 12\"></polyline></svg></button>\n"
"\t<button id=\"btnRotate\"><svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><polyline points=\"1 4 1 10 7 10\"></polyline><path d=\"M3.51 15a9 9 0 1 0 2.13-9.36L1 10\"></path></svg></button>\t\n"
"\t<button id=\"btnZoomIn\"><svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"11\" cy=\"11\" r=\"8\"></circle><line x1=\"21\" y1=\"21\" x2=\"16.65\" y2=\"16.65\"></line><line x1=\"11\" y1=\"8\" x2=\"11\" y2=\"14\"></line><line x1=\"8\" y1=\"11\" x2=\"14\" y2=\"11\"></line></svg></button>\n"
"\t<button id=\"btnZoomOut\"><svg xmlns=\"http://www.w3.org/2000/svg\" width=\"24\" height=\"24\" viewBox=\"0 0 24 24\" fill=\"none\" stroke=\"currentColor\" stroke-width=\"2\" stroke-linecap=\"round\" stroke-linejoin=\"round\"><circle cx=\"11\" cy=\"11\" r=\"8\"></circle><line x1=\"21\" y1=\"21\" x2=\"16.65\" y2=\"16.65\"></line><line x1=\"8\" y1=\"11\" x2=\"14\" y2=\"11\"></line></svg></button>\n"
"\t</div>\n"
"</body>\n"
"</html>\n"
;