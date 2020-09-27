@echo off
REM
REM Batch Script to build NT 4.0/5.0 Miniport and Display drivers.
REM Usage:
REM     blddisp <free,checked> <clean> <dbg> <nv3> <nv4> <dx6> <drv>
REM             free - Do a free (retail) build
REM             checked - Do a checked (debug) build
REM             clean - Do a complete rebuild
REM             dbg - do a free build with source level debugging
REM             dx6 - build dx6 driver support using Win9x source tree
REM             drv - build miniport/dx/display driver only, no RM
REM
REM     Defaults to a free build, only rebuild out of date components.
REM     If present, type of build must preceed clean in parameter list.
REM     The other parameters are not order dependent.
REM     If nv3 or nv4 is not specified, script will build both.
REM     dx6 parameter is valid for NT5 builds only.
REM
REM     This build script uses the following env vars:
REM      BUILDDIR = top level of source tree. required.
REM      RELEASE_BRANCH = branch to use for build. Default is MAIN.
REM      BUILD_TOOLS_DIR = top level tools dir. Default is %BUILDDIR%\tools.
REM      BUILD_DDK_DIR = ddk dir. Default is %BUILD_TOOLS_DIR%\ddk.
REM      BUILD_SDK_DIR = sdk dir (static sdk only). Default is %BUILD_TOOLS_DIR%\sdk.
REM      BUILD_DDK_VER = set to nt5 for nt5 driver builds. Default is nt4.
REM      BUILD_NVSDK_DIR = NVidia sdk dir. Default is %BUILDDIR%\%RELEASE_BRANCH%\sdk\nvidia.
REM      _NT_SYMBOL_PATH = If defined and this is a debug build, copy binaries to this dir for WinDBG debugger.
REM      DEBUG_TARGET_DIR = optional. pathname to %SystemRoot% of your debug
REM                        system. Driver binaries/sym files are copied here.

REM
REM Setup global env vars for source tree, tools
REM Save common MSVC/Win32 SDK/NT DDK related env vars
REM
set SAVEDDKDRIVE=%DDKDRIVE%
set SAVEMSDEV=%MSDEV%
set SAVEMSDEVDIR=%MSDEVDIR%
set SAVEMSVCDIR=%MSVCDIR%
set SAVEMSTOOLS=%MSTOOLS%
set SAVEBASEDIR=%BASEDIR%
set SAVENTROOT=%_NTROOT%

REM
REM Save env variables used by this batch file.
REM
set SAVE_NT_BUILD_NV3=%NT_BUILD_NV3%
set SAVE_NT_BUILD_NV4=%NT_BUILD_NV4%
set SAVE_BUILD_TREE=%BUILD_TREE%
set SAVE_RELEASE_BRANCH=%RELEASE_BRANCH%
set SAVE_BUILD_NVSDK_DIR=%BUILD_NVSDK_DIR%

REM
REM Clear env variables set and used by NT5 builds which can mess up
REM NT4 builds.
REM
set BUILD_ALT_DIR=
set SDK_INC_PATH=
set SDK_LIB_DEST=
set SDK_LIB_PATH=
set COFFBASE_TXT_FILE=
set CRT_INC_PATH=
set CRT_LIB_PATH=
set DDK_INC_PATH=
set DDK_LIB_DEST=
set DDK_LIB_PATH=
set OAK_INC_PATH=
set WDM_INC_PATH=

REM
REM Setup some environment ... make sure to setup BUILDDIR
REM
set BUILD_DDK_VER=winnt4
set PROCESSOR_ARCHITECTURE=x86


if not "%BUILDDIR%"=="" goto builddir_ok
echo . >> %BUILDLOG%
echo *** Error. BUILDDIR environment variable not set. >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:builddir_ok
if not "%BUILD_TOOLS_DIR%"=="" goto toolsdir_ok
set BUILD_TOOLS_DIR=%BUILDDIR%\tools

:toolsdir_ok
if not "%BUILD_SDK_DIR%"=="" goto sdkdir_ok
set BUILD_SDK_DIR=%BUILD_TOOLS_DIR%\sdk

