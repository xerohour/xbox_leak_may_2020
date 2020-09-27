!IF 0

Copyright (c) 1990  Microsoft Corporation

Module Name:

    sources.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

!ENDIF

!include $(VCROOT)\ide\sources.inc

INCLUDES=$(IDE_ROOT)\pkgs\bld;$(IDE_ROOT)\pkgs\bld\src;$(IDE_ROOT)\pkgs\include;$(IDE_INCLUDES)

C_DEFINES=$(C_DEFINES) /D_BLD_ADD_ON

!if "$(BTLOG)" == "1"
C_DEFINES = $(C_DEFINES) /D_BTLOG
!endif

DLLENTRY=_DllMainCRTStartup

USE_MFC=1
USE_ATL=1

UMTYPE=windows

TARGETLIBS=   \
    $(IDE_ROOT)\lib\*\devshl$(D).lib         \
    $(IDE_ROOT)\lib\*\devbld$(D).lib \
    $(SDK_LIB_PATH)\ole32.lib       \
    $(SDK_LIB_PATH)\kernel32.lib    \
    $(SDK_LIB_PATH)\gdi32.lib       \
    $(SDK_LIB_PATH)\user32.lib      \
    $(SDK_LIB_PATH)\shell32.lib     \
    $(SDK_LIB_PATH)\advapi32.lib    \
    $(SDK_LIB_PATH)\comdlg32.lib    \
    $(SDK_LIB_PATH)\version.lib

PRECOMPILED_INCLUDE=stdafx.h
PRECOMPILED_PCH=stdafx.pch
PRECOMPILED_OBJ=stdafx.obj
PRECOMPILED_CXX=1

