@echo off
setlocal

set _idw=%_NTDrive%%_NTRoot%\public\idw
set _mstools=%_NTDrive%%_NTRoot%\public\mstools
set _wsdklib=%_NTDrive%%_NTRoot%\public\wsdk\lib

pushd %_mstools%
for %%f in (c1.dll c1xx.dll c2.dll cl.exe link.exe lib.exe mspdb60.dll mspdb70.dll msvcr70.dll) do sd sync -f %%f
for %%f in (msobj10.dll msvcr70d.dll) do del /f %%f
popd

rem sd sync -f %_idw%\binplace.exe

rem pushd %_wsdklib%
rem for %%f in (msvcrt.lib msvcrtd.lib delayimp.lib) do sd sync -f %%f
rem popd

endlocal
