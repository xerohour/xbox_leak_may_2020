@Echo Off

REM The build machine is special
If /i "%ComputerName%"=="XBuilds" goto BuildMachine

REM Normally this batch file (when run anywhere but the build machine) uses the latest bits from the
REM \\xbuilds\release\usa.  If you want to override that you must set _BuildVer in the environment.
REM You also must map the P: drive to the location of your files, which must be laid out like those on the
REM public share.

for /f %%n in (\\xbuilds\release\usa\latest.txt) do set _BuildVer=%%n
set _BuildVerSetHere=Y
set Pdrive=%_NT386TREE%
set _SETUP_FILE_PATH=%_NT386TREE%\dump
set _SETUP_TARGET_PATH=%_NT386TREE%
Goto StartBuild

:BuildMachine
If Not "%_BuildVer%"=="" Goto SkipUsage
Echo _BuildVer needs to be defined
Goto END
:SkipUsage
set Pdrive=%XBLD_PVT%\%_BuildVer%\free
set _SETUP_FILE_PATH=%Pdrive%\dump
set _SETUP_TARGET_PATH=%XBLD_PVT%\%_BuildVer%

:StartBuild
if "%XBLD_LOG%"==""  goto nologging

Echo XBSE Setup Build Started at %Date% %Time%
Echo XBSE Setup Build Started at %Date% %Time% >> %XBLD_LOG%
Pushd %_NTBINDIR%\private\setup\xbse
%_NT386TREE%\dump\xpacker.exe %1 %2 %3 %4 xbse.ini >> %XBLD_LOG%
PopD
Echo XBSE Setup Build Completed at %Date% %Time%
Echo XBSE Setup Build Completed at %Date% %Time% >> %XBLD_LOG%
Echo.
Echo.

goto builddone

:nologging
Echo XBSE Setup Build Started at %Date% %Time%
Pushd %_NTBINDIR%\private\setup\xbse
%_NT386TREE%\dump\xpacker.exe %1 %2 %3 %4 xbse.ini
PopD
Echo XBSE Setup Build Completed at %Date% %Time%
Echo

:builddone

set Pdrive=
set _SETUP_FILE_PATH=
set _SETUP_TARGET_PATH=
If Not "%_BuildVerSetHere%"=="Y" goto END
set _BuildVerSetHere=
set _BuildVer=


:END
