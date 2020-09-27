@echo off
setlocal

set _idw=%_NTDrive%%_NTRoot%\public\idw
set _mstools=%_NTDrive%%_NTRoot%\public\mstools
set _vc6=%_mstools%\vc60
set _wsdklib=%_NTDrive%%_NTRoot%\public\wsdk\lib

pushd %_mstools%
for %%f in (c1.dll c1xx.dll c2.dll cl.exe link.exe lib.exe mspdb60.dll) do attrib -R %%f & copy /y %_vc6%\%%f %%f
REM for %%f in (msobj10.dll mspdb70.dll msvcr70.dll msvcr70d.dll) do del /f %%f
popd

attrib -R %_idw%\binplace.exe
copy /y %_vc6%\binplace.exe %_idw%\binplace.exe

pushd %_wsdklib%
for %%f in (msvcrt.lib msvcrtd.lib delayimp.lib) do attrib -R %%f & copy /y %_vc6%\%%f %%f
popd

endlocal

set USE_VC7=0
