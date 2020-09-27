#
# Common sources file for all dsremote apps and libs
#

C_DEFINES=$(C_DEFINES) -DDPF_LIBRARY=\"DSREMOTE\" -DNAMESPACE=DSREMOTE -DDPFLVL_DEFAULT=4

INCLUDES=$(INCLUDES);\
    $(DSBASEDIR)\tools\dsremote

SOURCES=$(SOURCES) \
    ..\iremote.cpp \
    ..\ifileio.cpp