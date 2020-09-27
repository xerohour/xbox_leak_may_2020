#
# Common sources file for all XACT GUI components
#
# Be sure to include xacthost.inc before this file
#

XACTGUIBASEDIR = $(XACTHOSTBASEDIR)\gui

C_DEFINES = -DDPF_LIBRARY=\"XACTGUI\"

INCLUDES =\
    $(INCLUDES);\
    $(XACTGUIBASEDIR)\base;\
    $(XACTGUIBASEDIR)\controls;\
    $(XACTHOSTBASEDIR)\wavbndlr\common
