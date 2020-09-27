!include ..\..\..\..\sources.inc

TARGETNAME=m_bumpn0
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    base.bmp.cpp \
    bump.bmp.cpp \
    speculr8.vsh.cpp