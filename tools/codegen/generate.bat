py web2cpp.py --source ..\..\tools\third_party\panzoom\dist\panzoom.min.js --output ..\..\sources\TrakDiag\\TrakWebApi\panzoom_js.h --constant PANZOOM_JS
py web2cpp.py --source index.js --output ..\..\sources\TrakDiag\\TrakWebApi\index_js.h --constant INDEX_JS
py web2cpp.py --source index.css --output ..\..\sources\TrakDiag\\TrakWebApi\index_css.h --constant INDEX_CSS
py web2cpp.py --source index.html --output ..\..\sources\TrakDiag\\TrakWebApi\index_html.h --constant INDEX_HTML
timeout 5


