!include ..\..\..\sources.inc

TARGETNAME=l_rx3959
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm200

SOURCES=\
    test.cpp \
    face0.bmp.cpp \
    face1.bmp.cpp \
    face2.bmp.cpp \
    face3.bmp.cpp \
    face4.bmp.cpp \
    face5.bmp.cpp
