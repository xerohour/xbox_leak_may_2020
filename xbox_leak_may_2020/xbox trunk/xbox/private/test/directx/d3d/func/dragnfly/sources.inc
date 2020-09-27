!include ..\..\..\sources.inc

USER_C_FLAGS=$(USER_C_FLAGS) /Zm350

TARGETNAME=l_drgfly
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

SYNCHRONIZE_BLOCK=1

SOURCES=\
    dragnfly.cpp \
    dragnfly.xdg.cpp
