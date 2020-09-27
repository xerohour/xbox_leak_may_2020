@echo off
if "%1"=="" goto usage

REM xbcp -t -y -f -s -d -x %1 \\xdb01\stresscontent\DMusic\media\*.* xs:\a7049955\media
xbcp -t -y -f -s -d -x %1 \\xdb01\stresscontent\Dsound\media\*.* xs:\a7049955\media
xbcp -t -y -f -s -d -x %1 %XBLD_PVT%\%_BUILDVER%\checked\dsstdfx.bin xs:\a7049955\media\dspcode


echo Media Files copied to %1
goto end

:usage
echo %@ <target xbox>

:end
