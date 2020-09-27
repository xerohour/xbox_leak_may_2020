# Microsoft Developer Studio Project File - Name="MakeXBG" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=MakeXBG - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "MakeXBG.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "MakeXBG.mak" CFG="MakeXBG - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "MakeXBG - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "MakeXBG - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "MakeXBG - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir ".\Release"
# PROP BASE Intermediate_Dir ".\Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir ".\Release"
# PROP Intermediate_Dir ".\Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FD /c
# SUBTRACT CPP /Fr /YX /Yc /Yu
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 d3d8.lib d3dx8.lib d3dxof.lib dxguid.lib /nologo /subsystem:windows /machine:I386 /ignore:4089
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "MakeXBG - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\Debug"
# PROP BASE Intermediate_Dir ".\Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "c:\mssdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Fr /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 d3d8.lib d3dx8d.lib d3dxof.lib dxguid.lib /nologo /subsystem:windows /debug /machine:I386 /libpath:"c:\mssdk\lib"

!ENDIF 

# Begin Target

# Name "MakeXBG - Win32 Release"
# Name "MakeXBG - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;inl;fi;fd"
# Begin Source File

SOURCE=.\LoadOptionsDlg.h
# End Source File
# Begin Source File

SOURCE=.\MakeXBG.h
# End Source File
# Begin Source File

SOURCE=.\SetMeshFvfDlg.h
# End Source File
# Begin Source File

SOURCE=.\StripifyDlg.h
# End Source File
# Begin Source File

SOURCE=.\XBG.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\D3DFile.cpp
# End Source File
# Begin Source File

SOURCE=.\FVF.cpp
# End Source File
# Begin Source File

SOURCE=.\LoadOptionsDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\MakeXBG.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadX.cpp
# End Source File
# Begin Source File

SOURCE=.\ReadXBG.cpp
# End Source File
# Begin Source File

SOURCE=.\RenderView.cpp
# End Source File
# Begin Source File

SOURCE=.\SetMeshFvfDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\SplitterWnd.cpp
# End Source File
# Begin Source File

SOURCE=.\StripifyDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\TreeView.cpp
# End Source File
# Begin Source File

SOURCE=.\TriStripper.cpp
# End Source File
# Begin Source File

SOURCE=.\WriteXBG.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\MakeXBG.ico
# End Source File
# Begin Source File

SOURCE=.\MakeXBG.rc
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\Stdafx.cpp
# ADD CPP /Yc
# End Source File
# Begin Source File

SOURCE=.\Stdafx.h
# End Source File
# End Group
# End Target
# End Project
