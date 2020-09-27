#++
# Benjamin Vierck (benv)
# Copyright (c) 1997 Microsoft Corporation
#
# Module Name:
#	crcat5.pl
#
# Abstract:
#
#   Creates the source file with full binary list from dosnet.inf (srv & wks)
#   then inserts the file list in correct format to the CatFile
#
#--

#
# Set global vars
#
%Parameters = &GetArguments(@ARGV);
if($Parameters{"d"}) { $DEBUG=TRUE; }
if($Parameters{"p"}) { $PWLESS=TRUE; }
$RegDLLsCmd = "c:\\bldtools\\build\\regcryp.cmd";
$SignCATCmd = "c:\\bldtools\\build\\ntsign.cmd";
$CatFile = "c:\\bldtools\\build\\nt5cat";
$Binaries = $ENV{"BINARIES"};
$sigCat = "$Binaries\\nt5.cat";
$WDNFile = "$Binaries\\dosnet.inf";
$SDNFile = "$Binaries\\srvinf\\dosnet.inf";
$ThisScript = "crcat5";
$log=$ENV{"TMP"}."\\$ThisScript.log";
@DNFiles = ( "$WDNFile","$SDNFile" );

#
# Fork if necessary - error/argument checking
if($Parameters{"?"}) {&Usage;}
if( !(-e $WDNFile) || !(-e $SDNFile) ) { &LogMsg('err',"Invalid DOSNET file(s)"); }

#
# Extract filenames; compile list of non-overlapping files
#
&LogMsg('info',"Compiling file list from DOSNET files");
foreach $DNFile(@DNFiles) {
  # Send DOSNET INF to an array
  open(DNF,$DNFile);
  @DNArray = <DNF>;
  close(<DNF>);

  &LogMsg('dbg', "Parsing out $DNFile");
  if(!$PWLESS) {
    foreach $DNentry(@DNArray) {
      $DNentry=~/d\d,(\w+)(\W)(\w+)/;
      $token=$1;
      if($2 eq ".") { $token.=".$3"; }
      if($token && join(".",@list)=~/$token/ && $DEBUG) { push(@rejects,$token); }
      if( ($token) && !(join(".",@list)=~/$token/) ) { push(@list,$token); }
      }
    }
  }

#
# Init, Fill CatFile, End with info for user
#
&LogMsg('info',"Generating catalog file now...");
if(-e $CatFile) {
  &SysCmd("attrib -r $CatFile");
  &SysCmd("del $CatFile");
  }

@header=&GetHeader;
open(HFIL,">$CatFile");
foreach(@header) { print HFIL $_; }
close(HFIL);

if(!$PWLESS) {
  foreach(@list) {
    undef($file);
    s/(\s+)//g;
    if(-e "$Binaries\\$_") { $file = "$Binaries\\$_"; }
    else {
#      @FindFile = `dir /b /s $_ 2>nul`;
      @FindFile = `dir /b /s $_ `;
      if($FindFile[0]) { $file = $FindFile[0]; }
      }
    $file=~s/(\s+)//g;
    if(!$file) { $file = "NA"; }
    else { system("echo $_=$file>>$CatFile"); }
    }
  }
&LogMsg('info',"$#list total files in dosnet.inf files");
&LogMsg('dbg',"$#rejects files rejected");

# cdf -> cat with makecat.exe
#&LogMsg('info',"Converting CDF file into Catalog format...");
#&SysCmd($RegDLLsCmd);
#chdir($Binaries);
#&SysCmd("binaries\\makecat.exe $CatFile >nul 2>&1");

# sign cat file with signcode.exe
#&LogMsg('info',"Signing catalog file...");
#chomp($result = `$SignCATCmd $sigCat 2>&1`);
#&LogMsg('dbg',"res = ($result)");

#
# Sub(routine)s below
#

# SysCMD for debugging & powerless
sub SysCmd {
  my($cmd) = @_;
  &LogMsg('dbg',"($cmd)");
  if(!$PWLESS) { system($cmd); }
  return;
  }

# Parse arguments passed to bincomp into %Parameters
sub GetArguments {
  my(@ArgumentList)=@_;
  my(%Parameters,$flag,$value,$argument);
  foreach $argument(@ArgumentList) {
    if(substr(lc($argument),0,1) ne "-") {next;}
    $flag=substr($argument,1,1);
    if(substr($argument,2,1) eq ":") {
      $Parameters{$flag} = substr($argument,3);
      }
    else { $Parameters{$flag}="TRUE";}
    }
  return %Parameters;
  }

# Create Catalog header
sub GetHeader {
  my($HeadA);
  @HeadA = <<EOT;
[CatalogHeader]
Name=nt5
PublicVersion=0x0000001
EncodingType=0x00010001
CATATTR1=0x10010001:OSAttr:2:5.0

[CatalogFiles]
EOT
  return @HeadA;
  }

# Print Usage info
sub Usage {
print <<EOT;
The script $ThisScript is a utility which creates the full list of binaries
for cataloging & signing.
Usage Information:
$ThisScript -d -?
	? = HELP mode, show Usage information
	d = Debug mode, shows script debugging information
	p = Powerless mode, no system commands are executed
EOT
exit;
  }

# Output formatting/logging
sub LogMsg {
  my($type,$msg) = @_;
  my($fmsg);

  $fmsg = $ThisScript."[".$type."]: ".$msg;

  if( ($type ne "dbg") || $DEBUG ) { system("echo $fmsg>>$log"); }
  if( ( ($type ne "log") && ($type ne "dbg") ) || ( $DEBUG ) ) { print $fmsg."\n"; }
  if($type eq "err") { exit; }

  return;
  }
