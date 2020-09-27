@echo off
REM The build machine is special
If /i "%ComputerName%"=="XBuilds" goto BuildMachine

REM Normally this batch file (when run anywhere but the build machine) uses the latest bits from the
REM \\xbuilds\release\usa.  If you want to override that you must set _BuildVer in the environment.
REM You also must map the P: drive to the location of your files, which must be laid out like those on the
REM public share.
If Not "%_BuildVer%"=="" Goto AssumePDefined
for /f %%n in (\\xbuilds\release\usa\latest.txt) do set _BuildVer=%%n
set Pdrive=\\xbuilds\release\usa\%_BuildVer%
set _SETUP_FILE_PATH=%_NT386TREE%\dump
set _SETUP_TARGET_PATH=%_NT386TREE%
goto StartBuild

:AssumePDefined
set Pdrive=P:
set _SETUP_FILE_PATH=%_NT386TREE%\dump
set _SETUP_TARGET_PATH=%_NT386TREE%

goto StartBuild

:BuildMachine
If Not "%_BuildVer%"=="" Goto SkipUsage
Echo _BuildVer needs to be defined
Goto END

:SkipUsage
set Pdrive=%XBLD_PVT%\%_BuildVer%
set _SETUP_FILE_PATH=%Pdrive%\free\dump
set _SETUP_TARGET_PATH=%Pdrive%

:StartBuild
Echo XDK Setup Build Started at %Date% %Time%
if not "%XBLD_LOG%"=="" Echo XDK Setup Build Started at %Date% %Time% >> %XBLD_LOG%
Pushd %_NTBINDIR%\private\setup\xdk
if not "%XBLD_LOG%"=="" (
    %_NT386TREE%\dump\xpacker.exe %1 %2 %3 %4 xdk.ini >> %XBLD_LOG%
    ) else (
    %_NT386TREE%\dump\xpacker.exe %1 %2 %3 %4 xdk.ini
)
PopD
Echo XDK Setup Build Completed at %Date% %Time%
if not "%XBLD_LOG%"=="" Echo XDK Setup Build Completed at %Date% %Time% >> %XBLD_LOG%
Echo.
Echo.

set Pdrive=
set _SETUP_FILE_PATH=
set _SETUP_TARGET_PATH=

:END
