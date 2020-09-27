!include ..\..\..\sources.inc

TARGETNAME=l_vshade
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	VShader.cpp \
	Shaders.cpp \
	Fixed.cpp
