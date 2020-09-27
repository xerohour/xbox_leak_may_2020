xbcp -f "*.xtf"   xy:\

xbmkdir xy:\audio\
xbmkdir xy:\audio\mainaudio
xbcp -f "audio\mainaudio\*.wav"   xy:\audio\mainaudio
cd media
xbmkdir xy:\media
xbmkdir xy:\media\xbg
xbmkdir xy:\media\xbx
xbmkdir xy:\media\content

xbcp -f "Xbg\*.xbg"     xy:\media\xbg\
xbcp -f "Xbx\*.xbx" xy:\media\xbx\
xbcp -f -r "content\*.*" xy:\media\content\

cd..




pause 

