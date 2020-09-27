!include ..\..\..\..\sources.inc

TARGETNAME=m_vpshd1
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    edge.bmp.cpp \
    brick.bmp.cpp \
    vshader1.vsh.cpp \
    pshader1.psh.cpp