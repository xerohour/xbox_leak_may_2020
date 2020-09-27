# Microsoft Developer Studio Project File - Name="prjbldui" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 61000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=prjbldui - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "prjbldui.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "prjbldui.mak" CFG="prjbldui - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "prjbldui - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "prjbldui - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "prjbldui - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
F90=fl32.exe
86=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                          "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                          WIN32=100 /d WINNT=1
386=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                                         "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                                         WIN32=100 /d WINNT=1
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo
# ADD CPP /MT
# ADD CPP /W3
# ADD CPP /GX
# ADD CPP /O2
# ADD CPP /I "$(VSROOT)\src\vc\ide\pkgs\include"
# ADD CPP /D "WIN32" /D "NDEBUG" /D "_WINDOWS"
# ADD CPP /YX
# ADD CPP /FD
# ADD CPP /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo
# ADD MTL /D "NDEBUG"
# ADD MTL /mktyplib203
# ADD MTL /o "NUL"
# ADD MTL /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409
# ADD RSC /i "$(VSROOT)\src\vc\ide\pkgs\include"
# ADD RSC /d "_AFXDLL" /d "_BLD_INTERNAL" /d _DBCS=1 /d _DLL=1 /d "_MBCS" /d "_MFC25" /d _MT=1 /d _NT1X_=100 /d "_NTWIN" /d "_SUSHI" /d "_TEST" /d "_VPROJ" /d "_VRES" /d _WIN32_WINNT=0x0400 /d "_WINDOWS" /d _X86_=1 /d "ADDR_MIXED" /d "CODEVIEW" /d CONDITION_HANDLING=1 /d "CROSS_PLATFORM" /d DBG=1 /d DEVL=1 /d FPO=0 /d "HOST32" /d i386=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "PPC_PLATFORM" /d "SHIP" /d "STRICT" /d "TARGET32" /d WIN32=100 /d WINNT=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# ADD LINK32 /nologo
# ADD LINK32 /subsystem:windows
# ADD LINK32 /dll
# ADD LINK32 /machine:I386
# ADD LINK32 /NOENTRY
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "prjbldui - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 1
# PROP Target_Dir ""
F90=fl32.exe
86=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                          "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                          WIN32=100 /d WINNT=1
386=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "OSDEBUG4" /d                                                         "PPC_PLATFORM" /d "RESOURCE_NODE" /d "SHIP" /d "STRICT" /d "TARGET32" /d                                                         WIN32=100 /d WINNT=1
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /FD /c
# ADD CPP /nologo
# ADD CPP /MTd
# ADD CPP /W3
# ADD CPP /Gm
# ADD CPP /GX
# ADD CPP /Zi
# ADD CPP /Od
# ADD CPP /I "$(VSROOT)\src\vc\ide\pkgs\include" /I "$(VSROOT)\external\inc\office9"
# ADD CPP /D "WIN32" /D "_DEBUG" /D "_WINDOWS"
# ADD CPP /YX
# ADD CPP /FD
# ADD CPP /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /o "NUL" /win32
# ADD MTL /nologo
# ADD MTL /D "_DEBUG"
# ADD MTL /mktyplib203
# ADD MTL /o "NUL"
# ADD MTL /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409
# ADD RSC /i "$(VSROOT)\src\vc\ide\pkgs\include" /i "$(LANGAPI)\include" /i "..\src"
# ADD RSC /d "_AFXDLL" /d "_BLD_INTERNAL" /d _DBCS=1 /d _DLL=1 /d "_MBCS" /d "_MFC25" /d _MT=1 /d _NT1X_=100 /d "_NTWIN" /d "_SUSHI" /d "_TEST" /d "_VPROJ" /d "_VRES" /d _WIN32_WINNT=0x0400 /d "_WINDOWS" /d _X86_=1 /d "ADDR_MIXED" /d "CODEVIEW" /d CONDITION_HANDLING=1 /d "CROSS_PLATFORM" /d DBG=1 /d DEBUG=1 /d DEVL=1 /d FPO=0 /d "HOST32" /d i386=1 /d "NEW_PROJ_VIEW" /d "NT_BUILD" /d NT_INST=0 /d NT_UP=1 /d "PPC_PLATFORM" /d "SHIP" /d "STRICT" /d "TARGET32" /d WIN32=100 /d WINNT=1
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /pdbtype:con
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib
# ADD LINK32 /nologo
# ADD LINK32 /subsystem:windows
# ADD LINK32 /dll
# ADD LINK32 /pdb:none
# ADD LINK32 /machine:I386
# ADD LINK32 /nodefaultlib
# ADD LINK32 /NOENTRY
# Begin Custom Build
OutDir=.\Debug
TargetPath=.\Debug\prjbldui.dll
InputPath=..\ui_dll\resdll\prjbldui.dsp
SOURCE="$(InputPath)"

"$(OutDir)\copy.trg" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	copy $(TargetPath)  $(VSBUILT)\debug\bin\i386\resources\1033 
	echo copy file  time > "$(OutDir)\copy.trg" 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "prjbldui - Win32 Release"
# Name "prjbldui - Win32 Debug"
# Begin Group "Build Events"

# PROP Default_Filter "bev"
# Begin Time Custom Build Step

SOURCE=".\Post-Build"

!IF  "$(CFG)" == "prjbldui - Win32 Release"

# PROP Custom_Build_Time "Post Build"
# PROP Build_Tool "Custom Build"
# Begin Custom Build
InputPath=".\Post-Build"

	copy prjbldui.dll $(VSDROP)\Debug\Bin\i386\vspkgs\1033\prjbldui.dll

# End Custom Build

!ELSEIF  "$(CFG)" == "prjbldui - Win32 Debug"

# PROP Build_Tool "Custom Build"
# Begin Custom Build
InputPath=".\Post-Build"

	copy prjbldui.dll $(VSDROP)\Debug\Bin\i386\vspkgs\1033\prjbldui.dll

# End Custom Build

!ENDIF 

# End Time Custom Build Step
# End Group
# Begin Source File

SOURCE=.\gpcmd.h
# End Source File
# Begin Source File

SOURCE=.\gpmenus.cmd

!IF  "$(CFG)" == "prjbldui - Win32 Release"

# PROP Build_Tool "Custom Build"
# Begin Custom Build
InputPath=.\gpmenus.cmd

"gpmenus.cto" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(VSROOT)\public\tools\x86\ctc gpmenus.cmd gpmenus.cto /I$(VSROOT)\src\common\inc /I$(VSROOT)\src\vc\ide\pkgs\include /I$(VSROOT)\external\inc\office9

# End Custom Build

!ELSEIF  "$(CFG)" == "prjbldui - Win32 Debug"

# PROP Build_Tool "Custom Build"
# Begin Custom Build - CTC gpmenus.cmd
InputPath=.\gpmenus.cmd

"gpmenus.cto" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	$(VSROOT)\public\tools\x86\ctc gpmenus.cmd gpmenus.cto /I$(VSROOT)\src\common\inc /I$(VSROOT)\src\vc\ide\pkgs\include /I$(VSROOT)\external\inc\office9

# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\gpmenus.cto
# End Source File
# Begin Source File

SOURCE=.\ids.h
# End Source File
# Begin Source File

SOURCE=.\prjbldui.rc

!IF  "$(CFG)" == "prjbldui - Win32 Release"

!ELSEIF  "$(CFG)" == "prjbldui - Win32 Debug"

# ADD BASE RSC /l 0x409
# ADD RSC /l 0x409

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\prjbldui.rc2
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\resource.hm
# End Source File
# End Target
# End Project
