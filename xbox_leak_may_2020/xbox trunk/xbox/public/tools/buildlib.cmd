	@echo off

REM buildlib.cmd
REM Cool batch file to build a specific lib
REM Dan Rosenstein (dan@xbox.com) 2000

REM
REM check for parameters
REM

	if "%1"=="" goto USAGE

REM
REM switch to the directory and build
REM

	pushd %1
	build /c /z
	popd

REM
REM finish up
REM

	goto END

:USAGE

	echo "USAGE:   buildlib.cmd <LIB_DIR>"
	echo "EXAMPLE: buildlib.cmd c:\xbox\private\test\multimedia\xmo"

:END