!include ..\..\..\sources.inc

TARGETNAME=l_alpcmp
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	alphacmp.cpp \
	always.cpp \
	draw.cpp \
	equal.cpp \
	greater.cpp \
	less.cpp
