##############
# sources.inc
##############

### Jaybaz - enable PDB7 for all of VS.
BUILD_USING_VC7PDBS=1

### AndreiAk - enable _SHIP switch. See bug #70704 for info
!if "$(BT_BISLABMACHINE)"=="1"
_SHIP=1
!endif

# First, define some root variables for intra project references.
# By default, these are:
#
# SRC_ROOT          $(VSROOT)\src
# COMMON_ROOT       $(SRC_ROOT)\common
# EXTERNAL_ROOT     $(VSROOT)\external
# VSPUBLIC          $(VSROOT)\public

!ifndef SRC_ROOT
SRC_ROOT=$(VSROOT)\src
!endif

!ifndef COMMON_ROOT
COMMON_ROOT=$(SRC_ROOT)\common
!endif

!ifndef EXTERNAL_ROOT
EXTERNAL_ROOT=$(VSROOT)\external
!endif

!ifndef VSPUBLIC
VSPUBLIC=$(VSROOT)\public
!endif

!ifndef NONSHIP_LIB_PATH
NONSHIP_LIB_PATH=$(VSPUBLIC)\lib\$(TARGET_DIRECTORY)
!endif

!ifndef NONSHIP_INC_PATH
NONSHIP_INC_PATH=$(VSPUBLIC)\inc
!endif

!ifdef ATL_INC_PATH
ATL_INCLUDES=$(ATL_INC_PATH)
!endif

# (dougf) nobody needs the icecap 3.6 stuff, so we don't build it anymore
# !ifndef ICECAP_LIB_PATH
# ICECAP_LIB_PATH=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\icecap3.6
# !endif

# Office external includes
!ifndef EXT_OFFICE_INC
!if "$(VSMSO10)"=="1"
EXT_OFFICE_INC=$(EXTERNAL_ROOT)\inc\office10
!else
EXT_OFFICE_INC=$(EXTERNAL_ROOT)\inc\office9
!endif
!endif

# XML Parser SDK paths
!ifndef XML_INC_PATH
XML_INC_PATH=$(DEVTOOLS)\common\win32sdk\include
!endif
!ifndef XML_LIB_PATH
XML_LIB_PATH=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\lib
!endif

# Standard includes: 
COMMON_INCLUDES=$(COMMON_ROOT)\inc;$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY);$(DEVTOOLS)\common\win32sdk\include;$(DEVTOOLS)\common\win32sdk\include\nonship
INCLUDES=$(COMMON_INCLUDES);$(INCLUDES)
CTC_INCLUDES=$(COMMON_INCLUDES);$(CRT_INC_PATH);$(EXT_OFFICE_INC)

NO_OAK_INC_PATH=1
NO_UMINCL_PATH=1

# Tool definitions (explicitly pathed):

USE_XBOX_BUILD_TOOLS=1

!ifdef USE_XBOX_BUILD_TOOLS

# Use Xbox build tools
CC_NAME=$(_NTROOT)\public\mstools\vc70\cl -nologo
RC_NAME=$(_NTROOT)\public\mstools\rc
LIB_NAME=$(_NTROOT)\public\mstools\vc70\lib
LINK_NAME=$(_NTROOT)\public\mstools\vc70\link
BSCMAKE=$(_NTROOT)\public\mstools\bscmake
MIDL=$(_NTROOT)\public\mstools\midl
CVTRES=$(_NTROOT)\public\mstools\cvtres
MKTYPLIB=$(_NTROOT)\public\mstools\mktyplib
MC=$(_NTROOT)\public\mstools\mc
HCW=$(_NTROOT)\public\mstools\hcw
MAPSYM=$(_NTROOT)\public\mstools\mapsym
TARGET_CPP=$(_NTROOT)\public\mstools\vc70\cl

