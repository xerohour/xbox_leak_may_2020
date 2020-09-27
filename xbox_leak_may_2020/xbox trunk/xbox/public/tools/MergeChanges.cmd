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
perl.exe -w -S MergeChanges.cmd %ARGS%
goto endofperl
@rem ';
#
#
# Merges changes from one source depot branch to another.
#
# Based on WebTV's MergeChanges command.
#

# -----------------------------------------------------------
# Exit codes
# 
# 0 = -help specified on command line
# 1 = illegal/missing parameter
# -----------------------------------------------------------

#require "PE.pm";
#
#require "Common.Perforce.pl";
#require "CommonPerlCode.pl";

# use File::Find;
# use strict;

# ---- Utility functions ---

$gRedirection		= "2>&1";


$__g_common_ClientView = "";
$__g_common_CurrentClientView = "";
$__g_common_CurrentView = "";
$__g_common_DepotView = "";
$__g_common_DepotViewRoot = "";
$__g_common_P4 = "sd";
$__g_common_P4Root = "";
$__g_common_ProjectRoot = "";
@__g_common_ClientViewList = ();
@__g_common_DepotViewList = ();
@__g_common_DepotViewRootList = ();
@__g_common_ProjectRootList = ();

$gHome = "";
$gP4 = "";


sub ScriptName {

	my ($Script, $gScriptName, $gScriptPath);
	
	($Script		= "$0")			=~ tr/\\/\//;  
	($gScriptName	= "$Script")	=~ s%^.*/%%;	$gScriptName =~ s/\.cmd//i;
	$gScriptPath 	= ($Script	=~ m:^(.*/):)[0] || ".";

	return( "$Script", "$gScriptName", "$gScriptPath" );
	}

# Standardize `pwd` call.  Make it with Bash syntax (d:/development) and chomp it.
sub PE_pwd {
	my $pwd = &MakeBashPath( `cd` );
	chomp ( $pwd );
	return ( $pwd );
	}

sub MakeBashPath {

	my ( $path ) = shift || "";

	(my $BashPath = $path) =~ tr/\\/\//;

	if( "$BashPath" =~ /^\/\/([^\/])(\/.*)$/ ) {
		$BashPath = "${1}:${2}";
		}

	return( $BashPath );
	}

#--------------------------------------------------
# InitializeWhereAreWe
#--------------------------------------------------

sub InitializeWhereAreWe
{
	my $gOS = $^O;  # perl sets this variable (OS name)

	my ($gShortHost);
	my ($userName, $make, $whoami);

	$gSolaris = 0;
    $gLinux = 0;
	$gWindows = 0;
	my %osvars = (
		"dec_osf"	=> \$gSolaris,
		"solaris"	=> \$gSolaris,
		"linux"		=> \$gLinux,
		"MSWin32"	=> \$gWindows,
	);
	ref $osvars{$gOS} and ++${$osvars{$gOS}};

	$gClient = $ENV{'SDCLIENT'};
	$gUser = $ENV{'SDUSER'}; # OK to be undefined

	%uservars = (
		"User"		=> \$gUser,
		"FullName"	=> \$userName,
	);
	$userpattern = join("|", keys %uservars);

	open(SDUSER, "$__g_common_P4 user -o |") or die "Couldn't run sd user: $!";
	while (<SDUSER>) {
		my ($userkey, $userval) = /^($userpattern)\:\s+(.+)/;
		$userkey and $userval and ref $uservars{$userkey}
			and ${$uservars{$userkey}} = $userval;
	}
	close(SDUSER);

	{
		$gHome 		= &TranslatePath( "$ENV{'_NTDRIVE'}$ENV{'_NTROOT'}" );
		# $gFStype	= Win32::FsType();
		$whoami	   	= ""; # doesn't matter
		$make	   	= ""; # doesn't matter
		$gP4	   	= "sd";
		$gUser		||= lc(Win32::LoginName());
		$gHost 		= uc(Win32::NodeName());
		$gShortHost	= $gHost;
#		-e "/tmp" or mkdir("/tmp", 0777);

		if ($gUser =~ /^administrator/i) {
			die <<EOT;
###
### ERROR: you're logged in as administrator, relogin as you!
###
EOT
		}
    }

	return ($gSolaris, $gLinux, $gWindows, $gShortHost,
		$gHome, $make, $gP4, $gUser, $gClient, $whoami, $userName);
}

#--------------------------------------------------
# MakeDosPath
# TranslatePath
#--------------------------------------------------

sub MakeDosPath { $_[0] =~ tr|/|\\|; return $_[0]; }

sub TranslatePath { $_[0] =~ tr|\\|/|; return $_[0]; }

