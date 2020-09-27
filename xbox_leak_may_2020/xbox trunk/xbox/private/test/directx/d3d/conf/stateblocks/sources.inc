!include ..\..\..\sources.inc

TARGETNAME=l_sblock
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    StateBeast.cpp \
    StateBeast8.cpp \
    Main.cpp
