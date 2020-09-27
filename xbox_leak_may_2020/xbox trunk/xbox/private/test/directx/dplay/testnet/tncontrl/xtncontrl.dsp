# Microsoft Developer Studio Project File - Name="xtncontrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) External Target" 0x0106

CFG=xtncontrl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "xtncontrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "xtncontrl.mak" CFG="xtncontrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "xtncontrl - Win32 Release" (based on "Win32 (x86) External Target")
!MESSAGE "xtncontrl - Win32 Debug" (based on "Win32 (x86) External Target")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""

!IF  "$(CFG)" == "xtncontrl - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Cmd_Line "NMAKE /f xtncontrl.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "xtncontrl.exe"
# PROP BASE Bsc_Name "xtncontrl.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Cmd_Line "nmake /f "xtncontrl.mak""
# PROP Rebuild_Opt "/a"
# PROP Target_File "xtncontrl.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ELSEIF  "$(CFG)" == "xtncontrl - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "xtncontrl___Win32_Debug"
# PROP BASE Intermediate_Dir "xtncontrl___Win32_Debug"
# PROP BASE Cmd_Line "NMAKE /f xtncontrl.mak"
# PROP BASE Rebuild_Opt "/a"
# PROP BASE Target_File "xtncontrl.exe"
# PROP BASE Bsc_Name "xtncontrl.bsc"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "xtncontrl___Win32_Debug"
# PROP Intermediate_Dir "xtncontrl___Win32_Debug"
# PROP Cmd_Line "echo build -z | %SystemRoot%\system32\Cmd.exe /K c:\xbox\private\developr\tristanj\myrazzle.cmd pushd c:\xbox\private\test\directx\dplay\testnet\tncontrl\xbox"
# PROP Rebuild_Opt "/a"
# PROP Target_File "xtncontrl.exe"
# PROP Bsc_Name ""
# PROP Target_Dir ""

!ENDIF 

# Begin Target

# Name "xtncontrl - Win32 Release"
# Name "xtncontrl - Win32 Debug"

!IF  "$(CFG)" == "xtncontrl - Win32 Release"

!ELSEIF  "$(CFG)" == "xtncontrl - Win32 Debug"

!ENDIF 

# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\binmgmt.cpp
# End Source File
# Begin Source File

SOURCE=.\comm.cpp
# End Source File
# Begin Source File

SOURCE=.\commtcp.cpp
# End Source File
# Begin Source File

SOURCE=.\comports.cpp
# End Source File
# Begin Source File

SOURCE=.\control.cpp
# End Source File
# Begin Source File

SOURCE=.\excptns.cpp
# End Source File
# Begin Source File

SOURCE=.\executor.cpp
# End Source File
# Begin Source File

SOURCE=.\faultsim.cpp
# End Source File
# Begin Source File

SOURCE=.\faultsimimtest.cpp
# End Source File
# Begin Source File

SOURCE=.\info.cpp
# End Source File
# Begin Source File

SOURCE=.\ipaddrs.cpp
# End Source File
# Begin Source File

SOURCE=.\ipcobj.cpp
# End Source File
# Begin Source File

SOURCE=.\jobq.cpp
# End Source File
# Begin Source File

SOURCE=.\leech.cpp
# End Source File
# Begin Source File

SOURCE=.\main.cpp
# End Source File
# Begin Source File

SOURCE=.\master.cpp
# End Source File
# Begin Source File

SOURCE=.\masterinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\masterlist.cpp
# End Source File
# Begin Source File

SOURCE=.\meta.cpp
# End Source File
# Begin Source File

SOURCE=.\periodicq.cpp
# End Source File
# Begin Source File

SOURCE=.\reachcheck.cpp
# End Source File
# Begin Source File

SOURCE=.\reportitem.cpp
# End Source File
# Begin Source File

SOURCE=.\reportlist.cpp
# End Source File
# Begin Source File

SOURCE=.\rsltchain.cpp
# End Source File
# Begin Source File

SOURCE=.\script.cpp
# End Source File
# Begin Source File

SOURCE=.\sendq.cpp
# End Source File
# Begin Source File

SOURCE=.\slave.cpp
# End Source File
# Begin Source File

SOURCE=.\slaveinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\slavelist.cpp
# End Source File
# Begin Source File

