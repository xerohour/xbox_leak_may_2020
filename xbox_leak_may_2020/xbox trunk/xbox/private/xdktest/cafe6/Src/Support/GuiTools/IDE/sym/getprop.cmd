@echo off
rem
rem Changing defaults in this file:
rem
rem     1. Modify SUSHIDIR, WIZDIR, and GUIQADIR to your defaults
rem     2. Enable the following "goto begin" statement

rem goto begin

if not .%3==. goto begin
echo Usage:  %0 sushidir wizdir guiqadir
echo.
echo Purpose:
echo    Syncs and copies various property resource files to this directory.
echo    Checking out and in must be done manually.
echo.
echo    %0 requires these arguments and assumes these enlistments:
echo.
echo        sushidir = \\ikura\slm!src\sushi
echo        wizdir   = \\objects\dart!slm\src\wizards
echo        guiqadir = \\sl-test\dolphin!slm\src\guiqa
echo.
echo    Eg. %0 d:\sushi d:\src\wiz d:\testsrc\guiqa
echo.
echo    If any of these enlistments are the root, then use only the
echo    drive letter followed by a colon (i.e. c:  instead of c:\)
echo.
echo    All directories must be on the same drive, otherwise modify this
echo    batch file by hand.
goto end

:begin

if %1.==. goto sushi_default
set SUSHIDIR=%1
goto sushi_end
:sushi_default
set SUSHIDIR=f:\sushi
:sushi_end

if %2.==. goto wiz_default
set WIZDIR=%2
goto wiz_end
:wiz_default
set WIZDIR=f:\wizards
:wiz_end

if %3.==. goto guiqa_default
set GUIQADIR=%3
goto guiqa_end
:guiqa_default
set GUIQADIR=f:\guiqa
:guiqa_end

echo.
echo Ssync'ing and unghosting specific resource files
echo.
cd %SUSHIDIR%\include
ssync -u projprop.h props.h slob.h

echo.
echo Copying the property resources to %GUIQADIR%\cafe\wb\sym
echo.
cd %GUIQADIR%\cafe\wb\sym

out projprop.h props.h slob.h

copy %SUSHIDIR%\include\projprop.h
copy %SUSHIDIR%\include\props.h
copy %SUSHIDIR%\include\slob.h

set GUIQADIR=
set SUSHIDIR=
set WIZDIR=

echo.
echo Copying is complete.
echo.
echo Please ensure that the CAFE libraries build
echo before checking in the property resources.

:end