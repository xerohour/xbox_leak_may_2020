# Microsoft Developer Studio Project File - Name="mods" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=mods - Win32 Debug Profiled
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Mods.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Mods.mak" CFG="mods - Win32 Debug Profiled"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mods - Win32 Release" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mods - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mods - Win32 Hybrid" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE "mods - Win32 Debug Profiled" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mods - Win32 Release"

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
# ADD CPP /nologo /G6 /MD /W3 /O2 /I "..\..\include" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /Yu"mods.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "NDEBUG"
# ADD RSC /l 0x409 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x05830000" /subsystem:windows /dll /machine:I386 /out:"..\..\..\maxsdk\plugin\mods.dlm" /release /delayload:"edmodel.dll"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug"

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
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /Zi /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /YX /c
# ADD CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT CPP /YX /Yc /Yu
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x05830000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\mods.dlm" /delayload:"edmodel.dll"

!ELSEIF  "$(CFG)" == "mods - Win32 Hybrid"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir ".\mods___W"
# PROP BASE Intermediate_Dir ".\mods___W"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\Hybrid"
# PROP Intermediate_Dir ".\Hybrid"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G5 /MD /W3 /Gm /GX /Zi /Od /I "..\..\include" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /Yu"mods.h" /c
# ADD CPP /nologo /G6 /MD /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /Yu"mods.h" /FD /c
# SUBTRACT CPP /Fr
# ADD BASE MTL /nologo /D "_DEBUG" /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib /nologo /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\mods.dlm"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x05830000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\mods.dlm" /delayload:"edmodel.dll"

!ELSEIF  "$(CFG)" == "mods - Win32 Debug Profiled"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "mods___Win32_Debug_Profiled"
# PROP BASE Intermediate_Dir "mods___Win32_Debug_Profiled"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir ".\DbgProf"
# PROP Intermediate_Dir ".\DbgProf"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FD /c
# SUBTRACT BASE CPP /YX /Yc /Yu
# ADD CPP /nologo /G6 /MDd /W3 /Gm /ZI /Od /I "..\..\include" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /FD /Gh /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x05830000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\mods.dlm" /delayload:"edmodel.dll"
# ADD LINK32 comctl32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib delayimp.lib /nologo /base:"0x05830000" /subsystem:windows /dll /debug /machine:I386 /out:"..\..\..\maxsdk\plugin\mods.dlm" /delayload:"edmodel.dll"

!ENDIF 

# Begin Target

# Name "mods - Win32 Release"
# Name "mods - Win32 Debug"
# Name "mods - Win32 Hybrid"
# Name "mods - Win32 Debug Profiled"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;hpj;bat;for;f90"
# Begin Source File

SOURCE=.\afregion.cpp
# End Source File
# Begin Source File

SOURCE=.\bend.cpp
# End Source File
# Begin Source File

SOURCE=.\bomb.cpp
# End Source File
# Begin Source File

SOURCE=.\clstnode.cpp
# End Source File
# Begin Source File

SOURCE=.\clustmod.cpp
# End Source File
# Begin Source File

SOURCE=.\deflect.cpp
# End Source File
# Begin Source File

SOURCE=.\delmod.cpp
# End Source File
# Begin Source File

SOURCE=.\delsmod.cpp
# End Source File
# Begin Source File

SOURCE=.\dispApprox.cpp
# End Source File
# Begin Source File

SOURCE=.\dispmod.cpp
# End Source File
# Begin Source File

SOURCE=.\editmesh.cpp
# End Source File
# Begin Source File

SOURCE=.\editmops.cpp
# End Source File
# Begin Source File

SOURCE=.\editpat.cpp
# End Source File
# Begin Source File

SOURCE=.\editpops.cpp
# End Source File
# Begin Source File

SOURCE=.\editsops.cpp
# End Source File
# Begin Source File

SOURCE=.\editspl.cpp
# End Source File
# Begin Source File

SOURCE=.\edmdata.cpp
# End Source File
# Begin Source File

SOURCE=.\edmrest.cpp
# End Source File
# Begin Source File

SOURCE=.\edmui.cpp
# End Source File
# Begin Source File

SOURCE=.\extrude.cpp
# End Source File
# Begin Source File

SOURCE=.\fextrude.cpp
# End Source File
# Begin Source File

SOURCE=.\gravity.cpp
# End Source File
# Begin Source File

SOURCE=.\mapmod.cpp
# End Source File
# Begin Source File

SOURCE=.\meshsel.cpp
# End Source File
# Begin Source File

SOURCE=.\mirror.cpp
# End Source File
# Begin Source File

SOURCE=.\mods.cpp
# ADD CPP /Yc"mods.h"
# End Source File
# Begin Source File

SOURCE=.\mods.def
# End Source File
# Begin Source File

SOURCE=.\mods.rc
# End Source File
# Begin Source File

SOURCE=.\noisemod.cpp
# End Source File
# Begin Source File

SOURCE=.\nspline.cpp
# End Source File
# Begin Source File

SOURCE=.\optmod.cpp
# End Source File
# Begin Source File

SOURCE=.\resettm.cpp
# End Source File
# Begin Source File

SOURCE=.\sctex.cpp
# End Source File
# Begin Source File

SOURCE=.\selmod.cpp
# End Source File
# Begin Source File

