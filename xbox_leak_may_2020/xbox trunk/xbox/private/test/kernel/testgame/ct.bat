:
: before you run this, use copybins to copy a kernel
:

:
: copy in new default title 
: the ID of this one will be 0x01230000
:

xbcp -y %_NT386TREE%\dump\testgame.xbe xe:\testgame.xbe

:
: copy in the test games
:

xbcp -y -r %_NT386TREE%\testgame\ xy:\


