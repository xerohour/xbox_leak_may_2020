@echo off
setlocal

set _idw=%_NTDrive%%_NTRoot%\public\idw
set _mstools=%_NTDrive%%_NTRoot%\public\mstools
set _vc7=%_mstools%\vc70
set _wsdklib=%_NTDrive%%_NTRoot%\public\wsdk\lib

pushd %_mstools%
for %%f in (c1.dll c1xx.dll c2.dll cl.exe link.exe lib.exe mspdb60.dll mspdb70.dll msvcr70.dll) do attrib -R %%f & copy /y %_vc7%\%%f %%f
for %%f in (msobj10.dll msvcr70d.dll) do attrib -R %%f & copy %_vc7%\%%f %%f
popd

rem attrib -R %_idw%\binplace.exe
rem copy /y %_vc7%\binplace.exe %_idw%\binplace.exe

rem pushd  %_wsdklib%
rem for %%f in (msvcrt.lib msvcrtd.lib delayimp.lib) do attrib -R %%f & copy /y %_vc7%\%%f %%f
rem popd

endlocal