sub mydie
{
	local $_ = shift;
	s/^/### /m;
	s/^### //;
	die "\n### ERROR ", __FILE__, ":", __LINE__, ": $_\n";
}

sub mywarn
{
	local $_ = shift;
	s/^/### /m;
	s/^### //;
	warn "\n### WARNING ", __FILE__, ":", __LINE__, ": $_\n";
}


#--------------------------------------------------
# Init_Common_Perforce - inits everything - for performance reasons, that's why
#--------------------------------------------------
sub Init_Common_Perforce
{
	my (%P4Views, %P4Client);

	Set_Environment();
	Make_CurrentView();

	open(P4CLIENTOUT, "$__g_common_P4 client -o |") or
		mydie "couldn't run the sd client command to get needed info: $!";

	while ( <P4CLIENTOUT> )	{

		/^Root:\s+(\S*[^\/\\\s])/ and do {
			# PC can have a 'null' root allowing checkouts on multiple drives
			$__g_common_P4Root = $1 eq "null" ? "" : $1;
			next;
		};

		/^View:/ and do {
			while (<P4CLIENTOUT>) {
				last if /^\s*$/;
				if (/^\s+(\S+)\.{3}\s+(\S+)\.{3}/) {
					$P4Views{$2} = $1;
					exists $P4Client{$1} and mydie <<EOT;

You have the following duplicate depot mappings in your client profile:
		$1 $P4Client{$1}
		$1 $2
This is unsafe. If you have any questions, please email $email_errors.
EOT
					$P4Client{$1} = $2;
				}
			}
			next;
		};
	}
	close P4CLIENTOUT;

	#### ERROR checking #### needed for clueless p4 users ####
	my %rootViews =	map { $_ => 1 } qw(
		//depot/
		//depot/branches/
		//depot/branches/client/
		//depot/branches/network/
	);

    local $eachKey;
	for $eachKey (keys %P4Views)	{
		exists $rootViews{$P4Views{$eachKey}} and mydie <<EOT;

Oh boy! It appears your p4 client specification is incorrect. You have root
level p4 directories in your client. Here's the problem line in your client
specification:
		$P4Views{$eachKey}... $eachKey...

You're not allowed to map from the directory $P4Views{$eachKey}.

Send mail to $email_errors if you're still confused.

Exiting early, nothing updated!

EOT
	}

	# Sort the hash keys alphabetically.
	# Thus the first element will represent the highest directory.
	for $eachKey (sort keys %P4Views) {
		if ($__g_common_CurrentClientView =~ /^\Q${eachKey}\E/
			or $eachKey =~ /^\Q${__g_common_CurrentClientView}\E/)
		{
			push @__g_common_DepotViewList, "$P4Views{$eachKey}...";
			push @__g_common_DepotViewRootList, $P4Views{$eachKey} =~ m|(.*)/|;
			push @__g_common_ClientViewList, "$eachKey...";
			if ($eachKey =~ m|^//$ENV{'SDCLIENT'}/(.*)/|i) {
				push @__g_common_ProjectRootList,
					$__g_common_P4Root ? "$__g_common_P4Root/$1" : $1;
			}
		}
	}

	$__g_common_DepotView     = $__g_common_DepotViewList[0];
	$__g_common_DepotViewRoot = $__g_common_DepotViewRootList[0];
	$__g_common_ClientView    = $__g_common_ClientViewList[0];
	$__g_common_ProjectRoot   = $__g_common_ProjectRootList[0];

	# Check that they're on the right server, if they're using CE...
	# if ( (($__g_common_DepotView =~ m|^//depot/ce/|) ||
	#	   ($__g_common_DepotView =~ m|^//depot/branches/ce/|))
	#   && (($ENV{'P4PORT'} =~ /^sd-server-1.*:1668/) ||
	# 	   ($ENV{'P4PORT'} =~ /^perforce-server-1.*:1668/) ||
	#	   ($ENV{'P4PORT'} =~ /^faith.*:1668/)) ) {
	#	die "Sorry, you don't have permission to access $__g_common_DepotView.\n";
	#}

    if(!defined($__g_common_P4Root)){
        die " __g_common_P4Root undefined.\n";
    }
	
    if(!defined($__g_common_ProjectRoot)){
        die " This script is too simple-minded to understand your sd client view. Consider changing your client view so that you use null for your source depot root.\n";
    }
	
	# make the returned paths Windows compatible, needed for Win 98
	# Turns out this is also needed for PEWEB changes!  The call to "p4 diff"
	# returns DOS notation (i.e., backslashes)
	$__g_common_P4Root = MakeDosPath($__g_common_P4Root);
	$__g_common_ProjectRoot = MakeDosPath($__g_common_ProjectRoot);
}

