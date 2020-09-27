!include ..\..\..\..\sources.inc

TARGETNAME=m_ball
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    bubbles.bmp.cpp \
    chain.bmp.cpp \
    smile.bmp.cpp \
