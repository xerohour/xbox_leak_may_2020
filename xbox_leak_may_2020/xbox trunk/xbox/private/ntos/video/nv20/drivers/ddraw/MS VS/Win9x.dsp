# Microsoft Developer Studio Project File - Name="win9x" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=win9x - Win32 NV4 statistics
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "win9x.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "win9x.mak" CFG="win9x - Win32 NV4 statistics"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "win9x - Win32 NV4 debug" (based on "Win32 (x86) External Target")
!MESSAGE "win9x - Win32 NV4 develop" (based on "Win32 (x86) External Target")
!MESSAGE "win9x - Win32 NV4 retail" (based on "Win32 (x86) External Target")
!MESSAGE "win9x - Win32 NV4 statistics" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "win9x"
# PROP Scc_LocalPath "."

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "temp"
# PROP BASE Intermediate_Dir "temp"
# PROP BASE Cmd_Line "nmake /f "win9x.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "win9x.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "temp"
# PROP Intermediate_Dir "temp"
# PROP Cmd_Line "win9x kelvin debug win9x /browse"
# PROP Rebuild_Opt "/a"
# PROP Target_File "nvdd32.dll"
# PROP Bsc_Name "..\nvdd32.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "temp"
# PROP BASE Intermediate_Dir "temp"
# PROP BASE Cmd_Line "nmake /f "win9x.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "win9x.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "temp"
# PROP Intermediate_Dir "temp"
# PROP Cmd_Line "win9x kelvin develop win9x /browse"
# PROP Rebuild_Opt "/a"
# PROP Target_File "nvdd32.dll"
# PROP Bsc_Name "..\nvdd32.bsc"
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "temp"
# PROP BASE Intermediate_Dir "temp"
# PROP BASE Cmd_Line "nmake /f "win9x.mak""
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "win9x.exe"
# PROP BASE Bsc_Name ""
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "temp"
# PROP Intermediate_Dir "temp"
# PROP Cmd_Line "win9x kelvin retail win9x"
# PROP Rebuild_Opt "/a"
# PROP Target_File "nvdd32.dll"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "temp"
# PROP BASE Intermediate_Dir "temp"
# PROP BASE Cmd_Line "win9x celsius debug win9x /browse"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "nvdd32.dll"
# PROP BASE Bsc_Name "..\nvdd32.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "temp"
# PROP Intermediate_Dir "temp"
# PROP Cmd_Line "win9x kelvin stat win9x /browse"
# PROP Rebuild_Opt "/a"
# PROP Target_File "nvdd32.dll"
# PROP Bsc_Name "..\nvdd32.bsc"
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "win9x - Win32 NV4 debug"
# Name "win9x - Win32 NV4 develop"
# Name "win9x - Win32 NV4 retail"
# Name "win9x - Win32 NV4 statistics"

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\common\src\d3dContext.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\d3dDebug.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\d3dMath.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\d3dMini.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\d3dRender.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\d3dState.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\d3dTex.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\ddDrv.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\ddDrvMem.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\ddFlip.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\ddnvstat.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\ddProcMan.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\ddSurf.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\ddVideo.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\Global.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\MoComp.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\MoCompEx.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\MoCompExKelvin.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\MoCorr.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\MoCorrKelvin.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\MoInit.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\MoInitKelvin.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\MoPred.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\MoPredKelvin.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nv4Tables.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvAccess.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvAGP.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvBlit.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvCaps.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvCapture.c
# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusAA.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusAlphaCombiner.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusColorCombiner.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusComp.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusCompileVS_link.cpp
# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusInlPrim.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusInlStrPrim.cpp
# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusInlTri.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusNvTSS.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusPrim.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusState.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusStateHelp.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusSuperTri.cpp
# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusTables.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\src\nvCelsiusVbPrim.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvClear.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvComp.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvContext.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvD3DKelvinBackend.c
# End Source File
# Begin Source File

SOURCE=..\common\src\nvDefVB.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvDP2Help.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvDP2ops.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvEnable.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvFile.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvFlipper.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvGLBackend.c
# End Source File
# Begin Source File

SOURCE=..\common\src\nvHeap.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinAA.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinAlphaCombiners.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinColorCombiners.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinComp.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinCompileVS_link.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinInlPrim.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinOptimiseVS_link.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinPrim.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinProgram.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinState.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinStateHelp.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinSuperTri.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinTables.cpp
# End Source File
# Begin Source File

SOURCE=..\kelvin\src\nvKelvinVbPrim.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvLight.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvLine.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvLogo.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvMem.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvMini.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvMip.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvOverlaySurf.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvOverlaySurf_link.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvPalette.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvPatch.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvPatchConsts.c
# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvPatchUtil.c
# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvPM.c
# End Source File
# Begin Source File

