!include ..\..\..\sources.inc

TARGETNAME=l_lumin
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
        luminance.cpp \
        flags.cpp \
        address.cpp
