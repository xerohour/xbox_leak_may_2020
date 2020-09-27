!include ..\..\..\sources.inc

TARGETNAME=l_bumpn0
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\m_bumpn0.lib

SOURCES=\
    bumpnorm.cpp
