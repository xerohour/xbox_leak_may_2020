@if "%_echo%"=="" echo off
@rem
@rem Whoever is putting the PROMPT commands in this file, should put them
@rem in their CONFIG.SYS file instead.
@rem
if "%_NTDRIVE%" == "" set _NTDRIVE=D:
if "%_NTROOT%" == "" set _NTROOT=\xbox
path=%_NTDRIVE%%_NTROOT%\public\idw;%_NTDRIVE%%_NTROOT%\public\mstools;%_NTDRIVE%%_NTROOT%\public\tools;%PATH%
if EXIST %_NTDRIVE%%_NTROOT%\batch\setdbg.bat call %_NTDRIVE%%_NTROOT%\batch\setdbg.bat
cmd /K %_NTDRIVE%%_NTROOT%\PUBLIC\TOOLS\ntenv.cmd %1 %2 %3 %4 %5 %6 %7 %8 %9

