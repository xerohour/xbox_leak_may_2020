@echo off
if DEFINED _echo   echo on
if DEFINED verbose echo on

@rem
@rem Thierry 01/99
@rem
@rem MKKD: An attempt to make automatically the platform specific kd 
@rem       executables, dlls and extensions.
@rem 
@rem Usage: MKKD [-u] [Build_Arguments]
@rem 

set _MKKD_SDKLIB_UPDT=

:GETARG
for %%a in (./ .- .) do if ".%1." == "%%a?." goto USAGE

if /i "%1" == "/u"    set _MKKD_SDKLIB_UPDT=1&& goto SHIFTARG
if /i "%1" == "-u"    set _MKKD_SDKLIB_UPDT=1&& goto SHIFTARG
goto ENVCHECK

:SHIFTARG
shift
if NOT "%1" == "" goto GETARG
goto ENVCHECK

@rem ------------------------------------------------
@rem  Display Usage:
@rem ------------------------------------------------

:USAGE
echo.
echo Usage:  MKKD [-?] [-u] [Build_Arguments]
echo.
echo  Options:
echo.
echo     -?       - displays this help message.
echo.
echo     -u       - update sdk\lib\*\ with latest libs from build servers.
echo.
echo     [Build_Arguments] pass these arguments directly to build.
echo.
goto :EOF

@rem
@rem Environment check 
@rem
@rem Win64 Note: remember we are building KD64 w/ Win32 environments....
@rem

:ENVCHECK

echo.
set _MKKD_BUILD_ARGS=%1
if "%_TARGET%" == "i386"  goto BUILDKD_INIT
if "%_TARGET%" == "alpha" goto BUILDKD_INIT

if "%_TARGET%" == "" echo mkkd: ERROR - macro _TARGET should be defined like i386 or alpha&& echo. && goto :EOF
echo mkkd: ERROR - macro _TARGET should be defined to i386 or alpha&& echo. && goto :EOF

:BUILDKD_INIT
@rem
@rem Some IDW files are required to build the kernel debugger components.
@rem    - ksi386.inc, ksalpha.inc
@rem    - 
@rem

@rem
@rem Update libraries if requested
@rem

if NOT "%_MKKD_SDKLIB_UPDT%" == "" echo mkkd: copying libraries if newest... && xcopy /drek \\ntbuilds\release\usa\latest.tst\%PROCESSOR_ARCHITECTURE%\chk.pub\sdk\lib\%_TARGET% %_NTDRIVE%%_NTROOT%\public\sdk\lib\%_TARGET%

@rem
@rem ks%_TARGET%.inc is required
@rem

set _MKKD_TARGET_BLDSRV=\\x86chk
if "%_TARGET%" == "alpha" set _MKKD_TARGET_BLDSRV=\\alphachk

if NOT exist %_NTDRIVE%%_NTROOT%\public\sdk\inc\ks%_TARGET:i=%.inc echo mkkd: copying ks%_TARGET:i=% from %_MKKD_TARGET_BLDSRV%\sources... && copy %_MKKD_TARGET_BLDSRV%\sources\public\sdk\inc\ks%_TARGET:i=%.inc %_NTDRIVE%%_NTROOT%\public\sdk\inc

:BUILDKD
echo.
echo Building KD64...

@rem 
@rem build 
@rem

@rem
@rem Using a macro for saving/restoring current working directory instead of 
@rem using pushd/popd allows us to break in the failing directory.
@rem

set _MKKD_PREVCD=%CD%

@rem
@rem ntos\rtl\user\*\IMAGEDIR.OBJ is required for IMAGEHLP / DBGHELP
@rem

:IMAGEDIR
cd %_NTDRIVE%%_NTROOT%\private\ntos\rtl\user
if NOT exist obj\%_TARGET%\imagedir.obj (
   echo.
   echo imagedir.obj...
   echo.
   build %_MKKD_BUILD_ARGS%
)
if NOT exist %_NTDRIVE%%_NTROOT%\private\ntos\rtl\user\obj\%_TARGET%\imagedir.obj echo ntos\rtl\user\obj\%_TARGET%\imagedir.obj does not exist&& goto FAILED

@rem
@rem msdbi.dll is required for IMAGEHLP / DBGHELP
@rem

:MSDBI
cd %_NTDRIVE%%_NTROOT%\private\sdktools\vctools\pdb6
echo.
echo msdbi...
echo.
build %_MKKD_BUILD_ARGS%
if %ERRORLEVEL% GEQ 1 echo.&& echo building msdbi failed&& goto FAILED

@rem
@rem IMAGEHLP & DBGHLP
@rem

:IMAGEHLP
echo.
echo imagehlp...
echo.
cd %_NTROOT%\private\sdktools\imagehlp
build %_MKKD_BUILD_ARGS%
if %ERRORLEVEL% GEQ 1 echo.&& echo building imagehlp failed&& goto FAILED

@rem
@rem ntos\rtl\user\*\SPLAY.OBJ is required for NTSD64
@rem

:SPLAY
cd %_NTDRIVE%%_NTROOT%\private\ntos\rtl\user
if NOT exist obj\%_TARGET%\splay.obj (
   echo.
   echo splay.obj...
   echo.
   build %_MKKD_BUILD_ARGS%
)
if NOT exist %_NTDRIVE%%_NTROOT%\private\ntos\rtl\user\obj\%_TARGET%\splay.obj echo ntos\rtl\user\obj\%_TARGET%\splay.obj does not exist&& goto FAILED

@rem
@rem NTSD & KD 
@rem

:KD
echo.
echo kd...
echo.
cd %_NTROOT%\private\sdktools\NTSD64
build %_MKKD_BUILD_ARGS%
if %ERRORLEVEL% GEQ 1 echo.&& echo building kd failed&& goto FAILED

@rem
@rem KD Extensions
@rem

:KDEXTS
echo.
echo kdexts...
echo.
cd %_NTROOT%\private\sdktools\kdexts64
build %_MKKD_BUILD_ARGS%
if %ERRORLEVEL% GEQ 1 echo.&& echo building kdexts failed&& goto FAILED

@rem
@rem USERKD Extensions
@rem

:USERKDEXTS
echo.
echo userkdx...
echo.
cd %_NTROOT%\private\ntos\w32\ntuser\kdexts
build %_MKKD_BUILD_ARGS%
if %ERRORLEVEL% GEQ 1 echo.&& echo building userkdx failed&& goto FAILED

@rem
@rem NTSD Extensions
@rem

:NTSDEXTS
echo.
echo ntsdexts...
echo.
cd %_NTROOT%\private\sdktools\ntsdexts
build %_MKKD_BUILD_ARGS%
if %ERRORLEVEL% GEQ 1 echo.&& echo building userkdx failed&& goto FAILED

@rem
@rem WOW64 Extensions
@rem
:WOW64EXTS
echo.
echo wow64exts... 
echo.
echo WOW64 Extensions not built yet...
if %ERRORLEVEL% GEQ 1 echo.&& echo building wow64exts failed&& goto FAILED

:DONE
echo.
echo mkkd done.
echo.
cd /d %_MKKD_PREVCD%
goto :EOF

:FAILED
echo.
