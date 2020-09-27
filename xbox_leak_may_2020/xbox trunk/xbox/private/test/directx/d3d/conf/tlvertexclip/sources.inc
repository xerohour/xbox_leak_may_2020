!include ..\..\..\sources.inc

TARGETNAME=l_tlvclp
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    TLVertexClip.cpp \
    edge.cpp \
    quad.cpp
