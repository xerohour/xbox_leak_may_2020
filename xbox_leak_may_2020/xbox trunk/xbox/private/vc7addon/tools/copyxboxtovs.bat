@echo off

rem *** Only used for experimenting with Visual Studio .Net. source tree ***
rem

rem copy files from xbox build tree into visual studio .NET build tree to allow building of Xbox
rem support from within the visual studio .net build tree.

if not "%VSROOT%"=="" goto StartCopy

echo VSROOT must be defined in the environment, e.g. set VSROOT=e:\vs70
exit /B

:StartCopy
set S=%_NTDRIVE%%_NTROOT%\private\vc7addon
set D=%VSROOT%

attrib -r %d%\vs\src\common\inc\stdidcmd.h
  copy /y %s%\vs\src\common\inc\stdidcmd.h %d%\vs\src\common\inc

set S2=%S%\vs\src\vc\ide
set D2=%D%\vs\src\vc\ide

attrib -r %d2%\idl\vcpb2.idl
  copy /y %s2%\idl\vcpb2.idl %d2%\idl
attrib -r %d2%\idl\vcpbbuildengine.idl
  copy /y %s2%\idl\vcpbbuildengine.idl %d2%\idl
attrib -r %d2%\idl\vcpbsettingspages.idl
  copy /y %s2%\idl\vcpbsettingspages.idl %d2%\idl
attrib -r %d2%\idl\vcpbtools.idl
  copy /y %s2%\idl\vcpbtools.idl %d2%\idl
attrib -r %d2%\idl\vcpbtypedefs.idl
  copy /y %s2%\idl\vcpbtypedefs.idl %d2%\idl
attrib -r %d2%\idl\vcprojectprivatetypelib.idl
  copy /y %s2%\idl\vcprojectprivatetypelib.idl %d2%\idl

attrib -r %d2%\include\prjids.h
  copy /y %s2%\include\prjids.h %d2%\include

attrib -r %d2%\pkgs\projbld\ui_dll\resdll\gpmenus.cmd
  copy /y %s2%\pkgs\projbld\ui_dll\resdll\gpmenus.cmd %d2%\pkgs\projbld\ui_dll\resdll
attrib -r %d2%\pkgs\projbld\ui_dll\resdll\prjbldui.rc
  copy /y %s2%\pkgs\projbld\ui_dll\resdll\prjbldui.rc %d2%\pkgs\projbld\ui_dll\resdll
attrib -r %d2%\pkgs\projbld\ui_dll\resdll\resource.h
  copy /y %s2%\pkgs\projbld\ui_dll\resdll\resource.h %d2%\pkgs\projbld\ui_dll\resdll

attrib -r %d2%\pkgs\projbld\ui_dll\src\gencfg.cpp
  copy /y %s2%\pkgs\projbld\ui_dll\src\gencfg.cpp %d2%\pkgs\projbld\ui_dll\src
attrib -r %d2%\pkgs\projbld\ui_dll\src\prjnode.cpp
  copy /y %s2%\pkgs\projbld\ui_dll\src\prjnode.cpp %d2%\pkgs\projbld\ui_dll\src

attrib -r %d2%\pkgs\projbld\vcpb\bldactions.cpp
  copy /y %s2%\pkgs\projbld\vcpb\bldactions.cpp %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\buildengine.cpp
  copy /y %s2%\pkgs\projbld\vcpb\buildengine.cpp %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\configuration.cpp
  copy /y %s2%\pkgs\projbld\vcpb\configuration.cpp %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\linktool.cpp
  copy /y %s2%\pkgs\projbld\vcpb\linktool.cpp %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\linktool.h
  copy /y %s2%\pkgs\projbld\vcpb\linktool.h %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\linktoolbase.h
  copy /y %s2%\pkgs\projbld\vcpb\linktoolbase.h %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\platform.cpp
  copy /y %s2%\pkgs\projbld\vcpb\platform.cpp %d2%\pkgs\projbld\vcpb
copy /y %s2%\pkgs\projbld\vcpb\platformxbox.cpp %d2%\pkgs\projbld\vcpb
copy /y %s2%\pkgs\projbld\vcpb\platformxbox.h %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\resource.h
  copy /y %s2%\pkgs\projbld\vcpb\resource.h %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\sources
  copy /y %s2%\pkgs\projbld\vcpb\sources.vs %d2%\pkgs\projbld\vcpb\sources
