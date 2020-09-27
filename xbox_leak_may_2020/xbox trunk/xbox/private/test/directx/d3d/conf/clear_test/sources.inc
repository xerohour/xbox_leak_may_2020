!include ..\..\..\sources.inc

TARGETNAME=l_clear
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
	clear_test.cpp \
    DerivedClasses.cpp \
    describe.cpp \
    helper_fns.cpp
