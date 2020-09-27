@echo off
rem -----------------------------------------------------
rem set up the environment for copying files
rem -----------------------------------------------------
set pdrive=%XBLD_PVT%\%_BUILDVER%
set sdrive=%_NTBINDIR%\private

rem -----------------------------------------------------
rem creating a staging area
rem -----------------------------------------------------
if not exist %pdrive%\XboxSDK md %pdrive%\XboxSDK
cd /d %pdrive%\XboxSDK
pushd .

rem -----------------------------------------------------
rem copy the necessary files into the "root" of the CD
rem -----------------------------------------------------
echo Layout the root of the CD
copy %sdrive%\SDK\SDKSetup\AutoRun\autorun.exe .
copy %sdrive%\SDK\SDKSetup\AutoRun\autorun.ini .
copy %sdrive%\SDK\SDKSetup\AutoRun\autorun.inf .
copy %sdrive%\SDK\SDKSetup\AutoRun\setup.bmp .

copy %pdrive%\XDKSetup%_BUILDVER%.exe XDKSetupEng.exe
copy %pdrive%\XDKSetup%_BUILDVER%_jpn.exe XDKSetupJpn.exe

copy %sdrive%\ue\sdk\relnotes\readme1st.txt .
copy %sdrive%\ue\sdk\relnotes\readme1stJPN.txt .
copy %sdrive%\ue\sdk\relnotes\relnotes.htm .
copy %sdrive%\ue\sdk\relnotes\relnotesJPN.htm .

rem -----------------------------------------------------
rem set up the AutoRun area of the CD
rem -----------------------------------------------------
md AutoRun
cd AutoRun
echo CD AutoRun
copy %sdrive%\SDK\SDKSetup\AutoRun\AutoRun\autorun.bmp .
copy %sdrive%\SDK\SDKSetup\AutoRun\AutoRun\sdk.ico .
copy %sdrive%\SDK\SDKSetup\AutoRun\AutoRun\autorun.cur .
cd ..

rem -----------------------------------------------------
rem set up the DirectX 8 redist area of the CD
rem -----------------------------------------------------
md DX8Redist
cd DX8Redist
echo DX8Redist
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\Win9x\CabPack\bda.cab .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\CabPack\bdant.cab .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\DX5\cfgmgr32.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\Win9x\CabPack\directx.cab .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\Win9x\dsetup.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\CabPack\dxnt.cab .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\Win9x\Localized\Multi\dsetup32.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\Win9x\Localized\Multi\dxsetup.exe .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\DX5\setupapi.dll .
cd ..

md DX8Runtimes
cd DX8Runtimes
md Debug
cd Debug
echo DX8 Debug Runtimes
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Chk\Win9x\CabPack\bda.cab .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Chk\CabPack\bdant.cab .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Chk\DX5\cfgmgr32.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Chk\DX5\setupapi.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Chk\Win9x\CabPack\eng\directx.cab .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Chk\Win9x\dsetup.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Chk\Win9x\dsetup32.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Chk\Win9x\dxsetup.exe .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Chk\CabPack\eng\dxnt.cab .
cd ..

md Retail
cd Retail
echo DX8 Retail Runtimes
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\Win9x\CabPack\bda.cab .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\CabPack\bdant.cab .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\DX5\cfgmgr32.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\DX5\setupapi.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\Win9x\CabPack\eng\directx.cab .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\Win9x\dsetup.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\Win9x\dsetup32.dll .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\Win9x\dxsetup.exe .
copy %sdrive%\External\SDK\DX8Release\Binaries.x86Fre\CabPack\eng\dxnt.cab .
cd ..
cd ..

md DMusProdX
cd DMusProdX
echo DirectMusic Producer for Xbox
md DemoContent
cd DemoContent
copy %sdrive%\External\SDK\DX8Release\Extras\DMUSProducer\"Demo Content"\dmpdemocontent.exe .
cd ..
copy %sdrive%\External\SDK\dmp81\_isdel.exe .
copy %sdrive%\External\SDK\dmp81\_inst32I.ex_ .
copy %sdrive%\External\SDK\dmp81\_setup.dll .
copy %sdrive%\External\SDK\dmp81\_sys1.cab .
copy %sdrive%\External\SDK\dmp81\_user1.cab .
copy %sdrive%\External\SDK\dmp81\data.tag .
copy %sdrive%\External\SDK\dmp81\data1.cab .
copy %sdrive%\External\SDK\dmp81\lang.dat .
copy %sdrive%\External\SDK\dmp81\layout.bin .
copy %sdrive%\External\SDK\dmp81\os.dat .
copy %sdrive%\External\SDK\dmp81\readme.txt .
copy %sdrive%\External\SDK\dmp81\setup.bmp .
copy %sdrive%\External\SDK\dmp81\setup.exe .
copy %sdrive%\External\SDK\dmp81\setup.ini .
copy %sdrive%\External\SDK\dmp81\setup.ins .
copy %sdrive%\External\SDK\dmp81\setup.lid .
cd ..

