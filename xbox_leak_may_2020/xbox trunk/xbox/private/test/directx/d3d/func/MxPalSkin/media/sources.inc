!include ..\..\..\..\sources.inc

TARGETNAME=m_mxpskn
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm240

SOURCES=\
    mxpalskin.bmp.cpp \
    mxpalskin2.vsh.cpp \
    mxpalskin3.vsh.cpp \
    mxpalskin4.vsh.cpp
