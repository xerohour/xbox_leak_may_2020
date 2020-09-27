!include ..\..\..\sources.inc

TARGETNAME=l_textr3
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\m_textr3.lib

SOURCES=\
    textr.cpp
