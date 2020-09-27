
@   del /q windebug\*.*
@   del /q debug\*.*
@   del /q def.cpp
@   del /q def.h
@   del /q *.vcp
@   del /q results.log       2>nul
@echo Copy original files
@   copy defcpp.old  def.cpp  >nul
@   copy defh.old    def.h    >nul
@   touch  def.cpp def.h
