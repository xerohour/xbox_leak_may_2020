@if "%_echo%"=="" echo off
@rem
if "%_NTDRIVE%" == "" set _NTDRIVE=D:
if "%_NTROOT%" == "" set _NTROOT=\xbox
call path=%_NTDRIVE%%_NTROOT%\public\idw;%_NTDRIVE%%_NTROOT%\public\mstools;%_NTDRIVE%%_NTROOT%\public\tools;%PATH%
if EXIST %_NTDRIVE%%_NTROOT%\batch\setdbg.bat call %_NTDRIVE%%_NTROOT%\batch\setdbg.bat
call %_NTDRIVE%%_NTROOT%\PUBLIC\TOOLS\ntenv.cmd %1 %2 %3 %4 %5 %6 %7 %8 %9
call sd revert ..\xodashlib\ButtonHelp.h
call sd revert ..\xodashlib\ButtonId.h
call sd revert ..\xodashlib\ButtonName.h
call sd revert ..\xodashlib\SceneId.h
call sd revert ..\xodashlib\SceneName.h
call sd revert ..\xodashmain\navigationdesc.cpp
call sd revert ..\xodashmain\navigationmap.cpp
