!include ..\..\..\sources.inc

TARGETNAME=l_snow
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

USER_C_FLAGS=$(USER_C_FLAGS) /Zm240

SOURCES=\
    d3dsnow.cpp \
    backgrnd.bmp.cpp \
    snow.bmp.cpp \
    sheet.bmp.cpp
