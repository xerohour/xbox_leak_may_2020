@if "%_ECHO%" == "" @echo off

if "%1" == "" goto USAGE

set BINARY_NAME=%1

if "%COVERAGE%" == "" set COVERAGE=c:\coverage

if not exist %COVERAGE%\tools\bbcovrpt.exe          goto REINSTALL
if not exist %COVERAGE%\%BINARY_NAME%\%BINARY_NAME% goto BINARYNOTEXIST

REM This is for making sure we don't add too many c:\coverage\tools to the path
REM for %%i in (bbcovrpt.exe) do if not exist %%~PATH:i set path=%path%;%coverage%\tools

if "%RUNACOVERAGEBAT%" == "" set path=%PATH%;%COVERAGE%\tools
set RUNACOVERAGEBAT=hey

setlocal

REM This is to maintain compatibility with Win9x
if "%OS%" == "Windows_NT" goto WINNT
set PUSHDCMD=cd
set POPDCMD=
goto JUSTGO

:WINNT
set PUSHDCMD=pushd
set POPDCMD=popd

:JUSTGO

%PUSHDCMD% %COVERAGE%\%BINARY_NAME%

if not exist %BINARY_NAME%.covdata goto NOCOVDATA

bbcovrpt /i %BINARY_NAME% /SummaryReport /SourceReport /Src *\xbox\private* /Src *\xbox\public* /SrcSubst d:\nt=\\index1\win2ksrc

goto END

:REINSTALL

@echo ***********************************************************************
@echo *                                                                     *
@echo *  Coverage reporting tool missing.  Please reinstall coverage tools  *
@echo *                                                                     *
@echo ***********************************************************************

goto END

:BINARYNOTEXIST

@echo *******************************************************************
@echo *
@echo *  The %binary_name% and .pdb files must be in the
@echo *    %coverage%\%binary_name% directory!
@echo *
@echo *******************************************************************

goto END

:USAGE

@echo *********************************
@echo *                               *
@echo *  USAGE:  REPORTS binaryname   *
@echo *                               *
@echo *********************************

goto END

:NOCOVDATA

@echo **********************************************************
@echo *                                                        *
@echo *  No .covdata file for binary.  Use ^"COVERCMD /Save^"    *
@echo *                                                        *
@echo **********************************************************

:END

endlocal
%POPDCMD%

