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

#define STR(x) #x
#define STRINGIFY(x) STR(x)

const char* INDEX_HTML = ""
"\xef\xbb\xbf<!DOCTYPE html><html>\n"
"<head><!-- created by https://github.com/hilch/TrakDiag V " STRINGIFY(_TrakDiag_VERSION) " - TD_WebServices.cpp -->\n"
"\t<title>%s</title>\n"
/* styles */
"\t<style type=\"text/css\" media=\"screen\">\n"
/* animation */
"\t\t@keyframes blinkerOffline { 50%% { fill: red; } }\n"
"\t\t@keyframes blinkerNotReady { 50%% { fill: lightgray; } }\n"
"\n"
/* styles for shuttles */
"\t\t.shuttleDisabled {\n"
"\t\t\tfill: grey;\n"
"\t\t\tstroke: black;\n"
"\t\t\tstroke-width: 0.01;\n"
"\t\t}\n"
"\n"
"\t\t.shuttleReady {\n"
"\t\t\tfill: white;\n"
"\t\t\tstroke: black;\n"
"\t\t\tstroke-width: 0.01;\n"
"\t\t}\n"
"\n"
"\t\t.shuttleErrorStop {\n"
"\t\t\tfill: red;\n"
"\t\t\tstroke: black;\n"
"\t\t\tstroke-width: 0.01;\n"
"\t\t}\n"
"\n"
"\t\t.shuttleDeleted {\n"
"\t\t\tstroke-width: 0.001;\n"
"\t\t\topacity: 10;\n"
"\t\t}\n"
"\n"
"\t\t.highlightShuttle {\n"
"\t\t\tstroke: orange !important;\n"
"\t\t\tfill: orange !important;\n"
"\t\t}\n"
"\n"
//"\t\t#controlContainer {\n"
//"\t\t\tborder: 2px solid grey;\n"
//"\t\t\tbackground-color: lightgray;\n"
//"\t\t\tposition: fixed;\n"
//"\t\t\tbottom: 10px;\n"
//"\t\t\tright: 10px;\n"
//"\t\t\twidth: 50%%;\n"
//"\t\t\topacity: 70%%;\n"
//"\t\t}\n"
//"\n"