attrib -r %d2%\pkgs\projbld\vcpb\stdafx.h
  copy /y %s2%\pkgs\projbld\vcpb\stdafx.h %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\stylesheet.cpp
  copy /y %s2%\pkgs\projbld\vcpb\stylesheet.cpp %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\vcpb.cpp
  copy /y %s2%\pkgs\projbld\vcpb\vcpb.cpp %d2%\pkgs\projbld\vcpb
attrib -r %d2%\pkgs\projbld\vcpb\vcpb.rc
  copy /y %s2%\pkgs\projbld\vcpb\vcpb.rc %d2%\pkgs\projbld\vcpb
copy /y %s2%\pkgs\projbld\vcpb\xbox.rgs %d2%\pkgs\projbld\vcpb
copy /y %s2%\pkgs\projbld\vcpb\xboxdeploytool.cpp %d2%\pkgs\projbld\vcpb
copy /y %s2%\pkgs\projbld\vcpb\xboxdeploytool.h %d2%\pkgs\projbld\vcpb
copy /y %s2%\pkgs\projbld\vcpb\xboximagetool.cpp %d2%\pkgs\projbld\vcpb
copy /y %s2%\pkgs\projbld\vcpb\xboximagetool.h %d2%\pkgs\projbld\vcpb

attrib -r %d2%\pkgs\vcpkg\ui\resource.h
  copy /y %s2%\pkgs\vcpkg\ui\resource.h %d2%\pkgs\vcpkg\ui
attrib -r %d2%\pkgs\vcpkg\ui\vcui.rc
  copy /y %s2%\pkgs\vcpkg\ui\vcui.rc %d2%\pkgs\vcpkg\ui

attrib -r %d2%\wizards\vcprojects\sources
  copy /y %s2%\wizards\vcprojects\sources.vs %d2%\wizards\vcprojects\sources
attrib -r %d2%\wizards\vcprojects\vc.vsdir
  copy /y %s2%\wizards\vcprojects\vc.vsdir %d2%\wizards\vcprojects
copy /y %s2%\wizards\vcprojects\xboxwiz.ico %d2%\wizards\vcprojects
copy /y %s2%\wizards\vcprojects\xboxwiz.vsz %d2%\wizards\vcprojects

mkdir %d2%\wizards\xboxwiz
mkdir %d2%\wizards\xboxwiz\html
mkdir %d2%\wizards\xboxwiz\html\1033
mkdir %d2%\wizards\xboxwiz\images
mkdir %d2%\wizards\xboxwiz\scripts
mkdir %d2%\wizards\xboxwiz\scripts\1033
mkdir %d2%\wizards\xboxwiz\templates
mkdir %d2%\wizards\xboxwiz\templates\1033

copy /y %s2%\wizards\xboxwiz\default.vcproj %d2%\wizards\xboxwiz

copy /y %s2%\wizards\xboxwiz\html\1033\appsettings.htm %d2%\wizards\xboxwiz\html\1033
copy /y %s2%\wizards\xboxwiz\html\1033\default.htm %d2%\wizards\xboxwiz\html\1033

copy /y %s2%\wizards\xboxwiz\images\consoleapp.gif %d2%\wizards\xboxwiz\images
copy /y %s2%\wizards\xboxwiz\images\dllapp.gif %d2%\wizards\xboxwiz\images
copy /y %s2%\wizards\xboxwiz\images\staticlibraryapp.gif %d2%\wizards\xboxwiz\images
copy /y %s2%\wizards\xboxwiz\images\windowsapp.gif %d2%\wizards\xboxwiz\images
copy /y %s2%\wizards\xboxwiz\images\xbox_1.gif %d2%\wizards\xboxwiz\images
copy /y %s2%\wizards\xboxwiz\images\xbox_background.gif %d2%\wizards\xboxwiz\images

copy /y %s2%\wizards\xboxwiz\scripts\1033\default.js %d2%\wizards\xboxwiz\scripts\1033

copy /y %s2%\wizards\xboxwiz\templates\1033\readme.txt %d2%\wizards\xboxwiz\templates\1033
copy /y %s2%\wizards\xboxwiz\templates\1033\root.cpp %d2%\wizards\xboxwiz\templates\1033
copy /y %s2%\wizards\xboxwiz\templates\1033\stdafx.cpp %d2%\wizards\xboxwiz\templates\1033
copy /y %s2%\wizards\xboxwiz\templates\1033\stdafx.h %d2%\wizards\xboxwiz\templates\1033
copy /y %s2%\wizards\xboxwiz\templates\1033\templates.inf %d2%\wizards\xboxwiz\templates\1033
