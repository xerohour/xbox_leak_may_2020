@echo off
SETLOCAL ENABLEEXTENSIONS
if DEFINED _echo   echo on
if DEFINED verbose echo on

REM ------------------------------------------------
REM Sign the given file with the NT TEST certificate.
REM ------------------------------------------------

set __TIMESTAMPURL=http://timestamper/scripts/timstamp.dll
set __EXTTIMESTAMPURL=http://timestamp.verisign.com/scripts/timstamp.dll
set __FILE=%_ntbindir%\public\tools\driver

REM ------------------------------------------------
REM  Get command-line options:
REM ------------------------------------------------

set First=No
set Notime=No

REM Check the command line for /? -? or ?
for %%a in (./ .- .) do if ".%1." == "%%a?." goto :Usage

REM Set script_name to be used by the logging scripts
for %%i in (%0) do set script_name=%%~ni.cmd

set __CATALOGFILE=%1

call logmsg.cmd "Signing %__CATALOGFILE%..."

:SwitchLoop
if "%1" == "" goto EndLoop
shift

if /i "%1" == "first" set First=Yes&& goto NextArg
if /i "%1" == "notime" set Notime=Yes&&  goto NextArg

:NextArg
shift
goto SwitchLoop

:EndLoop

if "%First%" == "Yes" (
if exist %__FILE%.cer  del /q %__FILE%.cer
if exist %__FILE%.pvk  del /q %__FILE%.pvk
if exist %__FILE%.spc  del /q %__FILE%.spc
)

REM make certificate file (not needed if one already exists)
if "%First%"=="Yes" (
  makecert -eku "1.3.6.1.4.1.311.10.3.5,1.3.6.1.5.5.7.3.3" -sv %__FILE%.pvk -n "CN=Microsoft Windows NT Build Lab TEST" %__FILE%.cer
  if not "%errorlevel%" == "0" call errmsg.cmd "makecert.exe failed."& goto errend
)

REM make SPC file (not needed if one already exists)
if "%First%"=="Yes" (
  cert2spc %__FILE%.cer %__FILE%.spc
  if not "%errorlevel%" == "0" call errmsg.cmd "cert2spc.exe failed."& goto errend
)

setreg -q 1 TRUE
if not "%errorlevel%" == "0" call errmsg.cmd "setreq.exe failed."& goto errend

if "%Notime%"=="Yes" (
  signcode.exe -v %__FILE%.pvk -spc %__FILE%.spc -n "Microsoft Windows NT Driver Catalog TEST" -i "http://ntbld" %__CATALOGFILE%
  if not "%errorlevel%" == "0" call errmsg.cmd "signcode.exe no timestamp failed on %__CATALOGFILE%."& goto errend
) else (
  goto :TimeSign
)
goto end

:TimeSign
if exist %TEMP%\timestamp.tmp del /f %TEMP%\timestamp.tmp

signcode.exe -v %__FILE%.pvk -spc %__FILE%.spc -n "Microsoft Windows NT Driver Catalog TEST" -i "http://ntbld" %__CATALOGFILE% -t %__TIMESTAMPURL% -tr 5 -tw 5 | tee -a %TEMP%\timestamp.tmp
if not "%errorlevel%" == "0" call errmsg.cmd "signcode.exe time stamp failed on %__CATALOGFILE%."& goto errend

findstr /i Succeeded %TEMP%\timestamp.tmp
if ERRORLEVEL 1 (set /a Count=Count + 1) else (goto :End)
if %Count%==6 (set /a Count=0&&goto :ExtTimeSign) 

echo.
echo Attempting to connect to %__TIMESTAMPURL%
echo Retry #%Count% 
echo.

if %Count% LEQ 5 goto :TimeSign

:ExtTimeSign


if exist %TEMP%\timestamp.tmp del /f %TEMP%\timestamp.tmp

signcode.exe -v %__FILE%.pvk -spc %__FILE%.spc -n "Microsoft Windows NT Driver Catalog TEST" -i "http://ntbld" %__CATALOGFILE% -t %__EXTTIMESTAMPURL% -tr 5 -tw 5 | tee -a %TEMP%\timestamp.tmp
if not "%errorlevel%" == "0" call errmsg.cmd "signcode.exe time stamp failed on %__CATALOGFILE%."& goto errend

findstr /i Succeeded %TEMP%\timestamp.tmp
if ERRORLEVEL 1 (set /a Count=Count + 1) else (goto :End)
if %Count%==6 goto :TimeFail 

echo.
echo Attempting to connect to %__EXTTIMESTAMPURL%
echo Retry #%Count% 
echo.

if %Count% LEQ 5 goto :ExtTimeSign

:TimeFail
set errmsg=FAILED attempting to reach both %__TIMESTAMPURL% and %__EXTTIMESTAMPURL%.
set errmsg=%errmsg% You may try to run ntsign [filename] notime to bypass timestamping.
set errmsg=%errmsg% At this point your file has not been signed.

call errmsg.cmd "%errmsg%"
goto errend

:errend
endlocal
seterror.exe 1
goto :EOF

:end
endlocal
seterror.exe 0
goto :EOF

:Usage
echo	ntsign [filename] ^<[Notime]^> ^<[First]^>
echo.
echo		filename - Either a catalog or a binary
echo		^<Notime^> - Skip timestamping. Only the build lab needs to timestamp.
echo		^<First^> -  Use if certs don't exist. Currently they do exist for in 
echo			   nt\public\tools, which can be obtained from SLM sources or
echo			   \\ntbuilds release server.
:End
@ENDLOCAL

