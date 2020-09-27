
!ifndef WINX_INC_PATH
WINX_INC_PATH = $(BASEDIR)\public\wsdk\inc
!endif

!ifndef WINX_LIB_PATH
WINX_LIB_PATH = $(BASEDIR)\public\wsdk\lib
!endif

!ifndef WINX_DX8_INC_PATH
WINX_DX8_INC_PATH = $(BASEDIR)\public\wsdk\inc
!endif

!ifndef WINX_DX8_LIB_PATH
WINX_DX8_LIB_PATH = $(BASEDIR)\public\wsdk\lib
!endif

TEST_ROOT_PATH = $(BASEDIR)\private\test\directx\d3d
GENTEST_PATH = $(TEST_ROOT_PATH)\frame\gentest

BINPLACE_PLACEFILE=$(TEST_ROOT_PATH)\placebin.txt

BINPLACE_NO_VERSION_RESOURCE=1

!ifdef WINXTARGET

#USE_VC7_WINDOWS=1
USE_PDB=1

_NT386TREE=$(_NT386TREE)\winx

NO_NTDLL=1

COMPILER_WARNINGS=-FI$(BASEDIR)\public\sdk\inc\warning.h
!ifdef USE_LINT
LINT_FORCEDINCS=$(LINT_FORCEDINCS) -header($(BASEDIR)\public\sdk\inc\warning.h)
!endif

!if "$(TARGETTYPE)" == "PROGRAM"
UMENTRY=winmain
!elseif "$(TARGETTYPE)" == "DYNLINK"
DLLENTRY=_DllMainCRTStartup
!endif
UMTYPE=windows

SDK_INC_PATH = $(WINX_INC_PATH)
SDK_LIB_PATH = $(WINX_LIB_PATH)
CRT_INC_PATH = $(WINX_INC_PATH)
CRT_LIB_PATH = $(WINX_LIB_PATH)

TEST_LIB_PATH = $(BASEDIR)\private\test\lib_winx\i386
TEST_LIB_DEST = $(BASEDIR)\private\test\lib_winx
TEST_RES_PATH = ..\..\winx\obj\i386

INCLUDES=\
    $(WINX_DX8_INC_PATH); \
    $(WINX_INC_PATH); \
    $(INCLUDES); \
    ..\..; \
    $(GENTEST_PATH); \
    $(BASEDIR)\private\test\directx\d3d\inc;

TARGETLIBS=\
    $(WINX_LIB_PATH)\kernel32.lib \
    $(WINX_LIB_PATH)\user32.lib \
    $(WINX_LIB_PATH)\gdi32.lib \
    $(WINX_LIB_PATH)\winspool.lib \
    $(WINX_LIB_PATH)\comdlg32.lib \
    $(WINX_LIB_PATH)\advapi32.lib \
    $(WINX_LIB_PATH)\shell32.lib \
    $(WINX_LIB_PATH)\ole32.lib \
    $(WINX_LIB_PATH)\oleaut32.lib \
    $(WINX_LIB_PATH)\kernel32.lib \
    $(WINX_LIB_PATH)\odbc32.lib \
    $(WINX_LIB_PATH)\odbccp32.lib \
    $(WINX_LIB_PATH)\uuid.lib \
    $(WINX_LIB_PATH)\winmm.lib \
    $(WINX_LIB_PATH)\wsock32.lib \
    $(WINX_LIB_PATH)\ws2_32.lib \
    $(WINX_DX8_LIB_PATH)\dxguid.lib \
    $(WINX_DX8_LIB_PATH)\d3d8.lib \
!if $(FREEBUILD)
    $(WINX_DX8_LIB_PATH)\d3dx8.lib \
!else
    $(WINX_DX8_LIB_PATH)\d3dx8d.lib \
!endif
    $(WINX_DX8_LIB_PATH)\dinput8.lib

#C_DEFINES=$(C_DEFINES) -DUNICODE -D_UNICODE

!else

TEST_LIB_PATH = $(BASEDIR)\private\test\lib\i386
TEST_LIB_DEST = $(BASEDIR)\private\test\lib
TEST_RES_PATH = ..\..\xbox\obj\i386

!if "$(TARGETTYPE)" == "PROGRAM"
UMTYPE=xboxapp
UMENTRY=wWinMainCRTStartup
!elseif "$(TARGETTYPE)" == "DYNLINK"
UMTYPE=xboxdll
DLLENTRY=_DllMainCRTStartup
TARGETPATHLIB=$(TEST_LIB_DEST)
!endif

XEINCLUDES=$(_NTTREE)\xboxtest

INCLUDES=\
    $(INCLUDES); \
    ..\..; \
    $(GENTEST_PATH); \
    $(BASEDIR)\private\test\inc; \
    $(BASEDIR)\private\test\directx\d3d\inc;

TARGETLIBS=\
    $(BASEDIR)\private\test\lib\*\xtestlib.lib \
    $(BASEDIR)\private\test\lib\*\xnetrefi$(D).lib \
    $(LIBRARY_PATH)\xnet$(D).lib \
    $(LIBRARY_PATH)\d3d8$(D).lib \
    $(LIBRARY_PATH)\d3dx8$(D).lib \
    $(LIBRARY_PATH)\xgraphics$(D).lib \
    $(TEST_LIB_PATH)\xmem.lib \
    $(TEST_LIB_PATH)\xlog.lib \
 
C_DEFINES=$(C_DEFINES) -DUNDER_XBOX

!if defined(LOGTOWEB)
C_DEFINES=$(C_DEFINES) -DLOGTOWEB
!endif

!endif

