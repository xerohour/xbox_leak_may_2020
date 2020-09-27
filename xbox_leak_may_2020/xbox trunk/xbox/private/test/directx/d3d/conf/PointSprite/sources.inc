!include ..\..\..\sources.inc

TARGETNAME=l_psprit
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    PointSprite.cpp \
    attenfvfl.cpp \
    attenfvfls.cpp \
    scalefvfl.cpp \
    scalefvfls.cpp \
    scalefvftl.cpp \
    scalefvftls.cpp \
    flags.cpp \
    batch.cpp
