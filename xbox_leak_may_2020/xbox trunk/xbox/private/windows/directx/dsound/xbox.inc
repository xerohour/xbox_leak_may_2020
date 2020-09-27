#
# Include this from your sources file when building an Xbox app or library 
# that does not get linked into dsound.lib.
#

!include sources.inc

TARGETPATH=$(BASEDIR)\public\sdk\lib

INCLUDES=\
    $(SDK_INC_PATH);\
    $(DDK_INC_PATH);\
    $(PRIVATE_INC_PATH);\
    $(BASEDIR)\private\ntos\inc;\
    $(DSBASEDIR)\dsound;\
    $(DSBASEDIR)\ac97;\
    $(DSBASEDIR)\codecs\inc;\
    $(DSBASEDIR)\tools\inc;\
    $(INCLUDES)