md Extras
cd Extras
md Direct3D
cd Direct3D
md Tools
cd Tools
echo Direct3D Model Translation Tools
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\d3d8sd.exe .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\xskinexptemplates.h .
md 3DSMax3
cd 3DSMax3
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\dllentry.cpp .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\exportxfile.cpp .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\meshdata.cpp .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\meshdata.h .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\pch.h .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\readme.txt .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\resource.h .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\xskinexp.cpp .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\xskinexp.def .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\xskinexp.dsp .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\xskinexp.h .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\xskinexp.rc .
md Bin
cd Bin
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\3dsmax3\bin\xskinexp.dle .
cd ..
cd ..
md Maya25
cd Maya25
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya25\bicubicbezierpatches.mel .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya25\myassert.h .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya25\mydt.cpp .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya25\mydt.h .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya25\readme.txt .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya25\"xexport 000.dsp" .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya25\xfiletranslatoropts.mel .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya25\xporttranslator.cpp .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya25\xporttranslator.h .
md Bin
cd Bin
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya25\Bin\xexport.mll .
cd ..
cd ..
md Maya30
cd Maya30
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\bicubicbezierpatches.mel .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\myassert.h .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\mydt.cpp .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\mydt.h .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\readme.txt .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\xexport.dsp .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\xfiletranslatoropts.mel .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\xporttranslator.cpp .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\xporttranslator.h .
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\xskinexptemplates.h .
md Bin
cd Bin
copy %sdrive%\External\SDK\DX8Release\Extras\D3DIM\Tools\Maya30\Bin\xexport.mll .
cd ..
cd ..
cd ..
cd ..
md Fonts
cd Fonts
echo Fonts
copy %sdrive%\External\SDK\Fonts\ArialUni.ttf .
md Jpn
cd Jpn
copy %sdrive%\External\SDK\Fonts\Jpn\*.*
cd ..
cd ..
cd ..

md NetMon
cd NetMon
echo NetMon
copy %sdrive%\External\SDK\NetMon\may\setup.exe .
cd ..

md PeripheralKit
cd PeripheralKit
echo PeripheralKit
copy %sdrive%\UE\pxdk\"dock 5-4-01.zip" .
copy %sdrive%\UE\pxdk\"mu 5-4-01.zip" .
copy %sdrive%\UE\pxdk\"readme.txt" .
copy %sdrive%\UE\pxdk\"Xbox Compatible Logo files.zip" .
copy %sdrive%\UE\pxdk\"Xbox Compatible Test List (5-8).doc" "Xbox Compatible Test List.doc"
copy %sdrive%\UE\pxdk\"Xbox Connectors 4-23-01.zip" "Xbox Connectors.zip"
copy %sdrive%\UE\pxdk\"Xbox Third Party Peripheral Requirements (5-8).doc" "Xbox 3rd-Party Peripheral Requirements.doc"
copy %sdrive%\UE\pxdk\"Xbox Third-Party Peripheral Logo Guidelines (5-8).doc" "Xbox 3rd-Party Peripheral Logo Guidelines.doc"
copy %sdrive%\UE\pxdk\"Xbox XID Spec 1.0.doc" .
cd ..

md VCPP
cd VCPP
echo VCPP
copy %sdrive%\sdk\sdkfiles\visualstudio\readme.txt
md SP4
cd SP4
copy %sdrive%\sdk\sdkfiles\visualstudio\sp4\vcpp.exe .
copy %sdrive%\sdk\sdkfiles\visualstudio\sp4\ppreadme.htm .
cd ..
md SP5
cd SP5
copy %sdrive%\sdk\sdkfiles\visualstudio\sp5\vcpp.exe .
copy %sdrive%\sdk\sdkfiles\visualstudio\sp5\ppreadme.htm .
cd ..
cd ..

