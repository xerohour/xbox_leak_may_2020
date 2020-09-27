@rem Clean.BAT
@rem This batch file cleans the objs,exes
@rem
@echo Delete all files
@rem   del windebug\*.obj
@rem   del windebug\*.sbr
@rem   del windebug\*.pdb
@rem   del windebug\*.ilk
@rem   del windebug\*.pch
@rem   del windebug\*.bsc
@rem   del windebug\*.exe
@rem   del windebug\*.dll
@rem   del windebug\*.res
@   echo y | del windebug\*.*
@   del def.cpp
@   del def.h
@   del *.vcp
@   del *.ncb
@   del results.log
@echo Done
