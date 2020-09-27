!include ..\..\sources.inc

TARGETNAME = dmloader$(LIBEXT)
TARGETTYPE = LIBRARY
TARGETPATH = ..\..\obj

USE_MAPSYM = 1

!if "$(NTDEBUG)" == "ntsd" || "$(NTDEBUG)" == "cvp" || "$(NTDEBUG)" == "sym"
C_DEFINES = $(C_DEFINES) -DDEBUG=1
!endif

C_DEFINES = $(C_DEFINES) -DXBOX -DUNICODE

INCLUDES = $(INCLUDES);\
           ..\..\shared;\
           ..\..\xprivate; \
           ..

PRECOMPILED_CXX=1
PRECOMPILED_INCLUDE=..\pchloader.h
PRECOMPILED_PCH=pchloader.pch
PRECOMPILED_OBJ=pchloader.obj

SOURCES = container.cpp\
          dll.cpp\
          loader.cpp\
          object.cpp\
          stream.cpp

