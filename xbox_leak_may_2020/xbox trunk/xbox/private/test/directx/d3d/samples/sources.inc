!include ..\..\..\sources.inc

!ifdef WINXTARGET

TARGETLIBS=\
    $(TARGETLIBS) \
    $(TEST_LIB_PATH)\d3dbase.lib

!else

INCLUDES=\
    $(INCLUDES); \
    ..\..; \
    $(GENTEST_PATH); \
    $(BASEDIR)\private\test\directx\d3d\inc;

TARGETLIBS=\
    $(LIBRARY_PATH)\d3d8$(D).lib \
    $(LIBRARY_PATH)\d3dx8$(D).lib \
    $(LIBRARY_PATH)\xgraphics$(D).lib \
    $(TEST_LIB_PATH)\d3dbase.lib

!endif
