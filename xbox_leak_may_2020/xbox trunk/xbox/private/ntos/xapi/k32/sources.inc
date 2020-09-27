!IF 0

Copyright (c) 1999  Microsoft Corporation

Module Name:

    sources

Abstract:

    Specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

!ENDIF

MAJORCOMP=ntos
MINORCOMP=Xapi

TARGETTYPE=LIBRARY
UMTYPE=xbox

INCLUDES=..;..\..\inc;$(BASEDIR)\private\ntos\inc;$(BASEDIR)\private\inc\crypto;$(DDK_INC_PATH)

C_DEFINES=$(C_DEFINES) -D_XAPI_
MSC_WARNING_LEVEL=/W3 /WX

PRECOMPILED_INCLUDE=..\basedll.h
PRECOMPILED_PCH=basedll.pch
PRECOMPILED_OBJ=basedll.obj
