TARGETNAME=xbox11113

INCLUDES=\
    $(INCLUDES); \
    $(BASEDIR)\public\sdk\inc; \
    $(BASEDIR)\private\inc; \
    $(BASEDIR)\private\ntos\inc; \
    $(BASEDIR)\private\ntos\xapi\inc; \
    $(BASEDIR)\private\test\inc;

TARGETLIBS=\
    $(LIBRARY_PATH)\d3d8$(D).lib \
    $(LIBRARY_PATH)\d3dx8$(D).lib \
    $(LIBRARY_PATH)\xgraphics$(D).lib

SOURCES=\
    main.cpp \
    textr.cpp \
    util.cpp

XE_FLAGS=\
    /INSERTFILE:..\media\grid.bmp,grid.bmp,n \