/* styles for segments */
"\t\t.segReady {\n"
"\t\t\tfill: lightgray;\n"
"\t\t}\n"
"\n"
"\t\t.segNotReadyForPowerOn {\n"
"\t\t\tanimation: blinkerNotReady 2s step-start 0s infinite;\n"
"\t\t}\n"
"\n"
"\t\t.segDisabled {\n"
"\t\t\tfill: dimgray;\n"
"\t\t}\n"
"\n"
"\t\t.segOffline {\n"
"\t\t\tanimation: blinkerOffline 2s step-start 0s infinite;\n"
"\t\t}\n"
"\n"
"\t\t.highlightSegment {\n"
"\t\t\tfill: orange !important;\n"
"\t\t}\n"
"\n"
/* styles for hover info */
"\t\t#hoverInfo { padding: 0px 1em 0px 1em; margin: auto; border: none; background-color: orange;}\n"
/* styles for timeout box */
"\t\t#timeoutBox { position : fixed; top: 0; left : 0; width: 100%%; height: 100%%; border: none; background-color: rgba(0,0,0,0.4); display : none }\n"
"\t\t#timeoutBox h2 { padding: 0px 1em 0px 1em; margin: auto; color: red }\n"
/* styles for header */
"\t\th1,h2,h3,h4,h5,h6 { font-family: \"Segoe UI\",Arial,sans-serif; font-weight: 400; margin: 10px 0 }\n"
"\t\th1 { font-size: 30px }\n"
"\t\th2 { font-size: 24px }\n"
"\n"
"\t</style>\n"
/* scripting */
"\t<script>\n"
"\t\t\"use strict\";\n"
"\n"
/* class Segment */
"\t\tclass Segment {\n"
/* Segment.constructor */
"\t\t\tconstructor(ID, name, length) {\n"
"\t\t\t\tthis.ID = ID;\n"
"\t\t\t\tthis.name = name;\n"
"\t\t\t\tthis.length = length;\n"
"\t\t\t\tthis.segmentPath = undefined;\n"
"\t\t\t\tthis.segmentBody = undefined;\n"
"\t\t\t\tthis.segmentBody2 = undefined;\n"
"\t\t\t}\n"
"\n"
/* Segment.addEvents() */
"\t\t\taddEvents(segmentParent) {\n"
"\t\t\t\tsegmentParent.addEventListener('mouseover', (event) => {\n"
"\t\t\t\t\tconst hoverInfo = document.getElementById('hoverInfo');\n"
"\t\t\t\t\thoverInfo.innerHTML = 'name: ' + this.name + ' / length: ' + this.length.toFixed(3)\n"
"\t\t\t\t\t\t+ ' / ID: ' + this.ID\n"
//"\t\t\t\t\t\t+ '<br />' + 'PLC address: ' + this.plcAddress;\n"
"\t\t\t\t\thoverInfo.style.visibility='visible';\n"
"\t\t\t\t\tthis.segmentBody.classList.add('highlightSegment');\n"
"\t\t\t\t\tthis.segmentBody2.classList.add('highlightSegment');\n"
"\t\t\t\t});\n"
"\t\t\t\tsegmentParent.addEventListener('mouseout', (event) => {\n"
"\t\t\t\t\tconst hoverInfo = document.getElementById('hoverInfo');\n"
"\t\t\t\t\tthis.segmentBody.classList.remove('highlightSegment');\n"
"\t\t\t\t\tthis.segmentBody2.classList.remove('highlightSegment');\n"
"\t\t\t\t\thoverInfo.style.visibility='hidden';\n"
"\t\t\t\t});\n"
"\t\t\t}\n"
"\n"
/* Segment.findElementsV1 */
"\t\t\tfindElementsV1(container) { /* < 5.23 */\n"
"\t\t\t\tthis.segmentBody = container.querySelector(`g polygon[id]#${this.name}`);\n"
"\t\t\t\tthis.segmentBody.style.fill = '';\n"
"\t\t\t\tthis.segmentBody2 = this.segmentBody.nextElementSibling;"
"\t\t\t\tthis.segmentBody2.style.fill = '';\n"
"\t\t\t\tconst segmentParent = this.segmentBody.parentElement;\n"
"\t\t\t\tthis.segmentPath = segmentParent.querySelector('polyline');\n"
"\t\t\t\tthis.addEvents(segmentParent);\n"
"\t\t\t}\n"
"\n"
/* Segment.findElementsV2 */
"\t\t\tfindElementsV2(container) { /* >= 5.23 */\n"
"\t\t\t\tthis.segmentBody = container.querySelector(`#pgsg_${this.name}`);\n"
"\t\t\t\tthis.segmentBody.style.fill = '';\n"
"\t\t\t\tthis.segmentBody2 = this.segmentBody.nextElementSibling;"
"\t\t\t\tthis.segmentBody2.style.fill = '';\n"
"\t\t\t\tconst segmentParent = container.querySelector(`#gsg_${this.name}`);\n"
"\t\t\t\tthis.segmentPath = container.querySelector(`#plsg_${this.name}`);\t\t\t\t\n"
"\t\t\t\tthis.addEvents(segmentParent);\n"
"\t\t\t}\n"
"\n"
/* Segments.xy */
"\t\t\txy(percentage) { /* get coords from percentage position */\n"
"\t\t\t\tconst linepos = (percentage / 100.0) * this.segmentPath.getTotalLength();\n"
"\t\t\t\treturn this.segmentPath.getPointAtLength(linepos);\n"
"\t\t\t}\n"
"\n"
/* Segments.paintShuttle */
"\t\t\tpaintShuttle(svg) {\n"
"\t\t\t\tthis.segmentPath.parentElement.appendChild(svg);\n"
"\t\t\t}\n"
"\n"
/* Segments.segmentPosition */
"\t\t\tsegmentPosition(percentage) {\n"
"\t\t\t\treturn ((percentage/100)*this.length).toFixed(3);\n"
"\t\t\t}\n"
"\n"
/* Segment.setStatus */
"\t\t\tsetStatus(flags) {\n"
"\t\t\t\tconst commReady = !!(flags &0x01);\n"
"\t\t\t\tconst ready = !!(flags & 0x02);\n"
"\t\t\t\tconst power = !!(flags & 0x04);\n"
"\t\t\t\tconst enable = !!(flags & 0x08);\n"
"\t\t\t\tthis.segmentBody.classList.remove('segReady', 'segNotReadyForPowerOn', 'segDisabled', 'segOffline' );\n"
"\t\t\t\tthis.segmentBody2.classList.remove('segReady', 'segNotReadyForPowerOn', 'segDisabled', 'segOffline' );\n"
"\t\t\t\tif( commReady ){\n"
"\t\t\t\t\tif( ready ){\n"
"\t\t\t\t\t\tif( power ){\n"
"\t\t\t\t\t\t\tthis.segmentBody.classList.add('segReady');\n"
"\t\t\t\t\t\t\tthis.segmentBody2.classList.add('segReady');\n"
"\t\t\t\t\t\t}\n"
"\t\t\t\t\t\telse {\n"
"\t\t\t\t\t\t\tthis.segmentBody.classList.add('segDisabled');\n"
"\t\t\t\t\t\t\tthis.segmentBody2.classList.add('segDisabled');\n"
"\t\t\t\t\t\t}\n"
"\t\t\t\t\t}\n"
"\t\t\t\t\telse {\n"
"\t\t\t\t\t\tthis.segmentBody.classList.add('segNotReadyForPowerOn');\n"
"\t\t\t\t\t\tthis.segmentBody2.classList.add('segNotReadyForPowerOn');\n"
"\t\t\t\t\t}\n"
"\t\t\t\t}\n"
"\t\t\t\telse {\n"
"\t\t\t\t\tthis.segmentBody.classList.add('segOffline');\n"
"\t\t\t\t\tthis.segmentBody2.classList.add('segOffline');\n"
"\t\t\t\t}\n"
"\t\t\t}\n"
"\n"
"\t\t}\n"
"\n"
"\n"
/* class Shuttle */
"\t\tclass Shuttle {\n"
/* Shuttle.constructor */
"\t\t\tconstructor(index, active, PLCopen, segment, pos ) {\n"
"\t\t\t\tthis.index = index;\n"
"\t\t\t\tthis.active = active;\n"
"\t\t\t\tthis.PLCopen = PLCopen;\n"
"\t\t\t\tthis.segment = segment;\n"
"\t\t\t\tthis.pos = pos; /* [%%] */\n"
"\t\t\t}\n"
"\n"
/* Shuttle.plcOpenStatus */
"\t\t\tplcOpenStatus() {\n"
"\t\t\t\treturn ['disabled', 'standstill', 'homing', 'stopping', 'discrete motion', 'continous motion',\n"
"\t\t\t\t\t'synchronized motion', 'error stop', 'startup', 'invalid configuration'][this.PLCopen];\n"
"\t\t\t}\n"
"\n"
/* Shuttle.createSVG */
"\t\t\tcreateSVG() {\n"
"\t\t\t\tconst { x, y } = this.segment.xy(this.pos);\n"
"\t\t\t\tthis.svg = document.createElementNS('http://www.w3.org/2000/svg', 'circle');\n"
"\t\t\t\tthis.svg.classList.add('shuttle');\n"
"\t\t\t\tthis.svg.classList.add('shuttleId_' + this.index);\n"
"\t\t\t\tthis.svg.setAttribute('shuttle-id', this.index);\n"
"\t\t\t\tthis.svg.setAttribute('r', '0.012');\n"
"\t\t\t\tthis.svg.setAttribute('cx', x);\n"
"\t\t\t\tthis.svg.setAttribute('cy', y);\n"
"\t\t\t\tswitch (this.PLCopen) {\n"
"\t\t\t\t\tcase 0: /* disabled */\n"
"\t\t\t\t\tcase 8: /* startup */\n"
"\t\t\t\t\t\tthis.svg.classList.add('shuttleDisabled');\n"
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
"\t\t\t\tif (!this.active) this.svg.classList.add('shuttleDeleted');\n"
"\t\t\t\tthis.svg.addEventListener('mouseover', (event) => {\n"
"\t\t\t\t\tconst hoverInfo = document.getElementById('hoverInfo');\n"
"\t\t\t\t\tconst deleted = !this.active ? 'deleted shuttle !' : '';\n"
"\t\t\t\t\thoverInfo.innerHTML = deleted + 'index: ' + this.index + ' / ' + 'PLCopen: ' + this.plcOpenStatus() + ' / '\n"
"\t\t\t\t\t\t+ 'segment: ' + this.segment.name + ' / '\n"
"\t\t\t\t\t\t+ 'segment-position: ' + this.segment.segmentPosition(this.pos);\n"
"\t\t\t\t\tthis.svg.classList.add('highlightShuttle');\n"
"\t\t\t\t\thoverInfo.style.visibility='visible';\n"
"\t\t\t\t\tevent.stopPropagation();\n"
"\t\t\t\t});\n"
"\t\t\t\tthis.svg.addEventListener('mouseout', (event) => {\n"
"\t\t\t\t\tconst hoverInfo = document.getElementById('hoverInfo');\n"
"\t\t\t\t\tthis.svg.classList.remove('highlightShuttle');\n"
"\t\t\t\t\thoverInfo.style.visibility='hidden';\n"
"\t\t\t\t});\n"
"\t\t\t}\n"
"\n"
/* Shuttle.paint */
"\t\t\tpaint() {\n"
"\t\t\t\tthis.segment.paintShuttle(this.svg);\n"
"\t\t\t}\n"
"\t\t}\n"
"\n"
"\n"
/* class Assembly */
"\t\tclass Assembly {\n"
"\n"
/* Assembly.constructor */
"\t\t\tconstructor() {\n"
"\t\t\t\tthis.segment = [];\n"
"\t\t\t\tthis.offline = true;\n"
"\t\t\t}\n"
"\n"
/* Assembly.wait */
"\t\t\twait = async (milliseconds) => {  /* wait x ms */\n"
"\t\t\t\tconst promise = new Promise(res => setTimeout(() => res(), milliseconds));\n"
"\t\t\t\treturn promise;\n"
"\t\t\t}\n"
"\n"
/* Assembly.load */
"\t\t\tload = async () => {\n"
"\t\t\t\t/* load segment data */\n"
"\t\t\t\tconst hoverInfo = document.getElementById('hoverInfo');\n"
"\t\t\t\thoverInfo.innerText='loading SVG...';\n"
"\t\t\t\thoverInfo.style.visibility='visible';\n"
"\t\t\t\tlet request = fetch('/TrakWebApi/segments');\n"
"\t\t\t\tlet response = await request;\n"
"\t\t\t\tlet segmentInfo = await response.json()\n"
"\t\t\t\tsegmentInfo.forEach(item => this.segment.push(new Segment(item.ID, item.name, item.length)));\n"
"\n"
"\t\t\t\t/* load SvgData */\n"
"\t\t\t\tconst svgParent = document.createElement('div');\n"
"\t\t\t\tsvgParent.setAttribute('id', 'svgParent');\n"
"\t\t\t\trequest = fetch('/TrakWebApi/svgdata');\n"
"\t\t\t\tresponse = await request;\n"
"\t\t\t\tif (response.status !== 200)\n"
"\t\t\t\t\tthrow new Error(`Error fetching ${response.url}`);\n"
"\t\t\t\tsvgParent.innerHTML = await response.text();\n"
"\t\t\t\tconst svg = svgParent.querySelector('svg');\n"
"\n"
"\t\t\t\tconst workspace = svgParent.querySelector('#workspace');\n"
"\t\t\t\tconst container = workspace.parentElement;\n"
"\t\t\t\tconst segmentObjects = container.querySelector('#segments');\n"
"\t\t\t\tif (segmentObjects) { /* >= 5.23 */\n"
"\t\t\t\t\tsegmentObjects.querySelector('#sg_legend').remove();\n"
"\t\t\t\t\tcontainer.querySelector('#sectors').remove();\n"
"\t\t\t\t\tcontainer.querySelector('#barriers').remove();\n"
"\t\t\t\t\tcontainer.querySelector('#workspace').remove();\n"
"\t\t\t\t\tthis.segment.forEach(s => s.findElementsV2(segmentObjects.querySelector('#sg_layout')));\n"
"\t\t\t\t}\n"
"\t\t\t\telse { /* < 5.23 */\n"
"\t\t\t\t\t/* remove all sectors */\n"
"\t\t\t\t\tcontainer.querySelectorAll('#sector').forEach((e) => { e.remove(); })\n"
"\t\t\t\t\t/* remove triggerpoints and barriers */\n"
"\t\t\t\t\tcontainer.querySelectorAll('#triggerpoint').forEach((e) => { e.remove(); })\n"
"\t\t\t\t\tcontainer.querySelectorAll('polyline[stroke=\"darkorange\"]').forEach((e) => { e.remove(); })\n"
"\t\t\t\t\tcontainer.querySelectorAll('polyline[stroke=\"purple\"]').forEach((e) => { e.remove(); })\n"
"\t\t\t\t\t/* remove all legend tables */\n"
"\t\t\t\t\tsvgParent.querySelectorAll('text').forEach((e) => {\n"
"\t\t\t\t\t\tif (e.innerHTML == ' Sectors' || e.textContent == ' Segments')\n"
"\t\t\t\t\t\t\te.parentElement.parentElement.remove();\n"
"\t\t\t\t\t\telse if (/Process\\s*points/.test(e.innerHTML))\n"
"\t\t\t\t\t\t\te.parentElement.parentElement.remove();\n"
"\t\t\t\t\t})\n"
"\t\t\t\t\tworkspace.remove();\n"
"\t\t\t\t\tthis.segment.forEach(s => s.findElementsV1(container));\n"
"\t\t\t\t}\n"
/* adjust view box */
"\t\t\t\tdocument.querySelector('#svgParent').replaceWith(svgParent);\n"
"\t\t\t\tsvg.viewBox.baseVal.x = svg.getBBox().x-0.02;\n"
"\t\t\t\tsvg.viewBox.baseVal.width = svg.getBBox().width+0.04;\n"
"\t\t\t\tsvg.viewBox.baseVal.y = svg.getBBox().y-0.02;\n"
"\t\t\t\tsvg.viewBox.baseVal.height = svg.getBBox().height+0.04;\n"
"\t\t\t\thoverInfo.style.visibility='hidden';\n"
"\t\t\t}\n"
"\n"
/* Assembly.removeAllShuttles */
"\t\t\tremoveAllShuttles = () => {\n"
"\t\t\t\tconst shuttles = document.querySelectorAll('.shuttle');\n"
"\t\t\t\tif (shuttles.length != 0) {\n"
"\t\t\t\t\tshuttles.forEach((s) => s.parentNode.removeChild(s));\n"
"\t\t\t\t}\n"
"\t\t\t}\n"
"\n"
/* Assembly.readShuttlePositions */
"\t\t\t/* read shuttle positions */\n"
"\t\t\tasync readShuttlePositions() {\n"
"\t\t\t\ttry {\n"
"\t\t\t\t\tconst res = await fetch('/TrakWebApi/positions', { signal: AbortSignal.timeout(5000) });\n"
"\t\t\t\t\tif (res.status === 200) {\n"
"\t\t\t\t\t\tif( this.offline ) document.querySelector('#timeoutBox').style.display = 'none';\n"
"\t\t\t\t\t\tthis.offline = false;\n"
"\t\t\t\t\t\tconst monitor = await res.json();\n"
"\t\t\t\t\t\tthis.removeAllShuttles();\n"
"\t\t\t\t\t\tif(monitor.length){\n"
"\t\t\t\t\t\t\tmonitor.forEach( s => {\n"
"\t\t\t\t\t\t\t\tconst first = s[0]; \n"
"\t\t\t\t\t\t\t\tconst second = s[1];\n"
"\t\t\t\t\t\t\t\tconst index = first;\n"
"\t\t\t\t\t\t\t\tconst plcOpen = second & 0x7f;\n"
"\t\t\t\t\t\t\t\tconst active = !!(second & 0x100);\n"
"\t\t\t\t\t\t\t\tconst virtual = !!(second & 0x200);\n"
"\t\t\t\t\t\t\t\tconst segmentIndex = (second >> 10) & 0x7ff;\n"
"\t\t\t\t\t\t\t\tconst segmentPosition = (second >> 21) & 0x7f;\n"
"\t\t\t\t\t\t\t\tconst segment = this.segment[segmentIndex];\n"
"\t\t\t\t\t\t\t\tconst shuttle = new Shuttle( index, active, plcOpen, segment, segmentPosition);\n"
"\t\t\t\t\t\t\t\tshuttle.createSVG();\n"
"\t\t\t\t\t\t\t\tshuttle.paint();\n"
"\t\t\t\t\t\t\t});\n"
"\t\t\t\t\t\t}\n"
"\t\t\t\t\t}\n"
"\t\t\t\t} catch( err ){\n"
"\t\t\t\t\tif( err.name == 'AbortError' ){;\n"
"\t\t\t\t\t\tthis.offline = true;\n"
"\t\t\t\t\t\tdocument.querySelector('#timeoutBox').style.display = 'block';\n"
"\t\t\t\t\t}\n"
"\t\t\t\t}\n"
"\t\t\t}\n"
"\n"
/* Assembly.readSegmentFlags */
"\t\t\t/* read segment flags */\n"
"\t\t\tasync readSegmentFlags() {\n"
"\t\t\t\ttry {\n"
"\t\t\t\t\tconst res = await fetch('/TrakWebApi/segment_status');\n"
"\t\t\t\t\tif (res.status === 200) {\n"
"\t\t\t\t\t\tconst flags = await res.json();\n"
"\t\t\t\t\t\tthis.segment.forEach( (s,i) => s.setStatus(flags[i]) );\n"
"\t\t\t\t\t}\n"
"\t\t\t\t} catch(err){}\n"
"\t\t\t}\n"
"\n"
/* Assembly.cyclicRefresh */
"\t\t\t/* cyclic refresh */\n"
"\t\t\tasync cyclicRefresh(assembly) {\n"
"\t\t\t\tlet cycles = 10;\n"
"\t\t\t\twhile (true) {\n"
"\t\t\t\t\t++cycles;\n"
"\t\t\t\t\tif( (cycles >= 10) && !this.offline ) {\n"
"\t\t\t\t\t\tcycles = 0;\n"
"\t\t\t\t\t\tawait this.readSegmentFlags()\n"
"\t\t\t\t\t}\n"
"\t\t\t\t\tawait this.readShuttlePositions(assembly);\n"
"\t\t\t\t\t//await Assembly.wait(1000);\n"
"\t\t\t\t}\n"
"\t\t\t}\n"
"\n"
"\t\t}\n"
"\n"
"\n"
/* document.addEventListener */
"\t\tdocument.addEventListener('DOMContentLoaded', async () => {\n"
"\t\t\tconst assembly = new Assembly();\n"
"\t\t\tawait assembly.load();\n"
"\t\t\tawait assembly.cyclicRefresh();\n"
"\t\t})\n"
"\n"
"\n"
"\t</script>\n"
"</head>\n"
"\n"
/* body */
"<body>\n"
"\t<h2 id=\"hoverInfo\"></h2>\n"
"\t<div id=\"svgParent\"></div>\n"
"\t<div id=\"timeoutBox\"><h2>Error: Server is not responding !</h2></div>\n"
"</body>\n"
"\n"
"</html>\n"
"";


#define STARTUP					0
#define GET_SEGMENT_LIST		10
#define GET_SEGMENT_INFO		20
#define W_HTTP_REQUESTS			100
#define HTTP_SERV_RESPONSE		110
#define INFO_SVG_FILE			120
#define CHECK_DATA_OBJ			130
#define CREATE_DATA_OBJ			140
#define DELETE_DATA_OBJ			150
#define OPEN_SVG_FILE			160
#define READ_SVG_DATA			170
#define CLOSE_SVG_FILE			180
#define SEND_SVGDATA			190			
#define FREE_UP_MEMORY			200


#define INTERNAL_ERROR						9000
#define INTERNAL_ERROR_GETSEGMENT			9001
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


const char* AcpTrakPLCopenState[] = { "Disabled", "Homing", "Ready", "Stopping", "Errorstop", "Startup", "Invalid Configuration" };
const char* JavascriptBoolean[] = { "false", "true" };
