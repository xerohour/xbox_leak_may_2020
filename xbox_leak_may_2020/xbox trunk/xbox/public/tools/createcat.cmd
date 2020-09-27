@echo off
setlocal ENABLEEXTENSIONS
if DEFINED _echo on
if DEFINED verbose echo on

REM -----------------------------------------------------
REM     CreateCat.cmd
REM
REM     Purpose:  Create a NT 5 catalog file from a list
REM
REM     Author:   Barb Kess
REM     Date:     July 29, 1998
REM
REM     See Usage for details.
REM ------------------------------------------------------


REM ------------------------------------------------
REM  Get command-line options:
REM ------------------------------------------------

for %%a in (./ .- .) do if ".%1." == "%%a?." goto Usage

if "%4" == "" (
    echo ERROR: Too few arguments.
    goto Usage
)

rem Parse the command line

set list=%1
set catfile=%2
set tempdir=%3
set binout=%4

REM OSAttr is 2:5.0 (Win2k) by default.
set osattr=2:5.0
if not "%5" == "" (
    set osattr=%5
)

if not "%6" == "" (
    echo ERROR: Too many arguments.
    goto Usage
)
echo.

REM Global variables
for %%i in (%0) do set script_name=%%~ni.cmd
set cmdline=%0 %*

call :EchotimeMsg "Start %cmdline%."

REM --------------------------------------------------
REM  Create the CDF file
REM --------------------------------------------------

set tmp_catfile=%tempdir%\%catfile%.cat
set tmp_cdffile=%tempdir%\%catfile%.cdf

if /i NOT exist %tempdir% md %tempdir%
if errorlevel 1 (
    call :Error "Unable to create the temporary directory %tempdir%."
    goto end
)

REM Put the header on and output it as a CDF
call :EchoMsg "Creating %tmp_cdffile%..."

echo ^[CatalogHeader^]> %tmp_cdffile%
echo Name=%catfile%>> %tmp_cdffile%
echo PublicVersion=0x0000001>> %tmp_cdffile%
echo EncodingType=0x00010001>> %tmp_cdffile%
echo CATATTR1=0x10010001:OSAttr:%osattr%>> %tmp_cdffile%
echo ^[CatalogFiles^]>> %tmp_cdffile%
type %list%>> %tmp_cdffile%

if exist %tmp_cdffile% call :EchoMsg "Succeeded"

REM ---------------------------------------------------
REM  Create the CAT file
REM ---------------------------------------------------
set cmd=pushd %tempdir%
%cmd%
if errorlevel 1 (
    call :Error "%cmd% failed."
    goto end
)

call :EchoMsg "Creating %tmp_catfile%..."
set cmd=makecat -n %tmp_cdffile%
%cmd%
if errorlevel 1 (
    call :Error "%cmd% failed."
    goto end
)
REM ---------------------------------------------------
REM  Sign the CAT file with the test signature
REM ---------------------------------------------------

call :EchoMsg "Signing %tmp_catfile% with the test signature..."
set cmd=setreg -q 1 TRUE
%cmd%
if errorlevel 1 (
    call :Error "%cmd% failed."
    goto end
)
popd

REM if %tempdir% is the current directory, signcode.exe will fail without setting the errorlevel
signcode -v driver.pvk -spc driver.spc -n "Microsoft Windows NT Driver Catalog TEST" -i "http://ntbld" %tmp_catfile% -t http://timestamp.verisign.com/scripts/timstamp.dll
if errorlevel 1 (
    call :Error "signcode failed."
    goto end
)

REM ---------------------------------------------------
REM  Move CAT file to the output directory
REM ---------------------------------------------------

if /i "%tempdir%" == "%binout%" goto move_done

if /i NOT exist %binout% md %binout%
if errorlevel 1 (
    call :Error "Unable to make the output directory %binout%."
    goto end
)

echo.
call :EchoMsg "Copying %tmp_catfile% to %binout%..."
set cmd=xcopy /f %tmp_catfile% %binout%\
%cmd%
if errorlevel 1 (
    call :Error "%cmd% failed."
    goto end
)

REM Need the CDF file for testing purposes
echo.
call :EchoMsg "Copying %tmp_cdffile% to %binout%..."
set cmd=xcopy /f %tmp_cdffile% %binout%\
%cmd%
if errorlevel 1 (
    call :Error "%cmd% failed."
    goto end
)
:move_done
echo.
call :EchoMsg "%binout%\%catfile%.cat and %binout%\%catfile%.cdf generated successfully."
call :EchotimeMsg "End %cmdline%."

goto end

REM Subroutines

:EchoMsg
REM Writes a message on the screen and in the logfile.
    setlocal
    REM Remove quotes from the beginning and the end of message.
    set LOGMSG=%1
    set LOGMSG=%LOGMSG:~1%
    set LOGMSG=%LOGMSG:~0,-1%
    REM Echo message
    echo %LOGMSG%
    if "%logfile%" == "" goto EndEchoMsg
    echo %LOGMSG% >> %logfile%
:EndEchoMsg
    echo.
    endlocal
goto :EOF

:EchotimeMsg
REM Writes an echotime message in the logfile.
    setlocal
    if "%logfile%" == "" goto EndEchotimeText
    echo. >> %logfile%
    echotime /H:MbO-D-Y %1 >> %logfile%
    echo. >> %logfile%
:EndEchotimeText
    endlocal
goto :EOF

:Error
 REM Writes an error message on the screen and in the logfile.
    setlocal
    set ERRMSG=%1
    set ERRMSG=%ERRMSG:~1%
    set ERRMSG=%ERRMSG:~0,-1%
    call :EchoMsg "%script_name% : error: %ERRMSG%"
    endlocal
goto :EOF

REM ---------------------------------------------------
REM  Display Usage:
REM ---------------------------------------------------

:Usage
echo.
echo Creates a CAT file signed with the test certificate
echo given a file with the proper list format for a CDF.
echo.
echo   Usage:  createcat.cmd filelist catfile tempdir outdir [osattr]
echo.
echo   filelist  Specifies the file listing the files to be put 
echo             into the CAT file.  Each file needs to be listed
echo             on a separate line as follows:
echo.
echo             ^<hash^>path\filename=path\filename
echo.
echo   catfile   Specifies the name of the catalog file, with 
echo             no extension.
echo   tempdir   Specifies the temporary directory to use when 
echo             creating the CAT and CDF files.
echo   outdir    Specifies the directory to place the final 
echo             CAT and CDF files.
echo   osattr    Specifies the OSAttr used in CDF's CATATTR1 entry.
echo             2:5.0 is the default value, correct for Win2k files.
echo             Use 1:4.0 for Win98 files, and 2:4.x for NT4 SP files.
echo.
echo   Displays log and error information on the screen.
echo   If environment variable logfile already defined, writes the 
echo   output in %%logfile%% as well.
echo.
echo.

:end
endlocal

