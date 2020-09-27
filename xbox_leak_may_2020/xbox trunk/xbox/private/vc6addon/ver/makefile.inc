$(BASEDIR)\private\vc6addon\misc\inc\verstamp.h : $(BASEDIR)\private\inc\xboxverp.h
    perl fixver.prl < $(BASEDIR)\private\inc\xboxverp.h > $@