:sdkdir_ok
if not "%BUILD_DDK_DIR%"=="" goto ddkdir_ok
set BUILD_DDK_DIR=%BUILD_TOOLS_DIR%\ddk

:ddkdir_ok
if not "%BUILD_DDK_VER%"=="" goto ddkver_ok
set BUILD_DDK_VER=nt4

:ddkver_ok
if not "%RELEASE_BRANCH%"=="" goto branchname_ok
set RELEASE_BRANCH=main

:branchname_ok
REM
REM We now know the branch, define BUILD_TREE as top of source tree.
REM
set BUILD_TREE=%BUILDDIR%\%RELEASE_BRANCH%
set SAVE_BUILD_NVSDK_DIR=%BUILD_NVSDK_DIR%
if not "%BUILD_NVSDK_DIR%"=="" goto nvsdkdir_ok
set BUILD_NVSDK_DIR=%BUILD_TREE%\sdk\nvidia

:nvsdkdir_ok
REM
REM Setup enviroment vars needed for DDK environment
REM
set SRCDBG=
set DDKDRIVE=
set MSDEV=%BUILD_TOOLS_DIR%\msvc42
set MSDEVDIR=%MSDEV%
set MSVCDIR=
set NT_BUILD_NV3=
set NT_BUILD_NV4=
set NVD3D_BUILD_DEFS=
set NVBUILDOPT=
if not "%BUILD_DDK_VER%"=="nt5" goto mstools_4
set MSTOOLS=%BUILD_SDK_DIR%\win32.nt5
set NVBUILDOPT=-D -nmake NT5=1
goto save_env
:mstools_4
set MSTOOLS=%BUILD_SDK_DIR%\win32

:save_env
set SAVEINCLUDE=%INCLUDE%
set SAVELIB=%LIB%
set SAVEPATH=%PATH%
set SRCPATH=%BUILD_TREE%\drivers\display\%BUILD_DDK_VER%

:checkbindirs
if not exist %SRCPATH%\bin mkdir %SRCPATH%\bin 2> nul >nul
if not exist %SRCPATH%\bin\i386 mkdir %SRCPATH%\bin\i386 2> nul >nul
if not exist %SRCPATH%\bin\i386\checked mkdir %SRCPATH%\bin\i386\checked 2> nul >nul
if not exist %SRCPATH%\bin\i386\free mkdir %SRCPATH%\bin\i386\free 2> nul >nul

:setbasedir
REM
REM Use %BUILD_DDK_DIR%\nt4 rather than %BUILD_DDK_DIR%\%BUILD_DDK_VER% 
REM the win9x client won't match the way winnt Perforce clients are mapped
REM
set BASEDIR=%BUILD_DDK_DIR%\nt4
REM NT5 DDK needs _NTROOT for path to build.dat - doesn't hurt to do it for NT4 either
set _NTROOT=%BASEDIR%

REM
REM Set target dir for products of this build.
REM
set TARGETPATH=%SRCPATH%\bin
set SAVE_BUILD_RELEASE_DIR=%BUILD_RELEASE_DIR%
if not "%BUILD_RELEASE_DIR%"=="" goto get_buildlog
set BUILD_RELEASE_DIR=%SRCPATH%\install

:get_buildlog
REM
REM Direct output of this build.
REM
if not "%BUILDLOG%"=="" goto ddkenv_mods
set BUILDLOG=con:

:ddkenv_mods
REM
REM Additional env mods for NT4 DDK Only
REM
set PATH=%BASEDIR%\bin;%MSDEV%\bin;%MSTOOLS%\bin;%PATH%
set INCLUDE=%MSDEV%\inc;%MSTOOLS%\include
set LIB=%MSDEV%\lib;%MSTOOLS%\lib

