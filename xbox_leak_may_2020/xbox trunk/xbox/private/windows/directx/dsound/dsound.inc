#
# Include this from your sources file when building a lib that is linked into
# dsound.lib
#

!include xbox.inc

C_DEFINES=$(C_DEFINES) -DDSNDVER -DBUILDING_DSOUND