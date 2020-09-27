@echo off

if "" == "%1" goto usage

call GRegion1\copytoxb %1
call GRegion2\copytoxb %1
call GRegion3\copytoxb %1
call GRegionAll\copytoxb %1
goto end

:usage
Echo You must specify your Xbox name

:end