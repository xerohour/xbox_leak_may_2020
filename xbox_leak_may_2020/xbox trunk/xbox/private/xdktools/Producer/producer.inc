MAJORCOMP=Windows	
MINORCOMP=directx

USE_MFC=1
USE_MSVCRT=1
USE_STATIC_ATL=1
ATL_VER=30
USE_MAPSYM=1
NOT_UNICODE=1
PLATFORM_MFC_VER = 0x0421
WIN32_IE_VERSION=0x0400

PRECOMPILED_INCLUDE=stdafx.h
PRECOMPILED_CXX=1

!ifndef DMPROOT
DMPROOT = $(BASEDIR)\private\xdktools\Producer
!endif

#Shouldn't do -wd here, but I don't want to fix up all the source files yet...
C_DEFINES = $(C_DEFINES) -U_ATL_DLL -D_WIN32_IE=0x0400 -D_WIN32_WINNT=0x0400 -DDMP_XBOX
#-wd4242 -wd4244 -wd4296
!if "$(NTDEBUG)" == "ntsd" || "$(NTDEBUG)" == "cvp" || "$(NTDEBUG)" == "sym"

!if "$(BuildMode)" == "internal"
C_DEFINES = $(C_DEFINES) -DDEBUG=1
!else
C_DEFINES = $(C_DEFINES) -DRDEBUG=1
!endif

!endif


!if "$(NTDEBUG)"!="" && "$(NTDEBUG)"!="ntsdnodbg"
MFC_LIBS=  \
          $(WSDK_LIB_PATH)\mfc42d.lib \
          $(WSDK_LIB_PATH)\mfcd42d.lib \
          $(WSDK_LIB_PATH)\mfcn42d.lib \
          $(WSDK_LIB_PATH)\mfco42d.lib \
          $(WSDK_LIB_PATH)\mfcs42d.lib
!else
MFC_LIBS=  \
          $(WSDK_LIB_PATH)\mfc42.lib \
          $(WSDK_LIB_PATH)\mfcs42.lib
!endif

MFC_INCLUDES=$(WSDK_INC_PATH)\mfc42

MFC_STATIC_LIB=$(MFC_STATIC_LIB) \
    $(WSDK_LIB_PATH)\mfcs42.lib

TARGETLIBS=$(TARGETLIBS) \
    $(WSDK_LIB_PATH)\kernel32.lib \
    $(WSDK_LIB_PATH)\user32.lib \
    $(WSDK_LIB_PATH)\gdi32.lib \
    $(WSDK_LIB_PATH)\comdlg32.lib \
    $(WSDK_LIB_PATH)\comctl32.lib \
    $(WSDK_LIB_PATH)\shell32.lib \
    $(WSDK_LIB_PATH)\winmm.lib \
    $(WSDK_LIB_PATH)\ole32.lib \
    $(WSDK_LIB_PATH)\oleaut32.lib \
    $(WSDK_LIB_PATH)\htmlhelp.lib \
    $(WSDK_LIB_PATH)\msdmo.lib \
    $(WSDK_LIB_PATH)\version.lib \
    $(WSDK_LIB_PATH)\advapi32.lib \
    $(DMPROOT)\Libs\i386\JazzCommon.lib \
    $(DMPROOT)\Libs\i386\DMUSProdGUID.lib \
    $(DMPROOT)\Libs\i386\DMUSProdGUIDPrivate.lib \
    $(WSDK_LIB_PATH)\uuid.lib

INCLUDES=$(INCLUDES);   \
		$(DMPROOT)\Includes; \
		$(DMPROOT)\Shared; \
		$(DMPROOT)\JazzCommon; \
		$(WSDK_INC_PATH)\atlmfc; \
		$(WSDK_INC_PATH);
