@rem REPRO.BAT
@rem This batch file simulates the edit/build aspects of the editgo sniff test
@rem it is a good starting place for generating repro steps for entering Batch tool bugs
@rem
@echo Delete old objs' files
@   del /q windebug\*.*
@   del /q windebug.20\*.*
@   del /q results.log       2>nul
@echo Build Dll
@   nmake /af "spyxxhk2.mak" CFG="WinDebug" 1>>results.log 2>>&1
@echo Delete source files
@   cd ..
@   del /q msgdoc.cpp
@   del /q msgdoc.h
@echo Copy original files
@   copy msgdoccpp.0s  msgdoc.cpp  >nul
@   copy msgdoch.0h    msgdoc.h    >nul
@   touch  msgdoc.cpp msgdoc.h
@echo Build all
@   nmake /af "spyxx2.mak" CFG="WinDebug" 1>>results.log 2>>&1
@echo Copy files to simulate major change
@   copy msgdoc.1s  msgdoc.cpp      >nul
@   copy msgdoc.1h  msgdoc.h        >nul
@   touch  msgdoc.cpp msgdoc.h
@echo Build Major
@   nmake /f "spyxx2.mak" CFG="WinDebug" 1>>results.log 2>>&1
@echo Copy files to simulate minor change
@   copy msgdoc.2s  msgdoc.cpp      >nul
@   touch  msgdoc.cpp
@echo Build Minor
@   nmake /f "spyxx2.mak" CFG="WinDebug" 1>>results.log 2>>&1
@echo Checking for errors
    grep -y error results.log
@echo Done
