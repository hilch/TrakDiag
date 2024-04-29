V3.0.0
- mappMotion 5.26.x required as minimum. Discard support of older firmware.
- Rename TD_ServicesSegInfoType to TD_SegInfoType
- Remove obsolete datatype TD_RecorderSegInfoType

TD_WebServices:
- Remove output SegInfo.segmentCyclicInfo
- Add output SegInfo.segmentData
- Segments are colored dark red when in ErrorStop. Segments that initiate errors are colored red.
- Segments are colored yellow when enable input not active.
- Add PLCopen state and error reason to segment info dialog
TD_Recorder:
- Change datatype of .SegInfo to TD_SegInfoType


V2.7.2
- TD_Recorder & TD_Webservices: the shuttles are drawn last so that they are not overdrawn.

V2.7.1
- Bugfix: TD_Webservices /TrakWebApi/index sometimes showed 33556

V2.7.0
- TD_Recorder: Add mouse wheel for zoom and pan
- TD_Recorder: Increase size of shuttle circles
- TD_WebServices: Add /TrakWebApi/shuttle?index=<shuttle index> for single shuttle information (with optional error texts included)
- TD_WebServices: Change segment and shuttle tooltip position
- TD_WebServices: Add dialog for segments and shuttles by clicking on them 
- TD_Webservices: Increase size of shuttle circles
- Add TD_GetShuttles
- Bugfix: TD_WebServices: SegInfo.segmentInfo[].ID showed wrong ID

V2.6.0
- Add TD_ShuttleErrorTexts
- Add TD_SegmentsInfo
- TD_Recorder: Add new input 'FileNamePrefix' to prepend a text to the recorded files
- TD_Recorder: Add new input 'MaxNumberOfRecordings' to limit the recorded files (old files will be deleted)  
- Bugfix: SegInfo.segmentCylicInfo now shows values

V2.5.1
- TD_WebServices: Add pan and zoom

V2.4.4
- TD_WebServices: Improve connection after powerfail

V2.4.3
- Bugfix: /TrakWebApi/index.html was not generated if no processpoints existed

V2.4.2
- TD_Webservices: Bugfix: /TrakWebApi/info blocked everything

V2.4.1
- TD_Webservices: Bugfix: mappView WebView could be blocked
- TD_Webservices/ TD_Recorder: Table 'process points' was visible

V2.4.0
- TD_Webservices: /TrakWebApi/positions: not returning deleted shuttles when assembly is powered
- TD_Webservices: show info when web server is not responding
- TD_Recorder: create a systemdump file on trigger

V2.3.0
- TD_WebServices: add /TrakWebApi/segment_status
- TD_Webservices: /TrakWebApi/index.html: add displaying segment status
- TD_Webservices: /TrakWebApi/index.html: exchange pop-up dialog with status line
- TD_Webservices: Adjust SVG viewbox
- TD_Recorder: Adjust SVG viewbox
- TD_Recorder: Recalculate position of hover dialog to avoid page re-rendering

V2.2.0
- TD_WebServices: fix /TrakWebApi/info
- TD_WebServices: add /TrakWebApi/assembly
- TD_WebServices: bugfix: shuttles disappeared when disabled

V2.1.0
- TD_Recorder: adapted for new SvgData generation in mapp 5.23
- TD_Recorder: new input 'NumberOfCycles'
- TD_Recorder: Reduce default refresh cycle to 50 milliseconds
- TD_Recorder: Fix wrong shuttle positions
- New FB: TD_WebServices


V2.0.1
- TD_Recorder: correct shuttle's segment position shown in Quickinfo dialog.
- TD_Recorder: add segment's ID and PLC address to Quickinfo dialog











