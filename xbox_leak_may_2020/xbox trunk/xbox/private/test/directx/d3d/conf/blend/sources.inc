!include ..\..\..\sources.inc

TARGETNAME=l_blend
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	blend.cpp \
	add.cpp \
	decal.cpp \
	default.cpp \
	draw.cpp \
	modulate.cpp