# the following don't work yet...
# JVC_CMD=$(MSDEV_DIR)\vj$(MSDEV_VER)\bin\jvc
# JACTIVEX_CMD=$(MSDEV_DIR)\vj$(MSDEV_VER)\bin\jactivex
# JVIEW_CMD=$(MSDEV_DIR)\vj$(MSDEV_VER)\bin\jview
# DUBUILD_CMD=$(MSDEV_DIR)\vj$(MSDEV_VER)\bin\dubuild
# MAKECERT_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\makecert
# CERT2SPC_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\cert2spc
# SIGNCODE_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\signcode

!else

!ifdef BUILD_USING_VC70
VC=vc7
!else
!  ifdef BUILD_USING_VC60
VC=vc6
!  else
VC=vcLKG
!  endif
!endif

!ifndef USE_INSTALLED_TOOLS


# Allow CCover builds

!if "$(CCOVER)"=="YES"
CC_NAME=$(DEVTOOLS)\X86\CCover\bin\covc $(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\$(VC)\bin\cl -nologo
LINK_NAME=$(DEVTOOLS)\X86\CCover\bin\covlink $(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\$(VC)\bin\link
!else
!ifndef CC_NAME
!	if "$(BUILD_64BIT)"=="1" && "$(PROCESSOR_ARCHITECTURE)"=="x86"
CC_NAME=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)_$(BUILD_TARGET_ARCHITECTURE)\$(VC)\bin\cl -nologo
!	else
CC_NAME=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\$(VC)\bin\cl -nologo
!	endif
!endif
!ifndef LINK_NAME
!	if "$(BUILD_64BIT)"=="1" && "$(PROCESSOR_ARCHITECTURE)"=="x86"
LINK_NAME=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)_$(BUILD_TARGET_ARCHITECTURE)\$(VC)\bin\link
!	else
LINK_NAME=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\$(VC)\bin\link
!	endif
!endif
!endif

# End Allow CCover builds

RC_NAME=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\rc
!ifndef LIB_NAME
!	if "$(BUILD_64BIT)"=="1" && "$(PROCESSOR_ARCHITECTURE)"=="x86"
LIB_NAME=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)_$(BUILD_TARGET_ARCHITECTURE)\$(VC)\bin\lib
!	else
LIB_NAME=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\$(VC)\bin\lib
!	endif

!endif

!	if "$(BUILD_64BIT)"=="1" && "$(PROCESSOR_ARCHITECTURE)"=="x86"
BSCMAKE=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)_$(BUILD_TARGET_ARCHITECTURE)\$(VC)\bin\bscmake
!	else
BSCMAKE=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\$(VC)\bin\bscmake
!	endif

!ifndef BUILD_USING_VC60
!if "$(BUILD_64BIT)"=="1" && "$(PROCESSOR_ARCHITECTURE)"=="x86"
!ifndef BUILD_USING_VC7PDBS

!	if "$(BUILD_64BIT)"=="1" && "$(PROCESSOR_ARCHITECTURE)"=="x86"
BSCMAKE=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)_$(BUILD_TARGET_ARCHITECTURE)\$(VC)\bin\bscmake6
!	else
BSCMAKE=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\$(VC)\bin\bscmake6
!	endif

!endif
!endif
!endif

MIDL=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\midl

!	if "$(BUILD_64BIT)"=="1" && "$(PROCESSOR_ARCHITECTURE)"=="x86"
CVTRES=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)_$(BUILD_TARGET_ARCHITECTURE)\$(VC)\bin\cvtres
!	else
CVTRES=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\$(VC)\bin\cvtres
!	endif

MKTYPLIB=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\mktyplib
MC=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\mc
HCW=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\hcw
MAPSYM=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\mapsym

!	if "$(BUILD_64BIT)"=="1" && "$(PROCESSOR_ARCHITECTURE)"=="x86"
TARGET_CPP=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)_$(BUILD_TARGET_ARCHITECTURE)\$(VC)\bin\cl
!	else
TARGET_CPP=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\$(VC)\bin\cl
!	endif

