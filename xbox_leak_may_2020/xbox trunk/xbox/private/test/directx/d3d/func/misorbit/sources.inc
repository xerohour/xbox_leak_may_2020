!include ..\..\..\sources.inc

TARGETNAME=l_morbit
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\m_morbit.lib

SOURCES=\
    misorbit.cpp \
    revolve.cpp
