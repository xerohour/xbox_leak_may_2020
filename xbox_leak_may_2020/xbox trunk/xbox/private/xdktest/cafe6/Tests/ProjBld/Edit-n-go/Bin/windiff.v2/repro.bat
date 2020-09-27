@rem REPRO.BAT
@rem This batch file simulates the edit/build aspects of the editgo sniff test
@rem it is a good starting place for generating repro steps for entering Batch tool bugs
@rem
@echo Delete old objs' files
@   del /q windebug\*.*
@   del /q results.log             2>nul
@echo Build Dll
@   nmake /af gutils.mak 1>>results.log 2>>&1
@echo Delete source files
@   del /q windiff.c
@echo Copy original files
@   copy windiff.co  windiff.c      >nul
@   touch  windiff.c
@echo Build all
@   nmake /af "windiff.mak" CFG="Win32 (80x86) Debug" 1>>results.log 2>>&1
@echo Copy files to simulate major change
@   copy windiff.1s  windiff.c      >nul
@   touch  windiff.c
@echo Build Major
@   nmake /f "windiff.mak" CFG="Win32 (80x86) Debug" 1>>results.log 2>>&1
@echo Copy files to simulate minor change
@   copy windiff.2s  windiff.c      >nul
@   touch  windiff.c
@echo Build Minor
@   nmake /f "windiff.mak" CFG="Win32 (80x86) Debug" 1>>results.log 2>>&1
@echo Checking for errors
    grep -y error results.log
@echo Done
