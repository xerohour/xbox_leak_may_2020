# Microsoft Developer Studio Project File - Name="CUSTCTRL" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=CUSTCTRL - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Custctrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Custctrl.mak" CFG="CUSTCTRL - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "CUSTCTRL - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CUSTCTRL - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "CUSTCTRL - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/devel/3dswin/src/maxsdk/Samples/HowTo/CustCtrl", YIOBAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "CUSTCTRL - Win32 Release"

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
# ADD BASE CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G6 /MD /W3 /GX /O2 /I "\maxsdk\include" /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /Fd"CUSTCTRL.pdb" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x087C0000" /subsystem:windows /dll /machine:I386 /out:"..\..\..\..\maxsdk\plugin\custctrl.dlu" /release
# SUBTRACT LINK32 /debug

!ELSEIF  "$(CFG)" == "CUSTCTRL - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\CUSTCTRL"
# PROP BASE Intermediate_Dir ".\CUSTCTRL"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Hybrid"
# PROP Intermediate_Dir ".\Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /Gm /GX /Zi /Od /I "\MAXSDK\INCLUDE" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /Fd"CUSTCTRL.pdb" /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /GX /ZI /Od /I "\maxsdk\include" /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /Fd"CUSTCTRL.pdb" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"\MAXSDK\PLUGIN\CUSTCTRL.DLO"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x087C0000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\maxsdk\plugin\custctrl.dlu"

!ELSEIF  "$(CFG)" == "CUSTCTRL - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "CUSTCTRL"
# PROP BASE Intermediate_Dir "CUSTCTRL"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Debug"
# PROP Intermediate_Dir ".\Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MD /W3 /Gm /GX /Zi /Od /I "h:\devel\3dswin\src\maxsdk\include" /I "d:\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /Fd"CUSTCTRL.pdb" /FD /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /GX /ZI /Od /I "..\..\..\..\maxsdk\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /Fd"CUSTCTRL.pdb" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"\maxsdk\plugin\custctrl.dlu"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /base:"0x087C0000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\..\maxsdk\plugin\custctrl.dlu"

!ENDIF 

# Begin Target

# Name "CUSTCTRL - Win32 Release"
# Name "CUSTCTRL - Win32 Hybrid"
# Name "CUSTCTRL - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\CUSTCTRL.CPP
# End Source File
# Begin Source File

SOURCE=.\CUSTCTRL.DEF
# End Source File
# Begin Source File

SOURCE=.\CUSTCTRL.H
# End Source File
# Begin Source File

SOURCE=.\CUSTCTRL.RC
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\ccflyimg.bmp
# End Source File
# Begin Source File

SOURCE=.\ccflymsk.bmp
# End Source File
# Begin Source File

SOURCE=.\ccimage.bmp
# End Source File
# Begin Source File

SOURCE=.\imagemsk.bmp
# End Source File
# Begin Source File

SOURCE=.\tb.bmp
# End Source File
# Begin Source File

SOURCE=.\tbmask.bmp
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\..\..\MAXSDK\Lib\core.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\MAXSDK\Lib\geom.lib
# End Source File
# Begin Source File

SOURCE=..\..\..\..\MAXSDK\Lib\maxutil.lib
# End Source File
# End Target
# End Project
