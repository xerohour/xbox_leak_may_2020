xbmkdir xE:\samples\pshader
xbcp -f -y \xboxbins\dump\pshader.xbe xE:\samples\pshader\pshader.xbe


xbmkdir xE:\samples\pshader\media
xbcp -f -r -y media\*.* xE:\samples\pshader\media
xbcp -f pshader.xpu xE:\samples\pshader\media
xbcp -f pshader.xvu xE:\samples\pshader\media

