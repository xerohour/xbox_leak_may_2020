@setlocal
@echo off
if "%1" == "" (
   echo usage: ntsaveini {path to save all ntproject slm.ini's} {restore}
   echo.
   echo For instance:
   echo    ntsaveini f:\saveini
   echo.
   echo  would save all the slm.ini's for the projects listed in the NTPROJECTS
   echo  environment variable to f:\saveini.
   echo.
   echo    ntsaveini f:\saveini restore
   echo.
   echo  would restore the slm.ini's from f:\saveini to the correct dirs on
   echo  %_ntdrive% allowing you to ssync.
   goto end
   )
for %%i in (%NTPROJECTS%) do call _ntsaveini.cmd %%i %1 %2
:end
@endlocal
