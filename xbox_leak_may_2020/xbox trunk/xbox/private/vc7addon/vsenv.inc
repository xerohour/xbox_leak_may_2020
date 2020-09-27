# Define VS Build Environment flags
# For real visual studio these flags are defined in the environment before Visual Studio is built.
#
# In order for "build" to work correctly, this file must be !included as the first non-comment line
# of every vs7 "sources" file that is built in the Xbox tree.
#
# In order for "nmake" to work corectly, this file must be !included as the first non-comment line
# of every vs7 "makefile" file that is built in the Xbox tree.
#
# Note that during nmake-ing, this file is read twice, once in the makefile, and a
# second time in the sources file.
#
# The proper way to include this file in both the "sources" and "makefile" files is:
#
#  !include $(_NTROOT)\private\vc7addon\vsenv.inc
#

!ifndef _VSDRIVE

_VSDRIVE=$(_NTDRIVE)
_VSROOT=$(_NTROOT)\private\vc7addon\vs
VSROOT=$(_VSDRIVE)$(_VSROOT)
DEVTOOLS=$(_NTDRIVE)$(_NTROOT)\private\vc7addon\devtools
VSPREBUILT=$(_NTDRIVE)$(_NTROOT)\private\vc7addon\vsbuilt
VSBUILT=$(_NT386TREE)\vs7\built
VSREDIST=$(_NT386TREE)\vs7\redist

!if "$(NTDEBUG)" == "ntsd"
BUILT_FLAVOR=debug
!else if "$(NTDEBUG)" == "ntsdnodbg"
BUILT_FLAVOR=retail
!else
!error Unknown value of NTDEBUG: $(NTDEBUG)
!endif

BUILD_TARGET_ARCHITECTURE=$(PROCESSOR_ARCHITECTURE)

!if "$(BUILT_FLAVOR)" == "debug"
BUILD_RETAIL=no
!else
BUILD_RETAIL=yes
!endif

# Use Visual Studio 7.0 tools

USE_VC7=1
USE_VC7_WINDOWS=1

USE_VS7_MIDL=1
USE_VS70_RULES=1

ATL_INC_PATH=$(DEVTOOLS)\common\vcLKG\inc\atlmfc
ATL_LIB_PATH=$(DEVTOOLS)\x86\vcLKG\atlmfc\lib
MFC_INC_PATH=$(DEVTOOLS)\common\vcLKG\inc\atlmfc
MFC_SRC_PATH=$(DEVTOOLS)\common\vcLKG\src\mfc
MFC_LIB_PATH=$(DEVTOOLS)\x86\vcLKG\atlmfc\lib

CRT_INC_PATH=$(DEVTOOLS)\common\vcLKG\inc
CRT_LIB_PATH=$(DEVTOOLS)\x86\vcLKG\lib

SDK_INC_PATH=$(DEVTOOLS)\common\win32sdk\include
SDK_LIB_PATH=$(DEVTOOLS)\x86\win32sdk\lib

MASTER_VERSION_FILE=$(BASEDIR)\public\sdk\inc\ntverp.h

VSMSO10=1

!endif