SOURCE=.\sndcards.cpp
# End Source File
# Begin Source File

SOURCE=.\storedata.cpp
# End Source File
# Begin Source File

SOURCE=.\syncdata.cpp
# End Source File
# Begin Source File

SOURCE=.\tablecase.cpp
# End Source File
# Begin Source File

SOURCE=.\tablegroup.cpp
# End Source File
# Begin Source File

SOURCE=.\tableitem.cpp
# End Source File
# Begin Source File

SOURCE=.\tapidevs.cpp
# End Source File
# Begin Source File

SOURCE=.\testfromfiles.cpp
# End Source File
# Begin Source File

SOURCE=.\testinst.cpp
# End Source File
# Begin Source File

SOURCE=.\testlist.cpp
# End Source File
# Begin Source File

SOURCE=.\teststats.cpp
# End Source File
# Begin Source File

SOURCE=.\vars.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\binmgmt.h
# End Source File
# Begin Source File

SOURCE=.\classes.h
# End Source File
# Begin Source File

SOURCE=.\comm.h
# End Source File
# Begin Source File

SOURCE=.\commtcp.h
# End Source File
# Begin Source File

SOURCE=.\comports.h
# End Source File
# Begin Source File

SOURCE=.\control.h
# End Source File
# Begin Source File

SOURCE=.\excptns.h
# End Source File
# Begin Source File

SOURCE=.\executor.h
# End Source File
# Begin Source File

SOURCE=.\faultsim.h
# End Source File
# Begin Source File

SOURCE=.\faultsimimtest.h
# End Source File
# Begin Source File

SOURCE=.\info.h
# End Source File
# Begin Source File

SOURCE=.\ipaddrs.h
# End Source File
# Begin Source File

SOURCE=.\ipcobj.h
# End Source File
# Begin Source File

SOURCE=.\jobq.h
# End Source File
# Begin Source File

SOURCE=.\leech.h
# End Source File
# Begin Source File

SOURCE=.\macros.h
# End Source File
# Begin Source File

SOURCE=.\main.h
# End Source File
# Begin Source File

SOURCE=.\master.h
# End Source File
# Begin Source File

SOURCE=.\masterinfo.h
# End Source File
# Begin Source File

SOURCE=.\masterlist.h
# End Source File
# Begin Source File

SOURCE=.\meta.h
# End Source File
# Begin Source File

SOURCE=.\msgs.h
# End Source File
# Begin Source File

SOURCE=.\periodicq.h
# End Source File
# Begin Source File

SOURCE=.\procs.h
# End Source File
# Begin Source File

SOURCE=.\reachcheck.h
# End Source File
# Begin Source File

SOURCE=.\reportitem.h
# End Source File
# Begin Source File

SOURCE=.\reportlist.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\rsltchain.h
# End Source File
# Begin Source File

SOURCE=.\script.h
# End Source File
# Begin Source File

SOURCE=.\sendq.h
# End Source File
# Begin Source File

SOURCE=.\slave.h
# End Source File
# Begin Source File

SOURCE=.\slaveinfo.h
# End Source File
# Begin Source File

SOURCE=.\slavelist.h
# End Source File
# Begin Source File

SOURCE=.\sndcards.h
# End Source File
# Begin Source File

SOURCE=.\storedata.h
# End Source File
# Begin Source File

SOURCE=.\syncdata.h
# End Source File
# Begin Source File

SOURCE=.\tablecase.h
# End Source File
# Begin Source File

SOURCE=.\tablegroup.h
# End Source File
# Begin Source File

SOURCE=.\tableitem.h
# End Source File
# Begin Source File

SOURCE=.\tapidevs.h
# End Source File
# Begin Source File

SOURCE=.\testfromfiles.h
# End Source File
# Begin Source File

SOURCE=.\testinst.h
# End Source File
# Begin Source File

SOURCE=.\testlist.h
# End Source File
# Begin Source File

SOURCE=.\teststats.h
# End Source File
# Begin Source File

SOURCE=.\tncontrl.h
# End Source File
# Begin Source File

SOURCE=.\vars.h
# End Source File
# Begin Source File

SOURCE=.\version.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\tncontrl.rc
# End Source File
# End Group
# End Target
# End Project
