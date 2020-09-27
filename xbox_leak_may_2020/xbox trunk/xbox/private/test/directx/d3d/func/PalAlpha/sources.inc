!include ..\..\..\sources.inc

TARGETNAME=l_palalp
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	PalAlpha.rc \
	PalAlpha.cpp \
	PalUtils.cpp \
	PalUtils8.cpp
