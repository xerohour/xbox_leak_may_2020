@rem REPRO.BAT
@rem This batch file simulates the edit/build aspects of the editgo sniff test
@rem it is a good starting place for generating repro steps for entering Batch tool bugs
@rem
@echo Delete all files
@   del /q windebug\*.*
@   del /q multipad.c
@   del /q multipad.h
@   del /q results.log       2>nul
@echo Copy original files
@   copy multipad.co    multipad.c   >nul
@   copy multipad.ho    multipad.h   >nul
@   touch  multipad.c multipad.h
@echo Build all
@   nmake /af "multipad.mak" CFG="Win32 (80x86) Debug" 1>>results.log 2>>&1
@echo Copy files to simulate major change
@   copy multipad.1s  multipad.c     >nul
@   copy multipad.1h  multipad.h     >nul
@   touch  multipad.c multipad.h
@echo Build Major
@   nmake /f "multipad.mak" CFG="Win32 (80x86) Debug" 1>>results.log 2>>&1
@echo Copy files to simulate minor change
@   copy multipad.2s  multipad.c     >nul
@   touch  multipad.c
@echo Build Minor
@   nmake /f "multipad.mak" CFG="Win32 (80x86) Debug" 1>>results.log 2>>&1
@echo Checking for errors
    grep -y error results.log
@echo Done
