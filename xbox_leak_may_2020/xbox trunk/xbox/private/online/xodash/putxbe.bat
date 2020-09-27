echo off
if "%1" =="/?" GOTO USAGE
if NOT "%1" == "/lib" GOTO BUILDEXE
if "%2" == "/cl" rm %_NTDRIVE%\%_NTROOT%\private\online\xodash\obj\i386\*.obj
if "%2" == "/cl" rm %_NTDRIVE%\%_NTROOT%\private\online\xodash\xodashlib\obj\i386\*.obj

cd %_NTDRIVE%\%_NTROOT%\private\online\xodash\xodashlib
build -z

:BUILDEXE
cd %_NTDRIVE%\%_NTROOT%\private\online\xodash
build -z


REM xbcp -f %_NTDRIVE%\xboxbins\dump\xonlinedash.xbe xy:\xonlinedash.xbe
xbcp -f %_NT386TREE%\dump\xonlinedash.xbe xy:\xonlinedash.xbe

if NOT "%3"== "/media" goto EXIT
cd %_NTDRIVE%\%_NTROOT%\private\online\xodash\media
xbmkdir xy:\media
xbmkdir xy:\media\xtf
xbmkdir xy:\media\xbg

cd %_NTDRIVE%\%_NTROOT%\private\online\xodash\Media\Xtf
sd sync ...
xbcp -f "*.xtf"  xy:\media\xtf\

cd %_NTDRIVE%\%_NTROOT%\private\online\xodash\Media\xbg
sd sync ...
xbcp -f "*.xbg"  xy:\media\xbg\

cd %_NTDRIVE%\%_NTROOT%\private\online\xodash\Media\xbx
sd sync ...
xbcp -f "*.xbx"  xy:\media\xbx\

pause 


:USAGE
echo "USAGE: putxbe [/lib] [/cl] [/media]"

:EXIT