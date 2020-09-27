!include ..\..\..\sources.inc

TARGETNAME=l_textr1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\m_textr1.lib

SOURCES=\
    textr.cpp
