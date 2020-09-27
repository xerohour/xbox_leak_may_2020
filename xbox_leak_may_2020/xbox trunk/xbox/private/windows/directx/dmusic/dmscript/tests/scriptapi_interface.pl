# find method start
while (<>)
{
	last if /::ms_Methods\[\]/;
}

# skip open brace
while (<>)
{
	last if !/^\s*{\s*$/;
}

# skip comments
while (<>)
{
	last if !/^\s*\/\//;
}

# process records
do
{
	last if /DISPID_UNKNOWN/;

	# name line
	if (/L"(.*)",\s*(.*)/)
	{
		$mname = $1;
		$2 =~ /\s*\/*\s*§*\s*(.*)/;
		$mcomment = $1;
		print "<hr><h2>$mname</h2>\n";
		print " $mcomment<p>\n";
		print " <dl>\n";
	}

	# parameter
	elsif (/^\s*ADT_(\S*),\s*(\S*),\s*&IID_(\S*),(.*)/)
	{
		$ptype = $1;
		$popt = $2;
		$piface = $3;
		$4 =~ /\s*\/*\s*(.*)/;
		$pcomment = $1;
		print " <dt>$pcomment\n";
		print "  <dd> ";
		print "optional " if ($popt eq "true");
		print "  $ptype";
		if ($piface =~ /IDirectMusic(.*)/)
		{
			print " of type $1";
		}
		print "\n";
	}

	# end parameters
	elsif (/ADT_None\s*},/)
	{
		print " </dl>\n"
	}
} while (<>);
