!IF 0

Copyright (c) 1990-1998  Microsoft Corporation

Module Name:

    sources.

Abstract:

    This file specifies the target component being built and the list of
    sources files needed to build that component.  Also specifies optional
    compiler switches and libraries that are unique for the component being
    built.

!ENDIF

!include ..\..\..\sources.inc

TARGETNAME=VCProjectEngine
TARGETTYPE=DYNLINK
TARGETPATH=$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)\$(VC7)\$(VCPACKAGES)
TARGETPATHLIB=$(VSBUILT)\$(BUILT_FLAVOR)\lib\$(TARGET_DIRECTORY)
TARGETEXT=dll

NO_BBT_COMMANDS=1
UMTYPE=windows

USE_IOSTREAM=1
USE_STATIC_ATL=1
USE_VSANSIDELAYLOAD=1

COFFBASE=VCProjectEngine
DLLDEF=vcpb.def
DLLENTRY=_DllMainCRTStartup

# this allows the use of a localized version of the resource file
!if "$(RCFILE)" == ""
RCFILE=vcpb
!endif

# Turn on GX
NO_EH=0

USE_MSVCRT=1

PASS0_HEADERDIR=$(VSBUILT)\$(BUILT_FLAVOR)\inc\$(TARGET_DIRECTORY)
PASS0_TLBDIR=$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY)
USER_INCLUDES=$(VC_INCLUDES);$(EXTERNAL_ROOT)\inc;$(VSBUILT)\$(BUILT_FLAVOR)\bin\$(TARGET_DIRECTORY);$(VSROOT)\src\common\idl\vs;$(VSROOT)\src\common\win95
LAST_INCLUDES=$(LANGAPI_ROOT)\include;$(LANGAPI_ROOT)\ncb
USER_C_DEFINES=$(USER_C_DEFINES) -DVC_PROXY -DSTRICT -DVS_PACKAGE -DUNICODE
USER_C_FLAGS=$(USER_C_FLAGS) -Zm400
#USER_C_FLAGS=$(USER_C_FLAGS) /Wp64

LIBRARIAN_FLAGS=$(LIBRARIAN_FLAGS) -name:$(TARGETNAME)
LINK_LIB_IGNORE_FLAG=$(LINK_LIB_IGNORE_FLAG) -IGNORE:4075,4099

DELAYLOAD=shell32.dll;comdlg32.dll;vsansi.dll;wininet.dll;UrlMon.dll

# These files are listed in alphabetical order.  Keep them that way! (Exception vcpb2.idl)
# add:  bldhelpers.cpp 
SOURCES= \
	guid.c \
	bldactions.cpp \
	BldEventTool.cpp \
	bldhelpers.cpp \
	bldspawner.cpp \
	bldthread.cpp \
	bldtool.cpp \
	BscTool.cpp \
	buildengine.cpp \
	buildlogging.cpp \
	cltool.cpp \
	cmdlines.cpp \
	configrecords.cpp \
	Configuration.cpp \
	CustomBuildTool.cpp \
	DebugSettings.cpp \
	dependencygraph.cpp \
	DeployTool.cpp \
	XboxDeployTool.cpp \
	XboxImageTool.cpp \
	EnvDaemon.cpp \
	EnCBldThrd.cpp \
	File.cpp \
	FileConfiguration.cpp \
	fileregistry.cpp \
	Filter.cpp \
	Globals.cpp \
	LibTool.cpp \
	LinkLibHelp.cpp \
	LinkTool.cpp \
	MidlToolBase.cpp \
	mrdependencies.cpp \
	NMakeTool.cpp \
	Platform.cpp \
	PlatformXbox.cpp \
	Project.cpp \
	ProjectOptions.cpp \
	ProjEvaluator.cpp \
	ProjWriter.cpp \
	PropContainer.cpp \
	RCTool.cpp \
	regscan.cpp \
	scanner.cpp \
	SDLtool.cpp \
	SettingsPage.cpp \
	StyleSheet.cpp \
	util2.cpp \
	VCNodeFactory.cpp \
	vcpb.cpp \
	VCProjectEngine.cpp \
	VCTool.cpp \
	XMLFile.cpp \
	$(RCFILE).rc 

#	ScriptExecutor.cpp \


TARGETLIBS= \
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\vcutillib$(D).lib \
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\vcidl.lib \
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\vsguids.lib \
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\vseeguids.lib \
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\envguids.lib \
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\ad2ide.lib \
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\util$(D).lib \
!if "$(BUILD_RETAIL)"=="no"
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\vsassert.lib \
!endif
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\unilib$(D).lib \
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\dpclstub.lib \
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\toolhrt$(D).lib \
!IF "$(BUILD_TARGET_ARCHITECTURE)"=="x86" # This line MUST come after all of the VS libraries, but before the SDK libraries.
	$(VSBUILT)\$(BUILT_FLAVOR)\lib\*\vsansidelayimp.lib \
!ENDIF
    $(SDK_LIB_PATH)\rpcrt4.lib \
	$(XML_LIB_PATH)\msxml2.lib \
	$(SDK_LIB_PATH)\uuid.lib \
	$(SDK_LIB_PATH)\oleaut32.lib \
	$(SDK_LIB_PATH)\ole32.lib \
	$(SDK_LIB_PATH)\kernel32.lib \
	$(SDK_LIB_PATH)\gdi32.lib \
	$(SDK_LIB_PATH)\user32.lib \
	$(SDK_LIB_PATH)\shell32.lib \
	$(SDK_LIB_PATH)\comdlg32.lib \
	$(SDK_LIB_PATH)\comctl32.lib \
	$(SDK_LIB_PATH)\advapi32.lib \
	$(SDK_LIB_PATH)\wininet.lib \
	$(SDK_LIB_PATH)\urlmon.lib \
	$(SDK_LIB_PATH)\Ws2_32.lib \
	$(CRT_LIB_PATH)\oldnames.lib 


PRECOMPILED_INCLUDE=stdafx.h
PRECOMPILED_CXX=1

NTTARGETFILE0= \
        $(URTTARGET)\Microsoft.VisualStudio.VCProjectEngine.dll

