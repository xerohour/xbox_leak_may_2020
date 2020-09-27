!include ..\..\sources.inc

PRECOMPILED_CXX=1
PRECOMPILED_INCLUDE=..\dnproti.h
PRECOMPILED_PCH=dnproti.pch
PRECOMPILED_OBJ=dnproti.obj

!if $(FREEBUILD)
C_DEFINES=$(C_DEFINES) -DASSUME_VALID_PARAMETERS
!else
C_DEFINES=$(C_DEFINES) -DDEBUG
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /Od
!endif

SOURCES= ..\backend.cpp \
	 ..\command.cpp \
	 ..\connectp.cpp \
	 ..\endpt.cpp \
         ..\Enum.cpp \
	 ..\initialize.cpp \
         ..\lower_mn.cpp \
 	 ..\receivep.cpp \
	 ..\send.cpp \
	 ..\timer.cpp

USE_NOLIBS=1
