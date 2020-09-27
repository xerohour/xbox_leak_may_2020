@if NOT DEFINED _echo echo off

REM ****************************
REM *** DON'T EDIT THIS FILE ***
REM ****************************

REM ***************************************************************************
REM *
REM * UpdateSelf.cmd
REM *
REM * Updates Razzle environment files.
REM *
REM ***************************************************************************

setlocal
if "%1" == "-?" goto USAGE
if NOT DEFINED _NTBinDir set _NTBinDir=\nt& echo Defaulting _NTBinDir to \nt

set _XCOPYSW=/devrsfy
set _XCOPYNODIRSW=/dvrfy

@echo on
xcopy %_XCOPYSW% \\orville\razzle\src\private\developr\Template\* %_NTBinDir%\private\developr\%USERNAME%\
@if NOT DEFINED _echo echo off

goto DONE

REM ***************************************************************************
REM * Usage
REM ***************************************************************************

:USAGE

echo.
echo UpdateSelf
echo.
echo    Updates Razzle environment files.
echo.
echo Notes:
echo    If _NTBinDir is not set, defaults to \nt.

:DONE
endlocal
