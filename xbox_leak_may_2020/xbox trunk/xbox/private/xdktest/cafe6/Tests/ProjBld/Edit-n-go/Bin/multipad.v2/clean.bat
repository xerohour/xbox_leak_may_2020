@rem Clean.BAT
@rem This batch file cleans the objs,exes
@rem
@echo Delete all files
@   echo y | del windebug\*.*
@   del multipad.c
@   del multipad.h
@   del results.log
@echo Done