SOURCE=..\common\src\nvPoint.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvPrim.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvPrim2.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvPriv.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvPriv_link.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvProf.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvPShad.cpp
# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvPusher.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvPusher_link.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvRefCount.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvRZ.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvScene.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvSemaphore.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvState.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvStateSet.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvStereo.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvSurf.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvSurface.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvSwiz.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvTex.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvTexManage.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvTranslate.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvTri.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvUtil_link.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvVB.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\nvVideoPort.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\src\nvVPP.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvVPP_link.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\nvVShad.cpp
# End Source File
# Begin Source File

SOURCE=..\common\src\StereoFunc_i.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\Surfaces.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\src\targa.c
# End Source File
# Begin Source File

SOURCE=..\common\src\TexInit.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\src\TransInit.cpp

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\nv20\src\vpcompilekelvin.c
# End Source File
# Begin Source File

SOURCE=..\..\common\src\vpcompilex86.c
# End Source File
# Begin Source File

SOURCE=..\..\common\src\vpoptimize.c
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\common\inc\arb_fifo.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\CompileControl.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\d3dConst.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\d3dDebug.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\D3DINC.H

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\d3dMath.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\ddCapture.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\ddFlip.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\DDHal.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\ddkmapi.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\win9x\inc\DDMINI.H

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\ddProcMan.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\ddSurface.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\ddVideo.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\dxvadefs.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\eewinma2.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\elsaesc.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\GLOBAL.H

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\k32exp.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\MINIVDD.H

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nv4caps.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nv4Tables.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nv_name.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvAccess.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvBlit.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvCapture.h
# End Source File
# Begin Source File

SOURCE=..\celsius\inc\nvCelsiusAA.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\inc\nvCelsiusAACompatibility.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvCelsiusCaps.h
# End Source File
# Begin Source File

SOURCE=..\celsius\inc\nvCelsiusNvTSS.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\inc\nvCelsiusState.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\inc\nvCelsiusStateHelp.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\inc\nvCelsiusTables.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvClear.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvcom.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvComp.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvContext.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\NVD3DMAC.H

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvDbg.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvdblnd.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\win9x\inc\NVDDOBJ.H

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvDefVB.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvdinc.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvDMAC.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvDP2Help.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvdp2ops.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvEnable.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvEscDef.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvFile.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvFlipper.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvFourCC.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvHeap.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvILHash.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvioctl.h
# End Source File
# Begin Source File

SOURCE=..\kelvin\inc\nvKelvinAA.h
# End Source File
# Begin Source File

SOURCE=..\kelvin\inc\nvKelvinCaps.h
# End Source File
# Begin Source File

SOURCE=..\kelvin\inc\nvKelvinProgram.h
# End Source File
# Begin Source File

SOURCE=..\kelvin\inc\nvKelvinState.h
# End Source File
# Begin Source File

SOURCE=..\kelvin\inc\nvKelvinStateHelp.h
# End Source File
# Begin Source File

SOURCE=..\kelvin\inc\nvKelvinTables.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvLight.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvLogo.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvmocomp.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvMultiMon.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvObject.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvOverlay.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvOverlaySurf.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvPal.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvPalette.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvPatch.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvPatchInterface.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvPixelFmt.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvPM.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvprecomp.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvPriv.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvproto.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvPShad.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvPusher.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvRefCount.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvReg.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvSemaphore.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\NVSTAT.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvState.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvStereo.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvSurf.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvSurface.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvSwitches.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvSwiz.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvTex.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\NVTEXFMT.H

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvTexManage.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\inc\nvTimer.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvtracecom.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvtranslate.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvUniversal.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvUtil.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvVB.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvVer.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvVideoPort.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\nvVPP.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvVShad.h
# End Source File
# Begin Source File

SOURCE=..\common\inc\nvvxmac.h

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\inc\StereoDDK.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\targa.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\vtxpgmcomp.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\vtxpgmconsts.h
# End Source File
# Begin Source File

SOURCE=..\..\common\inc\x86.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=..\celsius\src\celsiusDD32.rc

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\..\common\win9x\inc\COMMON.VER

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\kelvin\src\kelvinDD32.rc
# End Source File
# Begin Source File

SOURCE=..\nv4\src\nv4dd32.rc

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# End Group
# Begin Group "Make Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\celsius\MAKE.celsiusOpts

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MAKE.d3ddefs

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MAKE.d3dopts

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\MAKE.rules

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\celsius\MAKEFILE

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\common\MAKEFILE

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# Begin Source File

SOURCE=..\kelvin\MAKEFILE
# End Source File
# Begin Source File

SOURCE=..\MAKEFILE

!IF  "$(CFG)" == "win9x - Win32 NV4 debug"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 develop"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 retail"

# PROP Intermediate_Dir "na"

!ELSEIF  "$(CFG)" == "win9x - Win32 NV4 statistics"

# PROP BASE Intermediate_Dir "na"
# PROP Intermediate_Dir "na"

!ENDIF 

# End Source File
# End Group
# End Target
# End Project
