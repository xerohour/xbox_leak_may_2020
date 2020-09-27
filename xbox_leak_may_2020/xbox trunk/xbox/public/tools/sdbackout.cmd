@rem = '--*-PERL-*--';
@rem = '
@echo off
rem setlocal
set ARGS=
:loop
if .%1==. goto endloop
set ARGS=%ARGS% %1
shift
goto loop
:endloop
rem ***** This assumes PERL is in the PATH *****
perl.exe -w -S sdbackout.cmd %ARGS%
goto endofperl
@rem ';
#
# SDBACKOUT - Back out a changelist.

# Does *not* reset integration information.  This means if you back out
# changes due to an integration, SD will not reapply the changes if the
# integration is attempted again.


# Simplify program name, if it is a path
$0 =~ s/.*\\//;


# Execute an SD command, keeping the output of the command in an array.
# Returns the array, unless an error occured, in which case an
# exception is thrown (via die) with an appropriate message.
sub command
{
	my($command) = @_;
	my(@results) = `$command`;
	if ($?)
	{
		my($err) = ($? >> 8);
		print STDERR @results;
		die qq($0: "$command" exited with status $err.\n);
	}
	@results
}


# Fatal usage error
sub usage
{
	my($err) = @_;
	die
	  "\n" .
	  "$0: $err\n" .
	  "\n" .
	  "    sdbackout - Undoes the effects of a submitted change.\n" .
	  "\n" .
	  "    sdbackout [ -y ] changenum\n" .
	  "\n" .
	  "        The 'changenum' argument specifies the change you want to back out.\n" .
	  "\n" .
	  "        The '-y' flag instructs sdbackout to do the work.  Otherwise it only\n" .
	  "        shows what would happen, but does not do anything.\n" .
	  "\n" .
	  "    Warning: This does *not* reset integration information.  If you back out\n" .
	  "             changes due to an integration, SD will *not* reapply the changes\n" .
	  "             if the integration is attempted again.  (You can back out the\n" .
	  "             back-out, though).\n";
}


# Default options
$doit = 0;				# show what would happen, without actually doing it
$debug = 0;


# Undocumented options
if (@ARGV && $ARGV[0] =~ /^-/)
{
#	$foo = 0;
}


# Parse options
while (@ARGV && $ARGV[0] =~ /^-/)
{
	$opt = shift;
	if ($opt eq '-y')
	{
		$doit = 1;						# do the work for real
	}
	elsif ($opt eq '-d')
	{
		$debug = 1;
	}
	else
	{
		usage("invalid option $opt.");
	}
}


# Get path argument.
usage("Changenum expected.") if !@ARGV;
usage("Invalid argument.") if @ARGV > 1;
$changenum = shift;


# get description of the change
@chg = command qq(sd describe -s $changenum);
die("$0: unable to describe change '$changenum'.\n") if not grep /^Affected files .../, @chg;


# make sure there are no opened files!
@opened = command qq(sd -s opened);
foreach $i (@opened)
{
	print $i if ($debug);

	if ($i =~ m/^warning:[ \t]*File\(s\) not opened on this client./)
	{
	}
	elsif ($i =~ m/^error:[ \t]*File\(s\) not opened on this client./)
	{
	}
	elsif ($i =~ m/^exit:/)
	{
		last;
	}
	elsif ($i =~ m/^info/)
	{
		die("$0: aborting; the client already has one or more files opened.\n");
	}
	else
	{
		die("$0: unexpected: \"$i\"\n");
	}
}


print "\n" .
	  "warning: If you are backing out a change that performed one or more\n" .
	  "         integrations, please note that SD will *not* reapply the changes\n" .
	  "         if the integration is attempted again.  (You can back out the\n" .
	  "         back-out, though).\n" .
	  "\n";


# step 1 - sync prior to change
$prev = $changenum - 1;
print "Running Automated Steps 1 thru 7:\n" .
	  "1.  sync to \@$prev (immediately prior to the change we're backing out)\n";
if ($doit)
{
	command qq("sd sync \@$prev");
}
else
{
	command qq("sd sync -n \@$prev");
}


# step 2 - edit all files edited in the change
print "2.  edit all files edited in change $changenum\n";
foreach $i (@chg)
{
	if ($i =~ m/^\.\.\. (\/\/.*)#.* ((edit)|(integrate))$/)
	{
		if ($doit)
		{
			print "opening \"$1\" for edit\n";
			command qq("sd edit \"$1\"");
		}
		else
		{
			print "sd edit \"$1\"\n";
		}
	}
}


# step 3 - add all files deleted in the change
print "3.  add all files deleted in change $changenum (this backs out the deletes)\n";
foreach $i (@chg)
{
	if ($i =~ m/^\.\.\. (\/\/.*)#.* delete$/)
	{
		$depot = $1;

		if ($doit)
		{
			@have = command qq("sd have \"$depot\"");
			die("$0: only expected one line of output.\n") if @have > 1;
			foreach $h (@have)
			{
				$h =~ m/^.*#.* - (.*)$/;
				$local = $1;
			}
			die("$0: can't add file, you don't have $depot\n") if ("$local" eq "");
			print "warning: autodetecting file type for \"$local\"\n";
			command qq("sd add \"$local\"");
		}
		else
		{
			print "sd add \"$depot\"\n";
		}
	}
}


# step 4 - sync to 'bad' change
print "4.  sync to \@$changenum (the change we're backing out)\n";
if ($doit)
{
	command qq("sd sync \@$changenum");
}
else
{
	command qq("sd sync -n \@$changenum");
}


# step 5 - delete all files added in the change
print "5.  delete all files added in \@$changenum (this backs out the adds)\n";
foreach $i (@chg)
{
	if ($i =~ m/^\.\.\. (\/\/.*)#.* ((add)|(branch))$/)
	{
		if ($doit)
		{
			print "opening \"$1\" for delete\n";
			command qq("sd delete \"$1\"");
		}
		else
		{
			print "sd delete \"$1\"\n";
		}
	}
}


# step 6 - accept the previous files; this backs out the edits
print "6.  sd resolve -ay (accept previous files; this backs out the edits)\n";
if ($doit)
{
	command qq("sd resolve -ay");
}


# step 7 - sync to head
print "7.  sync to head revisions of all files\n";
if ($doit)
{
	command qq("sd sync");
}
else
{
	command qq("sd sync -n");
}


# steps 8 and 9 are best done manually
print "\n\n" .
	  "Manual Steps:  You must manually perform steps 8 thru 10:\n" .
	  "8.  resolve the merges (run \"sd resolve\" or some variation thereof)\n" .
	  "9.  verify the merges (run \"sd diff -dc\" and carefully inspect the merges!!)\n" .
	  "10. submit the 'backout' change (run \"sd submit\")\n";

print "\nwarning: no work was performed; use the '-y' to make the command do work.\n" if (!$doit);

__END__
:endofperl
