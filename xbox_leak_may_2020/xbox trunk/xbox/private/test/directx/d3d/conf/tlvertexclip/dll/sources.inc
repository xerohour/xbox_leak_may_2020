!include ..\..\..\..\sources.inc

TARGETNAME=TLVClip
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

OBJLIBFILES=\
    $(OBJLIBFILES) \
    $(TEST_LIB_PATH)\l_tlvclp.lib

SOURCES=\
    include.cpp
