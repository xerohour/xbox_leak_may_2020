@echo off
if defined _echo echo on
if defined verbose echo on

REM ------------------------------------------------------------------
REM GETPRODS.CMD
REM     Defines _SRV, _ENT, and _DTC environment variables if server, 
REM     advanced server or datacenter apply to the given language, per
REM     -s \\orville\razzle -p public\tools\codes.txt.
REM ------------------------------------------------------------------

set _SRV=& set _ENT=& set _DTC=

REM Provide usage.
for %%a in (./ .- .) do if ".%1." == "%%a?." goto Usage

REM If no language specified, suppose all flavors are applicable.
if "%1" == "" set _SRV=1& set _ENT=1& set _DTC=1& goto :EOF

setlocal
REM Define script_name, used by the logging tools.
set script_name=%~n0

REM Define language
set lang=%1

REM Verify the existence of codes.txt.
set codesfile=%_ntbindir%\public\tools\codes.txt
if not exist %codesfile% (
  call errmsg.cmd "%codesfile% not found."
  goto errend
)

REM Verify if language is listed in codes.txt.
set found=0
for /f "tokens=1 eol=;" %%i in ('findstr /i %lang% %codesfile%') do (
  set found=1
)
if "%found%" == "0" (
  call errmsg.cmd "Language %lang% not listed in %codesfile%."
  goto errend
)
endlocal

REM Retrieve language from codes.txt.
set no_match=1
for /f "tokens=1,7 eol=;" %%i in (%_ntbindir%\public\tools\codes.txt) do (
  if /i "%%i" == "%1%" (
    (if /i "%%j" == "WKS" set _SRV=& set _ENT=& set _DTC=& set no_match=)
    (if /i "%%j" == "SRV" set _SRV=1& set _ENT=& set _DTC=& set no_match=)
    (if /i "%%j" == "ENT" set _SRV=1& set _ENT=1& set _DTC=& set no_match=)
    (if /i "%%j" == "DTC" set _SRV=1& set _ENT=1& set _DTC=1& set no_match=)
  )
)

REM A false comparison sets errorlevel to 1.  Because the for loop and nested
REM if loops above will generally not end on a comparison that's true, we
REM undefine no_match if we encounter a true comparison for any language and
REM product type.
if "%no_match%"=="" goto :EOF
goto errend

:Usage
echo.
echo Sets _SRV, _ENT and _DTC if server, advanced server and data center products
echo are applicable to the given language, per codes.txt.
echo.
echo   Usage: %0 [^<language^>]
echo.
echo   ^<language^>    Specifies the language. Must be listed in codes.txt.
echo                 If no language is specified, _SRV, _ENT, and _DTC are all set.
echo.
echo   Ex: %0
echo       %0 JPN
echo.
goto :EOF

:errend
endlocal
seterror.exe 1
goto :EOF
