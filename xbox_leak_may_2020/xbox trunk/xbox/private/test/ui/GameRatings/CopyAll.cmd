@echo off

if "" == "%1" goto usage

call GRate6EC\copytoxb %1
call GRate5KA\copytoxb %1
call GRate4E\copytoxb %1
call GRate3T\copytoxb %1
call GRate2M\copytoxb %1
call GRate1AO\copytoxb %1
call GRate0RP\copytoxb %1
goto end

:usage
Echo You must specify your Xbox name

:end