#-----------------------------------------------------------
# Set_Environment - sets environment in Init_Common_Perforce
#-----------------------------------------------------------
sub Set_Environment
{
	my $user;
	
	# Is their environment setup yet?
	if ( $ENV{'SDCLIENT'} eq "" )	{
	    mydie <<EOT;
### ERROR: Your environment variable, \"SDCLIENT\" was not initialized...
EOT
    }

	if ( $ENV{'SDPORT'} eq "" )	{
        mydie <<EOT;
### ERROR: Your environment variable, \"SDPORT\" was not initialized...
EOT
    }

	if ( $ENV{'TEMP'} eq "" )	{
	    mydie <<EOT;
### ERROR: Your environment variable, \"TEMP\" was not initialized...
EOT
    }

	if ( $ENV{'_NTROOT'} eq "" )	{
	    mydie <<EOT;
### ERROR: Your environment variable, \"_NTROOT\" was not initialized...
EOT
    }

	if ( $ENV{'_NTDRIVE'} eq "" )	{
	    mydie <<EOT;
### ERROR: Your environment variable, \"_NTDRIVE\" was not initialized...
EOT
    }
}


#--------------------------------------------------------------
# Make_CurrentView -- Creates CurrentView and CurrentClientView
#--------------------------------------------------------------
sub Make_CurrentView
{
	open CURVIEW, "$__g_common_P4 where $gRedirection |"
		or mydie "couldn't run the sd where command to get needed info: $!";
	local $_ = <CURVIEW>;
	close CURVIEW;

	{ # Switch Block

		/^(Perforce|SourceDepot) client error:/ and do {
			my $envvar = $1 eq "Perforce" ? "P4PORT" : "SDPORT";
			mydie <<EOT;

$1 is having difficulty contacting the server.

Perhaps the server is down, or your $envvar variable is not set properly.
Check your $envvar variable and try \"p4 info\" for more information.
EOT
			last;
		};

		/^Must create client .*/ and do {
			mydie "Your Perforce environment isn't setup properly. ",
				$gWindows ?
					"See http://peweb/tools/nt." :
					"Try the Perforce-Init command.";
			last;
		};

		/^Can\'t create a new user - over license quota/ and do {
			mydie <<EOT;
bummer, we've run out of licenses!
Please email $email_errors and they'll help clear up a spot for you. Sorry.
EOT
			last;
		};

		/^Path .* is not/ and do {
			mydie "which sources do you wish to update? Are you in the right directory?";
			last;
		};
	}

#	if (m!^\s*(//.+)\s(//.+)\s(/|[a-zA-Z]:)! ) {
#		$__g_common_CurrentView = $1;
#		$__g_common_CurrentClientView = $2;
#	} elsif (/^(.+)\s+(.+)\s*$/ ) {
#		$__g_common_CurrentView = $1;
#		$__g_common_CurrentClientView = $2;
#	}
	($__g_common_CurrentView, $__g_common_CurrentClientView) = split;
	$__g_common_CurrentView and $__g_common_CurrentClientView or
		mydie "can't figure out which sources you want to update, are you in the right directory?";
}

#--------------------------------------------------
# GetP4Root - returns a string with is the P4 Root directory
#--------------------------------------------------

sub GetP4Root
{
	return $__g_common_P4Root;
}

#--------------------------------------------------
# GetProjectRoot() - figures out the current "Project" directory.
#	A project is a src tree under the p4 root
#--------------------------------------------------
sub GetProjectRoot
{
	return $__g_common_ProjectRoot;
}

#--------------------------------------------------
# GetDepotView - based on the current directory, views,
#	and other info, return the the DepotView.
#--------------------------------------------------
sub GetDepotView
{
	return $__g_common_DepotView;
}

$gNoExecution = 0;

#--------------------------------------------------
# Execute - execute a shell command
#    This routine exists only so every "system" call 
#    goes through here (and it's easy to turn off).
#--------------------------------------------------
sub PE_Execute
{
	my $cmd = shift;
	my $useexec = shift;

	if ($gNoExecution) {
		print "=> cmd is: $cmd\n";
		return undef;
#	} elsif ($cmd =~ /|\s*$/) { # a pipe
#		mydie "Can't handle pipe commands: $cmd"; # return IO::Handle->new($cmd);
	} elsif ($useexec) {
		exec $cmd;
	} else {
		my $res = system $cmd;
		if ($res >> 8 == 255) {
			mywarn <<EOT;
The call \"$cmd\" died. Terminating the calling program.
Look above for the original error.
If you have any questions, please email $email_errors
EOT
		} elsif ($gScriptDebugging) {
			print "# cmd \"$cmd\" exited with status $res\n";
		}
		return $res;
	}
}


