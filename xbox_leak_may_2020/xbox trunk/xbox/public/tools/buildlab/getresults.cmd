@echo off
rem %1 is xbvt number
rem %2 xt:\ flag (optional)

if "%1"=="/?" goto usage
if "%1"=="" goto usage
if "%2"=="-x" (
    set source=%3
    shift /2
    shift /2
) else (
    set source=xbvt%1
)

set _xt=xt:
if "%2"=="-td" set _xt=xs:\a7049955

echo Source %source%
echo Drive %_xt%

if %1==1 set target=%XBLD_PVT%\%_BUILDVER%\checked\results
if %1==2 set target=%XBLD_PVT%\%_BUILDVER%\checked\results
if %1==3 set target=%XBLD_PVT%\%_BUILDVER%\free\results
if %1==4 set target=%XBLD_PVT%\%_BUILDVER%\free\results

echo Target %target%
xbcp -x %source% %_xt%\*.log %target%

if %1==1 set target=%XBLD_REL%\%_BUILDVER%\checked\results
if %1==2 set target=%XBLD_REL%\%_BUILDVER%\checked\results
if %1==3 set target=%XBLD_REL%\%_BUILDVER%\free\results
if %1==4 set target=%XBLD_REL%\%_BUILDVER%\free\results

echo Target %target%
xbcp -x %source% %_xt%\*.log %target%

goto end
:usage
echo getresults {xbvt#} [-x xbox] [-td]
echo    -x xbox if ip or name different from xbvt#
echo    -td     if booted to XDK Launcher

:end
