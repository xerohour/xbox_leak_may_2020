@rem Clean.BAT
@rem This batch file cleans the objs,exes
@rem
@echo Delete old objs' files
@   del /q windebug\*.*
@   del /q windebug.20\*.*
@   del /q results.log       2>nul
@echo Delete source files
@   cd ..
@   del /q msgdoc.cpp
@   del /q msgdoc.h
@   del /q *.vcp
@echo Done
