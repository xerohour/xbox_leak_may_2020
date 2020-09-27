@echo off
if defined _echo0 echo on
if defined verbose0 echo on
setlocal ENABLEEXTENSIONS

REM LOGMSG.CMD
REM   Prints a given text on the screen and in a log file.
REM   See Usage for more details.

REM Do not overwrite the value of SCRIPT_NAME, as
REM SCRIPT_NAME is the name of the calling script.

REM Remove (globally) the quotes from the input message.

set param=%1
if not defined param goto Usage
set param=%param:"=%

for %%a in (./ .- .) do if ".%param%." == "%%a?." goto Usage

REM Define the name of the calling tool.
set tool_name=()
if defined SCRIPT_NAME (
  set tool_name=%SCRIPT_NAME%
)

REM Verify if the first parameter is /t.
set time=0
set LOGMSG=%param%
if /i not "%param%" == "/t" goto write_message

REM Handle the case when /t is the first parameter.
set time=1
shift
set LOGMSG=%1
if not defined LOGMSG goto write_message
set LOGMSG=%LOGMSG:"=%

:write_message
if "%time%" == "0" set cmd=echo %tool_name% : %LOGMSG%
if "%time%" == "1" set cmd=echotime ; %tool_name% : /H:M:SbO-D-Y %LOGMSG% ;

REM Write the output message to the screen
echo.& %cmd%

REM If a log file name is provided, write the output to the file as well.
if NOT "%2" == "" set LOGFILE=%2
if not defined LOGFILE goto end
%cmd%>> %LOGFILE% 

goto end

:Usage
echo Prints the given string to the screen and into a file.
echo Used by command scripts to log messages.
echo.
echo usage: logmsg.cmd [/t] "<message>" [^<logfname^>]
echo.
echo   /t           If specified, the message is timestamped.
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
echo   The output message has the format:
echo   %%SCRIPT_NAME%% : ^<message^>
echo   or
echo   () : ^<message^>
echo   depending on whether the calling script defines SCRIPT_NAME or not.
echo.
echo ex: call logmsg.cmd "This is the message to display."
echo.

goto end

:end
endlocal
if defined _echo echo on
if defined verbose echo on