JVC_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\vj6\bin\jvc
JACTIVEX_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\vj6\bin\jactivex
JVIEW_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\vj6\bin\jview
DUBUILD_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\vj6\bin\dubuild
MAKECERT_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\vj6\bin\makecert
CERT2SPC_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\vj6\bin\cert2spc
SIGNCODE_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\vj6\bin\signcode

# trevors: TLBIMP_CMD points to tlbimp.exe.  For the 64-bit cross platform
# compile we need to force it to the 32-bit version of tlbimp.exe
# Note: This will NOT work for the special URT_COMPAT_BUILD switch
TLBIMP_CMD=$(URTSDKROOT)\bin\tlbimp.exe /asmversion:7.0.3300.0
!if "$(BUILD_64BIT)" != ""
TLBIMP_CMD=$(VSROOT)\src\common\buildrun\x86\complus\$(BUILT_FLAVOR)\sdk\bin\tlbimp.exe
!endif
!if "$(PROCESSOR_ARCHITECTURE)" == "IA64"
TLBIMP_CMD=$(VSROOT)\src\common\buildrun\x86\complus\$(BUILT_FLAVOR)\sdk\bin\tlbimp.exe
!endif

!if "$(BT_BISLABMACHINE)" == ""
# trevors: TLBIMP_CMD points to tlbimp.exe.  For the 64-bit cross platform
# compile we need to force it to the 32-bit version of tlbimp.exe
# Note: This will NOT work for the special URT_COMPAT_BUILD switch
!if "$(BUILD_64BIT)" != ""
GACUTIL_CMD=$(VSROOT)\src\common\buildrun\x86\complus\$(BUILT_FLAVOR)\gacutil.exe
!else
GACUTIL_CMD=$(URTTARGET)\gacutil.exe
!endif
!if "$(PROCESSOR_ARCHITECTURE)" == "IA64"
GACUTIL_CMD=$(VSROOT)\src\common\buildrun\x86\complus\$(BUILT_FLAVOR)\gacutil.exe
!endif

!endif

# trevors: TLBEXP_CMD points to tlbimp.exe.  For the 64-bit cross platform
# compile we need to force it to the 32-bit version of tlbimp.exe
# Note: This will NOT work for the special URT_COMPAT_BUILD switch
TLBEXP_CMD=$(URTSDKROOT)\bin\tlbexp.exe
!if "$(BUILD_64BIT)" != ""
TLBEXP_CMD=$(VSROOT)\src\common\buildrun\x86\complus\$(BUILT_FLAVOR)\sdk\bin\tlbexp.exe
!endif
!if "$(PROCESSOR_ARCHITECTURE)" == "IA64"
TLBEXP_CMD=$(VSROOT)\src\common\buildrun\x86\complus\$(BUILT_FLAVOR)\sdk\bin\tlbexp.exe
!endif

# trevors: RESGEN_CMD points to tlbimp.exe.  For the 64-bit cross platform
# compile we need to force it to the 32-bit version of tlbimp.exe
# Note: This will NOT work for the special URT_COMPAT_BUILD switch
RESGEN_CMD=$(URTSDKROOT)\bin\resgen.exe
!if "$(BUILD_64BIT)" != ""
RESGEN_CMD=$(VSROOT)\src\common\buildrun\x86\complus\$(BUILT_FLAVOR)\sdk\bin\resgen.exe
!endif
!if "$(PROCESSOR_ARCHITECTURE)" == "IA64"
RESGEN_CMD=$(VSROOT)\src\common\buildrun\x86\complus\$(BUILT_FLAVOR)\sdk\bin\resgen.exe
!endif

# trevors: REGASM_CMD points to tlbimp.exe.  For the 64-bit cross platform
# compile we need to force it to the 32-bit version of tlbimp.exe
# Note: This will NOT work for the special URT_COMPAT_BUILD switch
REGASM_CMD=$(URTTARGET)\regasm.exe
!if "$(BUILD_64BIT)" != ""
REGASM_CMD=$(VSROOT)\src\common\buildrun\x86\complus\$(BUILT_FLAVOR)\regasm.exe
!endif
!if "$(PROCESSOR_ARCHITECTURE)" == "IA64"
REGASM_CMD=$(VSROOT)\src\common\buildrun\x86\complus\$(BUILT_FLAVOR)\regasm.exe
!endif

