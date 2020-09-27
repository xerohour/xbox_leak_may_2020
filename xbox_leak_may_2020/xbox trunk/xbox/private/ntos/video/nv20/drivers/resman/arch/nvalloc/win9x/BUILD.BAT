REM Master Build Script for building the NV3/NV4/NV10 NVALLOC Resource Manager
REM USE "build 3 a" for a release build with clean for NV3.

@echo off
cls
SET BUILDLOG=logfile
del logfile

set IKOS=

IF "%1"=="3" goto nv3bld
IF "%1"=="4" goto nv4bld
IF "%1"=="a" goto nvAbld
IF "%1"=="A" goto nvAbld

ECHO Invalid choice.
goto menu

:nv3bld
ECHO Building for NV3.
set NV3=1
set NV4=
set NV10=
set NVAFX=
goto parm3

:nv4bld
ECHO Building for NV4.
set NV3=
set NV4=1
set NV10=
set NVAFX=
goto parm3

:nvAbld
ECHO Building for NV4 (AUDIO)
set NV3=
set NV4=1
set NV10=
set NVAFX=1

REM
REM build the DSP effects header files
REM

IF "%BUILDDIR%" == "" goto no_builddir
IF "%BUILD_TOOLS_DIR%" == "" goto no_buildtooldir

call %BUILDDIR%\main\drivers\resman\arch\nvalloc\audio\dspbld.bat
cd %BUILDDIR%\main\drivers\resman\arch\nvalloc\win9x

goto parm3

REM
REM Setup nmsyms source files.
REM
REM The vxd is too big now to load all of the source
REM info for all of the files, so we use these switches
REM to limit the set of files we include in the .nms.
REM
:parm3
set NVDFLTSYMS=1
set NV4SYMS=
set NV10SYMS=
set NV20SYMS=
IF "%3"=="" goto parm2
IF "%3"=="nv4syms" goto nv4syms
IF "%3"=="nv10syms" goto nv10syms
IF "%3"=="nv20syms" goto nv20syms
:nv4syms
set NV4SYMS=1
set NVDFLTSYMS=
goto parm2
:nv10syms
set NV10SYMS=1
set NVDFLTSYMS=
goto parm2
:nv20syms
set NV20SYMS=1
set NVDFLTSYMS=
goto parm2

:parm2
IF %2==a goto ret_cln
IF %2==A goto ret_cln
IF %2==b goto ret
IF %2==B goto ret
IF %2==c goto dev_cln
IF %2==C goto dev_cln
IF %2==d goto dev
IF %2==D goto dev
IF %2==e goto dbg_cln
IF %2==E goto dbg_cln
IF %2==f goto dbg
IF %2==F goto dbg
IF %2==g goto ret_nt
IF %2==G goto ret_nt
IF %2==h goto dbg_nt
IF %2==H goto dbg_nt
IF %2==i goto dbg_cln_ikos
IF %2==I goto dbg_cln_ikos
IF %2==j goto dbg_ikos
IF %2==J goto dbg_ikos
IF %2==k goto dbg_oem_cln
IF %2==K goto dbg_oem_cln
IF %2==l goto dbg_oem
IF %2==L goto dbg_oem

rem No params given. Get it interactively.
:menu
@echo off
cls
ECHO Master Build Script for building the NV3/NV4 NVALLOC Resource Manager
ECHO *************************************************************************
ECHO *************************************************************************
ECHO 3) NV3
ECHO 4) NV4
ECHO A) NV4 (AUDIO)
ECHO *************************************************************************
ECHO *************************************************************************
choice "Choose the target chip:" /c34

if errorlevel 2 goto bldnv4

ECHO Building for NV3.
set NV3=1
set NV4=
set NV10=
goto bldtyp

:bldnv4
ECHO Building for NV4.
set NV3=
set NV4=1
set NV10=
goto bldtyp

:bldtyp
ECHO *************************************************************************
ECHO *************************************************************************
ECHO A) Retail (with clean)
ECHO B) Retail (without clean)
ECHO C) Developer (with clean)
ECHO D) Developer (without clean)
ECHO E) Debug (with clean)
ECHO F) Debug (without clean)
ECHO G) Retail NT (with clean)
ECHO H) Debug NT (with clean)
ECHO I) IKOS Debug (with clean)
ECHO J) IKOS Debug (without clean)
ECHO K) OEM Debug (with clean)
ECHO L) OEM Debug (without clean)
ECHO *************************************************************************
ECHO *************************************************************************
choice "Choose a build script method:" /cABCDEFGHIJKL 

