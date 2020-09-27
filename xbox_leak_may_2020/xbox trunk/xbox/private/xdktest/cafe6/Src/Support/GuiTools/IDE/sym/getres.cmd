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
echo    Syncs and copies various resource files to this directory.  Checking
echo    out and in must be done manually.
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

cd %SUSHIDIR%\appwz
ssync -u resource.h
cd %SUSHIDIR%\vcpp32\rsc
ssync -u resource.h
cd %SUSHIDIR%\vproj
ssync -u resource.h
cd %SUSHIDIR%\vproj\common
ssync -u *.opt
cd %SUSHIDIR%\vproj\mac68k
ssync -u *.opt
cd %SUSHIDIR%\vres
ssync -u resource.h
cd %SUSHIDIR%\vshell
ssync -u resource.h
cd %SUSHIDIR%\include
ssync -u shrdres.h
ssync -u cmds.h
cd %WIZDIR%\clswiz
ssync -u resource.h

echo.
echo Copying the resources to %GUIQADIR%\cafe\wb\sym
echo.

cd %GUIQADIR%\cafe\wb\sym

out appwz.h vcpp32.h vproj.h optn*.h 68k\optn*.h vres.h vshell.h clswiz.h shrdres.h cmdids.h

copy %SUSHIDIR%\appwz\resource.h         appwz.h
copy %SUSHIDIR%\vcpp32\rsc\resource.h    vcpp32.h
copy %SUSHIDIR%\vproj\resource.h         vproj.h
copy %SUSHIDIR%\vproj\common\*.opt       *.h
copy %SUSHIDIR%\vproj\mac68k\*.opt       68k\*.h
copy %SUSHIDIR%\vres\resource.h          vres.h
copy %SUSHIDIR%\vshell\resource.h        vshell.h
copy %SUSHIDIR%\include\shrdres.h        shrdres.h
copy %SUSHIDIR%\include\cmds.h           cmdids.h
copy %WIZDIR%\clswiz\resource.h          clswiz.h

set GUIQADIR=
set SUSHIDIR=
set WIZDIR=

echo.
echo Parsing resource files and added prefixes to id's
echo.

parseres -s APPWZ_    appwz.h    appwz.h2
parseres -s CLSWIZ_   clswiz.h   clswiz.h2
parseres -s VCPP32_   vcpp32.h   vcpp32.h2
parseres -s VPROJ_    vproj.h    vproj.h2
parseres -s VRES_     vres.h     vres.h2
parseres -s VSHELL_   vshell.h   vshell.h2
parseres -s SHRDRES_  shrdres.h  shrdres.h2
parseres -s OPTNBSC_  optnbsc.h  optnbsc.h2
parseres -s OPTNCPLR_ optncplr.h optncplr.h2
parseres -s OPTNLIB_  optnlib.h  optnlib.h2
parseres -s OPTNLINK_ optnlink.h optnlink.h2
parseres -s OPTNMTL_  optnmtl.h  optnmtl.h2
parseres -s OPTNRC_   optnrc.h   optnrc.h2
parseres -s OPTNRC_   68k\optnmrc.h   68k\optnmrc.h2
parseres -s OPTNCPLR_ 68k\optncplr.h  68k\optncplr.h2
parseres -s OPTNLINK_ 68k\optnlink.h  68k\optnlink.h2

echo.
echo Moving the resources with prefixes id's to headers files
echo.

copy appwz.h2    appwz.h
copy clswiz.h2   clswiz.h
copy vcpp32.h2   vcpp32.h
copy vproj.h2    vproj.h
copy vres.h2     vres.h
copy vshell.h2   vshell.h
copy shrdres.h2  shrdres.h
copy optnbsc.h2  optnbsc.h
copy optncplr.h2 optncplr.h
copy optnlib.h2  optnlib.h
copy optnlink.h2 optnlink.h
copy optnmtl.h2  optnmtl.h
copy optnrc.h2   optnrc.h
copy 68k\optnmrc.h2  68k\optnmrc.h
copy 68k\optncplr.h2 68k\optncplr.h
copy 68k\optnlink.h2 68k\optnlink.h

del *.h2
del 68k\*.h2

echo.
echo Resource updating is complete.
echo.
echo Please ensure that the CAFE libraries build
echo before checking in the new resources.

:end
