!include ..\..\..\..\sources.inc

TARGETNAME=Texform
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_texfrm.lib

SOURCES=\
    include.cpp
