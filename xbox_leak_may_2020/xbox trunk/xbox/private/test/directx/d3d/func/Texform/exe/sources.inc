TARGETNAME=Texform
TARGETTYPE=PROGRAM
TARGETPATH=obj

!include ..\..\..\..\sources.inc

#UMRES=$(TEST_RES_PATH)\Texform.res

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dlocus.lib \
    $(TEST_LIB_PATH)\d3dftest.lib \
    $(TEST_LIB_PATH)\3dmath.lib \
    $(TEST_LIB_PATH)\CShapes.lib \
    $(TEST_LIB_PATH)\l_texfrm.lib

SOURCES=\
    include.cpp