if errorlevel 12 goto dbg_oem
if errorlevel 11 goto dbg_oem_cln
if errorlevel 10 goto dbg_ikos
if errorlevel 9  goto dbg_cln_ikos
if errorlevel 8  goto dbg_nt
if errorlevel 7  goto ret_nt
if errorlevel 6  goto dbg
if errorlevel 5  goto dbg_cln
if errorlevel 4  goto dev
if errorlevel 3  goto dev_cln
if errorlevel 2  goto ret
if errorlevel 1  goto ret_cln

:ret_cln
ECHO Running NMAKE CLEAN.
set DEBUG=
set DEVELOPER=
set EXCLUDE_SRCS=
tools\nmake clean
:ret
ECHO Building a RETAIL VERSION of the Resource Manager.
set DEBUG=
set DEVELOPER=
set EXCLUDE_SRCS=
tools\nmake >> %BUILDLOG% 
goto continue

:dev_cln
ECHO Running NMAKE CLEAN.
set DEBUG=
set DEVELOPER=1
set EXCLUDE_SRCS=
tools\nmake clean
:dev
ECHO Building a DEVELOPER VERSION of the Resource Manager.
set DEBUG=
set DEVELOPER=1
set EXCLUDE_SRCS=
tools\nmake >> %BUILDLOG% 
goto continue

:dbg_cln_ikos
set IKOS=1
:dbg_cln
ECHO Running NMAKE CLEAN.
set DEBUG=1
set DEVELOPER=
set EXCLUDE_SRCS=
tools\nmake NMSFILES=%NMSFILES% clean
goto dbg

:dbg_ikos
set IKOS=1
:dbg
ECHO Building a DEBUG VERSION Resource Manager.
set DEBUG=1
set DEVELOPER=
set EXCLUDE_SRCS=
tools\nmake NMSFILES=%NMSFILES% >> %BUILDLOG% 
goto continue

:ret_nt
IF "%BUILDDIR%" == "" goto no_builddir
IF "%NV3%" == "" goto retnv4_nt
ECHO Building a RETAIL WinNT4 NV3 Miniport Driver
call ..\..\..\..\display\winnt4\bldmin9x.bat free clean nv3
goto continue
:retnv4_nt
ECHO Building a RETAIL WinNT4 NV4 Miniport Driver
call ..\..\..\..\display\winnt4\bldmin9x.bat free clean nv4
goto continue

:dbg_nt
IF "%BUILDDIR%" == "" goto no_builddir
IF "%NV3%" == "" goto dbgnv4_nt
ECHO Building a DEBUG WinNT4 NV3 Miniport Driver
call ..\..\..\..\display\winnt4\bldmin9x.bat checked clean nv3
goto continue
:dbgnv4_nt
ECHO Building a DEBUG WinNT4 NV4 Miniport Driver
call ..\..\..\..\display\winnt4\bldmin9x.bat checked clean nv4
goto continue

:dbg_oem_cln
ECHO Running NMAKE CLEAN.
set DEBUG=
set DEVELOPER=1
set EXCLUDE_SRCS=1
tools\nmake clean
:dbg_oem
ECHO Building an OEM DEBUG VERSION of the Resource Manager.
set DEBUG=
set DEVELOPER=1
set EXCLUDE_SRCS=1
tools\nmake >> %BUILDLOG% 
goto continue

:no_buildtooldir
ECHO For Audio builds, you need to set your BUILD_TOOLS_DIR variable
goto continue

:no_builddir
ECHO For NT4 and Audio builds, you need to set your BUILDDIR variable

:continue
rem undefine all the environment variables set
set INCLUDE=
set LIB=
set NV3=
set NV4=
set DEBUG=
set DEVELOPER=
set IKOS=

IF ERRORLEVEL 2 GOTO FAIL
ECHO SUCCESS: Resource Manager Build Successful.
GOTO EXIT
:FAIL
ECHO %FILE% DIDN'T COMPILE >> %ERRORLOG%
PAUSE
:EXIT
rem call oldpath.bat


