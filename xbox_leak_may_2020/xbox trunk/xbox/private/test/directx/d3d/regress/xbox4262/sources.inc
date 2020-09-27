!include ..\..\..\sources.inc

TARGETNAME=l_rx4262
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm200

SOURCES=\
    vpshader.cpp \
    edge.bmp.cpp \
    brick.bmp.cpp \
    vshader1.vsh.cpp
