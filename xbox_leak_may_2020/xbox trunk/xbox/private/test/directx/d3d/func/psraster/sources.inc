!include ..\..\..\sources.inc

TARGETNAME=l_psrast
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\m_psrast.lib

SOURCES=\
    psraster.cpp
