@REM Create a VC7 patch
@echo off

setlocal

if "" == "%_NT386TREE%" goto usage
if "" == "%_NTDRIVE%" goto usage
if "" == "%_NTROOT%" goto usage
if "-?" == "%1" goto usage
if "/?" == "%1" goto usage

set SRC="%_NTDRIVE%%_NTROOT%\private\vc7addon"
set DST="%_NT386TREE%\XDK-VS7"

rmdir /q /s %DST%
if not "0" == "%errorlevel%" goto cantDeleteDirectory

md %DST%

if not "0" == "%errorlevel%" goto cantCreateDirectory

REM Create a directory containing just the files we want to add or patch in VS.Net

md %DST%\Vc7
if not "0" == "%errorlevel%" goto cantCreateDirectory

md %DST%\Vc7\vcpackages
if not "0" == "%errorlevel%" goto cantCreateDirectory

if not exist %SRC%\vs\src\vc\ide\pkgs\projbld\vcpb\obj\i386\VCProjectEngine.dll goto failure
xcopy /q %SRC%\vs\src\vc\ide\pkgs\projbld\vcpb\obj\i386\VCProjectEngine.dll %DST%\Vc7\vcpackages
if not "0" == "%errorlevel%" goto cantCopyFiles

if not exist %SRC%\vs\src\vc\ide\pkgs\projbld\ui_dll\src\obj\i386\VCProject.dll goto failure
xcopy /q %SRC%\vs\src\vc\ide\pkgs\projbld\ui_dll\src\obj\i386\VCProject.dll %DST%\Vc7\vcpackages
if not "0" == "%errorlevel%" goto cantCopyFiles

md %DST%\Vc7\vcpackages\1033
if not exist %SRC%\vs\src\vc\ide\pkgs\projbld\ui_dll\resdll\obj\i386\VCProjectUI.dll goto failure
xcopy /q %SRC%\vs\src\vc\ide\pkgs\projbld\ui_dll\resdll\obj\i386\VCProjectUI.dll %DST%\Vc7\vcpackages\1033
if not "0" == "%errorlevel%" goto cantCopyFiles

md %DST%\Vc7\vcprojects
xcopy /q %SRC%\vs\src\vc\ide\wizards\vcprojects\vc.vsdir %DST%\Vc7\vcprojects
if not "0" == "%errorlevel%" goto cantCopyFiles
xcopy /q %SRC%\vs\src\vc\ide\wizards\vcprojects\XboxWiz.ico %DST%\Vc7\vcprojects
if not "0" == "%errorlevel%" goto cantCopyFiles
xcopy /q %SRC%\vs\src\vc\ide\wizards\vcprojects\XboxWiz.vsz %DST%\Vc7\vcprojects
if not "0" == "%errorlevel%" goto cantCopyFiles

md %DST%\Vc7\VCWizards
xcopy /q /s /I %SRC%\vs\src\vc\ide\wizards\xboxwiz %DST%\Vc7\VCWizards\XboxWiz
if not "0" == "%errorlevel%" goto cantCopyFiles

REM Update the msm file

xcopy /q /r /y %SRC%\installer\XDK-VS7.msm %DST%
pushd %DST%
makecab /f %SRC%\installer\XDK-VS7.ddf
if not "0" == "%errorlevel%" goto cantMakeCab
msidb -d XDK-VS7.msm -a MergeModule.CABinet
popd

goto end

:usage
echo usage: makevc7patch
echo _NT386TREE, _NTDRIVE, and _NTROOT must be defined
goto end

:failure
echo "Failed to create patch file because one or more dlls were missing."
goto end

:cantDeleteDirectory
echo Can't delete directory %DST%
goto end

:cantCreateDirectory
echo Can't create directory tree
goto end

:cantCopyFiles
echo Can't copy files
goto end

:cantMakeCab
echo Can't make CAB file.
goto end

:end
endlocal