# --- End of utility functions ----



my ($script, $gScriptName, $gScriptPath) = ScriptName();

# -----------------------------------------------------------
# Global variables
# -----------------------------------------------------------

InitializeWhereAreWe();

Init_Common_Perforce();

$gCurDir = PE_pwd();

my ($secs, $mins, $hour, $day, $month, $year, $dow1) = localtime();
$year += 1900; if ( $year < 1998 ) { $year += 100; } # not y2k compliant, but good for 100 years
$month += 1; # it's zero based
if ( $hour < 10 ) { $hour = "0" . $hour; }
if ( $mins < 10 ) { $mins = "0" . $mins; }
if ( $secs < 10 ) { $secs = "0" . $secs; }
if ( $month < 10 ) { $month = "0" . $month; }
if ( $day < 10 ) { $day = "0" . $day; }
my $gTimeStamp = $year . $month . $day . ".$hour" . $mins . $secs;

my @gChangeArray;
my $gChangeList			= "";
my $gSourceBranch		= "";
my $gSourceBranchRoot	= "";

my $gAutoMode			= 0;
my $gForceOption		= "";
my @gSkippedArray;
my $gReviewCounter		= "";
my $gLastMergedChange	= 0;
my $gLatestChange2Merge	= 0;
my $gFoundChange		= 0;  # 1 = found match, 2 = within range but no match found
my $gReviewRange		= 250;

$gP4Root             = GetP4Root();
$gWinTemp            = TranslatePath($ENV{'TEMP'});
$gLogDir				= "$gWinTemp/logpile";
$gLogFile			= "$gLogDir/MergeChanges.log.$gTimeStamp.txt";
$gCmdFile			= "$gLogDir/MergeChanges.cmd.$gTimeStamp.bat";
$gCmdOutput			= "$gLogDir/MergeChanges.out.$gTimeStamp.txt";
$gTargRootDir 		= TranslatePath( GetProjectRoot() );
# $gdepot_view			= GetDepotView();
# my $gNotesFile			= "$gTargRootDir/merge_notes";

my $gNOEXEC	 			= 0;
my $gSubstitution		= 0; 
my $gSubstitutionText	= ""; 
my $gSubstStr1			= ""; 
my $gSubstStr2			= ""; 


# -----------------------------------------------------------
# parse arguments
# -----------------------------------------------------------
Parse_Args();

if ( $gAutoMode ) {
	open(COUNTER, "p4 counter $gReviewCounter |" );
	while ( <COUNTER> ) {
		$gLastMergedChange = $_;
	}
	close(COUNTER);
	chomp($gLastMergedChange);
	if ( $gLastMergedChange == 0 ) {
       	print "#\n### $gScriptName: ERROR: $gReviewCounter is not a valid review counter.\n";
       	HelpMessage(1);
	}
}

if ( ( ! $gAutoMode ) && ( $gChangeList eq "" ) ) {
	print "#\n### $gScriptName: ERROR: must specify a list of change #'s to integrate.\n";
	HelpMessage(1);
}

if ( $gSubstitution ) {
	if ($gSubstitutionText =~ /^%\/(.*)%\/(.*)%\/$/) {
		$gSubstStr1	= $1; 
		$gSubstStr2	= $2;
		print "### SubstStr1 = \"$gSubstStr1\"   SubstStr2 = \"$gSubstStr2\"\n";
	} else {
		print "#\n### $gScriptName: ERROR: illegal substitution string specified: \"$gSubstitutionText\"\n#\n";
		print "#   Substitution string must be of the form: %/str1%/str2%/\n";
		print "#   where \"str2\" will be used to replace \"str1\"\n";
		print "#   in the \"target\" path for all merge commands.\n";
		exit 4;
	}
}

if ($gSourceBranch eq "") {
	print "#\n### $gScriptName: ERROR: must specify a source branch *from* which the change(s) will be merged.\n";
	HelpMessage(1);
} elsif ($gSourceBranch eq "main") {
	$gSourceBranchRoot	= "//depot/xbox/";
} else {
	if ($gSourceBranch !~ /^\/\/depot\//) {
		open(BVIEW, "$gP4 branch -o $gSourceBranch 2>&1 |"); 
		my $line;
		while ($line = <BVIEW> ) {
			if ($line =~ /(.*)\/\/depot(.*)\/\/depot(.*)\/\.\.\./) {
				$gSourceBranchRoot = "//depot" . $3 . "/";
			}
		}
	} else {
		#gSourceBranchRoot was specified

		$gSourceBranchRoot = $gSourceBranch;
		chomp $gSourceBranchRoot;

		if ($gSourceBranchRoot !~ /\/$/) {
		  # If it doesn't end in a /, add one
		  $gSourceBranchRoot = $gSourceBranchRoot . "/";
		}
	}
}
		

