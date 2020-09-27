@if "%_echo%"=="" echo off
if NOT "%OS%" == "Windows_NT" GOTO NTONLY

rem set default enlistment drive and root directory
if "%_NTDRIVE%" == "" set _NTDRIVE=D:
if "%_NTROOT%" == "" set _NTROOT=\xbox
if NOT EXIST %_NTDRIVE%%_NTROOT% md %_NTDRIVE%%_NTROOT%
cd /d %_NTDRIVE%%_NTROOT%

rem We need SLM in our path to complete enlistment
path=\\cpitgcfs01\xboxro\src\public\idw;%path%

rem Make sure we are enlisting to the correct place
@echo ***********************************************************************
@echo You will now enlist in the XBox project in the %_NTROOT% dir of the
@echo following drive:
dir /AD | find /I "volume in drive"
@echo ***********************************************************************
@echo Please verify %_NTDRIVE%'s volume label IS a derivative of your email
@echo alias before continuing. If it is NOT, type ctrl-c and terminate.
@echo If all information is correct:
pause

rem Create private and public dirs in which to enlist
md private
md public
rem Do the enlistments
cd %_NTDRIVE%%_NTROOT%\public
enlist -c -s \\cpitgcfs01\xbox -p public
cd %_NTDRIVE%%_NTROOT%\private
enlist -c -s \\cpitgcfs01\xbox -p private
rem Get a start on our developr [sic] enviornment
cd developr
md %USERNAME%
copy template %USERNAME%

@echo ***********************************************************************
@echo We're now enlisted in \xbox\public and \xbox\private.
@echo See %_NTDRIVE%%_NTROOT%\public\idw\xcomer.doc for additional
@echo steps and instructions on building the project.
@echo ***********************************************************************
GOTO END

:NTONLY
@echo This command requires Windows 2000

:END