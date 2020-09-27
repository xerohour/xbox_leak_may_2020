@echo off
echo -----------------------------------
echo Copying needed MEDIA files to XBox.
echo -----------------------------------
set APPID=A7049955
xbmkdir xc:\tdata\%APPID%\
xbmkdir xc:\tdata\%APPID%\MEDIA\
xbcp -y -r -f %_NTDRIVE%%_NTROOT%\private\test\multimedia\dmusic\dmtest1\media\*.* xc:\tdata\%APPID%\MEDIA
echo -----------------------------------
echo Done copying MEDIA files to XBox.
echo -----------------------------------
