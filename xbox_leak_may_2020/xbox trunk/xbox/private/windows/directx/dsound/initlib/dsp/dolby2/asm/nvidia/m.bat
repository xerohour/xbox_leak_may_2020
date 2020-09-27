@echo off
rem ***************************************************************************
rem *   Unpublished work.  Copyright 1997-2000 Dolby Laboratories Inc
rem *   All Rights Reserved.
rem *
rem *   Workfile:   M.BAT
rem *
rem *   Contents:	Batch file to build various components for Dolby Game encoder
rem *
rem *   Revision:   1.0
rem *
rem *   Date:   24 Jul 1997 
rem *
rem *   Description:
rem *   For usage notes, type "m" with no other arguments.
rem *
rem ***************************************************************************

if "%1"=="l"		goto build_loader
if "%1"=="L"		goto build_loader
if "%1"=="m"		goto build_sysmem
if "%1"=="M"		goto build_sysmem
if "%1"=="s"		goto build_superexec
if "%1"=="S"		goto build_superexec
if "%1"=="sesim"	goto build_superexecsim


rem ***************************************************************************
rem	*	Error: invalid input, echo valid input options
echo **  m.bat usage: m target
echo **
echo **  Valid targets:
echo **
echo **  l:   Loader   (Game Encoder Loader only)
echo **  s:   SuperExec (Game Encoder SuperExec only)
echo **  sesim: SuperExec (Simulator-Version Only)
echo **  m:   System Memory (Game Encoder System Memory only)
echo **
echo **  You should build only one of the SuperExecs.
goto done

rem ***************************************************************************
rem *	Target shortcuts

rem * Game Encoder Modules

:build_loader

rem * Make the Loader
echo ** :build_loader
if exist encldr goto dir_ok_ldr
md encldr
:dir_ok_ldr
%MSDEV60%\nmake /nologo /f loader.mak encldr\loader.cld
goto make_done

:build_sysmem

rem * Make the System memory
echo ** :build_sysmem
if exist encsm goto dir_ok_sm
md encsm
:dir_ok_sm
%MSDEV60%\nmake /nologo /f sysmem.mak encsm\sysmem.cld
goto make_done

:build_superexec

rem * Make the Superexec (NOT the Motorola Simulator version)
echo ** :build_superexec
if exist encse goto dir_ok_se
md encse
:dir_ok_se
rem * Note: /A option is used since there are 2 possible builds (Moto and regular)
%MSDEV60%\nmake /A /nologo SIM_XBOX=0 /f suprexec.mak encse\suprexec.cld
goto make_done

:build_superexecsim

rem * Make the Superexec (Motorola Simulator version)
echo ** :build_superexecsim
if exist encse goto dir_ok_sesim
md encse
:dir_ok_sesim
rem * Note: /A option is used since there are 2 possible builds (Moto and regular)
%MSDEV60%\nmake /A /nologo SIM_XBOX=1 /f suprexec.mak encse\suprexec.cld
goto make_done

rem ***************************************************************************

:make_done

rem *	check for error, exit if error
if errorlevel 1 goto done
rem *	finished making executables with no problems
echo **
echo ** Finished
echo **

rem *	exit the batch file

:done
echo >nul
