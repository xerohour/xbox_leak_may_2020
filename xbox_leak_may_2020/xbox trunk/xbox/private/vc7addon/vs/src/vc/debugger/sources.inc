
#
# Pick up master directory definitions

BUILD_USING_VC7PDBS=1

!include $(VSROOT)\sources.inc

#
# if RCTOOL is not defined, default to "rc"
# this allows the use a different rc from NT-J
#

!if "$(RCTOOL)" == ""
RCTOOL=rc
!endif

DEBUGGER_ROOT=$(SRC_ROOT)\vc\debugger
DBGPROXY_IDL_ROOT=$(DEBUGGER_ROOT)\idl\dbgproxy

!ifndef LANGAPI
LANGAPI=$(SRC_ROOT)\vc\langapi
!endif


#
# compiler defines
#
USER_C_DEFINES = $(USER_C_DEFINES) -DHOST32 -DTARGET32 -DNT_BUILD -DSTRICT -D_ATL_NO_ATTRIBUTES -DCC_LAZYTYPES -D_WIN32_WINNT=0x400 -D_RTC -DENABLE_IA64_SUPPORT=1 -DLNGNM=1

#
# Is this a ship build, or are chickens made of cheese?
#


!if "$(_SHIP)" == "1"
!message Building _SHIPping version, really, honest.
USER_C_DEFINES = $(USER_C_DEFINES) -D_SHIP
!endif

!if !$(FREEBUILD)
MKTYPLIB_FLAGS=$(MKTYPLIB_FLAGS) /D_DEBUG
!if $(IA64)!=1
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /GR /RTCs /RTCu
!else
MSC_OPTIMIZATION=$(MSC_OPTIMIZATION) /GR
!endif
!else
!endif


# Get all the other project paths.


NOT_LEAN_AND_MEAN=
386_STDCALL = 1
USE_NATIVE_EH = 0

DLLENTRY = _DllMainCRTStartup

USER_C_DEFINES = $(USER_C_DEFINES) -DWIN32 -DTARGET32 -DHOST32 -D_CROSS_PLATFORM_

MIPS_FLAGS = -DHOST_MIPS
386_FLAGS  = -DHOST_i386
ALPHA_FLAGS= -DHOST_ALPHA
PPC_FLAGS  = -DHOST_PPC
IA64_FLAGS = -DHOST_IA64
AXP64_FLAGS= -DHOST_AXP64

MIDL_FLAGS = $(MIDL_FLAGS) -D_MIDL_USER_MARSHAL_DISABLED=1

DEBUG_INCLUDES=$(DEBUGGER_ROOT)\include

USER_INCLUDES=$(DEBUG_INCLUDES);$(LANGAPI)\undname;$(SRC_ROOT)\debugger\inc
LAST_INCLUDES=$(LANGAPI)\include

#
# And some Lib paths
#

DEBUG_BIN=$(VSBUILT)\$(BUILT_FLAVOR)\bin
DEBUG_LIB=$(VSBUILT)\$(BUILT_FLAVOR)\lib

# define which MSPDB we link with
# change when we switch toolsets
MSPDBLIB = mspdb.lib
MSPDBLIBPATH = $(DEVTOOLS)\$(BUILD_TARGET_ARCHITECTURE)\$(VC)\lib\nonship
MSPDBDLL = mspdb70.dll
!if !$(FREEBUILD)
MSDISLIB = $(DEVTOOLS)\$(BUILD_TARGET_ARCHITECTURE)\$(VC)\lib\nonship\msds130d.lib
!else
MSDISLIB = $(DEVTOOLS)\$(BUILD_TARGET_ARCHITECTURE)\$(VC)\lib\nonship\msdis130.lib
!endif
MSOBJLIB = msobj.lib
MSOBJDLL = msobj10.dll
# temporary measure until symwriter talks 70
MSPDB70LIB = mspdb70.lib
MSPDB70DLL = mspdb70.dll


!if $(IA64)!=1
USER_C_FLAGS=$(USER_C_FLAGS) /GS
!endif

# Don't link to ntdll.lib
NO_NTDLL=1
USE_MSVCRT=1


#
# Do not create build.exe warning message for the following files:
#

CONDITIONAL_INCLUDES=\
		$(CONDITIONAL_INCLUDES)\
		winwlm.h		\
		macwin32.h		\
		rpcerr.h		\
		rpcmac.h		\
		macname1.h		\
		macapi.h		\
		macname2.h		\
		macpub.h		\
		strstream		\
		macocidl.h      \
		pshpck16.h      \
		new             \
		exception		\
		skbapi.h		\
		exdisp.odl		\
		vidsvr.odl		\
		afxres.h		\
		mipsinst.h