:env
REM
REM Setup DDK environment by invoking MS supplied DDK batch script
REM
set BLDENV=free
if "%1"=="checked" goto setenvtype
if "%1"=="free" goto setenvtype
if "%1"=="" goto setupenv
if "%1"=="clean" goto setupenv
echo . >> %BUILDLOG%
echo *** Error setting up DDK build env. Usage: blddisp <free | checked>. *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:setenvtype
set BLDENV=%1
set NVBUILDOPT=%NVBUILDOPT% -nmake DDKBUILDENV=%BLDENV%
:setupenv
call %BASEDIR%\bin\setenv.bat %BASEDIR% %BLDENV%
set TARGETDIR=%TARGETPATH%\%Cpu%
if "%BUILD_DDK_VER%"=="nt5" goto createtargetdir
set TARGETDIR=%TARGETDIR%\%BLDENV%

:createtargetdir
if not exist %TARGETDIR% md %TARGETDIR% 2> nul >nul

REM
REM Get command line options
REM
set _CLEANBLD=
if "%1"=="clean" goto setbuildopt
if "%2"=="clean" goto setbuildopt
if "%3"=="clean" goto setbuildopt
if "%4"=="clean" goto setbuildopt
if "%5"=="clean" goto setbuildopt
if "%6"=="clean" goto setbuildopt
if "%7"=="clean" goto setbuildopt
goto chksourcedbg

:setbuildopt
REM
REM Tell MS DDK build utility to do a complete rebuild
REM
set NVBUILDOPT=-c %NVBUILDOPT%
set _CLEANBLD=1
if exist %TARGETDIR%\nv3_disp.dll del /f %TARGETDIR%\nv_disp.dll 2>nul >nul
if exist %TARGETDIR%\nv4_disp.dll del /f %TARGETDIR%\nv_disp.dll 2>nul >nul
if exist %TARGETDIR%\nv3_mini.sys del /f %TARGETDIR%\nv3_mini.sys 2>nul >nul
if exist %TARGETDIR%\nv4_mini.sys del /f %TARGETDIR%\nv4_mini.sys 2>nul >nul

:chksourcedbg
REM
REM "dbg" parameter only valid for free builds
REM
if not "%1"=="free" goto getchip
if "%2"=="dbg" goto setdbgsource
if "%3"=="dbg" goto setdbgsource
if "%4"=="dbg" goto setdbgsource
if "%5"=="dbg" goto setdbgsource
if "%6"=="dbg" goto setdbgsource
if "%7"=="dbg" goto setdbgsource
goto getchip

:setdbgsource
set SRCDBG=-nmake SRCDBG=1

:getchip
REM
REM Search for chip id in command line parameters.
REM
if "%2"=="nv3" goto setnv3
if "%2"=="NV3" goto setnv3
if "%2"=="nV3" goto setnv3
if "%2"=="Nv3" goto setnv3
if "%3"=="nv3" goto setnv3
if "%3"=="NV3" goto setnv3
if "%3"=="nV3" goto setnv3
if "%3"=="Nv3" goto setnv3
if "%4"=="nv3" goto setnv3
if "%4"=="NV3" goto setnv3
if "%4"=="nV3" goto setnv3
if "%5"=="Nv3" goto setnv3
if "%5"=="nv3" goto setnv3
if "%5"=="NV3" goto setnv3
if "%5"=="nV3" goto setnv3
if "%5"=="Nv3" goto setnv3
if "%6"=="nv3" goto setnv3
if "%6"=="NV3" goto setnv3
if "%6"=="nV3" goto setnv3
if "%6"=="Nv3" goto setnv3
if "%7"=="nV3" goto setnv3
if "%7"=="Nv3" goto setnv3

goto chknv4
:setnv3
set NT_BUILD_NV3=1
goto chkdx6

:chknv4
if "%2"=="nv4" goto setnv4
if "%2"=="NV4" goto setnv4
if "%2"=="nV4" goto setnv4
if "%2"=="Nv4" goto setnv4
if "%3"=="nv4" goto setnv4
if "%3"=="NV4" goto setnv4
if "%3"=="nV4" goto setnv4
if "%3"=="Nv4" goto setnv4
if "%4"=="nv4" goto setnv4
if "%4"=="NV4" goto setnv4
if "%4"=="nV4" goto setnv4
if "%4"=="Nv4" goto setnv4
if "%5"=="nv4" goto setnv4
if "%5"=="NV4" goto setnv4
if "%5"=="nV4" goto setnv4
if "%5"=="Nv4" goto setnv4
if "%6"=="nv4" goto setnv4
if "%6"=="NV4" goto setnv4
if "%6"=="nV4" goto setnv4
if "%6"=="Nv4" goto setnv4
if "%7"=="nV4" goto setnv4
if "%7"=="Nv4" goto setnv4

