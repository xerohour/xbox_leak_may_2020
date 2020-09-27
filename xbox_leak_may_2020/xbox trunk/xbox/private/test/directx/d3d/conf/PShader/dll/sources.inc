!include ..\..\..\..\sources.inc

TARGETNAME=PShader
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_pshade.lib

SOURCES=\
    include.cpp
