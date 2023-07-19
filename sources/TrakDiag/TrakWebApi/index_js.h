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

const char* INDEX_JS= "\xef\xbb\xbf"
"\"use strict\";\n"
"\n"
"/* created by https://github.com/hilch/TrakDiag V " STRINGIFY(_TrakDiag_VERSION) "- TD_WebServices.cpp */\n"
"\n"
"class Segment {\n"
"\tconstructor(ID, name, length) {\n"
"\t\tthis.ID = ID;\n"
"\t\tthis.name = name;\n"
"\t\tthis.length = length;\n"
"\t\tthis.segmentPath = undefined;\n"
"\t\tthis.segmentBody = undefined;\n"
"\t\tthis.segmentBody2 = undefined;\n"
"\t}\n"
"\n"
"\taddEvents(segmentParent) {\n"
"\t\tsegmentParent.addEventListener('mouseover', (event) => {\n"
"\t\t\tconst hoverInfo = document.getElementById('hoverInfo');\n"
"\t\t\thoverInfo.innerHTML = 'name: ' + this.name + ' / length: ' + this.length.toFixed(3)\n"
"\t\t\t\t+ ' / ID: ' + this.ID\n"
"\t\t\thoverInfo.style.visibility='visible';\n"
"\t\t\tthis.segmentBody.classList.add('highlightSegment');\n"
"\t\t\tthis.segmentBody2.classList.add('highlightSegment');\n"
"\t\t});\n"
"\t\tsegmentParent.addEventListener('mouseout', (event) => {\n"
"\t\t\tconst hoverInfo = document.getElementById('hoverInfo');\n"
"\t\t\tthis.segmentBody.classList.remove('highlightSegment');\n"
"\t\t\tthis.segmentBody2.classList.remove('highlightSegment');\n"
"\t\t\thoverInfo.style.visibility='hidden';\n"
"\t\t});\n"
"\t}\n"
"\n"
"\tfindElementsV1(container) { /* < 5.23 */\n"
"\t\tthis.segmentBody = container.querySelector(`g polygon[id]#${this.name}`);\n"
"\t\tthis.segmentBody.style.fill = '';\n"
"\t\tthis.segmentBody2 = this.segmentBody.nextElementSibling;\t\tthis.segmentBody2.style.fill = '';\n"
"\t\tconst segmentParent = this.segmentBody.parentElement;\n"
"\t\tthis.segmentPath = segmentParent.querySelector('polyline');\n"
"\t\tthis.addEvents(segmentParent);\n"
"\t}\n"
"\n"
"\tfindElementsV2(container) { /* >= 5.23 */\n"
"\t\tthis.segmentBody = container.querySelector(`#pgsg_${this.name}`);\n"
"\t\tthis.segmentBody.style.fill = '';\n"
"\t\tthis.segmentBody2 = this.segmentBody.nextElementSibling;\t\tthis.segmentBody2.style.fill = '';\n"
"\t\tconst segmentParent = container.querySelector(`#gsg_${this.name}`);\n"
"\t\tthis.segmentPath = container.querySelector(`#plsg_${this.name}`);\t\t\t\t\n"
"\t\tthis.addEvents(segmentParent);\n"
"\t}\n"
"\n"
"\txy(percentage) { /* get coords from percentage position */\n"
"\t\tconst linepos = (percentage / 100.0) * this.segmentPath.getTotalLength();\n"
"\t\treturn this.segmentPath.getPointAtLength(linepos);\n"
"\t}\n"
"\n"
"\tpaintShuttle(svg) {\n"
"\t\tthis.segmentPath.parentElement.appendChild(svg);\n"
"\t}\n"
"\n"
"\tsegmentPosition(percentage) {\n"
"\t\treturn ((percentage/100)*this.length).toFixed(3);\n"
"\t}\n"
"\n"
"\tsetStatus(flags) {\n"
"\t\tconst commReady = !!(flags &0x01);\n"
"\t\tconst ready = !!(flags & 0x02);\n"
"\t\tconst power = !!(flags & 0x04);\n"
"\t\tconst enable = !!(flags & 0x08);\n"
"\t\tthis.segmentBody.classList.remove('segReady', 'segNotReadyForPowerOn', 'segDisabled', 'segOffline' );\n"
"\t\tthis.segmentBody2.classList.remove('segReady', 'segNotReadyForPowerOn', 'segDisabled', 'segOffline' );\n"
"\t\tif( commReady ){\n"
"\t\t\tif( ready ){\n"
"\t\t\t\tif( power ){\n"
"\t\t\t\t\tthis.segmentBody.classList.add('segReady');\n"
"\t\t\t\t\tthis.segmentBody2.classList.add('segReady');\n"
"\t\t\t\t}\n"
"\t\t\t\telse {\n"
"\t\t\t\t\tthis.segmentBody.classList.add('segDisabled');\n"
"\t\t\t\t\tthis.segmentBody2.classList.add('segDisabled');\n"
"\t\t\t\t}\n"
"\t\t\t}\n"
"\t\t\telse {\n"
"\t\t\t\tthis.segmentBody.classList.add('segNotReadyForPowerOn');\n"
"\t\t\t\tthis.segmentBody2.classList.add('segNotReadyForPowerOn');\n"
"\t\t\t}\n"
"\t\t}\n"
"\t\telse {\n"
"\t\t\tthis.segmentBody.classList.add('segOffline');\n"
"\t\t\tthis.segmentBody2.classList.add('segOffline');\n"
"\t\t}\n"
"\t}\n"
"\n"
"}\n"
"\n"
"\n"
"class Shuttle {\n"
"\tconstructor(index, active, PLCopen, segment, pos ) {\n"
"\t\tthis.index = index;\n"
"\t\tthis.active = active;\n"
"\t\tthis.PLCopen = PLCopen;\n"
"\t\tthis.segment = segment;\n"
"\t\tthis.pos = pos; /* [%%] */\n"
"\t}\n"
"\n"
"\tplcOpenStatus() {\n"
"\t\treturn ['disabled', 'standstill', 'homing', 'stopping', 'discrete motion', 'continous motion',\n"
"\t\t\t'synchronized motion', 'error stop', 'startup', 'invalid configuration'][this.PLCopen];\n"
"\t}\n"
"\n"
"\tcreateSVG() {\n"
"\t\tconst { x, y } = this.segment.xy(this.pos);\n"
"\t\tthis.svg = document.createElementNS('http://www.w3.org/2000/svg', 'circle');\n"
"\t\tthis.svg.classList.add('shuttle');\n"
"\t\tthis.svg.classList.add('shuttleId_' + this.index);\n"
"\t\tthis.svg.setAttribute('shuttle-id', this.index);\n"
"\t\tthis.svg.setAttribute('r', '0.012');\n"
"\t\tthis.svg.setAttribute('cx', x);\n"
"\t\tthis.svg.setAttribute('cy', y);\n"
"\t\tswitch (this.PLCopen) {\n"
"\t\t\tcase 0: /* disabled */\n"
"\t\t\tcase 8: /* startup */\n"
"\t\t\t\tthis.svg.classList.add('shuttleDisabled');\n"
"\t\t\t\tbreak;\n"
"\n"
"\t\t\tcase 1: /* standstill */\n"
"\t\t\tcase 2: /* homing */\n"
"\t\t\tcase 3: /* stopping */\n"
"\t\t\tcase 4: /* discrete motion */\n"
"\t\t\tcase 5: /* continuous motion */\n"
"\t\t\tcase 6: /* synchronized motion */\n"
"\t\t\t\tthis.svg.classList.add('shuttleReady');\n"
"\t\t\t\tbreak;\n"
"\n"
"\t\t\tcase 7: /* errorstop */\n"
"\t\t\tcase 9: /* invalid configuration */\n"
"\t\t\t\tthis.svg.classList.add('shuttleErrorStop');\n"
"\t\t\t\tbreak;\n"
"\t\t}\n"
"\t\tif (!this.active) this.svg.classList.add('shuttleDeleted');\n"
"\t\tthis.svg.addEventListener('mouseover', (event) => {\n"
"\t\t\tconst hoverInfo = document.getElementById('hoverInfo');\n"
"\t\t\tconst deleted = !this.active ? 'deleted shuttle !' : '';\n"
"\t\t\thoverInfo.innerHTML = deleted + 'index: ' + this.index + ' / ' + 'PLCopen: ' + this.plcOpenStatus() + ' / '\n"
"\t\t\t\t+ 'segment: ' + this.segment.name + ' / '\n"
"\t\t\t\t+ 'segment-position: ' + this.segment.segmentPosition(this.pos);\n"
"\t\t\tthis.svg.classList.add('highlightShuttle');\n"
"\t\t\thoverInfo.style.visibility='visible';\n"
"\t\t\tevent.stopPropagation();\n"
"\t\t});\n"
"\t\tthis.svg.addEventListener('mouseout', (event) => {\n"
"\t\t\tconst hoverInfo = document.getElementById('hoverInfo');\n"
"\t\t\tthis.svg.classList.remove('highlightShuttle');\n"
"\t\t\thoverInfo.style.visibility='hidden';\n"
"\t\t});\n"
"\t}\n"
"\n"
"\tpaint() {\n"
"\t\tthis.segment.paintShuttle(this.svg);\n"
"\t}\n"
"}\n"
"\n"
"\n"
"class Assembly {\n"
"\n"
"\tconstructor() {\n"
"\t\tthis.segment = [];\n"
"\t\tthis.offline = true;\n"
"\t}\n"
"\n"
"\twait = async (milliseconds) => {  /* wait x ms */\n"
"\t\tconst promise = new Promise(res => setTimeout(() => res(), milliseconds));\n"
"\t\treturn promise;\n"
"\t}\n"
"\n"
"\tload = async () => {\n"
"\t\t/* load segment data */\n"
"\t\tconst hoverInfo = document.getElementById('hoverInfo');\n"
"\t\thoverInfo.innerText='loading SVG...';\n"
"\t\thoverInfo.style.visibility='visible';\n"
"\t\ttry {\n"
"\t\t\tlet response = await fetch('/TrakWebApi/segments');\n"
"\t\t\tlet segmentInfo = await response.json()\n"
"\t\t\tsegmentInfo.forEach(item => this.segment.push(new Segment(item.ID, item.name, item.length)));\n"
"\t\t}\n"
"\t\tcatch( err ){\n"
"\t\t\tdocument.getElementById('timeoutBox').style.display = 'block';\n"
"\t\t\tthis.offline = true;\n"
"\t\t\treturn false;\n"
"\t\t}\n"
"\n"
"\t\t/* load SvgData */\n"
"\t\tconst svgParent = document.createElement('div');\n"
"\t\tsvgParent.setAttribute('id', 'svgParent');\n"
"\t\tlet response = undefined;\n"
"\t\ttry {\n"
"\t\t\tresponse = await fetch('/TrakWebApi/svgdata');\n"
"\t\t\tif (response.status !== 200)\n"
"\t\t\t\tthrow new Error(`Error fetching ${response.url}`);\n"
"\t\t\tsvgParent.innerHTML = await response.text();\n"
"\t\t\tconst svg = svgParent.querySelector('svg');\n"
"\n"
"\t\t\tconst workspace = svgParent.querySelector('#workspace');\n"
"\t\t\tconst container = workspace.parentElement;\n"
"\t\t\tconst segmentObjects = container.querySelector('#segments');\n"
"\t\t\tif (segmentObjects) { /* >= 5.23 */\n"
"\t\t\t\tsegmentObjects.querySelector('#sg_legend').remove();\n"
"\t\t\t\tcontainer.querySelector('#sectors').remove();\n"
"\t\t\t\tcontainer.querySelector('#barriers').remove();\n"
"\t\t\t\tcontainer.querySelector('#workspace').remove();\n"
"\t\t\t\tcontainer.querySelector('#processpoints').remove();\n"
"\t\t\t\tthis.segment.forEach(s => s.findElementsV2(segmentObjects.querySelector('#sg_layout')));\n"
"\t\t\t}\n"
"\t\t\telse { /* < 5.23 */\n"
"\t\t\t\t/* remove all sectors */\n"
"\t\t\t\tcontainer.querySelectorAll('#sector').forEach((e) => { e.remove(); })\n"
"\n"
"\t\t\t\t/* remove triggerpoints and barriers */\n"
"\t\t\t\tcontainer.querySelectorAll('#triggerpoint').forEach((e) => { e.remove(); })\n"
"\t\t\t\tcontainer.querySelectorAll('polyline[stroke=\"darkorange\"]').forEach((e) => { e.remove(); })\n"
"\t\t\t\tcontainer.querySelectorAll('polyline[stroke=\"purple\"]').forEach((e) => { e.remove(); })\n"
"\t\t\t\t/* remove all legend tables */\n"
"\t\t\t\tsvgParent.querySelectorAll('text').forEach((e) => {\n"
"\t\t\t\t\tif (e.innerHTML == ' Sectors' || e.textContent == ' Segments')\n"
"\t\t\t\t\t\te.parentElement.parentElement.remove();\n"
"\t\t\t\t\telse if (/Process\\s*points/.test(e.innerHTML))\n"
"\t\t\t\t\t\te.parentElement.parentElement.remove();\n"
"\t\t\t\t})\n"
"\t\t\t\tworkspace.remove();\n"
"\t\t\t\tthis.segment.forEach(s => s.findElementsV1(container));\n"
"\t\t\t}\n"
"\t\t\tdocument.querySelector('#svgParent').replaceWith(svgParent);\n"
"\t\t\tsvg.viewBox.baseVal.x = svg.getBBox().x-0.02;\n"
"\t\t\tsvg.viewBox.baseVal.width = svg.getBBox().width+0.04;\n"
"\t\t\tsvg.viewBox.baseVal.y = svg.getBBox().y-0.02;\n"
"\t\t\tsvg.viewBox.baseVal.height = svg.getBBox().height+0.04;\n"
"\t\t\thoverInfo.style.visibility='hidden';\n"
"\t\t\treturn true;\n"
"\t\t}\n"
"\t\tcatch( err ){\n"
"\t\t\tdocument.getElementById('timeoutBox').style.display = 'block';\t\n"
"\t\t\tthis.offline = true\t\t\n"
"\t\t\treturn false;\n"
"\t\t}\n"
"\t}\n"
"\n"
"\tremoveAllShuttles = () => {\n"
"\t\tconst shuttles = document.querySelectorAll('.shuttle');\n"
"\t\tif (shuttles.length != 0) {\n"
"\t\t\tshuttles.forEach((s) => s.parentNode.removeChild(s));\n"
"\t\t}\n"
"\t}\n"
"\n"
"\t/* read shuttle positions */\n"
"\tasync readShuttlePositions() {\n"
"\t\ttry {\n"
"\t\t\tconst res = await fetch('/TrakWebApi/positions', { signal: AbortSignal.timeout(5000) });\n"
"\t\t\tif (res.status === 200) {\n"
"\t\t\t\tif( this.offline ) document.querySelector('#timeoutBox').style.display = 'none';\n"
"\t\t\t\tthis.offline = false;\n"
"\t\t\t\tconst monitor = await res.json();\n"
"\t\t\t\tthis.removeAllShuttles();\n"
"\t\t\t\tif(monitor.length){\n"
"\t\t\t\t\tmonitor.forEach( s => {\n"
"\t\t\t\t\t\tconst first = s[0]; \n"
"\t\t\t\t\t\tconst second = s[1];\n"
"\t\t\t\t\t\tconst index = first;\n"
"\t\t\t\t\t\tconst plcOpen = second & 0x7f;\n"
"\t\t\t\t\t\tconst active = !!(second & 0x100);\n"
"\t\t\t\t\t\tconst virtual = !!(second & 0x200);\n"
"\t\t\t\t\t\tconst segmentIndex = (second >> 10) & 0x7ff;\n"
"\t\t\t\t\t\tconst segmentPosition = (second >> 21) & 0x7f;\n"
"\t\t\t\t\t\tconst segment = this.segment[segmentIndex];\n"
"\t\t\t\t\t\tconst shuttle = new Shuttle( index, active, plcOpen, segment, segmentPosition);\n"
"\t\t\t\t\t\tshuttle.createSVG();\n"
"\t\t\t\t\t\tshuttle.paint();\n"
"\t\t\t\t\t});\n"
"\t\t\t\t}\n"
"\t\t\t}\n"
"\t\t} catch( err ){\n"
"\t\t\tif( err.name == 'AbortError' ){;\n"
"\t\t\t\tthis.offline = true;\n"
"\t\t\t\tdocument.querySelector('#timeoutBox').style.display = 'block';\n"
"\t\t\t}\n"
"\t\t}\n"
"\t}\n"
"\n"
"\t/* read segment flags */\n"
"\tasync readSegmentFlags() {\n"
"\t\ttry {\n"
"\t\t\tconst res = await fetch('/TrakWebApi/segment_status');\n"
"\t\t\tif (res.status === 200) {\n"
"\t\t\t\tconst flags = await res.json();\n"
"\t\t\t\tthis.segment.forEach( (s,i) => s.setStatus(flags[i]) );\n"
"\t\t\t}\n"
"\t\t} catch(err){}\n"
"\t}\n"
"\n"
"\t/* cyclic refresh */\n"
"\tasync cyclicRefresh(assembly) {\n"
"\t\tlet cycles = 10;\n"
"\t\twhile (true) {\n"
"\t\t\t++cycles;\n"
"\t\t\tif( (cycles >= 10) && !this.offline ) {\n"
"\t\t\t\tcycles = 0;\n"
"\t\t\t\tawait this.readSegmentFlags()\n"
"\t\t\t}\n"
"\t\t\tawait this.readShuttlePositions(assembly);\n"
"\t\t\tawait this.wait(50);\n"
"\t\t}\n"
"\t}\n"
"\n"
"}\n"
"\n"
"\n"
"document.addEventListener('DOMContentLoaded', async () => {\n"
"\tconst assembly = new Assembly();\n"
"\tif( await assembly.load() ){\n"
"\t\tawait assembly.cyclicRefresh();\n"
"\t}\n"
"})\n"
;