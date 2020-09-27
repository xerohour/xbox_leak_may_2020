!include ..\..\..\sources.inc

TARGETNAME=l_mxpskn
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	MxPalSkin.cpp \
	CBlend_NoPalette_FVF.cpp \
	CBlend_Palette_FVF.cpp \
	CBlend_VS.cpp
