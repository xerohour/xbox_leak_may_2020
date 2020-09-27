@echo off

if "%1" == "" goto NOPARAMS
goto %1

:NOPARAMS
echo Specify Debug or Retail as a parameter to copy the correct version of XSS

goto DONE

:DEBUG
copy /Y SDKDEV\Xbox\Debug c:\xbox
goto DONE

:RETAIL
copy /Y SDKDEV\Xbox\Retail c:\xbox
goto DONE

:DONE