SOURCE=.\sinwave.cpp
# End Source File
# Begin Source File

SOURCE=.\skew.cpp
# End Source File
# Begin Source File

SOURCE=.\surfmod.cpp
# End Source File
# Begin Source File

SOURCE=.\surfrev.cpp
# End Source File
# Begin Source File

SOURCE=.\taper.cpp
# End Source File
# Begin Source File

SOURCE=.\tessmod.cpp
# End Source File
# Begin Source File

SOURCE=.\TWIST.CPP
# PROP BASE Exclude_From_Build 1
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\twist1.cpp
# End Source File
# Begin Source File

SOURCE=.\twist2.cpp
# End Source File
# Begin Source File

SOURCE=.\unwrap.cpp
# End Source File
# Begin Source File

SOURCE=.\uvwxform.cpp
# End Source File
# Begin Source File

SOURCE=.\wind.cpp
# End Source File
# Begin Source File

SOURCE=..\..\Lib\BMM.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\core.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\edmodel.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\geom.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\gfx.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\mesh.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\MNMath.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\Paramblk2.lib
# End Source File
# Begin Source File

SOURCE=..\..\Lib\maxutil.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\acap.lib
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl;fi;fd"
# Begin Source File

SOURCE=.\EDITMESH.H
# End Source File
# Begin Source File

SOURCE=.\EDITPAT.H
# End Source File
# Begin Source File

SOURCE=.\EDITSPL.H
# End Source File
# Begin Source File

SOURCE=.\EDMDATA.H
# End Source File
# Begin Source File

SOURCE=.\MAPPING.H
# End Source File
# Begin Source File

SOURCE=..\..\Include\mesh.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\meshadj.h
# End Source File
# Begin Source File

SOURCE=..\..\Include\meshdelta.h
# End Source File
# Begin Source File

SOURCE=.\MODS.H
# End Source File
# Begin Source File

SOURCE=.\SCTEX.H
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;cnt;rtf;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\addvertc.cur
# End Source File
# Begin Source File

SOURCE=.\attach.cur
# End Source File
# Begin Source File

SOURCE=.\Bevel.cur
# End Source File
# Begin Source File

SOURCE=.\bmp00001.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00002.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00003.bmp
# End Source File
# Begin Source File

SOURCE=.\bmp00004.bmp
# End Source File
# Begin Source File

SOURCE=.\booleant.bmp
# End Source File
# Begin Source File

SOURCE=.\boolinte.cur
# End Source File
# Begin Source File

SOURCE=.\boolsubt.cur
# End Source File
# Begin Source File

SOURCE=.\boolunio.cur
# End Source File
# Begin Source File

SOURCE=.\bulbmask.bmp
# End Source File
# Begin Source File

SOURCE=.\bulbs.bmp
# End Source File
# Begin Source File

SOURCE=.\chamfer.cur
# End Source File
# Begin Source File

SOURCE=.\CROSSHR.CUR
# End Source File
# Begin Source File

SOURCE=.\crossins.cur
# End Source File
# Begin Source File

SOURCE=.\echamfer.cur
# End Source File
# Begin Source File

SOURCE=.\extrudec.cur
# End Source File
# Begin Source File

SOURCE=.\faceselt.bmp
# End Source File
# Begin Source File

SOURCE=.\fillet.cur
# End Source File
# Begin Source File

SOURCE=.\magnify.cur
# End Source File
# Begin Source File

SOURCE=.\mask_boo.bmp
# End Source File
# Begin Source File

SOURCE=.\mask_fac.bmp
# End Source File
# Begin Source File

SOURCE=.\mask_unw.bmp
# End Source File
# Begin Source File

SOURCE=.\move_x.cur
# End Source File
# Begin Source File

SOURCE=.\move_y.cur
# End Source File
# Begin Source File

SOURCE=.\outline.cur
# End Source File
# Begin Source File

SOURCE=.\panhand.cur
# End Source File
# Begin Source File

SOURCE=.\patchsel.bmp
# End Source File
# Begin Source File

SOURCE=.\patselm.bmp
# End Source File
# Begin Source File

SOURCE=.\region.cur
# End Source File
# Begin Source File

SOURCE=.\scale_x.cur
# End Source File
# Begin Source File

SOURCE=.\scale_y.cur
# End Source File
# Begin Source File

SOURCE=.\segbreak.cur
# End Source File
# Begin Source File

SOURCE=.\segrefin.cur
# End Source File
# Begin Source File

SOURCE=.\selmask.bmp
# End Source File
# Begin Source File

SOURCE=.\splselm.bmp
# End Source File
# Begin Source File

SOURCE=.\thselcur.cur
# End Source File
# Begin Source File

SOURCE=.\Trim.cur
# End Source File
# Begin Source File

SOURCE=.\trimbut.bmp
# End Source File
# Begin Source File

SOURCE=.\trimmask.bmp
# End Source File
# Begin Source File

SOURCE=.\unwrap_option.bmp
# End Source File
# Begin Source File

SOURCE=.\unwrapto.bmp
# End Source File
# Begin Source File

SOURCE=.\vchamfer.cur
# End Source File
# Begin Source File

SOURCE=.\vertconn.cur
# End Source File
# Begin Source File

SOURCE=.\vinsert.cur
# End Source File
# Begin Source File

SOURCE=.\weld.cur
# End Source File
# End Group
# End Target
# End Project
