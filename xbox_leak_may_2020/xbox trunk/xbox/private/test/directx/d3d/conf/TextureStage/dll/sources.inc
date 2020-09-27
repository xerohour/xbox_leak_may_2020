!include ..\..\..\..\sources.inc

TARGETNAME=TexStage
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_texstg.lib

SOURCES=\
    include.cpp