goto chkdx6
:setnv4
set NT_BUILD_NV4=1

:chkdx6
if "%2"=="dx6" goto setdx6opt
if "%3"=="dx6" goto setdx6opt
if "%4"=="dx6" goto setdx6opt
if "%5"=="dx6" goto setdx6opt
if "%6"=="dx6" goto setdx6opt
if "%7"=="dx6" goto setdx6opt

goto chkdrv

:setdx6opt
set NVD3D_BUILD_DEFS=-nmake NVD3D=1
REM
REM D3D uses MSVC50.
REM
rem set MSDEV=%BUILD_TOOLS_DIR%\msvc42
rem set INCLUDE=%MSDEV%\include;%MSTOOLS%\include

REM
REM Build miniport/dx/display driver only if "drv" is specified
REM on command line
REM
:chkdrv
cd %SRCPATH%
if "%2"=="drv" goto buildnvmini
if "%3"=="drv" goto buildnvmini
if "%4"=="drv" goto buildnvmini
if "%5"=="drv" goto buildnvmini
if "%6"=="drv" goto buildnvmini
if "%7"=="drv" goto buildnvmini

:buildrm
REM
REM Unfortunately the RM build proc does a complete rebuild
REM of the RM every time. So we will Do RM builds only if this is a
REM clean build AND the "drv" command line parameter is not set.
REM Otherwise, we will skip the RM build.
REM
if "%_CLEANBLD%"=="" goto buildnvmini

REM
REM Build RM libs for NV3 and NV4
REM
cd %BUILD_TREE%\drivers\resman
build %NVBUILDOPT% -nmake NV_BUILD_INTERNAL=1 %SRCDBG%
cd %SRCPATH%
if not exist build.err goto buildnvmini
echo . >> %BUILDLOG%
echo *** NV3 RM libary Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:buildnvmini
REM
REM Build Chip Independent Miniport Driver Library
REM
cd %SRCPATH%\miniport\nv_mini
build %NVBUILDOPT% -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 %SRCDBG%
if not exist build.err goto chkchipbuild
echo . >> %BUILDLOG%
echo *** NV Miniport Driver Chip Indepedent Library Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:chkchipbuild
if not "%NT_BUILD_NV4%" == "" goto buildnv4mini

:buildnv3mini
REM
REM Build Miniport Driver for NV3
REM
cd %SRCPATH%\miniport\nv3
build %NVBUILDOPT% -nmake TARGETPATH=%TARGETPATH% -nmake NV3=1 -nmake NV_BUILD_INTERNAL=1 %SRCDBG%
if not exist build.err goto nv3symbuild
echo . >> %BUILDLOG%
echo *** NV3 Miniport Driver Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:nv3symbuild
REM
REM For debug builds, create SoftICE .NMS symbol file.
REM
if not "%SRCDBG%"=="" goto nv3minisym
if not "%1"=="checked" goto buildnv3dd
:nv3minisym
%BUILD_TOOLS_DIR%\ntice\nmsym /translate:source,package,always /source:%SRCPATH%\miniport\nv_mini;%NMS_MINI_PATH% %TARGETDIR%\nv3_mini.sys

:buildnv3dd
REM
REM For win9x build testing, we'll just attempt to build the miniport
REM
goto fini

if not "%NVD3D_BUILD_DEFS%"=="" goto nv3dx6build
REM
REM Build NV3 ddraw driver library
REM
cd %SRCPATH%\ddraw\nv3
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 %SRCDBG% %NVBUILDOPT%
if not exist build.err goto nv3ddbuilt
echo . >> %BUILDLOG%
echo *** NV3 Ddraw Driver Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini
:nv3ddbuilt
echo . >> %BUILDLOG%
echo *** NV3 Ddraw Driver built successfully *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto nv3dispbuild

