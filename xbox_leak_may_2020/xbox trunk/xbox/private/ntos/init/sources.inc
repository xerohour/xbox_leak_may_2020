!IF 0

Copyright (c) 1989  Microsoft Corporation

Module Name:

    sources.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.


Author:

    Steve Wood (stevewo) 12-Apr-1990

NOTE:   Commented description of this file is in \nt\bak\bin\sources.tpl

!ENDIF

MAJORCOMP=ntos
MINORCOMP=init

TARGETNAME=init
TARGETPATH=obj
TARGETTYPE=LIBRARY

TARGETPATH2=..\..\obj

SYNCHRONIZE_DRAIN=1
SYNCHRONIZE_BLOCK=1

INCLUDES=..;$(BASEDIR)\private\ntos\inc;$(BASEDIR)\private\genx\ntos\nls\$(O);$(BASEDIR)\private\inc\crypto;$(BASEDIR)\private\inc\aug01
GPSIZE=32

MSC_WARNING_LEVEL=/W3 /WX

C_DEFINES=$(C_DEFINES) -D_NTSYSTEM_

!if defined(ARCADE)
C_DEFINES=$(C_DEFINES) -DARCADE
!endif

SOURCES=..\xboxkrnl.c \
        ..\init.c \
        ..\fatal.cpp

EXPORT_FILE=$(O)\$(NTTEST).exp

!IF defined(ARCADE)
KERNEL_LIB_SUFFIX=arc
!ELSE
KERNEL_LIB_SUFFIX=con
!ENDIF

KERNEL_LIBS = \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\ex.lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\halx.lib \
    $(TARGETPATH)\$(TARGET_DIRECTORY)\init.lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\io.lib \
!IF !defined(NODEVKIT)
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\kd.lib \
!ELSE
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\kdlite.lib \
!ENDIF
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\ke.lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\mm$(KERNEL_LIB_SUFFIX).lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\ob.lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\ps.lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\ntosrtl.lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\idex$(KERNEL_LIB_SUFFIX).lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\raw.lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\fatx.lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\gdfx.lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\udfx.lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\phy.lib \
    $(BASEDIR)\private\lib\aug01\$(TARGET_DIRECTORY)\libcntpr$(D).lib \
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\ldr$(KERNEL_LIB_SUFFIX).lib \
    $(BASEDIR)\private\lib\$(TARGET_DIRECTORY)\rsa32k.lib      \
    $(BASEDIR)\private\lib\$(TARGET_DIRECTORY)\xcryptk.lib     \
    $(BASEDIR)\public\sdk\lib\$(TARGET_DIRECTORY)\randlibk.lib \
!IF !defined(ARCADE)
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\ani.lib \
!ENDIF
    $(TARGETPATH2)\$(TARGET_DIRECTORY)\av.lib

LINKLIBS = $(EXPORT_FILE)

!IF !defined(ARCADE)
LINKLIBS = $(LINKLIBS) \
    $(BASEDIR)\private\lib\aug01\$(TARGET_DIRECTORY)\d3d8ntpr$(D).lib \
    $(BASEDIR)\public\xdk\lib-aug01\xgraphics$(D).lib\
    $(BASEDIR)\private\lib\aug01\$(TARGET_DIRECTORY)\bootsnd$(D).lib
!ENDIF

LINKER_FLAGS=$(LINKER_FLAGS) /map
LINKER_FLAGS=$(LINKER_FLAGS) /merge:.edata=.text /merge:.XBLD=INIT
LINKER_FLAGS=$(LINKER_FLAGS) /merge:PAGER32C=.text /merge:PAGER32R=.rdata /merge:PAGER32D=.data