if ($gSourceBranchRoot eq "//depot/") {
	print "#\n### $gScriptName: ERROR: illegal \"from\" branch specified, \"$gSourceBranch\".\n";
    HelpMessage(1);
} 

if ($gCurDir eq $gHome || $gCurDir eq $gP4Root) {
	print "#\n### $gScriptName: ERROR: can't figure out which sources you want to update, are you in the right directory?\n\n\n";
	exit(1);
} 

# -----------------------------------------------------------
# Constants
# -----------------------------------------------------------
#$kErrorPerl						= "0";
#$kNoErrorPerl					= "1";
#$kErrorUnix						= "1";
#$kNoErrorUnix					= "0";
$kForceLogPrint					= "1";
$kDebugLogPrint					= "2";
$gShowProgress					= "1";
$gScriptDebugging				= "0";

#--------------------------------------------------
# LogPrint - prints to stdout and to the log file
#--------------------------------------------------
sub LogPrint {

	my( $myMessage, $myPrintOption ) = @_;	# Should be this way, but it does not work correctly on Solaris this way.
	$myPrintOption .= "";
	
	if( defined( LOGFILE ) ) {
		print( LOGFILE "$myMessage" );
		if( "$gShowProgress" || "$gScriptDebugging" || ("$gScriptDebugging" && ("$myPrintOption" eq "$kDebugLogPrint")) || ("$myPrintOption" eq "$kForceLogPrint") ) {
			print( STDOUT "$myMessage" );
			}
		}
	else {
		print( STDOUT "$myMessage" );
		}
	}


#--------------------------------------------------
# Main - grand central
#--------------------------------------------------
my $rVal;

if ( ! -e $gLogDir ) { my $dosLogDir = MakeDosPath($gLogDir); PE_Execute("mkdir $dosLogDir"); }
open( LOGFILE, ">$gLogFile" );

LogPrint ("#\n# $gScriptName: starting...\n", $gShowProgress);

if ( $gAutoMode ) { GetChangeList(); }
LogPrint ("#\n# Parameters:\n", $gShowProgress);
LogPrint ("#             from branch: $gSourceBranchRoot\n", $gShowProgress);
LogPrint ("#               to branch: $gTargRootDir\n", $gShowProgress);

if ( $gAutoMode ) { 
	LogPrint ("#      last change merged: $gLastMergedChange\n", $gShowProgress);
	LogPrint ("#  latest change to merge: $gLatestChange2Merge\n", $gShowProgress);	
	LogPrint ("#        changes to merge: @gChangeArray\n", $gShowProgress);
	LogPrint ("#         changes to skip: @gSkippedArray\n", $gShowProgress);	
} else {
	LogPrint ("#             change list: $gChangeList\n", $gShowProgress);
}

# Generate the perforce commands to perform the merge...
open( CMDFILE, ">$gCmdFile" );
print CMDFILE "REM Batch File generated by MergeChanges\nREM\n\n";
$gTargRootDirDos = MakeDosPath($gTargRootDir);
print CMDFILE "cd $gTargRootDirDos\n";

GenMergeCmd();

close(CMDFILE);

# Now PE_Execute the perforce commands to perform the merge...
LogPrint ("#\n# Now merging all changes ...\n#\n", $gShowProgress);
LogPrint ("# Command file: $gCmdFile\n", $gShowProgress);
LogPrint ("#  Output file: $gCmdOutput\n", $gShowProgress);
if ( $gAutoMode ) { 
	LogPrint ("#\n# latest change to merge: $gLatestChange2Merge\n", $gShowProgress);	
}

# $dos_gNotesFile = MakeDosPath($gNotesFile);
# if ( -e $dos_gNotesFile ) {
#    PE_Execute("rm -f $dos_gNotesFile");
# }

