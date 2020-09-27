# Microsoft Developer Studio Project File - Name="tncontrl" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Dynamic-Link Library" 0x0102

CFG=tncontrl - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "tncontrl.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "tncontrl.mak" CFG="tncontrl - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "tncontrl - Win32 Debug" (based on "Win32 (x86) Dynamic-Link Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe
# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_USRDLL" /D "TNCONTRL_EXPORTS" /YX /FD /GZ /c
# ADD CPP /nologo /Gz /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\inc" /I "..\..\..\common\testmgr\tmagent" /D "WIN32" /D "_DEBUG" /D "DEBUG" /D "_WINDOWS" /D "_USRDLL" /D "TNCONTRL_EXPORTS" /D "BUILDENV_VISUALC" /D "XBOX" /YX /FD /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x409 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /dll /debug /machine:I386 /pdbtype:sept
# ADD LINK32 ..\..\imtest\lib\Debug\imtlib.lib ..\tncommon\Debug\tncommon.lib version.lib winmm.lib wsock32.lib advapi32.lib ole32.lib user32.lib kernel32.lib /nologo /dll /map /debug /machine:I386 /pdbtype:sept
# Begin Special Build Tool
SOURCE="$(InputPath)"
PostBuild_Cmds=echo - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	mapsym -o Debug\tncontrl.sym Debug\tncontrl.map	echo - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -	echo Copying built binaries to C:\dplay\testnet...	copy Debug\tncontrl.dll C:\dplay\testnet	copy Debug\tncontrl.pdb C:\dplay\testnet	copy Debug\tncontrl.sym C:\dplay\testnet
# End Special Build Tool
# Begin Target

# Name "tncontrl - Win32 Debug"
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

SOURCE=.\tncontrl.def
# End Source File
# Begin Source File

SOURCE=.\tncontrl.rc
# End Source File
# End Group
# End Target
# End Project
