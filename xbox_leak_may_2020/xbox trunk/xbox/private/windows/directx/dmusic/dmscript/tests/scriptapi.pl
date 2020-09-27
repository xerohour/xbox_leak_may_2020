@interfaces = ( Band, Chordmap, Composer, Loader, Performance, Segment, SegmentState, Style );

#print heading
print "<h1>Audio Scripting Interfaces</h1>\n\n";

#link to each interface
for $x (@interfaces)
{
	print "<a href=\#$x>$x</a><p>\n";
}

#link to constants
print "<p>\n<a href=\#Constants>Constants</a><p>\n";

#listing of each interface
for $x (@interfaces)
{
	#anchor
	print "<hr><a name=\#$x>\n";
	print "<h1>$x interface</h1>\n";
	print `start scriptapi_interface.pl <aut$x.cpp`;
}

#listing of constants
print "<hr><a name=\#Constants>\n";
print "<h1>Constants</h1>\n";
print `start scriptapi_constants.pl <autconstants.cpp`;
