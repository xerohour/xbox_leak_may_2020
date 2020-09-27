!include ..\..\..\sources.inc

TARGETNAME=l_textr2
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\m_textr2.lib

SOURCES=\
    textr.cpp
