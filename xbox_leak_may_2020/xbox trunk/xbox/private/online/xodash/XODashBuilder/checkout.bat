@if "%_echo%"=="" echo off
@rem
if "%_NTDRIVE%" == "" set _NTDRIVE=D:
if "%_NTROOT%" == "" set _NTROOT=\xbox
call path=%_NTDRIVE%%_NTROOT%\public\idw;%_NTDRIVE%%_NTROOT%\public\mstools;%_NTDRIVE%%_NTROOT%\public\tools;%PATH%
if EXIST %_NTDRIVE%%_NTROOT%\batch\setdbg.bat call %_NTDRIVE%%_NTROOT%\batch\setdbg.bat
call %_NTDRIVE%%_NTROOT%\PUBLIC\TOOLS\ntenv.cmd %1 %2 %3 %4 %5 %6 %7 %8 %9
call sd edit ..\xodashlib\ButtonHelp.h
call sd edit ..\xodashlib\ButtonId.h
call sd edit ..\xodashlib\ButtonName.h
call sd edit ..\xodashlib\SceneId.h
call sd edit ..\xodashlib\SceneName.h
call sd edit ..\xodashmain\navigationdesc.cpp
call sd edit ..\xodashmain\navigationmap.cpp
