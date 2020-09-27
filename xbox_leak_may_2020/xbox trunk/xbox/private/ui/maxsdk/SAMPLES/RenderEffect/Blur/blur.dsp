# Microsoft Developer Studio Project File - Name="blur" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=blur - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "blur.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "blur.mak" CFG="blur - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "blur - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "blur - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "blur - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "blur - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
blur=midl.exe
# ADD BASE blur /nologo /D "NDEBUG" /win32
# ADD blur /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "..\..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Yu"pch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x075B0000" /subsystem:windows /dll /machine:I386 /out:"..\..\..\..\maxsdk\plugin\blur.dlv" /release

!ELSEIF  "$(CFG)" == "blur - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
blur=midl.exe
# ADD BASE blur /nologo /D "_DEBUG" /win32
# ADD blur /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Yu"pch.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x075B0000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\maxsdk\plugin\blur.dlv"

!ELSEIF  "$(CFG)" == "blur - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\blur___Wi"
# PROP BASE Intermediate_Dir ".\blur___Wi"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Hybrid"
# PROP Intermediate_Dir ".\Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
blur=midl.exe
# ADD BASE blur /nologo /D "_DEBUG" /win32
# ADD blur /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE CPP /nologo /G5 /MD /W3 /Gm /GX /Zi /Od /I "..\include" /I "..\MaxSDK\Include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"blur.h" /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "..\..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Yu"pch.h" /FD /c
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\maxsdk\plugin\blur.dlx"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /base:"0x075B0000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\maxsdk\plugin\blur.dlv"

!ENDIF 

# Begin Target

# Name "blur - Win32 Release"
# Name "blur - Win32 Debug"
# Name "blur - Win32 Hybrid"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "_blurTypes (.cpp)"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\_blurTypes\blurBase.cpp
# End Source File
# Begin Source File

SOURCE=.\_blurTypes\blurDirectional.cpp
# End Source File
# Begin Source File

SOURCE=.\_blurTypes\blurRadial.cpp
# End Source File
# Begin Source File

SOURCE=.\_blurTypes\blurUniform.cpp
# End Source File
# End Group
# Begin Group "_selTypes (.cpp)"

# PROP Default_Filter ".cpp"
# Begin Source File

SOURCE=.\_selectTypes\scTex.cpp
# End Source File
# Begin Source File

SOURCE=.\_selectTypes\selIgnBack.cpp
# End Source File
# Begin Source File

SOURCE=.\_selectTypes\selImage.cpp
# End Source File
# Begin Source File

SOURCE=.\_selectTypes\selLum.cpp
# End Source File
# Begin Source File

SOURCE=.\_selectTypes\selMaps.cpp
# End Source File
# End Group
# Begin Source File

SOURCE=.\blur.rc
# End Source File
# Begin Source File

SOURCE=.\blurMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\dlgProcs.cpp
# End Source File
# Begin Source File

SOURCE=.\dllMain.cpp
# ADD CPP /Yc"pch.h"
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Group "_blurTypes (.h)"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\_blurTypes\blurBase.h
# End Source File
# Begin Source File

SOURCE=.\_blurTypes\blurDirectional.h
# End Source File
# Begin Source File

SOURCE=.\_blurTypes\blurRadial.h
# End Source File
# Begin Source File

SOURCE=.\_blurTypes\blurUniform.h
# End Source File
# End Group
# Begin Group "_selTypes (.h)"

# PROP Default_Filter ".h"
# Begin Source File

SOURCE=.\_selectTypes\scTex.h
# End Source File
# Begin Source File

SOURCE=.\_selectTypes\selBase.h
# End Source File
# Begin Source File

SOURCE=.\_selectTypes\selIgnBack.h
# End Source File
# Begin Source File

SOURCE=.\_selectTypes\selImage.h
# End Source File
# Begin Source File

SOURCE=.\_selectTypes\selLum.h
# End Source File
# Begin Source File

SOURCE=.\_selectTypes\selMaps.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\blurMgr.h
# End Source File
# Begin Source File

SOURCE=.\dlgProcs.h
# End Source File
# Begin Source File

SOURCE=.\dllMain.h
# End Source File
# Begin Source File

SOURCE=.\globals.h
# End Source File
# Begin Source File

SOURCE=.\pch.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# End Group
# Begin Group "Library Files"

# PROP Default_Filter ".lib"
# Begin Source File

SOURCE=..\..\..\lib\core.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\geom.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\maxutil.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\Paramblk2.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\lib\bmm.lib
# End Source File
# End Group
# Begin Source File

SOURCE=.\blur.def
# End Source File
# End Target
# End Project
