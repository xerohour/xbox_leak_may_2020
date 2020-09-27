# find method start
while (<>)
{
	last if /gs_Constants\[\]/;
}

# skip open brace
while (<>)
{
	last if !/^\s*{\s*$/;
}

# process records
do
{
	exit() if /DISPID_UNKNOWN/;

	# name line
	if (/L"(.*)",/)
	{
		print "$1<P>\n";
	}
} while (<>);
