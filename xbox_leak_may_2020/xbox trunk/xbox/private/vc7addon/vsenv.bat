@if "%_ECHO%"=="" @echo off

REM Define VS Build Environment flags for batch file
REM For real visual studio these flags are defined in the environment before Visual Studio is built.

set _VSDRIVE=%_NTDRIVE%

set _VSROOT=%_NTROOT%\private\vc7addon\vs

set VSROOT=%_VSDRIVE%%_VSROOT%

set DEVTOOLS=%_NTROOT%\private\vc7addon\devtools

set VSBUILT=%_NT386TREE%\vs7\built

set VSREDIST=%_NT386TREE%\dump\vs7\redist

set BUILT_FLAVOR=debug

REM For Xbox makefile.def - use Visual Studio 7 tools

set USE_VC7=1

set VC_PATH=$(BASEDIR)\public\mstools\vc70

set USE_VS7_MIDL=1
