# sources.inc

# First, define some root variables for intra project references.
# By default, these are:
#
# VCROOT            $(SRC_ROOT)\vc
# PKG_ROOT          $(VCROOT)\pkgs
# LANGAPI_ROOT      $(VCROOT)\langapi
# APPWIZ_ROOT       $(VCROOT)\wizards\appwiz
# VC7		    VC7
# VCADDCLASS	    VCAddClass
# VCPACKAGES        VCPackages
# VCPROJECTITEMS    VCProjectItems
# VCWIZARDS         VCWizards
# VCPROJECTS        VCProjects
# VCCONTEXTITEMS    VCContextItems
#VCSERVERTEMPLATES	DesignerTemplates

BUILD_USING_VC7PDBS=1

!include $(VSROOT)\sources.inc

!ifndef VC7
VC7=VC7
!endif

!ifndef VCPACKAGES
VCPACKAGES=VCPackages
!endif

!ifndef VCPROJECTITEMS
VCPROJECTITEMS=VCProjectItems
!endif

!ifndef VCADDCLASS
VCADDCLASS=VCAddClass
!endif

!ifndef VCSERVERTEMPLATES
VCSERVERTEMPLATES=DesignerTemplates
!endif

!ifndef VCWIZARDS
VCWIZARDS=VCWizards
!endif

!ifndef VCPROJECTS
VCPROJECTS=VCProjects
!endif

!ifndef VCCONTEXTITEMS
VCCONTEXTITEMS=VCContextItems
!endif

!ifndef VCROOT
VCROOT=$(SRC_ROOT)\vc\ide
!endif

!ifndef PKG_ROOT
PKG_ROOT=$(VCROOT)\pkgs
!endif

!ifndef LANGAPI_ROOT
! ifdef LANGAPI
LANGAPI_ROOT=$(LANGAPI)
! else
LANGAPI_ROOT=$(SRC_ROOT)\vc\langapi
! endif
!endif

!ifndef APPWIZ_ROOT
APPWIZ_ROOT=$(VCROOT)\wizards\appwiz
!endif

!ifndef OSDEBUG4
OSDEBUG4=1
!endif

# turn on 32-bit types everywhere with one swell foop.
386_CPPFLAGS = -DCC_CVTYPE32 -DCC_LAZYTYPES

!ifndef DEBUGGER_BUILD
DEBUGGER_BUILD=1
!endif

USE_PDB=1
NTLEGO=1
USE_MSVCRT=1
NO_NTDLL=0

# Call Conventions

386_STDCALL=0 

!ifndef VC_INCLUDES
VC_INCLUDES=$(VCROOT)\include
!endif

!ifndef VC_PKG_INCLUDES
VC_PKG_INCLUDES=$(PKG_ROOT)\include
!endif

VC_INCLUDES=$(VC_INCLUDES);$(VC_PKG_INCLUDES)

# Turn on use of synchronous exception handling:
!ifndef NO_EH
USE_SYNCHRONOUS_EH=1
!endif

# Temporary: our current version of rc does not handle the -z switch
RCNOFONTMAP=1

# add $(VCROOT)\inc first on the path so that the .tlb imports in .idl files
# can be found without having to specify a relative path
PATH=$(VCROOT)\inc;$(PATH)

# no synchrounous error handling
NO_EH=0

# VC specific stuff
NOT_LEAN_AND_MEAN=1

COMPILER_WARNINGS=/FIwarning.h
USER_C_DEFINES=-D_WINDOWS -D_DBCS=1 -D_NTWIN -D_SUSHI -DCC_MULTIPLE_STORES=1 -D_TEST -DADDR_MIXED \
	-DCODEVIEW -DCONDITION_HANDLING -DCROSS_PLATFORM -DHOST32 -DHOST_i386 -Dx86 -D_MBCS -D_WINDLL \
	-D_ATL_STATIC_REGISTRY

!if 0
USER_C_DEFINES=/D_AFX_ENABLE_INLINES -DWIN32 -D_WINDOWS -D_USRDLL \
	-D_DBCS=1 -D_DLL=1 -D_MFC25 -D_MT=1 -D_NT1X_=100 -D_NTWIN -D_SUSHI \
	-D_TEST -D_WIN32_WINNT=0x0400 -D_X86_=1 -DADDR_MIXED -DCODEVIEW -DCONDITION_HANDLING=1  \
	-DCROSS_PLATFORM -DDBG=1 -DDEVL=1 -DFPO=0 -DHOST32 -DHOST_i386 -Di386=1 \
	-DNT_BUILD -DNT_INST=0 -DNT_UP=1 -DPPC_PLATFORM -DWIN32=100 \
	-DWINNT=1 -Dx86 -D_MBCS -D_WINDLL -D_AFXDLL
!endif

USER_C_DEFINES=$(USER_C_DEFINES) -DNEW_PROJBLD_TURNED_ON

!IF ("$(D)" == "d")
USER_C_DEFINES=$(USER_C_DEFINES) -D_DEBUG -DDEBUG=1
!ENDIF


#
# Reduce build warnings
#
CONDITIONAL_INCLUDES=\
    $(CONDITIONAL_INCLUDES) \
	afxcoll.h \
	afxcview.h \
	afxdllx.h \
	afxstr.h \
	atlbase.h \
	atlcoll.h \
	atlcom.h \
	atlctl.h \
	atlctrls.h \
	atldbcli.h \
	atldbsch.h \
	atlfile.h \
	atlhost.h \
	atlimage.h \
	atlimage.h \
	atlimpl.cpp \
	atlsafe.h \
	atlsoap.h \
	atlstr.h \
	atlsync.h \
	atlsync.inl \
	atltime.h \
	atltypes.h \
	atlwin.h \
	cstringt.h \
	macos\types.h \
	statreg.cpp \
	statreg.h \
	afxctl.h \
	shell.h \
	about.h \
	shell.hid \
	toolexpt.h \
	afxplex_.h \
	imeutil.h \
	barglob.h \
	imeutil.h \
	autowind.h \
	bardockx.h \
	docobfrm.h \
	docobvw.h \
	toolexpt.h \
	shellrec.h \
	ipcmpvw.h \
	shldocs_.h \
	imeutil.h \
	find.h \
	remi.h \
	bardockx.h \
	toolexpt.h \
	vcver.h \
	pkg.h \
	afxdlgs.h \
	mfcappwz.rc_ \
	afxdlgs.h \
