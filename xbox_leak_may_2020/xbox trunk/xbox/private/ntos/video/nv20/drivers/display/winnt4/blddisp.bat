@echo off
REM
REM Batch Script to build NT 4.0/5.0 Miniport and Display drivers.
REM Usage:
REM     blddisp <free,checked> <clean> <dbg> <nv3> <nv4> <dx6> <dx7> <drv> <disp> <vpe> <nosrc> <ia64> <gdi> </browse> </dev>
REM             free - Do a free (retail) build
REM             checked - Do a checked (debug) build
REM             clean - Do a complete rebuild
REM             dbg - do a free build with source level debugging
REM             dx6 - build dx6 driver support Win2K branch of Win9x source tree
REM             dx7 - build dx7 driver support using current Win9x source tree
REM             drv - build miniport/dx/display driver only, no RM
REM             disp - build dx/display driver only, no RM/miniport
REM                    disp takes precedence over drv
REM             vpe - build the VPE (Video Port Extension) library
REM                 (if dx7 is specified, the VPE library will be built automatically)
REM             nosrc - excludes source code from .nms files for "dbg" builds
REM             ia64 - build the driver for IA-64 hardware
REM             gdi - just support GDI function calls, ie, no DDRAW and D3D
REM      /browse - build d3d MSVC browse info file
REM      /dev    - build d3d develop build
REM      /stat   - build d3d statistics build
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
set SAVENVDIRS=%NVDIRS%
set SAVENVIA64=%NVIA64%
set SAVENVGDI=%NVGDI%
set SAVENVPE=%NVPE%
set SAVEDX7=%DX7%
set SAVEWINNT=%WINNT%
set DX7=
set DX8=
REM
REM Save env variables used by this batch file.
REM
set SAVE_NT_BUILD_NV3=%NT_BUILD_NV3%
set SAVE_NT_BUILD_NV4=%NT_BUILD_NV4%
set SAVE_BUILD_TREE=%BUILD_TREE%
set SAVE_RELEASE_BRANCH=%RELEASE_BRANCH%
set SAVE_BUILD_NVSDK_DIR=%BUILD_NVSDK_DIR%
set SAVE_DISP_ONLY_BUILD=%DISP_ONLY_BUILD%
set SAVE_SRC_PACKAGING=%SRC_PACKAGING%
set SAVE_NMS_MINI_PATH=%NMS_MINI_PATH%

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
set DISP_ONLY_BUILD=

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

REM Search for 64-bit platform in command line parameters.
if "%2"=="gdi" goto setgdi
if "%3"=="gdi" goto setgdi
if "%4"=="gdi" goto setgdi
if "%5"=="gdi" goto setgdi
if "%6"=="gdi" goto setgdi
if "%7"=="gdi" goto setgdi
if "%8"=="gdi" goto setgdi
if "%9"=="gdi" goto setgdi
if "%10"=="gdi" goto setgdi
if "%11"=="gdi" goto setgdi
if "%12"=="gdi" goto setgdi
if "%13"=="gdi" goto setgdi
if "%14"=="gdi" goto setgdi
set NVGDI=
goto continue_gdi
:setgdi
set NVGDI=1
:continue_gdi

REM Search for 64-bit platform in command line parameters.
if "%2"=="ia64" goto setia64
if "%3"=="ia64" goto setia64
if "%4"=="ia64" goto setia64
if "%5"=="ia64" goto setia64
if "%6"=="ia64" goto setia64
if "%7"=="ia64" goto setia64
if "%8"=="ia64" goto setia64
if "%9"=="ia64" goto setia64
if "%10"=="ia64" goto setia64
if "%11"=="ia64" goto setia64
if "%12"=="ia64" goto setia64
if "%13"=="ia64" goto setia64
if "%14"=="ia64" goto setia64
set NVIA64=
goto continue_ia64
:setia64
set NVIA64=1
:continue_ia64

set SRCDBG=
set DDKDRIVE=
set MSDEV=%BUILD_TOOLS_DIR%\msvc50
set MSDEVDIR=%MSDEV%
set MSVCDIR=
set NT_BUILD_NV3=
set NT_BUILD_NV4=
set NVD3D_BUILD_DEFS=
set NVBUILDOPT=
set NVDIRS=
set SRC_PACKAGING=package
set NMS_MINI_PATH=%BUILD_TREE%\drivers\resman\kernel\dac\nv;%BUILD_TREE%\drivers\resman\kernel\fb\nv;%BUILD_TREE%\drivers\resman\kernel\fb\nv4;%BUILD_TREE%\drivers\resman\kernel\fb\nv10;%BUILD_TREE%\drivers\resman\kernel\fifo\nv;%BUILD_TREE%\drivers\resman\kernel\fifo\nv4;%BUILD_TREE%\drivers\resman\kernel\fifo\nv10;%BUILD_TREE%\drivers\resman\kernel\gr\nv;%BUILD_TREE%\drivers\resman\kernel\gr\nv4;%BUILD_TREE%\drivers\resman\kernel\gr\nv10;%BUILD_TREE%\drivers\resman\kernel\heap\nv;%BUILD_TREE%\drivers\resman\arch\nvalloc\common\src\nv;%BUILD_TREE%\drivers\resman\arch\nvalloc\winnt4\nvgfx\nv;%BUILD_TREE%\drivers\resman\kernel\state\nv;%BUILD_TREE%\drivers\resman\arch\nvalloc\common\src\nv;%NMS_MINI_PATH%
if not "%NVIA64%"=="1" goto mstools_nt5
set MSTOOLS=%BUILD_SDK_DIR%\win64
set NVBUILDOPT=-D -nmake NT5=1 -nmake DDK_VC5_COMPILE=1
goto save_env
:mstools_nt5
if not "%BUILD_DDK_VER%"=="nt5" goto mstools_4
set MSTOOLS=%BUILD_SDK_DIR%\win32.nt5
set NVBUILDOPT=-D -nmake NT5=1 -nmake DDK_VC5_COMPILE=1
goto save_env
:mstools_4
set MSTOOLS=%BUILD_SDK_DIR%\win32

