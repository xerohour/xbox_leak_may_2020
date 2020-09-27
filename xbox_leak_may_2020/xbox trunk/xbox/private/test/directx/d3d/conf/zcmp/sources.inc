!include ..\..\..\sources.inc

TARGETNAME=l_zcmp
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    zcmp.cpp \
    always.cpp \
    combo.cpp \
    equal.cpp \
    greater.cpp \
    less.cpp