!else  # USE_INSTALLED_TOOLS

!ifndef MSDEV_VER
MSDEV_VER=98
!endif

!ifndef VC_TOOLS_DIR
VC_TOOLS_DIR=$(MSDEV_DIR)\VC$(MSDEV_VER)
!endif

!ifndef VC_COMMON_DIR
VC_COMMON_DIR=$(MSDEV_DIR)\Common\MsDev$(MSDEV_VER)
!endif

CC_NAME=$(VC_TOOLS_DIR)\bin\cl -nologo
RC_NAME=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\rc
LIB_NAME=$(VC_TOOLS_DIR)\bin\lib
LINK_NAME=$(VC_TOOLS_DIR)\bin\link
BSCMAKE=$(VC_TOOLS_DIR)\bin\bscmake
MIDL=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\midl
CVTRES=$(VC_TOOLS_DIR)\bin\cvtres
MKTYPLIB=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\mktyplib
MC=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\mc
HCW=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\hcw
MAPSYM=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\mapsym
TARGET_CPP=$(VC_TOOLS_DIR)\bin\cl
JVC_CMD=$(MSDEV_DIR)\vj$(MSDEV_VER)\bin\jvc
JACTIVEX_CMD=$(MSDEV_DIR)\vj$(MSDEV_VER)\bin\jactivex
JVIEW_CMD=$(MSDEV_DIR)\vj$(MSDEV_VER)\bin\jview
DUBUILD_CMD=$(MSDEV_DIR)\vj$(MSDEV_VER)\bin\dubuild
MAKECERT_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\makecert
CERT2SPC_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\cert2spc
SIGNCODE_CMD=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\win32sdk\bin\signcode

!endif # USE_INSTALLED_TOOLS

!endif # USE_XBOX_BUILD_TOOLS

CAPSIZE=$(DEVTOOLS)\$(PROCESSOR_ARCHITECTURE)\BBT3.0\capsize
IMPORT_CHECK=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\impchk
PERL_CMD=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\perl
BINPLACE=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\binplace
CTC_NAME=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\ctc -nologo
TOK_NAME=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\tok
INTLTOOL=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\intltool
IDRBYHLP=$(VSPUBLIC)\tools\idrbyhlp
IDHEADER=$(VSPUBLIC)\tools\idheader
IDRES=$(VSPUBLIC)\tools\idres
ZIP_CMD=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\zip
UNZIP_CMD=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\unzip
JDATE_CMD=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\jdate
DELNODE_CMD=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\delnode
VJEXEGEN_CMD=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\vjexegen
VSAUTODOC=$(VSPUBLIC)\tools\$(PROCESSOR_ARCHITECTURE)\vsautodoc
PIAPREFIX=

# Tool flag definitions:
MIDL_FLAGS=$(MIDL_FLAGS) -D_MIDL_USER_MARSHAL_DISABLED=1 -D__MIDL__=1
MIDL_NO_ROBUST=1

#!if "$(BUILD_64BIT)"=="1" || "$(PROCESSOR_ARCHITECTURE)"=="IA64"
#COFFBASE_TXT_FILE=$(VSPUBLIC)\dllbase64.txt
#!else
#COFFBASE_TXT_FILE=$(VSPUBLIC)\dllbase.txt
#!endif

IMPORT_CHECK_FILE=$(VSPUBLIC)\impchk.ini

# Set the global compiler warning file.
COMPILER_WARNINGS=/FI$(VSPUBLIC)\inc\warning.h

