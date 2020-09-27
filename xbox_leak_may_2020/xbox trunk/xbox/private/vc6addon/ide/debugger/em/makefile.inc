$O\res_str.i: res_str.h
    $(TARGET_CPP) -EP -DRESOURCES $(CDEFINES) $(INCPATH0) -Tc res_str.h > $@


$O\em.res: $O\res_str.i

