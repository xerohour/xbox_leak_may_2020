!include ..\..\..\sources.inc

TARGETNAME=l_addrs
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	address.cpp \
	border.cpp \
	clamp.cpp \
	drawmesh.cpp \
	independentUV.cpp \
	mirror.cpp \
	wrap.cpp \
	wrapu.cpp \
	wrapuv.cpp \
	wrapv.cpp