# (dougf) nobody needs the icecap 3.6 stuff, so we don't build it anymore
!if 0
# Set up retail-to-icecap conversion commands
!if "$(BUILD_RETAIL)" == "yes"
!if "$(BUILD_CAPSIZE)"!="NO"
!if "$(PROCESSOR_ARCHITECTURE)"=="x86"
CAPSIZE_CMD=if exist $(@) $(CAPSIZE) /nologo /terse /o $(@:retail=icecap) /pdb $(@D:retail=icecap)\$(@B).pdb $(@)
!else
CAPSIZE_CMD=if exist $(@) copy $(@) $(@:retail=icecap) & copy $(@D)\$(@B).pdb $(@D:retail=icecap)\$(@B).pdb
!endif
COPYEXE_CMD=if exist $(@) copy $(@) $(@:retail=icecap) & copy $(@D)\$(@B).pdb $(@D:retail=icecap)\$(@B).pdb
IMPORT_CHECK_CMD=$(IMPORT_CHECK) -F:$(IMPORT_CHECK_FILE) $(@)
!endif
!endif
!endif

!if !$(FREEBUILD)
DEBUG_CRTS=1
C_DEFINES=$(C_DEFINES) -DDEBUG
! endif

!if !$(FREEBUILD)
D=d
!else
D=
!endif

!if !$(FREEBUILD)
! ifndef NO_INCREMENTAL_LINKING
USE_INCREMENTAL_LINKING=1
! endif
USE_INCREMENTAL_COMPILING=1
NO_OPTIDATA=1
!endif

# Enable nmake compile batching
BATCH_NMAKE=1

# We're not building for NT 5.0 (yet)
WIN32_WINNT_VERSION=0x0400

!ifdef BUILD_USING_VC60
MFC_VER=42
ATL_VER=21
!else
MFC_VER=70
ATL_VER=70
!endif

!ifndef BUILD_USING_VC60
C_FLAGS=$(C_FLAGS) 
!ifdef LOG_BUILD_TIMES
C_FLAGS=$(C_FLAGS) /Bt
!endif
#build using VC6-compatible PDB format for now
!if "$(BUILD_64BIT)"=="" && "$(PROCESSOR_ARCHITECTURE)"=="x86"
!ifndef BUILD_USING_VC7PDBS
C_FLAGS=$(C_FLAGS) /Zvc6 /noHRESULT
ASM_FLAGS=$(ASM_FLAGS) /Zvc6
LINKER_VC6PDBS=/debugtype:vc6
LINKER_FLAGS=$(LINKER_FLAGS) $(LINKER_VC6PDBS)
!endif
!endif
!if "$(BUILD_64BIT)" == "1" || "$(PROCESSOR_ARCHITECTURE)" == "IA64"
MIDL_FLAGS=$(MIDL_FLAGS) /win64
!else
MIDL_FLAGS=$(MIDL_FLAGS) /win32
!endif


!endif

!if "$(DISABLE_SECURITY_CHECKS)" != "1"
# REVIEW [tliang] Turning off security check for 64Bit build because Toolset 9175DF 64-bit can't seem to handle /GS and /RTC1 switch.
# REVIEW [v-liti] Add processor_architecture check since native 64Bit build cannot handle /GS and /RTC1 switch
!if "$(BUILD_64BIT)" == "" && "$(PROCESSOR_ARCHITECTURE)" == "x86"
C_FLAGS=$(C_FLAGS) /GS
!if "$(BUILT_FLAVOR)"=="debug" && "$(PROCESSOR_ARCHITECTURE)" == "x86"
C_FLAGS=$(C_FLAGS) /RTC1
!endif
!endif
!endif

!ifdef BUILD_USING_VC60
C_DEFINES=$(C_DEFINES) -D_BUILD_USING_VC60
!endif

!ifdef BUILD_USING_VC70
C_DEFINES=$(C_DEFINES) -D_BUILD_USING_VC70
!endif

# Set this until all _NEW_SDK switches have been safely removed
C_DEFINES=$(C_DEFINES) -D_NEW_SDK=1

!ifdef VSMSODEBUG
C_DEFINES=$(C_DEFINES) -DVSMSODEBUG
!endif


