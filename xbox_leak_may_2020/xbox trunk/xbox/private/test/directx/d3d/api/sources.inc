!include ..\..\sources.inc

TARGETNAME=l_d3dapi
TARGETTYPE=LIBRARY
TARGETPATH=$(TEST_LIB_DEST)

#USE_MAPSYM=1

SOURCES=d3dapi.cpp				\
        math.cpp                \
        d3dapi.rc               \
		volume.cpp				\
		cubetexture.cpp			\
		volumetexture.cpp		\
		texture.cpp				\
		direct3d8apiinit.cpp	\
		direct3d8api.cpp		\
		direct3ddevice8init.cpp	\
		direct3ddevice8.cpp		\
		resource.cpp			\
		linkme.cpp				\
        vertexbuffer.cpp        

#       uma.cpp                 \ //broken for the time being
#		cursor.cpp				\
#		swizzler.cpp			\
#       lock.cpp                \
#       cubelock.cpp            \
#       volumelock.cpp          \


INCLUDES=$(SDK_INC_PATH); $(BASEDIR)\private\ntos\inc;\
         $(BASEDIR)\private\test\inc;..

USE_LIBCMT=1

TARGETLIBS= \
        $(BASEDIR)\private\test\lib\*\xlog.lib \
        $(LIBRARY_PATH)\d3d8$(D).lib \
        $(LIBRARY_PATH)\d3dx8$(D).lib
 

#        $(BASEDIR)\private\test\lib\*\xtestlib.lib \

PRECOMPILED_INCLUDE=..\d3dapi.hpp
PRECOMPILED_PCH=..\precomp.pch
PRECOMPILED_OBJ=..\precomp.obj