# Nothing to do on DOS PE_Execute("chmod +x $gCmdFile");
#my $shell = "";
#if ( $gNT ) {
#	$shell = "$gHome/tools/gnuwin32/b20/H-i586-cygwin32/bin/bash";
#}
if ( $gNOEXEC ) {
	LogPrint ("#\n###########\n# WARNING #: Merge skipped due to \"-n\" option.\n###########\n", $gShowProgress); 
} else {
	PE_Execute("$gCmdFile > $gCmdOutput 2>&1");
	$rVal = CheckForErrors($gCmdOutput); 
	if ( $rVal == 1) {
		LogPrint ("\n\n#\n################################\n###### PROBLEMS FOUND !!! ######\n################################\n# Note the ERROR\(s\) above and look in $gCmdOutput for details.\n", $gShowProgress);
	} elsif ( $rVal == 2) {
		LogPrint ("\n\n#\n#\n# THERE WERE WARNINGS !!!\n#\n# Note the WARNING\(s\) above and look in $gCmdOutput for details.\n", $gShowProgress);
	}
}

# PE_Execute("rm -f $gNotesFile");

close (LOGFILE );

# If running in auto mode, create the notes file in preparation for checkin
#if ( $gAutoMode ) {
#	open(OLDNOTES, "$gTargRootDir/notes");
#	open(NEWNOTES, ">$gTargRootDir/notes.new");
#	while( <OLDNOTES> ) {
#		print NEWNOTES $_;
#		if ( /^Notes:/ ) {
#			print NEWNOTES "* Automated update from $gSourceBranch\n";
#			print NEWNOTES "   Merged changes: @gChangeArray\n" if ( @gChangeArray );
#			print NEWNOTES "  Skipped changes: @gSkippedArray\n" if ( @gSkippedArray );
#		}
#	}
#	close(OLDNOTES);
#	close(NEWNOTES);
#	unlink("$gTargRootDir/notes");
#	rename("$gTargRootDir/notes.new", "$gTargRootDir/notes");
#
#}

### done

#_____________________________
# Parse_Args
# parse arguments
#_____________________________
sub Parse_Args {
	while (@ARGV) {
		if ($ARGV[0] =~ /^-/) {
			if ($ARGV[0] =~ /^-c=(.*)/i)
				{ $gChangeList = $1;
				  @gChangeArray = split(":", $gChangeList); }
			elsif ($ARGV[0] =~ /^-c/i)
				{ shift @ARGV; $gChangeList = $ARGV[0];
				  @gChangeArray = split(":", $gChangeList); }
			elsif ($ARGV[0] =~ /^-auto=(.*)/i)
				{ $gAutoMode = 1; $gReviewCounter = $1; }
			elsif ($ARGV[0] =~ /^-auto/i)
				{ $gAutoMode = 1; shift @ARGV; $gReviewCounter = $ARGV[0]; }
			elsif ($ARGV[0] =~ /^-force/i)
				{ $gForceOption = " -f "; }
			elsif ($ARGV[0] =~ /^-from=(.*)/i)
				{ $gSourceBranch = $1; }
			elsif ($ARGV[0] =~ /^-from/i)
				{ shift @ARGV; $gSourceBranch = $ARGV[0]; }
			elsif ($ARGV[0] =~ /^-latest=(.*)/i)
				{ $gLatestChange2Merge = $1; }
			elsif ($ARGV[0] =~ /^-latest/i)
				{ shift @ARGV; $gLatestChange2Merge = $ARGV[0]; }
			elsif ($ARGV[0] =~ /^-high=(.*)/i)
				{ $gLatestChange2Merge = $1; }
			elsif ($ARGV[0] =~ /^-high/i)
				{ shift @ARGV; $gLatestChange2Merge = $ARGV[0]; }
			elsif ($ARGV[0] =~ /^-subst=(.*)/i)
				{ $gSubstitutionText = $1; $gSubstitution = 1; }
			elsif ($ARGV[0] =~ /^-subst/i)
				{ shift @ARGV; $gSubstitutionText = $ARGV[0]; $gSubstitution = 1; }
			elsif ($ARGV[0] =~ /^-n/i)
				{ $gNOEXEC = 1; }
			elsif ($ARGV[0] =~ /^-help/i)
				{ HelpMessage(0); }
			else
				{ print "#\n### $gScriptName: ERROR: Illegal option: $ARGV[0]\n"; HelpMessage(1); }
		} else {
			print "#\n### $gScriptName: ERROR: unexpected argument, \"$ARGV[0]\".\n";
			HelpMessage(1);
		}
		shift @ARGV;
	}
}

