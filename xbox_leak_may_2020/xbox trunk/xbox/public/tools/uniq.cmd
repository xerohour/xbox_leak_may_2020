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
perl.exe -S uniq.cmd %ARGS%
goto endofperl
@rem ';
@ARGV = sort @ARGV;

while($u <= $#ARGV)
{
	if ($ARGV[$u] eq $ARGV[$u+1] && $ARGV[$u+1] ne "")
	{
		splice(@ARGV,$u,1);
	}
	else
	{
	$u++;
	}
}

while($ARGV[0] =~ /^[-]/)
{
	if ($args[0] eq '-?' || $ARGV[0] ne '-r' && $ARGV[0] ne '-s' && $ARGV[0] ne '-m' && $ARGV[0] ne '-f' && $ARGV[0] ne '-u')
	{
		&usage;
	}	
	if ($ARGV[0] eq '-f')
	{
		$printfilename = 1;
		shift @ARGV;
	}
	if ($ARGV[0] eq '-m')
	{
		if ($printfilename)
		{
			&usage;
		}
		$merge = 1;
		shift @ARGV;
	}
	if ($ARGV[0] eq '-r')
	{
		$reverse = 1;
		shift @ARGV;
	}
	if ($ARGV[0] eq '-s')
	{
		$sort = 1;
		shift @ARGV;
	}
	if ($ARGV[0] eq '-u')
	{
		$unix = 1;
		shift @ARGV;
	}		
}

if ($merge)
{
	while(<>)
	{
		$arrayin[$arraynum++] = $_;
	}   
	if ($unix)
	{
		&unixuniq(*arrayin);
	}
	else
	{
		&unique(*arrayin);
	}					
}
else
{
	if ($ARGV[0] eq '')
	{
		while(<>)
		{
			$arrayin[$arraycount++] = $_;                
			if (eof)
			{
				$namesave = "<STDIN>";
				if ($unix)
					{
						&unixuniq(*arrayin);
					}
					else
					{
						&unique(*arrayin);
					}
				@arrayin="";
				$arraycount="";
			}                    
		}
	}

	else
	{
		while(@ARGV)
		{
			while(<>)
			{
				$namesave = $ARGV;
				$arrayin[$arraycount++] = $_;
				if (eof)
				{              
					if ($unix)
					{
						&unixuniq(*arrayin);
					}
					else
					{
						&unique(*arrayin);
					}
					@arrayin="";
					$arraycount="";
				}
			}
			shift @ARGV;
		}
	}
}

sub unique
{
	local(*a) = @_;
	
	$i = 0;
	
	if ($sort)
	{
		&dosort(*a);
	}
	if ($reverse)
	{
		&doreverse(*a);
	}	
	if ($printfilename)
	{
		print "\nFILE BEGIN ---- $namesave ---- FILE BEGIN\n\n";
	}
	
	while($i <= $#a)
	{
  		 $u = $i + 1;
  		 while($u <= $#a)
  		 {  
    			  if ($a[$i] eq $a[$u])
     			 {
        				 $count = 0;
        				 $tempu = $u;
        				 while($a[$i] eq $a[$tempu])
        				 {
          				 	 $count++;
          				 	 $tempu++;
        				 }
        				 splice(@a,$u,$count);
     			 }
     			 $u++;
  		 }
		 $i++;
	}
		
	print @a;       

	if ($printfilename)
	{
		print "\nFILE END ---- $namesave ---- FILE END\n\n";
	}
	
	@a = "";
	return;
}

sub unixuniq
{
	local(*a) = @_;
	
	$i = 0;
	while($i <= $#a)
	{
	 	
		print $a[$i] unless $a[$i] eq $oldline;
	 	$oldline = $a[$i];		
		$i++;
	}
	@a = "";
}

sub dosort
{
	local(*in) = @_;
	@in  = sort @in;
}

sub doreverse
{
	local(*in) = @_;
	@in = reverse @in;
}

sub usage
{
		print "\nUNIQUES A LIST'S CONTENTS TO SCREEN OR PIPE.\n\n";
		print "This script takes a list and eliminates duplicates. This script may be used\n"; 
		print "on files or as a pipe filter.\n\n";
		print "Usage: perl uniq.prl [yourfile(s)] [-f] [-m) [-r] [-s] [-u]\n\n";
		print "  -f(FILE)    | Outputs the current file name in a banner. May be used with\n";
		print "                sort & reverse. May not be used with -m.\n\n";						
		print "  -m(MERGE)   | Merges the arguments as one list for output. May be used with\n";
		print "                sort & reverse. May not be used with -f.\n\n";
		print "  -s(SORT)    | Sorts the list in ascending order.\n\n";
		print "  -r(REVERSE) | Reverses the list order. May be used with sort to achieve\n";
		print "                descending order.\n\n";
		print "  -u(UNIX)    | Mimics the unix utility uniq.exe, which only strips out the\n";
		print "                current line if it equals the previous line.\n\n";		
		exit;
}

__END__
:endofperl