:nv3dx6build
REM
REM Build initialization library
REM
cd %BUILD_TREE%\drivers\common\nv3\src
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 %NVD3D_BUILD_DEFS% %SRCDBG% %NVBUILDOPT%
if not exist build.err goto nv3ddinitbuilt
echo . >> %BUILDLOG%
echo *** NV3 Initialzation Library Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:nv3ddinitbuilt
echo . >> %BUILDLOG%
echo *** NV3 Initialzation Library built successfully *** >> %BUILDLOG%
echo . >> %BUILDLOG%


REM
REM Build NV3 DX6 driver libraries
REM
cd %BUILD_TREE%\drivers\ddraw\win9x.nt5\nv3\src
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 %NVD3D_BUILD_DEFS% %SRCDBG% %NVBUILDOPT%
if not exist build.err goto nv3dx6built
echo . >> %BUILDLOG%
echo *** NV3 D3D Driver Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:nv3dx6built
echo . >> %BUILDLOG%
echo *** NV3 D3D Driver built successfully *** >> %BUILDLOG%
echo . >> %BUILDLOG%


:nv3dispbuild
REM
REM Build NV3 Display Driver
REM
cd %SRCPATH%\displays\nv_disp
del /f/q %_OBJ_DIR%\%Cpu%\*.res
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 -nmake NV3=1 %NVD3D_BUILD_DEFS% %SRCDBG% %NVBUILDOPT%
if exist build.err goto dispfail

REM
REM For debug builds, create SoftICE .NMS symbol file.
REM
if not "%SRCDBG%"=="" goto nv3dispsym
if not "%1"=="checked" goto nv3copybin
:nv3dispsym
%BUILD_TOOLS_DIR%\ntice\nmsym /translate:source,package,always /source:%SRCPATH%\displays\nv_disp\i386;%BUILD_TREE%\drivers\ddraw\win9x.nt5\nv3\src %TARGETDIR%\nv3_disp.dll

:nv3copybin
REM
REM NV3 build successful, copy build targets to an installation directory.
REM
if not exist %SRCPATH%\install mkdir %SRCPATH%\install 2> nul >nul
if exist %SRCPATH%\install\nv3_mini.sys del /f %SRCPATH%\install\nv3_mini.sys 2>nul >nul
if exist %SRCPATH%\install\nv3_disp.dll del /f %SRCPATH%\install\nv3_disp.dll 2>nul >nul
if exist %SRCPATH%\install\nv3_disp.inf del /f %SRCPATH%\install\nv3_disp.inf 2>nul >nul

copy %TARGETDIR%\nv3_mini.sys %SRCPATH%\install
copy %TARGETDIR%\nv3_disp.dll %SRCPATH%\install
copy %SRCPATH%\inf\%BUILD_DDK_VER%\nv3_disp.inf %SRCPATH%\install

REM
REM Copy targets to symbol path if one is defined. Needed for WinDbg debugger.
REM
if "%_NT_SYMBOL_PATH%"=="" goto nv3chktarget

if exist %_NT_SYMBOL_PATH%\sys\nv3_mini.sys del /f %_NT_SYMBOL_PATH%\sys\nv3_mini.sys 2>nul >nul
if exist %_NT_SYMBOL_PATH%\dll\nv3_disp.dll del /f %_NT_SYMBOL_PATH%\dll\nv3_disp.dll 2>nul >nul
copy %TARGETDIR%\nv3_mini.sys %_NT_SYMBOL_PATH%\sys
copy %TARGETDIR%\nv3_disp.dll %_NT_SYMBOL_PATH%\dll

:nv3chktarget
if "%DEBUG_TARGET_DIR%"=="" goto nv3fini
copy %TARGETDIR%\nv3_mini.sys %DEBUG_TARGET_DIR%\system32\drivers
copy %TARGETDIR%\nv3_disp.dll %DEBUG_TARGET_DIR%\system32

REM
REM If Softice .NMS symbol files were generated, copy them to target system.
REM
if not "%SRCDBG%"=="" goto nv3copynms
if not "%1"=="checked" goto nv3fini
:nv3copynms
copy %TARGETDIR%\nv3_mini.nms %DEBUG_TARGET_DIR%\system32\drivers
copy %TARGETDIR%\nv3_disp.nms %DEBUG_TARGET_DIR%\system32

