!IF 0

Copyright (c) 2000 Microsoft Corporation

Module Name:

    tncontrl.

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

C_DEFINES=$(C_DEFINES) -DUNICODE -D_UNICODE -DTNCONTRL_EXPORTS -DDONT_CATCH_DOC_EXCEPTIONS -DXBOX

INCLUDES= \
        $(INCLUDES); \
        $(BASEDIR)\private\test\directx\dplay\testnet\tncontrl; \
        $(BASEDIR)\private\test\directx\dplay\testnet\tncommon; \
        $(BASEDIR)\private\windows\directx\dplay\inc; \
        $(BASEDIR)\private\test\inc; \

SOURCES=\
        ..\tncontrl.rc        \
        ..\binmgmt.cpp        \
        ..\comm.cpp           \
        ..\commtcp.cpp        \
        ..\comports.cpp       \
        ..\control.cpp        \
        ..\excptns.cpp        \
        ..\executor.cpp       \
        ..\faultsim.cpp       \
        ..\faultsimimtest.cpp \
        ..\info.cpp           \
        ..\ipaddrs.cpp        \
        ..\ipcobj.cpp         \
        ..\jobq.cpp           \
        ..\leech.cpp          \
        ..\main.cpp           \
        ..\master.cpp         \
        ..\masterinfo.cpp     \
        ..\masterlist.cpp     \
        ..\meta.cpp           \
        ..\periodicq.cpp      \
        ..\reachcheck.cpp     \
        ..\reportitem.cpp     \
        ..\reportlist.cpp     \
        ..\rsltchain.cpp      \
        ..\script.cpp         \
        ..\sendq.cpp          \
        ..\slave.cpp          \
        ..\slaveinfo.cpp      \
        ..\slavelist.cpp      \
        ..\sndcards.cpp       \
        ..\storedata.cpp      \
        ..\syncdata.cpp       \
        ..\tablecase.cpp      \
        ..\tablegroup.cpp     \
        ..\tableitem.cpp      \
        ..\tapidevs.cpp       \
        ..\testfromfiles.cpp  \
        ..\testinst.cpp       \
        ..\testlist.cpp       \
        ..\teststats.cpp      \
        ..\vars.cpp           \
