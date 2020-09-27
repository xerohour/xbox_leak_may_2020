!IF 0

Copyright (c) 1989-1999 Microsoft Corporation

Module Name:

	sources.

Abstract:

	This file specifies the target component being built and the list of
	sources files needed to build that component.  Also specifies optional
	compiler switches and libraries that are unique for the component being
	built.


Author:

	Jim Geist (JimGe) 29-Aug-97

NOTE:	Commented description of this file is in \nt\oak\bin\sources.tpl

!ENDIF

!include ..\..\sources.inc

MAJORCOMP=windows
MINORCOMP=directx

TARGETNAME=dmscript$(LIBEXT)
TARGETTYPE = LIBRARY
TARGETPATH = ..\..\obj
USE_MAPSYM=1

!if $(USE_VC7)
ENABLE_LINK_TIME_CODE_GENERATION=0
!endif

!if "$(NTDEBUG)" == "ntsd" || "$(NTDEBUG)" == "cvp" || "$(NTDEBUG)" == "sym"

!if "$(BuildMode)" == "internal"
C_DEFINES = $(C_DEFINES) -DDEBUG=1
!else
C_DEFINES = $(C_DEFINES) -DRDEBUG=1
!endif

!endif

C_DEFINES= $(C_DEFINES) -D_WIN32 -DDMS_NEVER_USE_OLEAUT -D_MT

PRECOMPILED_CXX=1
PRECOMPILED_INCLUDE=..\pchscript.h
PRECOMPILED_PCH=pchscript.pch
PRECOMPILED_OBJ=pchscript.obj

INCLUDES = $(INCLUDES);   \
	       ..\..\shared; \
	       ..\..\xprivate; \
               ..

SOURCES=\
	activescript.cpp			\
	autaudiopath.cpp			\
	autaudiopathconfig.cpp	\
	autconstants.cpp			\
	authelper.cpp			\
	autperformance.cpp		\
	autsegment.cpp			\
	autsegmentstate.cpp		\
	autsong.cpp				\
	containerdisp.cpp		\
	dll_dmscript.cpp		\
	dmscript.cpp				\
	engdisp.cpp				\
	engerror.cpp				\
	engexec.cpp				\
	engexpr.cpp				\
	enginc.cpp				\
	engine.cpp				\
	englex.cpp				\
	englookup.cpp			\
	engparse.cpp				\
	globaldisp.cpp			\
	oleaut.cpp				\
	packexception.cpp		\
	sourcetext.cpp			\
	track.cpp				\
	trackshared.cpp			\
	unkhelp.cpp				
