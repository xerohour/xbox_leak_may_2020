@if "%_echo%"=="" echo off
setlocal
set _NTPROJECT=%NTPROJECTS%
set _NTPROJECT_EXTRA=%NTPROJECTS_EXTRA%
set NTPROJECTS=%IEPROJECTS%
set NTPROJECTS_EXTRA=%IEPROJECTS_EXTRA%
if "%_ieroot%" == "" set _ieroot=%_ntroot%
set _projects_cmd=%_ieroot%\public\tools\iproject.cmd
if "%_NTSTATLOG%" == "" set _NTSTATLOG=%_ieroot%\iestat.log
if "%_NTSTATCMD%" == "" set _NTSTATCMD=%_ieroot%\iestatsr.cmd
:moreprojects
call ntstat.cmd %1
shift
if "%1" == "" goto done
goto moreprojects
:done
endlocal



