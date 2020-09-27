@echo off
set TARGET=\\xdk\xdktest\cafe6

echo making CafeDrv
msdev cafe.dsw /MAKE "cafedrv - Win32 Debug"
msdev cafe.dsw /MAKE "cafedrv - Win32 Release"

echo copying the CafeDrv binaries
mkdir %TARGET%
mkdir %TARGET%\bin
del /s /q %TARGET%\bin\*.*
copy /Y bin\*.exe %TARGET%\bin
copy /Y bin\*.dll %TARGET%\bin

echo building the Xbox debugger test DLLs
del xtests\debugger\bin\core\*.dll
pushd xtests\debugger\core
msdev core.dsw /MAKE all
popd

echo building the IDE test DLLs
del xtests\ide\bin\core\*.dll
pushd xtests\ide\core\appwiz
msdev appwiz.dsw /MAKE all
popd
pushd xtests\ide\core\bldxquake
msdev bldxquake.dsp /MAKE all
popd

mkdir %TARGET%\xtests

mkdir %TARGET%\xtests\debugger
mkdir %TARGET%\xtests\debugger\bin
mkdir %TARGET%\xtests\debugger\bin\core
del /s /q %TARGET%\xtests\debugger\bin\core\*.dll
copy /Y xtests\debugger\bin\core\*.dll %TARGET%\xtests\debugger\bin\core
copy /Y xtests\debugger\bin\core\*.stf %TARGET%\xtests\debugger\bin\core

echo copying the IDE test DLLs
mkdir %TARGET%\xtests\ide
mkdir %TARGET%\xtests\ide\bin
mkdir %TARGET%\xtests\ide\bin\core
del /s /q %TARGET%\xtests\ide\bin\core\*.dll
copy /Y xtests\ide\bin\core\*.dll %TARGET%\xtests\ide\bin\core
copy /Y xtests\ide\bin\core\*.stf %TARGET%\xtests\ide\bin\core

rem create the target sources
mkdir %TARGET%\xtests\debugger\bin\core\src
del /s /q %TARGET%\xtests\debugger\bin\core\src\*.*
mkdir %TARGET%\xtests\ide\bin\core\src
del /s /q %TARGET%\xtests\ide\bin\core\src\*.*

rem copy the target sources down
xcopy /I /Y /S xtests\debugger\bin\core\src\*.* %TARGET%\xtests\debugger\bin\core\src
xcopy /I /Y /S xtests\ide\bin\core\src\*.* %TARGET%\xtests\ide\bin\core\src
