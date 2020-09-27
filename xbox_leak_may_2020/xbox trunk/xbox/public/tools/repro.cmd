	@echo off

REM repro.cmd
REM Cool batch file to repro a bug using the harness
REM Dan Rosenstein (dan@xbox.com) 2000

REM
REM check for parameters
REM

	if "%1"=="" goto USAGE
	if "%2"=="" goto USAGE

REM
REM set up the bug id
REM

	set BUGID=%1
	set XBOXID=%2

REM 
REM build the intermediate libs
REM

	for /f %%f in (%_NTBINDIR%\private\test\bugs\%BUGID%\libs.txt) do call buildlib.cmd %%f

REM
REM build and copy the harness
REM
	call buildlib.cmd %_NTBINDIR%\private\test\buildxbe\harness
	xbcp /f /x %XBOXID% %_NT386TREE%\dump\harness.xbe xe:\%BUGID%.xbe

REM
REM make the tree
REM

	for /f %%f in (%_NTBINDIR%\private\test\bugs\%BUGID%\dirs.txt) do xbmkdir /x %XBOXID% %%f

REM
REM copy over the ini
REM

	xbcp /f /x %XBOXID% %_NTBINDIR%\private\test\bugs\%BUGID%\testini.ini xc:\tdata\a7049955
	
REM
REM copy over the media files
REM
	for /f "eol=; tokens=1,2 delims=, " %%i in (%_NTBINDIR%\private\test\bugs\%BUGID%\media.txt) do xbcp /f /x %XBOXID% %_NTBINDIR%\private\%%i %%j

REM
REM finish up
REM

	xbreboot xe:\%BUGID%.xbe

	goto END

REM
REM how to use the batch file
REM

:USAGE
	echo "USAGE:   repro.cmd <BUG_ID> <XBOX_NAME>"
	echo "EXAMPLE: repro.cmd 5668 danx"

:END


	
