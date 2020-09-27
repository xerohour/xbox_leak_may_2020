
!include ..\..\sources.inc

TARGETNAME = dmband$(LIBEXT)
TARGETTYPE = LIBRARY
TARGETPATH = ..\..\obj

USE_MAPSYM = 1
NTDBGFILES = 1

!if "$(NTDEBUG)" == "ntsd" || "$(NTDEBUG)" == "cvp" || "$(NTDEBUG)" == "sym"
C_DEFINES = $(C_DEFINES) -DDEBUG=1
!endif

C_DEFINES = $(C_DEFINES) -DXBOX -DUNICODE

INCLUDES = $(INCLUDES);\
    $(PRIVATE_INC_PATH);\
           ..\..\shared; \
           ..\..\xprivate; \
           ..

PRECOMPILED_CXX=1
PRECOMPILED_INCLUDE=..\pchdmband.h
PRECOMPILED_PCH=pchdmband.pch
PRECOMPILED_OBJ=pchdmband.obj

SOURCES = bandinst.cpp\
          bandtrk.cpp\
          dmband.cpp\
          dmbdll.cpp\
          pchmap.cpp