#--------------------------------------------------
# GetChangeList - get list of changes to be merged in auto mode ... 
#--------------------------------------------------
sub GetChangeList {
	my ( $line, $change, @merge, @skip );

	return if (!$gAutoMode);

	open(CHANGELIST, "$gP4 changes -m $gReviewRange $gSourceBranchRoot... |"); 
	while ($line = <CHANGELIST> ) {
		if ($line =~ /^Change ([0-9]+) on (.*)/) {
			$change = $1;
			# assign the very first value of $change to $gLatestChange2Merge 
			$gLatestChange2Merge ||= $change;
			if ( $change == $gLastMergedChange ) {
				$gFoundChange = 1;
				last;
			} elsif ( $change > $gLastMergedChange ) {
				if ( ValidChange($change) ) {
					push(@merge, $change) unless ( $change > $gLatestChange2Merge );
				} else {
					push(@skip, $change) unless ( $change > $gLatestChange2Merge );
				}
			} else {  # all remaining changes have already been evaluated
				$gFoundChange = 2;
				last;
			}
		}
	}

	@gChangeArray = sort { $a <=> $b } @merge;
	@gSkippedArray = sort { $a <=> $b } @skip;
}


#--------------------------------------------------
# ValidChange - should this change be merged ?
#--------------------------------------------------
sub ValidChange {
	my $change = shift;
	my $line;
	my $valid = 1;   # assume the change is valid until proven otherwise

	open(CHANGEDESC, "$gP4 describe -s $change |");
	while (defined ($line = <CHANGEDESC> ) && ($line) && ( $valid ) ) {
		if ($line =~ m/DO(\s)*NOT(\s)*MERGE/i) {
			$valid = 0;
		}
	}

	return ($valid);
}	


#--------------------------------------------------
# GenMergeCmd - go through each change... 
#--------------------------------------------------
sub GenMergeCmd {
	my ( $changeNum, $noChanges, $line, $fileName, $fileName2, $fileDir, $revNum, $chgState );

	chdir($gTargRootDir);

#	print CMDFILE "\n# Beginning, generate modern notes file first, and add to it below.....\n";
#	print CMDFILE "perl $gHome/PE/common/changes.common -noedit //depot/missing/project/...\n";
#	print CMDFILE "sed -e '/^#[ ]*end[.0-9]/d' $gTargRootDir/notes > $gTargRootDir/notes.new;\n";
#	print CMDFILE "mv $gTargRootDir/notes.new $gTargRootDir/notes \n\n";

	foreach $changeNum (@gChangeArray) {
		LogPrint ("#\n# Generating merge commands for change: $changeNum\n", $gShowProgress);
		print CMDFILE "\nREM ######### Change $changeNum #########\n";
#       print CMDFILE "echo \"* Merged from \" >> $gNotesFile\n";
#		print CMDFILE "$gP4 describe -s $changeNum >> $gNotesFile\n";
#		print CMDFILE "echo \"######### Change $changeNum #########\"\n";
		
		$noChanges = 1;

		open(BLIST, "$gP4 describe -s $changeNum 2>&1 |"); 
		while ($line = <BLIST> ) {
			if ($line =~ /^... $gSourceBranchRoot(.*)#([0-9]+)( *)(.*)/) {
				$fileName = $1;
				$revNum = $2;
				$chgState = $4;
				$noChanges = 0;
				$fileName2 = $fileName ;
				$fileName2 =~ s/\Q$gSubstStr1\E/$gSubstStr2/ if ( $gSubstitution ) ;
				if ( $chgState eq "delete" ) {
					print CMDFILE "$gP4 -d $gTargRootDir delete \"$fileName2\"\n";
				} else {
					if ($fileName2 =~ /(.*)\/(.*)$/) {
						$fileDir = $1;
						if ( ! -e $fileDir ) {
							print CMDFILE "mkdir -p $fileDir\n";
						}
					}
					print CMDFILE "$gP4 -d $gTargRootDir integrate $gForceOption \"$gSourceBranchRoot$fileName\"#$revNum,$revNum \"$fileName2\"\n";
					if ( -e $fileName2 ) {
						if ( IsTextFile($fileName) ) {
							print CMDFILE "$gP4 -d $gTargRootDir resolve -af \"$fileName2\"\n";
						}
						else {
							print CMDFILE "$gP4 -d $gTargRootDir resolve -at \"$fileName2\"\n";
						}
					}
				}
			}
		}
		close(BLIST);
		if ($noChanges > 0) {
			print CMDFILE "echo \"WARNING - No files integrated from change $changeNum\"\n";
		}
	}
#	print CMDFILE "\n##### Append merge notes to \"$gTargRootDir/notes\" file ... #####\nsed -e '/Affected files .../d' -e '/... \\/\\/depot/d' -e '/#[ ]*end[.0-9]/d' -e 's/^/\t> /' $gNotesFile >> $gTargRootDir/notes\n";

	chdir($gCurDir);
}


