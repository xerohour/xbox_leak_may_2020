#
# Standard inference rule for generating dsp files.
#
.SUFFIXES: .dsp .dsx

{}.dsx{}.dsp:
    m4 -DPROJNAME=$(PROJNAME) -DTARGETNAME=$(TARGETNAME) -DTITLEID=$(TITLEID) -DCOMMONSOURCEFILELIST= -DRELEASELIBS= -DDEBUGLIBS= $** > tmp.m4
    m4 -DPOUND=# tmp.m4 > $@
    del tmp.m4
