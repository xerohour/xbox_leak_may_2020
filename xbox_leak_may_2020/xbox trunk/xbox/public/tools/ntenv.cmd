@if "%_echo%" == "" echo off
@rem
@rem If no drive has been specified for the NT development tree, assume
@rem C:.  To override this, place a SET _NTDRIVE=X: in your CONFIG.SYS
@rem
if "%_NTDRIVE%" == "" set _NTDRIVE=C:
@rem
@rem If no directory has been specified for the NT development tree, assume
@rem \nt.  To override this, place a SET _NTROOT=\xt in your CONFIG.SYS
@rem
if "%_NTROOT%" == "" set _NTROOT=\NT
set _NTBINDIR=%_NTDRIVE%%_NTROOT%
@rem This is being added for NTTEST projects.
set NTTESTENV=%_NTDRIVE%%_NTROOT%\NTTEST
@rem
@rem This command file assumes that the developer has already defined
@rem the USERNAME environment variable to match their email name (e.g.
@rem stevewo).
@rem
@rem We want to remember some environment variables so we can restore later
@rem if necessary (see NTUSER.CMD)
@rem
set _NTUSER=%USERNAME%
@rem
@rem Assume that the developer has already included \NT\PUBLIC\TOOLS
@rem in their path.  If not, then it is doubtful they got this far.
@rem
path %_NTBINDIR%\PUBLIC\OAK\BIN;%PATH%
@rem
@rem No hidden semantics of where to get libraries and include files.  All
@rem information is included in the command lines that invoke the compilers
@rem and linkers.
@rem
set LIB=
set INCLUDE=
@rem
@rem Setup default build parameters.
@rem
if "%BUILD_DEFAULT%" == ""         set BUILD_DEFAULT=ntoskrnl ntkrnlmp xbox daytona -e -nmake -i -w
if "%BUILD_DEFAULT_TARGETS%" == "" set BUILD_DEFAULT_TARGETS=-386
if "%BUILD_MAKE_PROGRAM%" == ""    set BUILD_MAKE_PROGRAM=nmake.exe
if "%BUILD_PRODUCT%" == ""     set BUILD_PRODUCT=NT
if "%BUILD_PRODUCT_VER%" == ""     set BUILD_PRODUCT_VER=500
@rem
@rem Setup default nmake parameters.
@rem
if "%NTMAKEENV%" == "" set NTMAKEENV=%_NTBINDIR%\PUBLIC\OAK\BIN
if "%COPYCMD%" == "" set COPYCMD=/Y
@rem
@rem By default, net uses are NOT persistent.  Do this here in case
@rem user wants to override in their setenv.cmd
@rem
net use /PER:NO >nul
@rem
@rem Setup the user specific environment information
@rem
call %_NTBINDIR%\PUBLIC\TOOLS\ntuser.cmd
@rem
@rem Optional parameters to this script are command line to execute
@rem
%1 %2 %3 %4 %5 %6 %7 %8 %9
