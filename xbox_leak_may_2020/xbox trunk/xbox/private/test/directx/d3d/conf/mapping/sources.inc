!include ..\..\..\sources.inc

TARGETNAME=l_maping
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
        drawing.cpp \
        linear.cpp \
        mapping.cpp \
        point.cpp
