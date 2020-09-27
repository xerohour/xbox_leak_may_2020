USE_LIBCMT=1
 
SYNCRHONIZE_DRAIN=1

LINKER_FLAGS=$(LINKER_FLAGS) /IGNORE:4049,4217 /MAP

C_DEFINES=$(C_DEFINES) -DXONLINE_BUILD_LIBX

INCLUDES=\
    $(BASEDIR)\public\sdk\inc; \
    $(BASEDIR)\private\inc;\
    $(BASEDIR)\private\inc\crypto;\
    $(BASEDIR)\private\ntos\inc;\
    $(BASEDIR)\private\ntos\xapi\inc;\
    $(BASEDIR)\private\online\kerb;\
    $(BASEDIR)\private\online\util;\
    $(BASEDIR)\private\online;\
    $(BASEDIR)\private\ntos\net;\
    $(BASEDIR)\private\online\test\common;

SOURCES=\
    OnlineTitleUpdate.cpp

TARGETLIBS=\
    $(SDK_LIB_PATH)\xonline$(D).lib \
    $(BASEDIR)\private\online\test\common\obj\i386\testutil$(D).lib \
    $(SDK_LIB_PATH)\xgraphics$(D).lib \
    $(LIBRARY_PATH)\d3d8$(D).lib \
    $(LIBRARY_PATH)\d3dx8$(D).lib
    
