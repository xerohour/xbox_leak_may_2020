!include ..\..\..\sources.inc

TARGETNAME=l_ltball
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\m_ltball.lib

SOURCES=\
    LightBall.cpp \
    Default.cpp \
    Properties.cpp
