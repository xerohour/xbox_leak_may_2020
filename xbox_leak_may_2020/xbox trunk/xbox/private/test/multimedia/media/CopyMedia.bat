@echo off
REM Only copies the media to the XBox.
REM ----------------------------------

if not "%1" == "" call p %1


:EXIST
set APPID=A7049955


REM echo -----------------------------------
REM echo Wiping old MEDIA files from XBox.
REM echo -----------------------------------
REM call WipeMedia
echo -----------------------------------
echo Copying needed MEDIA files to XBox.
echo -----------------------------------
 xbmkdir xc:\tdata
 xbmkdir xc:\tdata\%APPID%\
 xbmkdir xc:\tdata\%APPID%\MEDIA\
REM xbcp -f -y -r -t -d %_NTDRIVE%%_NTROOT%\private\test\multimedia\media\media\*.* xc:\tdata\%APPID%\MEDIA

echo -----------------------------------
echo Done copying MEDIA files to XBox.
echo -----------------------------------
goto END


:END
