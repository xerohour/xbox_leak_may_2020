# Sample name and title ID
my $name;
my $titleid;

# Set name and title ID
$name = shift;
$titleid = shift;

#
# Create directory structure and copy template files
#
mkdir $name,0;
chdir $name;
#TODO use environment for path
my $tmppath = $ENV{"_NTDRIVE"}.$ENV{"_NTROOT"}."\\private\\atg\\samples\\template\\";
@args = ("xcopy", "/s", $tmppath."*.*"); 
system(@args);

#
# build a date string
#
my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = localtime(time);
my $date = ($mon + 1).".".($mday).".";
if( ($year-100) < 10 )
{
    $date = $date."0".($year-100);
}
else
{
    $date = $date.($year-100);
}

print "Name = <$name>\nTitleID = <$titleid>\nDate = <$date>\n";

#
# Replace our placeholder strings
# %%NAME%% is the name of the sample
# %%TITLEID%% is the title ID of the sample
# %%DATE%% is today's date
#
sub DoReplace
{
    my($infile, $outfile) = @_;
    open INFILE, "<$infile" or die "Can't open $infile\n";
    my @lines = <INFILE>;
    close INFILE;

    open OUTFILE, ">$outfile" or die "Can't open $outfile\n";

    foreach(@lines)
    {
        s/%%NAME%%/$name/g;
        s/%%TITLEID%%/$titleid/g;
        s/%%DATE%%/$date/g;
        print OUTFILE $_;
    }
    close OUTFILE;
}
        
#
# Replace our placeholder strings
#
DoReplace("sources.tmp", "sources");
DoReplace("readme.tmp", "readme.txt");
DoReplace("template.dsx", "$name.dsx");
DoReplace("template.cpp", "$name.cpp");

#
# delete template files
#
unlink "sources.tmp";
unlink "readme.tmp";
unlink "template.dsx";
unlink "template.cpp";

#
# sd add all the files
#
@args = ("sd", "add", "*");
system(@args);
chdir "media";
system(@args);
chdir "fonts";
system(@args);
chdir "..\\textures";
system(@args);

