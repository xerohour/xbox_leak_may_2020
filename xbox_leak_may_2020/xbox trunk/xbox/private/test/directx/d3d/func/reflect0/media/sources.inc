!include ..\..\..\..\sources.inc

TARGETNAME=m_rflct0
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm180

SOURCES=\
    face0.bmp.cpp \
    face1.bmp.cpp \
    face2.bmp.cpp \
    face3.bmp.cpp \
    face4.bmp.cpp \
    face5.bmp.cpp