:nv3fini
if not "%NT_BUILD_NV3%"=="" goto success

:buildnv4mini
REM
REM Build Miniport Driver for NV4
REM
cd %SRCPATH%\miniport\nv4
build %NVBUILDOPT% -nmake TARGETPATH=%TARGETPATH% -nmake NV4=1 -nmake NV_BUILD_INTERNAL=1 %SRCDBG%
if not exist build.err goto nv4symbuild
echo . >> %BUILDLOG%
echo *** NV4 Miniport Driver Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:nv4symbuild
REM
REM For debug builds, create SoftICE .NMS symbol file.
REM
if not "%SRCDBG%"=="" goto nv4minisym
if not "%1"=="checked" goto nv4ddbuild
:nv4minisym
%BUILD_TOOLS_DIR%\ntice\nmsym /translate:source,package,always /source:%SRCPATH%\miniport\nv_mini;%NMS_MINI_PATH% %TARGETDIR%\nv4_mini.sys

:nv4ddbuild
REM 
REM For win9x build testing, we'll just attempt to build the miniport
REM
goto fini

if not "%NVD3D_BUILD_DEFS%"=="" goto nv4dx6build
REM
REM Build NV4 ddraw driver library
REM
cd %SRCPATH%\ddraw\nv4
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 %SRCDBG% %NVBUILDOPT%
if not exist build.err goto nv4ddbuilt
echo . >> %BUILDLOG%
echo *** Ddraw Driver Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini
:nv4ddbuilt
echo . >> %BUILDLOG%
echo *** NV4 Ddraw Driver built successfully *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto nv4dispbuild

:nv4dx6build
REM
REM Build initialization library
REM
cd %BUILD_TREE%\drivers\common\nv4\src
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 %NVD3D_BUILD_DEFS% %SRCDBG% %NVBUILDOPT%
if not exist build.err goto nv4ddinitbuilt
echo . >> %BUILDLOG%
echo *** NV4 Initialzation Library Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:nv4ddinitbuilt
echo . >> %BUILDLOG%
echo *** NV4 Initialzation Library built successfully *** >> %BUILDLOG%
echo . >> %BUILDLOG%

REM
REM Build NV4 DX6 driver libraries
REM
cd %BUILD_TREE%\drivers\ddraw\win9x.nt5\nv4\src
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 %NVD3D_BUILD_DEFS% %SRCDBG% %NVBUILDOPT%
if not exist build.err goto nv4dx6built
echo . >> %BUILDLOG%
echo *** NV4 D3D Driver Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini
:nv4dx6built
echo . >> %BUILDLOG%
echo *** NV4 D3D Driver built successfully *** >> %BUILDLOG%
echo . >> %BUILDLOG%


:nv4dispbuild
REM
REM Build NV4 Display Driver
REM
cd %SRCPATH%\displays\nv_disp
del /f/q %_OBJ_DIR%\%Cpu%\*.res
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 -nmake NV4=1 %NVD3D_BUILD_DEFS% %SRCDBG% %NVBUILDOPT%
if exist build.err goto dispfail

REM
REM For debug builds, create SoftICE .NMS symbol file.
REM
if not "%SRCDBG%"=="" goto nv4dispsym
if not "%1"=="checked" goto nv4copybin
:nv4dispsym
%BUILD_TOOLS_DIR%\ntice\nmsym /translate:source,package,always /source:%SRCPATH%\displays\nv_disp\i386;%BUILD_TREE%\drivers\ddraw\win9x.nt5\nv4\src %TARGETDIR%\nv4_disp.dll

:nv4copybin
REM
REM Build successful, copy build targets to an installation directory.
REM
if not exist %SRCPATH%\install mkdir %SRCPATH%\install 2> nul >nul
if exist %SRCPATH%\install\nv4_mini.sys del /f %SRCPATH%\install\nv4_mini.sys 2>nul >nul
if exist %SRCPATH%\install\nv4_disp.dll del /f %SRCPATH%\install\nv4_disp.dll 2>nul >nul
if exist %SRCPATH%\install\nv4_disp.inf del /f %SRCPATH%\install\nv4_disp.inf 2>nul >nul

