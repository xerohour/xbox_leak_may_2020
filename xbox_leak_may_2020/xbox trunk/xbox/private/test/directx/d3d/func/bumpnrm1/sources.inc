!include ..\..\..\sources.inc

TARGETNAME=l_bumpn1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\m_bumpn1.lib

SOURCES=\
    bumpnorm.cpp
