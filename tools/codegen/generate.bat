py web2cpp.py --source ..\..\tools\third_party\panzoom\dist\panzoom.min.js --output ..\..\sources\TrakDiag\\TD_WebServices\panzoom_js.h --constant PANZOOM_JS
py web2cpp.py --source index.js --output ..\..\sources\TrakDiag\\TD_WebServices\index_js.h --constant INDEX_JS
py web2cpp.py --source index.css --output ..\..\sources\TrakDiag\\TD_WebServices\index_css.h --constant INDEX_CSS
py web2cpp.py --source index.html --output ..\..\sources\TrakDiag\\TD_WebServices\index_html.h --constant INDEX_HTML
py recorder2cpp.py --source recorder.html --output ..\..\sources\TrakDiag\TD_Recorder\recorder_html.h
timeout 5


