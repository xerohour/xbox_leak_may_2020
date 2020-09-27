!include ..\..\..\..\sources.inc

TARGETNAME=m_textr1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm460

SOURCES=\
    texture.bmp.cpp
