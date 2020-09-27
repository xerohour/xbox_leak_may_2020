!include ..\..\..\sources.inc

TARGETNAME=l_terain
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\m_terain.lib

SOURCES=\
    terrain.cpp \
    Perlin.cpp 
