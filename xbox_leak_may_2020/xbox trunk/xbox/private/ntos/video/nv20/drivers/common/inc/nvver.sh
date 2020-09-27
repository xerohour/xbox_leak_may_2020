#!/bin/sh
#
# Shell script to return our version numbers for UNIX
#
# $Header: //sw/main/drivers/common/inc/nvVer.sh#10 $
#

# general dso revisions
DSOMAJOR=1
DSOMINOR=10
DSOPATCH=80

# specific dso revisions
# dsomajor is used for setting the DSONAME
DSOMAJOR_GLCORE=$DSOMAJOR
DSOMAJOR_GLXLIB=$DSOMAJOR
DSOMAJOR_GLXSRV=$DSOMAJOR

DSOMINOR_GLXSRV=$DSOMINOR
DSOPATCH_GLXSRV=$DSOPATCH

# full filename suffixes
DSOVERSION_GLCORE=$DSOMAJOR_GLCORE.$DSOMINOR.$DSOPATCH
DSOVERSION_GLXLIB=$DSOMAJOR_GLXLIB.$DSOMINOR.$DSOPATCH
DSOVERSION_GLXSRV=$DSOMAJOR_GLXSRV.$DSOMINOR.$DSOPATCH

# what is our core gl lib called?
GLCORE=libGLcore.so

# package revisions
BUILDVER=$DSOMAJOR.$DSOMINOR
# XXX for now, we are beta
#BUILDVER=0.9

# just return whatever is in our accumulated state.
# Look ma, no switch statements! :-)
eval "echo \$$1"