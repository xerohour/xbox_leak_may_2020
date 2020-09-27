!include ..\..\..\sources.inc

TARGETNAME=l_texstg
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    CTextureStage.cpp \
    CTextureStage8.cpp \
    CStage.cpp
