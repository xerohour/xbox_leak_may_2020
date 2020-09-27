@if "%_ECHO%" == "" @echo off

if     "%1" == "" goto USAGE
if NOT "%3" == "" goto USAGE

if "%COVERAGE%" == "" set COVERAGE=c:\coverage

if not exist "%COVERAGE%\tools\bbcover.exe" goto REINSTALL

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

set SOURCE_FILE_PATH_AND_NAME=%1
set TARGET_PATH=%2

if not exist %SOURCE_FILE_PATH_AND_NAME% goto USAGE

if not "%TARGET_PATH%" == "" if not exist %TARGET_PATH%\* goto USAGE

set FSPLIT_FILENAME=
call fsplit %SOURCE_FILE_PATH_AND_NAME% >"%temp%\fsplit1.bat"
if not exist "%temp%\fsplit1.bat" goto ERROR1
call "%temp%\fsplit1.bat"
if "%FSPLIT_FILENAME%" == "" goto ERROR1

set SOURCE_PATH=%FSPLIT_PATH%
set BINARY_NAME=%FSPLIT_FILENAME%
set BINARY_PDB=%FSPLIT_BASENAME%.pdb
set BINARY_CMD=%FSPLIT_BASENAME%.bbtf

if not exist %SOURCE_PATH%\%BINARY_CMD% set BINARY_CMD=%FSPLIT_BASENAME%.cmdf

if not exist %SOURCE_PATH%\%BINARY_NAME% goto ERROR1
if not exist %SOURCE_PATH%\%BINARY_PDB%  set BINARY_PDB=
if not exist %SOURCE_PATH%\%BINARY_CMD%  set BINARY_CMD=

if not exist %COVERAGE%\%BINARY_NAME%\* mkdir %COVERAGE%\%BINARY_NAME%
if not exist %COVERAGE%\%BINARY_NAME%\* goto ERROR2

%PUSHDCMD% %COVERAGE%\%BINARY_NAME%

if NOT "%BINARY_NAME%" == "" xcopy /d %SOURCE_PATH%\%BINARY_NAME%
if NOT "%BINARY_PDB%"  == "" xcopy /d %SOURCE_PATH%\%BINARY_PDB%
if NOT "%BINARY_CMD%"  == "" xcopy /d %SOURCE_PATH%\%BINARY_CMD%

if exist %BINARY_NAME%.instr del /q %BINARY_NAME%.instr

set BBCOVERCMD=bbcover /i %BINARY_NAME% /NoImportDll

if not "%BINARY_CMD%" == "" set BBCOVERCMD=%BBCOVERCMD% /cmd %BINARY_CMD%

call %BBCOVERCMD%

if exist %BINARY_NAME%.instr goto INSTR_SUCCESS

@echo **********************************
@echo *                                *
@echo *  Failed to instrument binary.  *
@echo *                                *
@echo **********************************

goto END

:INSTR_SUCCESS

if "%TARGET_PATH%" == "" goto INSTR_ONLY

if exist %TARGET_PATH%\%BINARY_NAME%.original~ del %TARGET_PATH%\%BINARY_NAME%.original~
if exist %TARGET_PATH%\%BINARY_NAME%.original  ren %TARGET_PATH%\%BINARY_NAME%.original %BINARY_NAME%.original~
if exist %TARGET_PATH%\%BINARY_NAME%           ren %TARGET_PATH%\%BINARY_NAME%          %BINARY_NAME%.original
copy %BINARY_NAME%.instr %TARGET_PATH%\%BINARY_NAME%

@echo *********************************************************************
@echo *                                                                   *
@echo *  Binary instrumented and installed.  Continue with BINPREP.BAT    *
@echo *  with more binaries if desired, or start running test scenarios.  *
@echo *                                                                   *
@echo *  If you replaced a system binary, you should reboot before        *
@echo *  running test scenarios.                                          *
@echo *                                                                   *
@echo *  Once finished running test scenarios (or however often desired   *
@echo *  while running test scenarios), run ^"COVERCMD /Save^" to save      *
@echo *  the coverage results to disk.                                    *
@echo *                                                                   *
@echo *  Once results are saved to disk, run REPORTS.BAT to create the    *
@echo *  coverage reports.                                                *
@echo *                                                                   *
@echo *********************************************************************

goto END

:INSTR_ONLY

@echo *********************************************************************
@echo *                                                                   *
@echo *  Binary instrumented successfully.                                *
@echo *                                                                   *
@echo *  Before running test scenarios, you must copy the instrumented    *
@echo *  binary to the location where it will be run and rename it to     *
@echo *  the original binary's name.  If it is a system binary, you will  *
@echo *  need to reboot before running test scenarios.                    *
@echo *                                                                   *
@echo *  Once finished running test scenarios (or however often desired   *
@echo *  while running test scenarios), run ^"COVERCMD /Save^" to save      *
@echo *  the coverage results to disk.                                    *
@echo *                                                                   *
@echo *  Once results are saved to disk, run REPORTS.BAT to create the    *
@echo *  coverage reports.                                                *
@echo *                                                                   *
@echo *********************************************************************

goto END

:REINSTALL

@echo *****************************************************************************
@echo *                                                                           *
@echo *  Coverage instrumentation tool missing.  Please reinstall coverage tools  *
@echo *                                                                           *
@echo *****************************************************************************

goto END

:USAGE

@echo *********************************************************************
@echo *                                                                   *
@echo *  USAGE:    BINPREP sourcepath\filename [targetpath]               *
@echo *                                                                   *
@echo *  EXAMPLES:                                                        *
@echo *                                                                   *
@echo *    BINPREP \\x86fre\binaries.ns\explorer.exe                      *
@echo *                                                                   *
@echo *    BINPREP \\x86fre\binaries.ns\explorer.exe c:\winnt\system32    *
@echo *                                                                   *
@echo *  The .pdb file is assumed to exist in the same directory as the   *
@echo *  source file.  If a .bbtf file exists in the same directory as    *
@echo *  the source file, it is also used.                                *
@echo *                                                                   *
@echo *  If targetpath is specified, the instrumented binary is copied    *
@echo *  to that directory.                                               *
@echo *                                                                   *
@echo *********************************************************************

goto END

:ERROR1

@echo *********************************************************************
@echo *                                                                   *
@echo *  Failed to parse filename %1
@echo *                                                                   *
@echo *********************************************************************

:ERROR2

@echo *********************************************************************
@echo *                                                                   *
@echo *  Failed to create directory %COVERAGE%\%BINARY_NAME%.
@echo *                                                                   *
@echo *********************************************************************

:END

endlocal
%POPDCMD%

