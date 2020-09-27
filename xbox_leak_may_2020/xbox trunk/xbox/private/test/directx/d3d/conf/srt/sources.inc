!include ..\..\..\sources.inc

TARGETNAME=l_srt
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
        buffer.cpp \
        describe.cpp \
        invalid.cpp \
        offscreen.cpp \
        srt.cpp \
        texture.cpp
