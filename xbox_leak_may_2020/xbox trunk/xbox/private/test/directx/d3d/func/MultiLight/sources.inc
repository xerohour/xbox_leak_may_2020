!include ..\..\..\sources.inc

TARGETNAME=l_mullit
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\m_mullit.lib

SOURCES=\
	MultiLight.cpp \
	Default.cpp
