setlocal
@echo off
if /i NOT "%_echo%" == ""   echo on
if /i NOT "%verbose%" == "" echo on
REM -------------------------------------------------------------------------
REM  drvcab.cmd - cab up the drivers for NT5 - VijeshS, owner
REM -------------------------------------------------------------------------


echo Genrating drvindex.inf

if /i "%PROCESSOR_ARCHITECTURE%" == "x86" set gencabarch=i386
if /i "%PROCESSOR_ARCHITECTURE%" == "alpha" set gencabarch=alpha

if /i "%LANGUAGE%" == "" set LANGUAGE=usa


REM ------------------------------------------------
REM  Get command-line options:
REM ------------------------------------------------

for %%a in (./ .- .) do if ".%1." == "%%a?."  goto Usage

if not "%1" == "" goto NotDefault
if "%bindrive%" == "" goto UseNTTREE
if "%binroot%" == "" goto UseNTTREE
goto UseBinDriveRoot
:UseNTTREE
if "%PROCESSOR_ARCHITECTURE%"=="x86"   goto X86
if "%PROCESSOR_ARCHITECTURE%"=="ALPHA" goto ALPHA
echo PROCESSOR_ARCHITECTURE not defined.
goto EXIT

:X86
set binplacedir=%_nt386tree%
goto GotBinplaceDir

:ALPHA
set binplacedir=%_ntalphatree%
if DEFINED AXP64 set binplacedir=%_ntaxp64tree%
goto GotBinplaceDir

:UseBinDriveRoot
set binplacedir=%bindrive%%binroot%

:GotBinplaceDir

pushd %binplacedir%

if not exist drvcab md drvcab

cd drvcab


if exist out.ddf del /q *.ddf
if exist sorted.lst del /q *.lst
if exist cabprep.err del /q *.err
if exist setup.inf del /q *.lst
if exist setup.rpt del /q *.err


echo ***Processing driver list

cabprep /s:%_NTBINDIR%\private\windows\setup\inf\win4\inf\lib\%language%\%gencabarch%\sorted.lst,driver,..\
copy /y drvindex.inf ..
copy /y drvindex.inf %_NTBINDIR%\private\windows\setup\inf\win4\inf\lib\%language%\%gencabarch%\

echo ***Genrating driver.cab

diamond /f out.ddf
move /y disk1\*.cab ..
rd /s/q disk1

echo ***Generating new dosnet.inf

cd /d %_NTBINDIR%\private\windows\setup\inf\win4\inf
build -z


popd

goto exit

:Usage
echo %0 <creates a driver cab file for your machine>

:exit
endlocal
