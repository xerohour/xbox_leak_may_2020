C_DEFINES=$(C_DEFINES) -DNT -DXONLINE_BUILD_LIB$(LIBTAG)

#
# IMPORTANT: This is required for early online titles that includes the updated dash.
# This enables to code to detect the current dash version and allow autoupdate of the
# dash through the title. In the future, if we no longer ship the dash update with 
# online titles, we should remove this define tag.
#

C_DEFINES=$(C_DEFINES) -DDASH_UPDATE_IN_TITLE

INCLUDES=\
    $(BASEDIR)\public\sdk\inc;\
    $(BASEDIR)\private\inc\crypto;\
    $(BASEDIR)\private\ntos\inc;\
    $(BASEDIR)\private\ntos\fatx;\
    $(BASEDIR)\private\ntos\xapi\inc;\
    $(BASEDIR)\private\ntos\xapi\k32;\
    $(BASEDIR)\private\ntos\net;\
    $(BASEDIR)\private\online;

!if "$(LIBTAG)"=="W"
INCLUDES=$(BASEDIR)\public\wsdk\inc;$(INCLUDES)
UMTYPE=windows
!endif

TARGETPATH=$(BASEDIR)\public\sdk\lib
TARGETTYPE=LIBRARY

!if "$(LIBTAG)"=="X"
TARGETNAME=xonline$(D)
!endif

!if "$(LIBTAG)"=="S"
TARGETNAME=xonlines$(D)
SECTAG=s
!endif

!if "$(LIBTAG)"=="W"
TARGETNAME=xonlinew$(D)
SECTAG=w
!endif

MSC_WARNING_LEVEL=/W3 /WX /Ob1

!if defined(GENASM)
MSC_WARNING_LEVEL=$(MSC_WARNING_LEVEL) /Fa
!endif

!if $(FREEBUILD) == 0
MSC_OPTIMIZATION=/Odi /Ob1
!endif

PRECOMPILED_INCLUDE=..\xonp.h
PRECOMPILED_PCH=xonp.pch
PRECOMPILED_OBJ=xonp.obj
PRECOMPILED_CXX=1

SOURCES=\
    stats.cpp \
    xonline.cpp \
    kerberos.cpp \
    krb5.cpp \
    msr_md5.cpp \
    md4ms.cpp \
    msasn1.cpp \
    autoupd.cpp \
    baseio.cpp \
    cfcache.cpp \
    contutil.cpp \
    contdl.cpp \
    contrm.cpp \
    contver.cpp \
    contenum.cpp \
    dirops.cpp \
    download.cpp \
    symmdec.cpp \
    upload.cpp \
    xontask.cpp \
    xrlutil.cpp \
    decalign.cpp \
    decblk.cpp \
    decin.cpp \
    decout.cpp \
    dectree.cpp \
    decuncmp.cpp \
    decverb.cpp \
    decxlat.cpp \
    maketbl.cpp \
    xonlzx.cpp \
    logon.cpp \
    match.cpp \
    service.cpp \
    users.cpp \
    presence.cpp \
    billing.cpp \
    xbosutil.cpp \
    localcache.cpp \
    msgclient.cpp \
    accounts.cpp \
    difpatch.cpp \
    patchutl.cpp \
    dvdload.cpp \
    olddash.cpp \

OBJLIBFILES=\
    $(BASEDIR)\private\ntos\net\libo$(SECTAG)\obj\*\xneto$(SECTAG)$(D).lib
