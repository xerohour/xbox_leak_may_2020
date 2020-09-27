@echo off

call GRate6EC\createiso.cmd
REM move GRate6EC\*.iso .\
call GRate5KA\createiso.cmd
REM move GRate5KA\*.iso .\
call GRate4E\createiso.cmd
REM move GRate4E\*.iso .\
call GRate3T\createiso.cmd
REM move GRate3T\*.iso .\
call GRate2M\createiso.cmd
REM move GRate2M\*.iso .\
call GRate1AO\createiso.cmd
REM move GRate1AO\*.iso .\
call GRate0RP\createiso.cmd
REM move GRate0RP\*.iso .\
goto end

:end