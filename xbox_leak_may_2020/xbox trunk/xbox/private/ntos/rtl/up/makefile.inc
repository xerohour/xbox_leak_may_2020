!ifdef PASS0ONLY
.SUFFIXES: .c
!endif

obj\$(TARGET_DIRECTORY)\generr.obj: ..\generr.c

..\error.h: obj\$(TARGET_DIRECTORY)\generr.obj
    generr $? $@
