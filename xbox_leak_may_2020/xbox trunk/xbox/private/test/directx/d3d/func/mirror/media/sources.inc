!include ..\..\..\..\sources.inc

TARGETNAME=m_mirror
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    texture.bmp.cpp \
    shade.bmp.cpp
