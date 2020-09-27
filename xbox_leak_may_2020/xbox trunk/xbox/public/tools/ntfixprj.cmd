@if "%_echo%"=="" echo off
setlocal ENABLEEXTENSIONS
if ERRORLEVEL 1 goto oldcmd

if "%_projects_cmd%" == "" set _projects_cmd=projects.cmd
if NOT "%1" == "" goto fixprojs
if "%NTPROJECTS%" == "" goto noprojs
call %0 %NTPROJECTS% %NTPROJECTS_EXTRA%
goto done

:fixprojs
set DIRCMD=
if "%_ntdrive%"=="" goto nodrive
if "%_ntroot%"=="" goto nodrive
for /F "usebackq tokens=6" %%i in (`dir %_ntdrive%\`) do set vol=%%i
if "%vol%" == "" goto novol

:fixloop
if "%1" == "" goto done
echo "Fixing %1 project"
call %_projects_cmd% %1
shift

if "%proj_path%"=="" goto badproject
if EXIST %proj_path%\slm.ini goto syncit
mkdir %proj_path% 2>nul

echo project = %project%>%_ntdrive%%proj_path%\slm.ini
echo %slm_root%| sed -e "s#\\#/#g" -e "s#\(.*\)#slm root = \1#" >>%_ntdrive%%proj_path%\slm.ini
echo %_ntdrive%%vol%%proj_path%| sed -e "s#\\#/#g" -e "s#\(.*\)#user root = //\1#" >>%_ntdrive%%proj_path%\slm.ini
echo sub dir = "/">>%_ntdrive%%proj_path%\slm.ini 
attrib +r +h %_ntdrive%%proj_path%\slm.ini

:syncit
call ntsync.cmd %project%
goto fixloop

:badproject
echo Bad project name
goto fixloop


:oldcmd
echo This command will not work with this version of cmd.exe
goto done
:nodrive
echo Must define _NTDRIVE and _NTROOT environment variables to use this command
goto done
:noprojs
echo Must define NTPROJECTS environment variable to use this command without arguments
goto done
:novol
echo NT source drive %_ntdrive% must have a volume label to use this command
goto done

:done
endlocal