:save_env
set SAVEINCLUDE=%INCLUDE%
set SAVELIB=%LIB%
set SAVEPATH=%PATH%
set SRCPATH=%BUILD_TREE%\drivers\display\winnt4

:setbasedir
if not "%NVIA64%"=="1" goto win32_basedir
set BASEDIR=%BUILD_DDK_DIR%\nt5.ia64
goto set_ntroot
:win32_basedir
set BASEDIR=%BUILD_DDK_DIR%\%BUILD_DDK_VER%
REM NT5 DDK needs _NTROOT for path to build.dat - doesn't hurt to do it for NT4 either
:set_ntroot
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
set PATH=%BASEDIR%\bin;%MSDEV%\bin;%MSTOOLS%\bin;%SYSTEMROOT%\system32
set INCLUDE=%MSDEV%\include;%MSTOOLS%\include;%BUILD_TREE%\drivers\common\inc
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
echo "*** Error setting up DDK build env. Usage: blddisp <free | checked>. ***" >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:setenvtype
set BLDENV=%1

REM Options for BUILD.EXE program
REM
REM         [-3] same as -Z but will add a third pass zero pass
REM         [-a] allows synchronized blocks and drains during link pass
REM         [-b] displays full error message text (doesn't truncate)
REM         [-c] deletes all object files
REM         [-C] deletes all .lib files only
REM         [-D] check dependencies before building (on by default if BUILD_PRODUCT != NT)
REM         [-e] generates build.log, build.wrn & build.err files
REM         [-E] always keep the log/wrn/err files (use with -z)
REM         [-f] force rescan of all source and include files
REM         [-F] when displaying errors/warnings to stdout, print the full path
REM         [-G] enables target specific dirs files iff one target
REM         [-i] ignore extraneous compiler warning messages
REM         [-I] do not display thread index if multiprocessor build
REM         [-k] keep (don't delete) out-of-date targets
REM         [-l] link only, no compiles
REM         [-L] compile only, no link phase
REM         [-m] run build in the idle priority class
REM         [-M [n]] Multiprocessor build (for MP machines)
REM         [-o] display out-of-date files
REM         [-O] generate obj\_objects.mac file for current directory
REM         [-p] pause' before compile and link phases
REM         [-P] Print elapsed time after every directory
REM         [-q] query only, don't run NMAKE
REM         [-r dirPath] restarts clean build at specified directory path
REM         [-s] display status line at top of display
REM         [-S] display status line with include file line counts
REM         [-t] display the first level of the dependency tree
REM         [-T] display the complete dependency tree
REM         [-$] display the complete dependency tree hierarchically
REM         [-u] display unused BUILD_OPTIONS
REM         [-v] enable include file version checking
REM         [-w] show warnings on screen
REM         [-y] show files scanned
REM         [-z] no dependency checking or scanning of source files -
REM              one pass compile/link
				
set NVBUILDOPT=%NVBUILDOPT% -nmake DDKBUILDENV=%BLDENV% -b -e -E -F -w -z

:setupenv
if not "%NVIA64%"=="" goto set64bitevn
call %BASEDIR%\bin\setenv.bat %BASEDIR% %BLDENV%
goto continueSetenv
:set64bitevn
set Cpu=ia64
set NVDIRS=~nv3
call %BASEDIR%\bin\setenv64.bat %BASEDIR% %BLDENV%
:continueSetenv
set TARGETDIR=%TARGETPATH%\%Cpu%
if "%BUILD_DDK_VER%"=="nt5" goto createtargetdir
popd
set TARGETDIR=%TARGETDIR%\%BLDENV%

:createtargetdir
if not exist %TARGETDIR% md %TARGETDIR% 2> nul >nul

REM
REM Get command line options
REM
set D3DDEVBUILD=
set D3DOPT=
set D3DSTATBUILD=

if "%1"=="/dev" goto setdevelopopt
if "%2"=="/dev" goto setdevelopopt
if "%3"=="/dev" goto setdevelopopt
if "%4"=="/dev" goto setdevelopopt
if "%5"=="/dev" goto setdevelopopt
if "%6"=="/dev" goto setdevelopopt
if "%7"=="/dev" goto setdevelopopt
if "%8"=="/dev" goto setdevelopopt
if "%9"=="/dev" goto setdevelopopt
if "%10"=="/dev" goto setdevelopopt
goto chkstatbuild

:setdevelopopt
set D3DDEVBUILD=1
goto chkbrowseopt

:chkstatbuild
if "%1"=="/stat" goto setstatopt
if "%2"=="/stat" goto setstatopt
if "%3"=="/stat" goto setstatopt
if "%4"=="/stat" goto setstatopt
if "%5"=="/stat" goto setstatopt
if "%6"=="/stat" goto setstatopt
if "%7"=="/stat" goto setstatopt
if "%8"=="/stat" goto setstatopt
if "%9"=="/stat" goto setstatopt
if "%10"=="/stat" goto setstatopt
goto chkbrowseopt

:setstatopt
set D3DSTATBUILD=1
goto chkbrowseopt

:chkbrowseopt
if "%1"=="/browse" goto setbrowseopt
if "%2"=="/browse" goto setbrowseopt
if "%3"=="/browse" goto setbrowseopt
if "%4"=="/browse" goto setbrowseopt
if "%5"=="/browse" goto setbrowseopt
if "%6"=="/browse" goto setbrowseopt
if "%7"=="/browse" goto setbrowseopt
if "%8"=="/browse" goto setbrowseopt
if "%9"=="/browse" goto setbrowseopt
if "%10"=="/browse" goto setbrowseopt
goto chkcleanbld

:setbrowseopt
set D3DOPT=browse=1

:chkcleanbld

set _CLEANBLD=
if "%1"=="clean" goto setbuildopt
if "%2"=="clean" goto setbuildopt
if "%3"=="clean" goto setbuildopt
if "%4"=="clean" goto setbuildopt
if "%5"=="clean" goto setbuildopt
if "%6"=="clean" goto setbuildopt
if "%7"=="clean" goto setbuildopt
if "%8"=="clean" goto setbuildopt
if "%9"=="clean" goto setbuildopt
if "%10"=="clean" goto setbuildopt
if "%11"=="clean" goto setbuildopt
if "%12"=="clean" goto setbuildopt
if "%13"=="clean" goto setbuildopt
if "%14"=="clean" goto setbuildopt
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
if "%8"=="dbg" goto setdbgsource
if "%9"=="dbg" goto setdbgsource
if "%10"=="dbg" goto setdbgsource
if "%11"=="dbg" goto setdbgsource
if "%12"=="dbg" goto setdbgsource
if "%13"=="dbg" goto setdbgsource
if "%14"=="dbg" goto setdbgsource
goto getchip

:setdbgsource
set SRCDBG=-nmake SRCDBG=1

:chknosrc
REM
REM "nosrc" parameter only valid for free dbg builds
REM
if "%2"=="nosrc" goto setnosource
if "%3"=="nosrc" goto setnosource
if "%4"=="nosrc" goto setnosource
if "%5"=="nosrc" goto setnosource
if "%6"=="nosrc" goto setnosource
if "%7"=="nosrc" goto setnosource
if "%8"=="nosrc" goto setnosource
if "%9"=="nosrc" goto setnosource
if "%10"=="nosrc" goto setnosource
if "%11"=="nosrc" goto setnosource
if "%12"=="nosrc" goto setnosource
if "%13"=="nosrc" goto setnosource
if "%14"=="nosrc" goto setnosource
goto getchip

:setnosource
set SRC_PACKAGING=nopackage

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
if "%7"=="nv3" goto setnv3
if "%7"=="NV3" goto setnv3
if "%7"=="nV3" goto setnv3
if "%7"=="Nv3" goto setnv3
if "%8"=="nV3" goto setnv3
if "%8"=="Nv3" goto setnv3
if "%9"=="nv3" goto setnv3
if "%9"=="NV3" goto setnv3
if "%9"=="nV3" goto setnv3
if "%9"=="Nv3" goto setnv3
if "%10"=="nv3" goto setnv3
if "%10"=="NV3" goto setnv3
if "%10"=="nV3" goto setnv3
if "%10"=="Nv3" goto setnv3
if "%11"=="nV3" goto setnv3
if "%11"=="Nv3" goto setnv3
if "%12"=="nV3" goto setnv3
if "%12"=="Nv3" goto setnv3
if "%13"=="nV3" goto setnv3
if "%13"=="Nv3" goto setnv3
if "%14"=="nV3" goto setnv3
if "%14"=="Nv3" goto setnv3

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
if "%7"=="nv4" goto setnv4
if "%7"=="NV4" goto setnv4
if "%7"=="nV4" goto setnv4
if "%7"=="Nv4" goto setnv4
if "%8"=="nV4" goto setnv4
if "%8"=="Nv4" goto setnv4
if "%9"=="nv4" goto setnv4
if "%9"=="NV4" goto setnv4
if "%9"=="nV4" goto setnv4
if "%9"=="Nv4" goto setnv4
if "%10"=="nv4" goto setnv4
if "%10"=="NV4" goto setnv4
if "%10"=="nV4" goto setnv4
if "%10"=="Nv4" goto setnv4
if "%11"=="nV4" goto setnv4
if "%11"=="Nv4" goto setnv4
if "%12"=="nV4" goto setnv4
if "%12"=="Nv4" goto setnv4
if "%13"=="nV4" goto setnv4
if "%13"=="Nv4" goto setnv4
if "%14"=="nV4" goto setnv4
if "%14"=="Nv4" goto setnv4

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
if "%8"=="dx6" goto setdx6opt
if "%9"=="dx6" goto setdx6opt
if "%10"=="dx6" goto setdx6opt
if "%11"=="dx6" goto setdx6opt
if "%12"=="dx6" goto setdx6opt
if "%13"=="dx6" goto setdx6opt
if "%14"=="dx6" goto setdx6opt
goto chkdx7

:setdx6opt
set NVD3D_BUILD_DEFS=-nmake NVD3D=1
REM
REM D3D uses MSVC50.
REM
rem set MSDEV=%BUILD_TOOLS_DIR%\msvc50
rem set INCLUDE=%MSDEV%\include;%MSTOOLS%\include
:chkdx7
if "%2"=="dx7" goto setdx7opt
if "%3"=="dx7" goto setdx7opt
if "%4"=="dx7" goto setdx7opt
if "%5"=="dx7" goto setdx7opt
if "%6"=="dx7" goto setdx7opt
if "%7"=="dx7" goto setdx7opt
if "%8"=="dx7" goto setdx7opt
if "%9"=="dx7" goto setdx7opt
if "%10"=="dx7" goto setdx7opt
if "%11"=="dx7" goto setdx7opt
if "%12"=="dx7" goto setdx7opt
if "%13"=="dx7" goto setdx7opt
if "%14"=="dx7" goto setdx7opt
goto chkdx8

:setdx7opt
set NVD3D_BUILD_DEFS=-nmake NVD3D=1 -nmake DX7=1
set DX7=1
if not "%NVIA64%"=="" goto chkdx8
if not "%NVGDI%"=="" goto chkdx8
if "%NT_BUILD_NV3%" == "" set NVPE=-nmake NVPE=1

:chkdx8
if "%2"=="dx8" goto setdx8opt
if "%3"=="dx8" goto setdx8opt
if "%4"=="dx8" goto setdx8opt
if "%5"=="dx8" goto setdx8opt
if "%6"=="dx8" goto setdx8opt
if "%7"=="dx8" goto setdx8opt
if "%8"=="dx8" goto setdx8opt
if "%9"=="dx8" goto setdx8opt
if "%10"=="dx8" goto setdx8opt
goto chknvpe

:setdx8opt
set NVD3D_BUILD_DEFS=-nmake NVD3D=1 -nmake DX7=1 -nmake DX8=1
set DX7=1
set DX8=1
if "%NT_BUILD_NV3%" == "" set NVPE=-nmake NVPE=1

REM Build VPE (Video Port Extension) library only if "vpe" is specified
REM on command line... dx6 is also needed
REM
:chknvpe
if "%2"=="vpe" goto setnvpeopt
if "%3"=="vpe" goto setnvpeopt
if "%4"=="vpe" goto setnvpeopt
if "%5"=="vpe" goto setnvpeopt
if "%6"=="vpe" goto setnvpeopt
if "%7"=="vpe" goto setnvpeopt
if "%8"=="vpe" goto setnvpeopt
if "%9"=="vpe" goto setnvpeopt
if "%10"=="vpe" goto setnvpeopt
if "%11"=="vpe" goto setnvpeopt
if "%12"=="vpe" goto setnvpeopt
if "%13"=="vpe" goto setnvpeopt
if "%14"=="vpe" goto setnvpeopt

goto checkd3dlevel

:setnvpeopt
set NVPE=-nmake NVPE=1

REM
REM determine hardware flavor of d3d build
REM
:checkd3dlevel
if "%2"=="kelvin" goto setkelvinbuild
if "%3"=="kelvin" goto setkelvinbuild
if "%4"=="kelvin" goto setkelvinbuild
if "%5"=="kelvin" goto setkelvinbuild
if "%6"=="kelvin" goto setkelvinbuild
if "%7"=="kelvin" goto setkelvinbuild
if "%8"=="kelvin" goto setkelvinbuild
if "%9"=="kelvin" goto setkelvinbuild
if "%10"=="kelvin" goto setkelvinbuild
if "%11"=="kelvin" goto setkelvinbuild
if "%12"=="kelvin" goto setkelvinbuild
if "%13"=="kelvin" goto setkelvinbuild
if "%14"=="kelvin" goto setkelvinbuild

set D3DBUILDLEVEL=celsius
goto chkdisp

:setkelvinbuild
set D3DBUILDLEVEL=kelvin
set NVD3D_BUILD_DEFS=%NVD3D_BUILD_DEFS% -nmake KELVIN=1

REM
REM Build only dx/display driver if "drv" is specified
REM on command line
REM
:chkdisp
pushd %SRCPATH%
if "%2"=="disp" goto drvonly
if "%3"=="disp" goto drvonly
if "%4"=="disp" goto drvonly
if "%5"=="disp" goto drvonly
if "%6"=="disp" goto drvonly
if "%7"=="disp" goto drvonly
if "%8"=="disp" goto drvonly
if "%9"=="disp" goto drvonly
if "%10"=="disp" goto drvonly
if "%11"=="disp" goto drvonly
if "%12"=="disp" goto drvonly
if "%13"=="disp" goto drvonly
if "%14"=="disp" goto drvonly

goto chkdrv

:drvonly
set BUILD_DISP_ONLY=1
if not "%NT_BUILD_NV3%" == "" goto buildnv3dd
if not "%NT_BUILD_NV4%" == "" goto buildnv4dd
goto buildnv3dd

REM
REM Build miniport/dx/display driver only if "drv" is specified
REM on command line
REM
:chkdrv
if "%2"=="drv" goto buildnvmini
if "%3"=="drv" goto buildnvmini
if "%4"=="drv" goto buildnvmini
if "%5"=="drv" goto buildnvmini
if "%6"=="drv" goto buildnvmini
if "%7"=="drv" goto buildnvmini
if "%8"=="drv" goto buildnvmini
if "%9"=="drv" goto buildnvmini
if "%10"=="drv" goto buildnvmini
if "%11"=="drv" goto buildnvmini
if "%12"=="drv" goto buildnvmini
if "%13"=="drv" goto buildnvmini
if "%14"=="drv" goto buildnvmini

:buildrm
REM
REM Build RM libs for NV3 and NV4
REM
cd %BUILD_TREE%\drivers\resman
if exist build.err del build.err
build %NVBUILDOPT% -nmake NV_BUILD_INTERNAL=1 %SRCDBG% %NVDIRS%
popd
if not exist build.err goto buildnvmini
echo . >> %BUILDLOG%
echo *** NV3 RM libary Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:buildnvmini
REM
REM Build Chip Independent Miniport Driver Library
REM
pushd %SRCPATH%\miniport\nv_mini
if exist build.err del build.err
build %NVBUILDOPT% -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 %SRCDBG% %NVPE%
if not exist build.err goto chkchipbuild
echo . >> %BUILDLOG%
echo *** NV Miniport Driver Chip Indepedent Library Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:chkchipbuild
if not "%NT_BUILD_NV4%" == "" goto buildnv4modedata

:buildnv3mini
REM
REM Build the OS independent modeset common code first for NV3
REM
goto buildmodesetcode
:NV3_AfterModeset
REM
REM Build Miniport Driver for NV3
REM
cd %SRCPATH%\miniport\nv3
if exist build.err del build.err
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
if "%var_NONMSYM%"=="1" goto buildnv3dd
%BUILD_TOOLS_DIR%\ntice\nmsym /translate:source,%SRC_PACKAGING%,always /source:%SRCPATH%\miniport\nv_mini;%NMS_MINI_PATH% %TARGETDIR%\nv3_mini.sys

:buildnv3dd
if not "%NVD3D_BUILD_DEFS%"=="" goto nv3dx6build
REM
REM Build NV3 ddraw driver library
REM
cd %SRCPATH%\ddraw\nv3
if exist build.err del build.err
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
if exist build.err del build.err
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
if exist build.err del build.err
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
if exist build.err del build.err
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 -nmake NV3=1 %NVD3D_BUILD_DEFS% %SRCDBG% %NVBUILDOPT%
if exist build.err goto dispfail

REM
REM For debug builds, create SoftICE .NMS symbol file.
REM
if not "%SRCDBG%"=="" goto nv3dispsym
if not "%1"=="checked" goto nv3copybin
:nv3dispsym
if "%var_NONMSYM%"=="1" goto nv3copybin
%BUILD_TOOLS_DIR%\ntice\nmsym /translate:source,package,always /source:%SRCPATH%\displays\nv_disp\i386;%BUILD_TREE%\drivers\ddraw\win9x.nt5\nv3\src %TARGETDIR%\nv3_disp.dll

:nv3copybin
REM
REM NV3 build successful, copy build targets to an installation directory.
REM
if not exist %SRCPATH%\install mkdir %SRCPATH%\install 2> nul >nul
if exist %SRCPATH%\install\nv3_mini.sys del /f %SRCPATH%\install\nv3_mini.sys 2>nul >nul
if exist %SRCPATH%\install\nv3_disp.dll del /f %SRCPATH%\install\nv3_disp.dll 2>nul >nul
if exist %SRCPATH%\install\nv3_disp.inf del /f %SRCPATH%\install\nv3_disp.inf 2>nul >nul

if exist %TARGETDIR%\nv3_mini.sys copy %TARGETDIR%\nv3_mini.sys %SRCPATH%\install
copy %TARGETDIR%\nv3_disp.dll %SRCPATH%\install
copy %SRCPATH%\inf\%BUILD_DDK_VER%\nv3_disp.inf %SRCPATH%\install

REM
REM Copy targets to symbol path if one is defined. Needed for WinDbg debugger.
REM
if "%_NT_SYMBOL_PATH%"=="" goto nv3chktarget

if exist %_NT_SYMBOL_PATH%\sys\nv3_mini.sys del /f %_NT_SYMBOL_PATH%\sys\nv3_mini.sys 2>nul >nul
if exist %_NT_SYMBOL_PATH%\dll\nv3_disp.dll del /f %_NT_SYMBOL_PATH%\dll\nv3_disp.dll 2>nul >nul
if exist %TARGETDIR%\nv3_mini.sys copy %TARGETDIR%\nv3_mini.sys %_NT_SYMBOL_PATH%\sys
copy %TARGETDIR%\nv3_disp.dll %_NT_SYMBOL_PATH%\dll

:nv3chktarget
if "%DEBUG_TARGET_DIR%"=="" goto nv3fini
if exist %TARGETDIR%\nv3_mini.sys copy %TARGETDIR%\nv3_mini.sys %DEBUG_TARGET_DIR%\system32\drivers
copy %TARGETDIR%\nv3_disp.dll %DEBUG_TARGET_DIR%\system32

REM
REM If Softice .NMS symbol files were generated, copy them to target system.
REM
if not "%SRCDBG%"=="" goto nv3copynms
if not "%1"=="checked" goto nv3fini
:nv3copynms
if exist %TARGETDIR%\nv3_mini.nms copy %TARGETDIR%\nv3_mini.nms %DEBUG_TARGET_DIR%\system32\drivers
if exist %TARGETDIR%\nv3_disp.nms copy %TARGETDIR%\nv3_disp.nms %DEBUG_TARGET_DIR%\system32

:nv3fini
if not "%NT_BUILD_NV3%"=="" goto success

:buildnv4modedata
REM
REM Build Common Mode Data for NV4 modeset code
REM
cd %BUILD_TREE%\drivers\common\vesa
if exist build.err del build.err
build %NVBUILDOPT% -nmake TARGETPATH=%TARGETPATH% -nmake NV4=1 -nmake NV_BUILD_INTERNAL=1 %SRCDBG%
if not exist build.err goto buildmodesetcode
echo . >> %BUILDLOG%
echo *** NV4 Common ModeSet Code Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini


:buildmodesetcode
REM
REM Build the OS independent modeset code
REM
REM Get the tools needed to build this. The following code is similar to the Win98 code in modeset.mhd/include/makefile.fr
REM
set H2INC=%BUILD_TOOLS_DIR%\masm611\bin\h2inc.exe /nologo
set SED=%BUILD_TOOLS_DIR%\gmake\sed.exe
set FGREP=%BUILD_TOOLS_DIR%\gmake\fgrep.exe
cd %BUILD_TREE%\drivers\modeset.mhd\code
REM
REM Convert the gtfmath.h into gtfmath.inc file. 
REM
if exist temp.inc del temp.inc
if exist temp.h del temp.h
if exist temp1.h del temp1.h
if exist temp2.h del temp2.h
echo typedef unsigned long ULONG; > temp.h
type gtfmath.h >> temp.h
%SED% "s/FAR//g" temp.h > temp1.h
%FGREP% -v WINAPI temp1.h > temp.h
%H2INC% /Ht /D_WIN32 /Zngtfmath temp.h
copy temp.inc i386\gtfmath.inc
REM
REM Copy the gtfmath.asm file into a i386 subdirectory. Otherwise, build.exe will not compile gtfmath.asm
REM
%SED% "s/macros.dat/..\\include\\macros.dat/g" gtfmath.asm > temp2.asm
copy temp2.asm i386\gtfmath.asm
copy ..\include\macros.dat i386\macros.dat
REM
REM Now we are setup to do the actual build
REM
if exist build.err del build.err
build %NVBUILDOPT% -nmake -fmakefileNT -nmake TARGETPATH=%TARGETPATH% -nmake NV4=1 -nmake NV_BUILD_INTERNAL=1 %SRCDBG%
del i386\gtfmath.asm
del i386\gtfmath.inc
del i386\macros.dat
del temp2.asm
del temp.h
del temp1.h
del temp.inc
if not "%NT_BUILD_NV3%"=="" goto NV3_AfterModeset
if not exist build.err goto buildnv4nvpecore
echo . >> %BUILDLOG%
echo *** NV4 modesetcode.lib Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini

:buildnv4nvpecore
REM
REM Build Common VPE core code
REM
if "%NVPE%"=="" goto buildnv4mini
cd %BUILD_TREE%\drivers\common\vpe\src\core
if exist build.err del build.err
build %NVBUILDOPT% -nmake TARGETPATH=%TARGETPATH% -nmake NV4=1 -nmake NV_BUILD_INTERNAL=1 %SRCDBG% %NVPE%
if not exist build.err goto buildnv4mini
echo . >> %BUILDLOG%
echo *** NV4 nvpecore.lib Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini


:buildnv4mini
REM
REM Build Miniport Driver for NV4
REM
if "%BUILD_DISP_ONLY%"=="1" goto buildnv4dd
cd %SRCPATH%\miniport\nv4
if exist build.err del build.err
build %NVBUILDOPT% -nmake TARGETPATH=%TARGETPATH% -nmake NV4=1 -nmake NV_BUILD_INTERNAL=1 %SRCDBG% %NVPE%
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
if not "%1"=="checked" goto buildnv4dd
:nv4minisym
if "%var_NONMSYM%"=="1" goto buildnv4dd
%BUILD_TOOLS_DIR%\ntice\nmsym /translate:source,%SRC_PACKAGING%,always /source:%BUILD_TREE%\drivers\modeset.mhd\code;%BUILD_TREE%\drivers\modeset.mhd\code\i386;%SRCPATH%\miniport\nv_mini;%NMS_MINI_PATH%;%BUILD_TREE%\drivers\common\VPE\src\core %TARGETDIR%\nv4_mini.sys

:buildnv4dd
if not "%NVD3D_BUILD_DEFS%"=="" goto nv4dx6build
REM
REM Build NV4 ddraw driver library
REM
cd %SRCPATH%\ddraw\nv4
if exist build.err del build.err
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
if not "%DX7%"=="" goto nv4dx7build
REM
REM Build initialization library
REM
cd %BUILD_TREE%\drivers\common\nv4\src
if exist build.err del build.err
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
cd %BUILD_TREE%\drivers\ddraw
if exist build.err del build.err
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 %NVD3D_BUILD_DEFS% %SRCDBG% %NVBUILDOPT% %NVPE%
if not exist build.err goto nv4dxbuild_ok
:nv4dxbuild_fail
echo . >> %BUILDLOG%
echo *** NV4 D3D Driver Failed to build *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto fini
:nv4dxbuild_ok
echo . >> %BUILDLOG%
echo *** NV4 D3D Driver built successfully *** >> %BUILDLOG%
echo . >> %BUILDLOG%
goto nv4dispbuild

:nv4dx7build
REM
REM Build DX7 library
REM
set D3DTARGETPATH=
set D3DTARGET=
if "%D3DDEVBUILD%"=="1" goto dodx7devtarget
if "%D3DSTATBUILD%"=="1" goto dodx7stattarget
if "%BLDENV%"=="checked" goto dodx7chktarget
:dodx7retailtarget
set D3DTARGETPATH=%D3DBUILDLEVEL%.winnt.retail
set D3DTARGET=%D3DBUILDLEVEL%.retail.winnt
goto dodx7prebuild

:dodx7devtarget
set D3DTARGETPATH=%D3DBUILDLEVEL%.winnt.develop
set D3DTARGET=%D3DBUILDLEVEL%.develop.winnt
goto dodx7prebuild

:dodx7stattarget
set D3DTARGETPATH=%D3DBUILDLEVEL%.winnt.stat
set D3DTARGET=%D3DBUILDLEVEL%.stat.winnt
goto dodx7prebuild

:dodx7chktarget
set D3DTARGETPATH=%D3DBUILDLEVEL%.winnt.debug
set D3DTARGET=%D3DBUILDLEVEL%.debug.winnt

:dodx7prebuild
if not "%NVIA64%"=="" goto nv4encryptkey
if not "%NVGDI%"==""  goto nv4encryptkey
cd /d %BUILD_TREE%\drivers\ddraw
if "%_CLEANBLD%"=="" goto dodx7build

:dx7clean
%BUILD_TOOLS_DIR%\msvc50\bin\nmake %D3DTARGET% clean=1
if %ERRORLEVEL% GEQ 1 goto nv4dxbuild_fail

:dodx7build
%BUILD_TOOLS_DIR%\msvc50\bin\nmake %D3DTARGET% %D3DOPT%
if %ERRORLEVEL% GEQ 1 goto nv4dxbuild_fail

:nv4encryptkey
if "%NVIA64%"=="1" goto nv4dispbuild
REM
REM Encrypting registry keys
REM
%BUILD_TOOLS_DIR%\nvidia\bin\encrypt %BUILD_TREE%\drivers\ddraw\common\obj.%D3DTARGETPATH%\global.obj
if %ERRORLEVEL% GEQ 1 goto nv4dxbuild_fail

:nv4dispbuild
REM
REM Build NV4 Display Driver
REM
cd %SRCPATH%\displays\nv_disp
if exist %TARGETDIR%\nv4_disp.dll del /f %TARGETDIR%\nv4_disp.dll
if exist %_OBJ_DIR%\%Cpu%\*.res del /f/q %_OBJ_DIR%\%Cpu%\*.res
if exist build.err del build.err
build -nmake TARGETPATH=%TARGETPATH% -nmake NV_BUILD_INTERNAL=1 -nmake NV4=1 %NVD3D_BUILD_DEFS% %SRCDBG% %NVBUILDOPT% %NVPE% D3DTARGETPATH=%D3DTARGETPATH%
if exist build.err goto dispfail
if not exist %TARGETDIR%\nv4_disp.dll goto dispfail

REM
REM For debug builds, create SoftICE .NMS symbol file.
REM
if not "%SRCDBG%"=="" goto nv4dispsym
if not "%1"=="checked" goto nv4copybin
:nv4dispsym
if "%var_NONMSYM%"=="1" goto nv4copybin
%BUILD_TOOLS_DIR%\ntice\nmsym /translate:source,%SRC_PACKAGING%,always /source:%SRCPATH%\displays\nv_disp\i386;%BUILD_TREE%\drivers\ddraw\common\src;%BUILD_TREE%\drivers\ddraw\common_disp\src;%BUILD_TREE%\drivers\common\src;%BUILD_TREE%\drivers\ddraw\celsius\src;%BUILD_TREE%\drivers\common\celsius\lib;%BUILD_TREE%\drivers\ddraw\kelvin\src;%BUILD_TREE%\drivers\common\kelvin\lib;%BUILD_TREE%\drivers\OpenGL\nv10Shared;%BUILD_TREE%\drivers\OpenGL\nvshared;%BUILD_TREE%\drivers\OpenGL\nv4shared;%BUILD_TREE%\drivers\common\VPE\src %TARGETDIR%\nv4_disp.dll

:nv4copybin
REM
REM Build successful, copy build targets to an installation directory.
REM
if not exist %SRCPATH%\install mkdir %SRCPATH%\install 2> nul >nul
if exist %SRCPATH%\install\nv4_mini.sys del /f %SRCPATH%\install\nv4_mini.sys 2>nul >nul
if exist %SRCPATH%\install\nv4_disp.dll del /f %SRCPATH%\install\nv4_disp.dll 2>nul >nul
if exist %SRCPATH%\install\nv4_disp.inf del /f %SRCPATH%\install\nv4_disp.inf 2>nul >nul

if exist %TARGETDIR%\nv4_mini.sys copy %TARGETDIR%\nv4_mini.sys %SRCPATH%\install
copy %TARGETDIR%\nv4_disp.dll %SRCPATH%\install
copy %SRCPATH%\inf\%BUILD_DDK_VER%\nv4_disp.inf %SRCPATH%\install

REM
REM Copy targets to symbol path/debug system.
REM
if "%_NT_SYMBOL_PATH%"=="" goto nv4chktarget

if exist %_NT_SYMBOL_PATH%\sys\nv4_mini.sys del /f %_NT_SYMBOL_PATH%\sys\nv4_mini.sys 2>nul >nul
if exist %_NT_SYMBOL_PATH%\dll\nv4_disp.dll del /f %_NT_SYMBOL_PATH%\dll\nv4_disp.dll 2>nul >nul
if exist %SRCPATH%\install\nv4_mini.sys copy %TARGETDIR%\nv4_mini.sys %_NT_SYMBOL_PATH%\sys
copy %TARGETDIR%\nv4_disp.dll %_NT_SYMBOL_PATH%\dll

:nv4chktarget
if "%DEBUG_TARGET_DIR%"=="" goto success
if exist %SRCPATH%\install\nv4_mini.sys copy %TARGETDIR%\nv4_mini.sys %DEBUG_TARGET_DIR%\system32\drivers
copy %TARGETDIR%\nv4_disp.dll %DEBUG_TARGET_DIR%\system32

REM
REM If Softice .NMS symbol files were generated, copy them to target system.
REM
if not "%SRCDBG%"=="" goto nv4copynms
if not "%1"=="checked" goto success
:nv4copynms
if exist %TARGETDIR%\nv4_mini.nms copy %TARGETDIR%\nv4_mini.nms %DEBUG_TARGET_DIR%\system32\drivers
if exist %TARGETDIR%\nv4_disp.dll copy %TARGETDIR%\nv4_disp.nms %DEBUG_TARGET_DIR%\system32

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
popd
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
set NVDIRS=%SAVENVDIRS%
set NVIA64=%SAVENVIA64%
set NVGDI=%SAVENVGDI%
set NVPE=%SAVENVPE%
set NT_BUILD_NV3=%SAVE_NT_BUILD_NV3%
set NT_BUILD_NV4=%SAVE_NT_BUILD_NV4%
set BUILD_NVSDK_DIR=%SAVE_BUILD_NVSDK_DIR%
set BUILD_RELEASE_DIR=%SAVE_BUILD_RELEASE_DIR%
set RELEASE_BRANCH=%SAVE_RELEASE_BRANCH%
set BUILD_TREE=%SAVE_BUILD_TREE%
set DISP_ONLY_BUILD=%SAVE_DISP_ONLY_BUILD%
set SRC_PACKAGING=%SAVE_SRC_PACKAGING%
set NMS_MINI_PATH=%SAVe_NMS_MINI_PATH%
set DX7=%SAVEDX7%
set WINNT=%SAVEWINNT%
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
set SAVENVPE=
set SRCDBG=
set SAVE_NT_BUILD_NV3=
set SAVE_NT_BUILD_NV4=
set SAVE_BUILD_NVSDK_DIR=
set SAVE_BUILD_RELEASE_DIR=
set SAVE_BUILD_TREE=
set NVD3D_BUILD_DEFS=
set SAVE_RELEASE_BRANCH=
set SAVE_DISP_ONLY_BUILD=
set SAVE_SRC_PACKAGING=
set SAVE_NMS_MINI_PATH=
set SAVEDX7=
set SAVEWINNT=
REM clean up after DDK setenv batch file
set NTMAKEENV=
set DDKBUILDENV=