# Compile warnings are build errors
!if ("$(BT_BISLABMACHINE)"=="1" && "$(BT_BFORCEWX)"!="1")
MSC_WARNING_LEVEL=-W3
!else
MSC_WARNING_LEVEL=-W3 -WX
!endif


# Prevent some other compiler command-line switches and defines
NT_UP=0
NO_NT_INST=1
NO_X86_CPU_OPT=1
NO_ERRATA_FLAGS=1
NO_MSC_CALL_DEFINE=1
NO_CONDITION_HANDLING=1
NO_TARGET_DBG_DEFINES=1
NO_NATIVE_DEF_FLAGS=1
NO_RTTI_DEF_FLAGS=1
SIMPLE_WIN32_DEFINE=1
NOT_LEAN_AND_MEAN=1

# Turn on use of synchronous exception handling:
USE_SYNCHRONOUS_EH=1

# Set default directory for satellite DLLs
!ifndef CODEPAGE
CODEPAGE=1033
!endif

# Temporary: our current version of rc does not handle the -z switch
RCNOFONTMAP=1

DARWIN=$(VSROOT)\external\darwin
!if "$(BUILD_64BIT)"=="1"
DARWIN_V11=$(DARWIN)\v1.1
DARWIN_V15=$(DARWIN)\v1.5
DARWIN_V11_INCLUDES=$(DARWIN_V11)\i386\Include
DARWIN_V15_INCLUDES=$(DARWIN_V15)\i386\Include
!else
DARWIN_V11=$(DARWIN)\v1.1
DARWIN_V15=$(DARWIN)\v1.5
DARWIN_V11_INCLUDES=$(DARWIN_V11)\$(TARGETDIR)\Include
DARWIN_V15_INCLUDES=$(DARWIN_V15)\$(TARGETDIR)\Include
!endif

DARWIN_V11_LIB=$(DARWIN_V11)\$(TARGETDIR)\Lib
DARWIN_V15_LIB=$(DARWIN_V15)\$(TARGETDIR)\Lib



CONDITIONAL_INCLUDES=\
	$(CONDITIONAL_INCLUDES)\
	afxres.h		\
	afxwin.h		\
	afxext.h		\
	afxole.h		\
	afxpriv.h		\
	afxtempl.h		\
	afxodlgs.h		\
	afx.h			\
	afxmt.h			\
	afxv_cfg.h		\
	afxdisp.h  		\
	afxdb.h    		\
	afxdao.h   		\
	afxcmn.h   		\
	afxres.rc		\
	\
	resource.hm		\
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
	mipsinst.h		\
	msomac2.h		\
	events.h		\
	msorid.h		\
	cvinfo32.h		\
	shared.idh		\
	csm.h			\
	machack.h		\
	compman.h		\
	drawdib.h		\
	optncplr.h2		\
	optnbsc.h2 		\
	optnrc.h2  		\
	optnlib.h2 		\
	optnlink.h2		\
	optnmtl.h2 		\
	compman.h  		\
	drawdib.h  		\
	avifile.h  		\
	mciwnd.h   		\
	msvideo.h  		\
	avicap.h   		\
	types.h			\
	cvinfo32.h 		\
	msostiwd.h 		\
	msostiw.h  		\
	msosviwd.h 		\
	msosviw.h  		\
	optnlink.h2		\
	afxcmn.h   		\
	optncplr.h2		\
	afxcmn.h   		\
	optnbsc.h2 		\
	optnmtl.h2 		\
	optnlib.h2 		\
	optnrc.h2  		\
	miscvb.h		\
	cvinfo32.h		\
	vproj.hid		\
	ctldispids.h	\
	dispids.h		\
	_frmpch.h		\
	ole.h			\
	winbase.inl		\
	winuser.inl		\
	wsipv6ok.h		\
	ver.h			\
	icecap.h		\
	indentstyle.idh	\
	..\defs.h		\
