!include ..\..\..\sources.inc

TARGETNAME=l_alpbld
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	alphabld.cpp \
	both.cpp \
	destalpha.cpp \
	destcolor.cpp \
	draw.cpp \
	false.cpp \
	invdestalpha.cpp \
	invdestcolor.cpp \
	invsrcalpha.cpp \
	invsrccolor.cpp \
	one.cpp \
	srcalpha.cpp \
	srcalphasat.cpp \
	srccolor.cpp \
	zero.cpp
