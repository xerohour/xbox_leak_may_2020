!include ..\..\..\sources.inc

TARGETNAME=l_rx2251
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

USER_C_FLAGS=$(USER_C_FLAGS) /Zm140

SOURCES=\
    basic.cpp \
    basic.rc \
    dxlogo.bmp.cpp
