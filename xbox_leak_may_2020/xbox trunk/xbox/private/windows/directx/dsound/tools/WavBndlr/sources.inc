#
# Common sources file for all Wave Bundler apps and libs
#

C_DEFINES=$(C_DEFINES) -DDPF_LIBRARY=\"WAVBNDLR\"

INCLUDES=$(INCLUDES);\
    $(DSBASEDIR)\tools\wavbndlr\common
