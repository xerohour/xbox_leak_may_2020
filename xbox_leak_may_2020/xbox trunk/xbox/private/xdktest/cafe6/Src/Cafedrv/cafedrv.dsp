# Microsoft Developer Studio Project File - Name="cafedrv" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 60000
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=cafedrv - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "cafedrv.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "cafedrv.mak" CFG="cafedrv - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "cafedrv - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "cafedrv - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "cafedrv - Win32 Release NoOptimizations" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/cafe6/Src/Cafedrv", ZJLCAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "cafedrv - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MD /W3 /WX /GX /Zi /O1 /I "..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /i "res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /pdb:"../../bin/cafedrv.pdb" /debug /machine:I386 /out:"../../bin/cafedrv.exe" /libpath:"..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cafedrv - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /MDd /W3 /WX /Gm /GR /GX /ZI /Od /I "..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG" /d "_AFXDLL" /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386
# ADD LINK32 winmm.lib /nologo /subsystem:windows /pdb:"../../bin/cafedrvd.pdb" /debug /machine:I386 /out:"../../bin/cafedrvd.exe" /libpath:"..\..\lib" /heap:4096
# SUBTRACT LINK32 /pdb:none

!ELSEIF  "$(CFG)" == "cafedrv - Win32 Release NoOptimizations"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "PseudoDebug"
# PROP Intermediate_Dir "PseudoDebug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /WX /GX /ZI /Od /I "..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /WX /GX /ZI /Od /I "..\..\include" /FI"stdafx.h" /FI"W32Repl.h" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /win32
# ADD BASE RSC /l 0x409 /i "res" /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x409 /i "res" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib /nologo /subsystem:windows /pdb:"../../bin/cafedrv.pdb" /debug /machine:I386 /out:"../../bin/cafedrv.exe" /libpath:"..\..\lib"
# SUBTRACT BASE LINK32 /pdb:none
# ADD LINK32 winmm.lib /nologo /subsystem:windows /pdb:"../../bin/cafedrv.pdb" /debug /machine:I386 /out:"../../bin/cafedrv.exe" /libpath:"..\..\lib"
# SUBTRACT LINK32 /pdb:none

!ENDIF 

# Begin Target

# Name "cafedrv - Win32 Release"
# Name "cafedrv - Win32 Debug"
# Name "cafedrv - Win32 Release NoOptimizations"
# Begin Group "Source Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aplogdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\cafe.rc
# End Source File
# Begin Source File

SOURCE=.\cafebar.cpp
# End Source File
# Begin Source File

SOURCE=.\cafedrv.cpp
# End Source File
# Begin Source File

SOURCE=.\cafemdi.cpp
# End Source File
# Begin Source File

SOURCE=.\cafepage.cpp
# End Source File
# Begin Source File

SOURCE=.\cafexcpt.cpp
# End Source File
# Begin Source File

SOURCE=.\graph.cpp
# End Source File
# Begin Source File

SOURCE=.\graphdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\graphwnd.cpp
# End Source File
# Begin Source File

SOURCE=.\hotkey.cpp
# End Source File
# Begin Source File

SOURCE=.\logpage.cpp
# End Source File
# Begin Source File

SOURCE=.\logview.cpp
# End Source File
# Begin Source File

SOURCE=.\mainfrm.cpp
# End Source File
# Begin Source File

SOURCE=.\portdoc.cpp
# End Source File
# Begin Source File

SOURCE=.\portview.cpp
# End Source File
# Begin Source File

SOURCE=.\statepg.cpp
# End Source File
# Begin Source File

SOURCE=.\stdafx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\suitebar.cpp
# End Source File
# Begin Source File

SOURCE=.\suitedoc.cpp
# End Source File
# Begin Source File

SOURCE=.\suitepg.cpp
# End Source File
# Begin Source File

SOURCE=.\suitevw.cpp
# End Source File
# Begin Source File

SOURCE=.\tbdlg.cpp
# End Source File
# Begin Source File

SOURCE=.\testspg.cpp
# End Source File
# Begin Source File

SOURCE=.\treectl.cpp
# End Source File
# Begin Source File

SOURCE=.\vwprtbar.cpp
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter ""
# Begin Group "BMP"

# PROP Default_Filter "*.bmp"
# Begin Source File

SOURCE=.\RES\cafebar.BMP
# End Source File
# Begin Source File

SOURCE=.\Res\fldrcls.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\fldropen.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\glyphbad.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\glyphcol.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\glyphexp.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\glyphtst.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\subsuite.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\suitebar.bmp
# End Source File
# Begin Source File

SOURCE=.\Res\vwprtbar.bmp
# End Source File
# End Group
# Begin Group "ICO"

# PROP Default_Filter ".ico"
# Begin Source File

SOURCE=.\RES\CAFE.ICO
# End Source File
# Begin Source File

SOURCE=.\Res\DLLDOC.ICO
# End Source File
# Begin Source File

SOURCE=.\res\failed1.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico164.ico
# End Source File
# Begin Source File

SOURCE=.\res\ico605.ico
# End Source File
# Begin Source File

SOURCE=.\RES\PORTDOC.ICO
# End Source File
# Begin Source File

SOURCE=.\RES\UTENSILX.ICO
# End Source File
# End Group
# Begin Source File

SOURCE=.\Res\cafe.rc2
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\aplogdlg.h
# End Source File
# Begin Source File

SOURCE=.\cafebar.h
# End Source File
# Begin Source File

SOURCE=.\cafedrv.h
# End Source File
# Begin Source File

SOURCE=.\cafemdi.h
# End Source File
# Begin Source File

SOURCE=.\cafepage.h
# End Source File
# Begin Source File

SOURCE=.\caferes.h
# End Source File
# Begin Source File

SOURCE=.\cafexcpt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\edge.h
# End Source File
# Begin Source File

SOURCE=.\graph.h
# End Source File
# Begin Source File

SOURCE=.\graphdlg.h
# End Source File
# Begin Source File

SOURCE=.\graphwnd.h
# End Source File
# Begin Source File

SOURCE=.\hotkey.h
# End Source File
# Begin Source File

SOURCE=.\logpage.h
# End Source File
# Begin Source File

SOURCE=.\logview.h
# End Source File
# Begin Source File

SOURCE=.\mainfrm.h
# End Source File
# Begin Source File

SOURCE=.\portdoc.h
# End Source File
# Begin Source File

SOURCE=.\portview.h
# End Source File
# Begin Source File

SOURCE=.\statepg.h
# End Source File
# Begin Source File

SOURCE=.\stdafx.h
# End Source File
# Begin Source File

SOURCE=.\stepbar.h
# End Source File
# Begin Source File

SOURCE=.\suitebar.h
# End Source File
# Begin Source File

SOURCE=.\suitedoc.h
# End Source File
# Begin Source File

SOURCE=.\suitepg.h
# End Source File
# Begin Source File

SOURCE=.\suitevw.h
# End Source File
# Begin Source File

SOURCE=.\tbDlg.h
# End Source File
# Begin Source File

SOURCE=.\testspg.h
# End Source File
# Begin Source File

SOURCE=.\treectl.h
# End Source File
# Begin Source File

SOURCE=.\vwprtbar.h
# End Source File
# End Group
# End Target
# End Project
