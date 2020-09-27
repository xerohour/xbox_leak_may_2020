C_DEFINES=$(C_DEFINES) -DNT -DXNET_BUILD_LIB$(LIBTAG)

INCLUDES=\
    $(BASEDIR)\public\sdk\inc; \
    $(BASEDIR)\private\ntos\net; \
    $(BASEDIR)\private\ntos\inc; \
    $(BASEDIR)\private\inc\crypto; \

!if "$(LIBTAG)"=="XW" || "$(LIBTAG)"=="OW"
INCLUDES=$(BASEDIR)\public\wsdk\inc;$(INCLUDES)
UMTYPE=windows
!endif

TARGETPATH=obj
TARGETTYPE=LIBRARY

!if "$(LIBTAG)"=="M"
TARGETNAME=xnetm$(D)
!endif

!if "$(LIBTAG)"=="X"
TARGETNAME=xnet$(D)
TARGETPATH=$(BASEDIR)\public\sdk\lib
!endif

!if "$(LIBTAG)"=="XS"
TARGETNAME=xnets$(D)
TARGETPATH=$(BASEDIR)\public\sdk\lib
!endif

!if "$(LIBTAG)"=="XW"
TARGETNAME=xnetw$(D)
TARGETPATH=$(BASEDIR)\public\sdk\lib
!endif

!if "$(LIBTAG)"=="O"
TARGETNAME=xneto$(D)
!endif

!if "$(LIBTAG)"=="OS"
TARGETNAME=xnetos$(D)
!endif

!if "$(LIBTAG)"=="OW"
TARGETNAME=xnetow$(D)
!endif

MSC_WARNING_LEVEL=/W3 /WX /Ob1

!if defined(GENASM)
MSC_WARNING_LEVEL=$(MSC_WARNING_LEVEL) /Fa
!endif

!if $(FREEBUILD) == 0
MSC_OPTIMIZATION=/Odi /Ob1
!endif

PRECOMPILED_INCLUDE=..\xnp.h
PRECOMPILED_PCH=xnp.pch
PRECOMPILED_OBJ=xnp.obj
PRECOMPILED_CXX=1

SOURCES=\
    tcpipxsum.asm \
    base.cpp \
    enet.cpp \
    halw.cpp \
    halx.cpp \
    ip.cpp \
    ipdhcp.cpp \
    ipdns.cpp \
    ipicmp.cpp \
    ipqos.cpp \
    nicw.cpp \
    nicx.cpp \
    sock.cpp \
    socktcp.cpp \
    sockudp.cpp \
    xnet.cpp \
    xnetp.cpp \

