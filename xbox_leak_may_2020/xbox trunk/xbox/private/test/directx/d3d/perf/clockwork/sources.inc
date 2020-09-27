TARGETNAME=clockwork
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\sources.inc

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dbase.lib

SOURCES=\
    main.cpp \
    clockwork.cpp \
    cogs.cpp \
    clockwork.rc
