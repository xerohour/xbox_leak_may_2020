ECHO Copying Build...

if "%1"=="-h" (
    rem all I want in the harness
    xcopy /e /h /r /y /i /d %_NT386TREE%\xboxtest\*.* %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest
    xcopy /y /d %_NT386TREE%\dump\harness.xbe %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\dump\harness.xbe
    copy /y %_NTDRIVE%%_NTROOT%\private\test\buildlab\ini\bvt\testini.ini %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest\bvt.ini
    copy /y %_NTDRIVE%%_NTROOT%\private\test\buildlab\ini\stress\testini.ini %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest\stress.ini
    copy /y %_NTDRIVE%%_NTROOT%\private\test\buildlab\ini\media\testini.ini %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest\media.ini
    copy /y %_NTDRIVE%%_NTROOT%\private\test\buildlab\ini\wfvo\testini.ini %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest\wfvo.ini
    copy /y %_NTDRIVE%%_NTROOT%\private\online\test\serverip\internal\xonline.ini %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest\xonline.ini
    goto end
)

xcopy /e /h /r /y /i /d %_NT386TREE%\*.* %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%
copy /y %_NTDRIVE%%_NTROOT%\private\test\buildlab\ini\bvt\testini.ini %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest\bvt.ini
copy /y %_NTDRIVE%%_NTROOT%\private\test\buildlab\ini\stress\testini.ini %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest\stress.ini
copy /y %_NTDRIVE%%_NTROOT%\private\test\buildlab\ini\media\testini.ini %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest\media.ini
copy /y %_NTDRIVE%%_NTROOT%\private\test\buildlab\ini\wfvo\testini.ini %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest\wfvo.ini
copy /y %_NTDRIVE%%_NTROOT%\private\online\test\serverip\internal\xonline.ini %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\xboxtest\xonline.ini

xcopy /r /y /i /d %_NTDRIVE%%_NTROOT%\private\build*.* %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%

if "%1"=="-q" goto end

md %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\results
md %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\public
xcopy /e /h /r /y /i /d %_NTDRIVE%%_NTROOT%\public\*.* %XBLD_PVT%\%_BUILDVER%\%XBLABTYPE%\public


:end
