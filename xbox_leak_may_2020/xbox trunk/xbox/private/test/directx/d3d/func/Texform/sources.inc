!include ..\..\..\sources.inc

TARGETNAME=l_texfrm
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	Texform.cpp \
	Default.cpp \
	Properties.cpp
