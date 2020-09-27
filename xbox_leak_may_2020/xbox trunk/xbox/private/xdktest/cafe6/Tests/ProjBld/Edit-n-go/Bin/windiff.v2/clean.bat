@rem Clean.BAT
@rem This batch file cleans the objs,exes
@rem
@rem
@echo Delete old objs' files
@   echo y | del windebug\*.*
@   del results.log
@echo Delete source files
@   del windiff.c
@echo Done