copy %TARGETDIR%\nv4_mini.sys %SRCPATH%\install
copy %TARGETDIR%\nv4_disp.dll %SRCPATH%\install
copy %SRCPATH%\inf\%BUILD_DDK_VER%\nv4_disp.inf %SRCPATH%\install

REM
REM Copy targets to symbol path/debug system.
REM
if "%_NT_SYMBOL_PATH%"=="" goto nv4chktarget

if exist %_NT_SYMBOL_PATH%\sys\nv4_mini.sys del /f %_NT_SYMBOL_PATH%\sys\nv4_mini.sys 2>nul >nul
if exist %_NT_SYMBOL_PATH%\dll\nv4_disp.dll del /f %_NT_SYMBOL_PATH%\dll\nv4_disp.dll 2>nul >nul
copy %TARGETDIR%\nv4_mini.sys %_NT_SYMBOL_PATH%\sys
copy %TARGETDIR%\nv4_disp.dll %_NT_SYMBOL_PATH%\dll

:nv4chktarget
if "%DEBUG_TARGET_DIR%"=="" goto success
copy %TARGETDIR%\nv4_mini.sys %DEBUG_TARGET_DIR%\system32\drivers
copy %TARGETDIR%\nv4_disp.dll %DEBUG_TARGET_DIR%\system32

REM
REM If Softice .NMS symbol files were generated, copy them to target system.
REM
if not "%SRCDBG%"=="" goto nv4copynms
if not "%1"=="checked" goto success
:nv4copynms
copy %TARGETDIR%\nv4_mini.nms %DEBUG_TARGET_DIR%\system32\drivers
copy %TARGETDIR%\nv4_disp.nms %DEBUG_TARGET_DIR%\system32

:success
echo . >> %BUILDLOG%
echo *** Miniport and Display Driver built successfully *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:dispfail
echo . >> %BUILDLOG%
echo *** Display Driver Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:fini
REM
REM Clean up after this batch file
REM
cd %SRCPATH%
set INCLUDE=%SAVEINCLUDE%
set LIB=%SAVELIB%
set PATH=%SAVEPATH%
set MSDEV=%SAVEMSDEV%
set MSDEVDIR=%SAVEMSDEVDIR%
set MSVCDIR=%SAVEMSVCDIR%
set MSTOOLS=%SAVEMSTOOLS%
set BASEDIR=%SAVEBASEDIR%
set DDKDRIVE=%SAVEDDKDRIVE%
set _NTROOT=%SAVENTROOT%
set NT_BUILD_NV3=%SAVE_NT_BUILD_NV3%
set NT_BUILD_NV4=%SAVE_NT_BUILD_NV4%
set BUILD_NVSDK_DIR=%SAVE_BUILD_NVSDK_DIR%
set BUILD_RELEASE_DIR=%SAVE_BUILD_RELEASE_DIR%
set RELEASE_BRANCH=%SAVE_RELEASE_BRANCH%
set BUILD_TREE=%SAVE_BUILD_TREE%
set SAVEINCLUDE=
set SAVELIB=
set SAVEPATH=
set SAVEMSDEV=
set SAVEMSDEVDIR=
set SAVEMSVCDIR=
set SAVEMSTOOLS=
set SAVEBASEDIR=
set SAVEDDKDRIVE=
set SAVENTROOT=
set SRCPATH=
set TARGETDIR=
set BLDENV=
set NVBUILDOPT=
set SRCDBG=
set SAVE_NT_BUILD_NV3=
set SAVE_NT_BUILD_NV4=
set SAVE_BUILD_NVSDK_DIR=
set SAVE_BUILD_RELEASE_DIR=
set SAVE_BUILD_TREE=
set NVD3D_BUILD_DEFS=
set SAVE_RELEASE_BRANCH=

REM clean up after DDK setenv batch file
set NTMAKEENV=
set DDKBUILDENV=

