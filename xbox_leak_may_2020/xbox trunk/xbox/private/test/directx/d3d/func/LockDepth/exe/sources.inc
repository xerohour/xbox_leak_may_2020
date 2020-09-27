TARGETNAME=LockDpth
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

#UMRES=$(TEST_RES_PATH)\lockdepth.res

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\d3dftest.lib \
    $(TEST_LIB_PATH)\l_lckdep.lib

SOURCES=\
    include.cpp
