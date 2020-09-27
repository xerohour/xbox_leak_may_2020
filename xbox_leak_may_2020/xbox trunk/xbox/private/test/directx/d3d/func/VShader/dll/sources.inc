!include ..\..\..\..\sources.inc

TARGETNAME=VShader
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_vshade.lib

SOURCES=\
    include.cpp
