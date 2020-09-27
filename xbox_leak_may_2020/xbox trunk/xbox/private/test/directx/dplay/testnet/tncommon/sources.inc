!IF 0

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tncommon

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

Author:

    Tristan Jackson (tristanj)

!ENDIF

TARGETTYPE=DYNLINK
TARGETPATH=$(BASEDIR)\private\test\directx\dplay\testnet\bins

DLLENTRY=_DllMainCRTStartup

C_DEFINES=$(C_DEFINES) -DUNICODE -D_UNICODE -DTNCOMMON_EXPORTS -DXBOX -DNO_ERRORS_DVOICE -DNO_SPRINTF_DPLAY8 -DNO_SPRINTF_DVOICE -DNO_SPRINTF_REGISTRY -DNO_CPPOBJHELP_LEAKCHECK -DNO_TNCOMMON_DEBUG_SPEW -DNO_ERRORS_DPLAY -DNO_ERRORS_DPLAY8


INCLUDES= \
        $(INCLUDES); \
        $(BASEDIR)\private\test\directx\dplay\testnet\tncontrl; \
        $(BASEDIR)\private\test\directx\dplay\testnet\tncommon; \
        $(BASEDIR)\private\windows\directx\dplay\inc; \
        $(BASEDIR)\private\test\inc; \

SOURCES=\
        ..\tncommon.rc    \
        ..\excptns.cpp    \
        ..\linkedstr.cpp  \
        ..\linklist.cpp   \
        ..\main.cpp       \
        ..\strutils.cpp   \
        ..\text.cpp       \
        ..\verutils.cpp   \
        ..\errors.cpp     \
        ..\cmdline.cpp    \
        ..\cppobjhelp.cpp \
        ..\stack.cpp      \
        ..\sprintf.cpp    \
        ..\symbols.cpp    \
        ..\debugprint.cpp \
        ..\fileutils.cpp  \
        ..\convhelp.cpp   \
        ..\crc32.c        \        
