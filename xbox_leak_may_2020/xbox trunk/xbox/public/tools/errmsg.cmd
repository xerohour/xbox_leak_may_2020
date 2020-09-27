@echo off
if defined _echo0 echo on
if defined verbose0 echo on
setlocal ENABLEEXTENSIONS

REM ERRMSG.CMD
REM   Prints a given error message on the screen and in a log file.
REM   See usage for more details.

REM Do not overwirte the value of SCRIPT_NAME,
REM as SCRIPT_NAME is the name of the calling script.

REM Remove (globally) the quotes from the input message.

set ERRMSG=%1
if not defined ERRMSG goto Usage
set ERRMSG=%ERRMSG:"=%

for %%a in (./ .- .) do if ".%ERRMSG%." == "%%a?." goto Usage

REM Define the name of the calling tool.
set tool_name=()
if defined SCRIPT_NAME (
  set tool_name=%SCRIPT_NAME%
)

REM Write the output message to the screen.
echo %tool_name% : error: %ERRMSG%

REM If a log file is provided, write the output to the file as well.

if NOT "%2" == "" set LOGFILE=%2
if not defined LOGFILE goto end
echo %tool_name% : error: %ERRMSG%>> %LOGFILE%

goto end

:Usage
echo Prints the given string to the screen and into a file.
echo.
echo usage: errmsg.cmd "<message>" [^<logfile^>]
echo.
echo   "<message>"  Specifies the text to be displayed,
echo                in quotes.
echo.
echo   ^<logfname^>   Specifies the name of the log file.
echo                LOGFILE, if defined, is the default.
echo                If this parameter is not specified and 
echo                LOGFILE is not defined, the message is 
echo                displayed on the screen only.
echo.
echo   Used by command scripts to log error messages.
echo.
echo   The error message follows the format detalied in MsCompilerFilter,
echo   \\orville\razzle\src\sdktools\build\buildexe.c.
echo.
echo   The output error message has the format:
echo   %%SCRIPT_NAME%% : error: ^<message^>
echo   or
echo   () : error: ^<message^>
echo   depending on whether the calling script defines SCRIPT_NAME or not.
echo.
echo ex: call errmsg.cmd "This is the error message."
echo.
goto end

:end
endlocal
rem re-enable debugging of calling scripts, if '_echo' or 'verbose' is set.
if not "%_echo%%verbose%" == "" @echo on
