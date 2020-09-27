!include ..\..\..\sources.inc

TARGETNAME=l_wclip
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

C_DEFINES=$(C_DEFINES) -DTESTTYPE_WIREFRAME

SOURCES=\
    Clipping.cpp \
    Defaults.cpp \
    Line.cpp \
    LStrip.cpp \
    Point.cpp \
    Properties.cpp \
    TFan.cpp \
    Triangle.cpp \
    TStrip.cpp
