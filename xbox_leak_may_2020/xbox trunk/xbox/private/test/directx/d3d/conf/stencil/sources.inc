!include ..\..\..\sources.inc

TARGETNAME=l_stncil
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    stencil.cpp \
    flags.cpp \
    cap.cpp \
    clear.cpp \
    func.cpp \
    ignore.cpp \
    op.cpp \
    scorrupt.cpp \
    zcorrupt.cpp
