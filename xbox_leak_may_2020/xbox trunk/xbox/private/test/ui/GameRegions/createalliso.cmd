@echo off

call GRegion1\createiso.cmd
call GRegion2\createiso.cmd
call GRegion3\createiso.cmd
call GRegionAll\createiso.cmd
goto end

:end