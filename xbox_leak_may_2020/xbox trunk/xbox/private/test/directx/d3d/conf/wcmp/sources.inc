!include ..\..\..\sources.inc

TARGETNAME=l_wcmp
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
        wcmp.cpp \
        always.cpp \
        combo.cpp \
        equal.cpp \
        greater.cpp \
        less.cpp
