@echo off
setlocal enableextensions

set _NTDRIVE=q:
set _NTROOT=\%computername%\nt
set SOURCE=\\ntqfe\iebuild
set USERNAME=IEBuild

net use %_NTDRIVE% /d /y
net use %_NTDRIVE% %SOURCE%
if NOT exist %_NTDRIVE%%_NTROOT% goto err_nodrive

set path=%_NTDRIVE%\idw;%_NTDRIVE%\mstools;%PATH%
call %_NTDRIVE%%_NTROOT%\public\tools\razzle.cmd

goto end

:err_nodrive
echo %0 unable to map drive set %_NTDRIVE% to %SOURCE%
goto end

:end
endlocal