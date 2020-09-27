@rem REPRO.BAT
@rem This batch file simulates the edit/build aspects of the editgo sniff test
@rem it is a good starting place for generating repro steps for entering Batch tool bugs
@rem
@echo Delete all files
@   del /q windebug\*.*
@   del /q def.cpp
@   del /q def.h
@   del /q results.log       2>nul
@echo Copy original files
@   copy defcpp.old  def.cpp  >nul
@   copy defh.old    def.h    >nul
@   touch  def.cpp def.h
@echo Build all
@   nmake /af "def.mak" CFG="Win32 Debug" 1>>results.log 2>>&1
@echo Copy files to simulate major change
@   copy def.1s  def.cpp      >nul
@   copy def.1h  def.h        >nul
@   touch  def.cpp def.h
@echo Build Major
@   nmake /f "def.mak" CFG="Win32 Debug" 1>>results.log 2>>&1
@echo Copy files to simulate minor change
@   copy def.2s  def.cpp      >nul
@   touch  def.cpp
@echo Build Minor
@   nmake /f "def.mak" CFG="Win32 Debug" 1>>results.log 2>>&1
@echo Checking for errors
    grep -y error results.log
@echo Done
