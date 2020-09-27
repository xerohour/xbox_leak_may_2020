# First, define some root variables for intra project references.
# By default, these are:
#
# LANGAPI_ROOT      $(VCROOT)\langapi
# CRT_ROOT          $(VCROOT)\crt
# PDBSRC_ROOT       $(VCROOT)\pdb
# IDE_ROOT          $(VCROOT)\ide
# DEBUGGER_ROOT     $(IDE_ROOT)\debugger
# VCPUBLIC          $(VCROOT)\public

SDK_INC_PATH=$(BASEDIR)\public\wsdk\inc
SDK_LIB_PATH=$(BASEDIR)\public\wsdk\lib
WIN32_IE_VERSION=0x0400
ATL_VER=30

!ifndef LANGAPI_ROOT
LANGAPI_ROOT=$(VCROOT)\langapi
!endif
LANGAPI=$(LANGAPI_ROOT)

!ifndef CRT_ROOT
CRT_ROOT = $(VCROOT)\crt
!endif

!ifndef PDBSRC_ROOT
PDBSRC_ROOT = $(VCROOT)\pdb
!endif

!ifndef IDE_ROOT
IDE_ROOT=$(VCROOT)\ide
!endif

!ifndef DEBUGGER_ROOT
DEBUGGER_ROOT = $(IDE_ROOT)\debugger
!endif

#!ifndef VCPUBLIC
#VCPUBLIC = $(VCROOT)\public
#!endif

!ifndef VCMISC
VCMISC = $(VCROOT)\misc
!endif

!ifndef NONSHIP_LIB_PATH
NONSHIP_LIB_PATH = $(VCPUBLIC)\nonship\lib\$(TARGET_DIRECTORY)
!endif

!ifndef NONSHIP_INC_PATH
NONSHIP_INC_PATH = $(VCPUBLIC)\nonship\inc
!endif

!ifndef MISC_LIB_PATH
MISC_LIB_PATH = $(VCMISC)\lib
!endif

!ifndef MFC_LIBDIR
MFC_LIBDIR=$(SDK_LIB_PATH)
!endif

!ifndef OSDEBUG4
OSDEBUG4=1
!endif

# turn on 32-bit types everywhere with one swell foop.
386_CPPFLAGS = -DCC_CVTYPE32 -DCC_LAZYTYPES
ALPHA_CPPFLAGS = -DCC_CVTYPE32 -DCC_LAZYTYPES

!ifndef DEBUGGER_BUILD
DEBUGGER_BUILD=1
!endif

MASTER_VERSION_FILE = $(LANGAPI_ROOT)\include\version.h

# Then define the SDK, MFC and CRT paths needed to build VC
# (these are different from the paths used to build NT)

DBC=1

NEW_CRTS = 1

!ifndef MFC_VER
MFC_VER = 42
!endif

!IF $(FREEBUILD)
! IFDEF USE_MFCUNICODE
MFC_LIBS= \
    $(MFC_LIBDIR)\mfc$(MFC_VER)u.lib \
    $(MFC_LIBDIR)\mfcs$(MFC_VER)u.lib
! ELSE
MFC_LIBS= \
    $(MFC_LIBDIR)\mfc$(MFC_VER).lib \
    $(MFC_LIBDIR)\mfcs$(MFC_VER).lib
! ENDIF

!ELSE

DEBUG_CRTS=1
! IFDEF USE_MFCUNICODE
MFC_LIBS= \
    $(MFC_LIBDIR)\mfc$(MFC_VER)ud.lib \
    $(MFC_LIBDIR)\mfcs$(MFC_VER)ud.lib \
    $(MFC_LIBDIR)\mfco$(MFC_VER)ud.lib \
    $(MFC_LIBDIR)\mfcd$(MFC_VER)ud.lib \
    $(MFC_LIBDIR)\mfcn$(MFC_VER)ud.lib
! ELSE
MFC_LIBS= \
    $(MFC_LIBDIR)\mfc$(MFC_VER)d.lib \
    $(MFC_LIBDIR)\mfcs$(MFC_VER)d.lib \
    $(MFC_LIBDIR)\mfco$(MFC_VER)d.lib \
    $(MFC_LIBDIR)\mfcd$(MFC_VER)d.lib \
    $(MFC_LIBDIR)\mfcn$(MFC_VER)d.lib
! ENDIF
!ENDIF

# Override the binplace and placefil flags so we can use the VC coffbase/placefil.txt

COFFBASE_TXT_FILE =$(VCMISC)\dllbase.txt

# Set the global compiler warning file.
COMPILER_WARNINGS = /FI$(VCMISC)\inc\warning.h

# All of the VC product can use Native EH, RTTI, PDB's, and Lego

USE_NATIVE_EH=1
USE_PDB=1
NTLEGO=1
USE_MSVCRT=1
NO_NTDLL=1
USE_RTTI=1

# Finally, define a master VC_INCLUDES macro to collect the non-ship includes
# and langapi headers

VC_INCLUDES = $(LANGAPI_ROOT)\include;$(VCMISC)\inc

!ifdef _VCBUILD
NO_INCREMENTAL_LINKING=1
!endif

!if !$(FREEBUILD)
! ifndef NO_INCREMENTAL_LINKING
USE_INCREMENTAL_LINKING=1
! endif
NO_OPTIDATA=1
!endif

# Delay load some libraries
!if $(FREEBUILD)
LINKER_FLAGS=$(LINKER_FLAGS) \
	/delayload:mfcclwz.dll \
	/delayload:version.dll \
	/delayload:comdlg32.dll \
	/delayload:odbc32.dll \
	/delayload:msenc10.dll \
	$(CRT_LIB_PATH)\delayimp.lib
!if $(386)
LINKER_FLAGS=$(LINKER_FLAGS) /filealign:4096 
!endif
!endif


# Turn on lazy pdbs except for build lab or explicit request not to.
! if ("$(NO_SEPARATE_TYPES)" == "" && "$(_VCBUILD)" == "")
! if "$(DEBUG)" == "1" && $(386)
LINKER_FLAGS=$(LINKER_FLAGS) -pdbtype:sept
! endif 
! endif

# Temporary: our current version of rc does not handle the -z switch
RCNOFONTMAP=1