#--------------------------------------------------
# IsTextFile - returns 1 if file is of type *text, 0 otherwise...
#--------------------------------------------------
sub IsTextFile {
	my $fullpathname = shift;
	my $returnValue  = 0;
	my $line;

	open(POUT, "$gP4 -d $gTargRootDir files $fullpathname 2>&1 |"); 
	while ($line = <POUT> ) {
		if ($line =~ /(.*)text\)$/) {
			$returnValue = 1;
		}
	}	
	close(POUT);
	return( $returnValue );
}


#--------------------------------------------------
# CheckForErrors - returns 1 if there are merge problems, 
#					       2 if there is a merge warning,
#						   0 if no problems found.
#--------------------------------------------------
sub CheckForErrors {
	my $outputfile  = shift;
	my $returnValue = 0;
	my ($line, $previousline, $curChg );

	open(OUTFILE, "$outputfile") || die "Cannot open $outputfile: $!";
	while ($line = <OUTFILE> ) {
		if ($line =~ /######### Change ([0-9]+) #########/) {
			$curChg = $1;
		} elsif ($line =~ /(.*) - all revision\(s\) already integrated\./) {
			LogPrint ("\nWARNING - already integrated. (from change $curChg)\n", $gShowProgress);
			LogPrint ($line, $gShowProgress);
			if ( $returnValue == 0) { 
				$returnValue = 2;
			}
		} elsif ($line =~ /^WARNING - No files integrated from change ([0-9]+)/) {
			LogPrint ("\n$line", $gShowProgress);
			if ( $returnValue == 0) { 
				$returnValue = 2;
			}
		} elsif ($line =~ /(.*) - resolve skipped\./) {
			LogPrint ("\nERROR - could not resolve. (from change $curChg)\n", $gShowProgress);
			LogPrint ($line, $gShowProgress);
			$returnValue = 1;
		} elsif ($line =~ /(.*) - can't branch \(already opened for branch\)/) {
			LogPrint ("\nERROR - must commit previous revision first. (from change $curChg)\n", $gShowProgress);
			LogPrint ($line, $gShowProgress);
			$returnValue = 1;
		} elsif ($line =~ /(.*) - no permission for operation on file\(s\)\./) {
			LogPrint ("\nERROR - No automerging allowed on this file.\n", $gShowProgress);
			LogPrint ($line, $gShowProgress);
			$returnValue = 1;
		} elsif ($line =~ /(.*) ([0-9]+) conflicting$/) {
			if ( $2 > 0 ) {	
				LogPrint ("\nERROR - merge conflicts found. (from change $curChg)\n", $gShowProgress);
				LogPrint ($previousline, $gShowProgress);
				LogPrint ($line, $gShowProgress);
				if ($previousline =~ /(.*) - (.*)/) {
					PE_Execute("$gP4 edit \"$1\"");
				}
				$returnValue = 1;
			}
		}
		$previousline = $line;
	}	
	close(OUTFILE);
	return( $returnValue );
}


#--------------------------------------------------
# HelpMessage - prints the legal options - help info
#--------------------------------------------------
sub HelpMessage {
    my $exitCode = shift;

	print "Usage:\n";
	print "       $gScriptName -c chg1[:chg2:chg3:...] -from <branch>|main";
	print " [-subst %/string1%/string2%/]\n";
	print "\n           OR\n\n";
	print "       $gScriptName -auto <review counter> -from <depotpath>|<branch>|main [-latest <change #>]\n";
	print "\n\n";
	print "Examples: \n";
	print "    $gScriptName -c 12609:12634 -from xbox-may01\n\n# Integrates change #'s: 12609 and 12634 from the 'xbox-may01' source tree\n  into the \"current\" source tree.  The \"current\" source tree is defined as\n  the source tree from which you are executing the MergeChanges script.\n";
	print "\n\n";
	print "    $gScriptName -c 112178 -from //depot/foo\n\n# Integrates change #'s 112178 from //depot/foo into the \"current\" source tree.\n";
	print "\n\n";
	print "    $gScriptName -auto last_sdk_to_main -from sdk\n\n# Integrates all changes that have been made in the \"sdk\" branch \n  since the change specified by review counter, \"last_sdk_to_main\",\n  into the \"current\" source tree.  The \"current\" source tree is defined as\n  the source tree from which you are executing the MergeChanges script.\n";
	print "\n\n";
	print "    $gScriptName -auto last_sdk_to_main -from sdk -latest 1234\n\n# Same as above with the additional restriction that only changes\n  up to and including the change # specified by the -latest option\n  will be integrated.\n";
	print "\n\n";
	print "### See http://peweb/systems/Perforce/mergechanges.asp for a more detailed explanation.";

    exit $exitCode;
}

__END__
:endofperl
