!include ..\..\..\sources.inc

TARGETNAME=l_comprs
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	compress.cpp \
	drawgrid.cpp \
	load8.cpp    \
	dxtn.cpp