md VTune
cd VTune
echo VTune
copy %sdrive%\external\sdk\vtune\july01\Win32\"50comupd.exe" .
copy %sdrive%\external\sdk\vtune\july01\Win32\"data.cab" .
copy %sdrive%\external\sdk\vtune\july01\Win32\"instmsia.exe" .
copy %sdrive%\external\sdk\vtune\july01\Win32\"instmsiw.exe" .
copy %sdrive%\external\sdk\vtune\july01\Win32\"readme.htm" .
copy %sdrive%\external\sdk\vtune\july01\Win32\"setup.exe" .
copy %sdrive%\external\sdk\vtune\july01\Win32\"setup.ini" .
copy %sdrive%\external\sdk\vtune\july01\Win32\"vtune(tm) performance analyzer 6.0 for xbox.msi" .
cd ..

md DVDEmulation
cd DVDEmulation
echo DVD Emulation
copy %sdrive%\external\sdk\amc\release\august01\amcdvdemulator.dll .
copy %sdrive%\external\sdk\amc\release\august01\amcerrorq.dll .
copy %sdrive%\external\sdk\amc\release\august01\amclayoutengine.dll .
copy %sdrive%\external\sdk\amc\release\august01\amcmedia.dll .
copy %sdrive%\external\sdk\amc\release\august01\amcpcidrv.dll .
copy %sdrive%\external\sdk\amc\release\august01\amcphilipsemulator.ini .
copy %sdrive%\external\sdk\amc\release\august01\amcraptorcontroller.dll .
copy %sdrive%\external\sdk\amc\release\august01\amcstringtable.dll .
copy %sdrive%\external\sdk\amc\release\august01\amcthomsonemulator.ini .
copy %sdrive%\external\sdk\amc\release\august01\amcturboemulator.ini .
copy %sdrive%\external\sdk\amc\release\august01\amcudfs.dll .
copy %sdrive%\external\sdk\amc\release\august01\amcxdfs.dll .
copy %sdrive%\external\sdk\amc\release\august01\dumpfst.exe .
copy %sdrive%\external\sdk\amc\release\august01\dvdemulation.cnt .
copy %sdrive%\external\sdk\amc\release\august01\dvdemulation.hlp .
copy %sdrive%\external\sdk\amc\release\august01\fpgaloader.exe .
copy %sdrive%\external\sdk\amc\release\august01\odemusvr.exe .
copy %sdrive%\external\sdk\amc\release\august01\roboex32.dll .
copy %sdrive%\external\sdk\amc\release\august01\xbdvdemu.exe .
copy %sdrive%\external\sdk\amc\release\august01\xbdvdemupanel.exe .
copy %sdrive%\external\sdk\amc\release\august01\xbdvdimg.com .
copy %sdrive%\external\sdk\amc\release\august01\xbdvdimg.exe .
copy %sdrive%\external\sdk\amc\release\june01\dvdemulation.doc DVDEmulation-UsersGuide.doc
copy %sdrive%\external\sdk\amc\release\august01\readme.txt DVDEmulation-Readme.txt
copy %sdrive%\external\sdk\amc\release\august01\amcinfo.exe .
md Driver
cd Driver
copy %sdrive%\external\sdk\amc\release\august01\driver\amcpci.inf .
copy %sdrive%\external\sdk\amc\release\august01\driver\amcpci.sys .
copy %sdrive%\external\sdk\amc\release\july01\driver\q291855_w2k_sp3_x86_en.exe .
copy %sdrive%\external\sdk\amc\release\july01\driver\16056_JPN_i386_zip.exe .
copy %sdrive%\external\sdk\amc\release\july01\driver\16056_JPN_nec_98_zip.exe .
md cmdide
cd cmdide
copy %sdrive%\external\sdk\amc\release\august01\driver\cmdide\cmdide.cat .
copy %sdrive%\external\sdk\amc\release\august01\driver\cmdide\cmdide.inf .
copy %sdrive%\external\sdk\amc\release\august01\driver\cmdide\cmdide.sys .
copy %sdrive%\external\sdk\amc\release\august01\driver\cmdide\readme.txt .
cd ..
cd ..
cd ..

md DemoInstaller
cd DemoInstaller
echo DemoInstaller
copy %pdrive%\free\dump\installer.xbe default.xbe
copy %sdrive%\xbetools\installer\readme.txt .
md Files
md Media
cd Media
copy %sdrive%\xbetools\installer\media\*.* .
cd ..
cd ..

md ADPCM
cd ADPCM
echo ADPCM
copy %sdrive%\external\sdk\xbadpcm\readme.txt .
copy %sdrive%\external\sdk\xbadpcm\xbadpcm.acm .
copy %sdrive%\external\sdk\xbadpcm\xbadpcm.inf .
cd ..

rem -----------------------------------------------------
rem Return to CDStage
rem -----------------------------------------------------
popd
cdimage -lXboxSDK -n -d -c -x -ocis . %pdrive%\XboxSDK.iso
