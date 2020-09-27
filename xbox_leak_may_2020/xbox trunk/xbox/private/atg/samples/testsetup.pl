#
# Performs setup test
# 1) Find the latest build, and point PDrive env var to it
# 2) Check out \private\setup\xdk\eng\xdk.ini
# 3) Replace the XBOXBINSRETAIL line with razzle binplace directory
# 4) Run the packer in test mode
# 5) Revert xdk.ini
# 

# Go to the setup directory
my $setupdir = $ENV{"_NTDRIVE"}.$ENV{"_NTROOT"}."\\private\\setup\\xdk";
chdir $setupdir;

# Find the latest build and point PDrive at it
open INFILE, "<\\\\xbuilds\\release\\usa\\latest.txt";
my $version = <INFILE>;
close INFILE;
($version) = split / /,$version;
chomp $version;
print "Latest version: <$version>\n";
$ENV{"PDrive"} = "\\\\xbuilds\\release\\usa\\$version";
print "Using PDrive=".$ENV{"PDrive"}."\n";
$ENV{"_SETUP_FILE_PATH"} = $ENV{"_NT386TREE"}."\\dump";
$ENV{"_SETUP_TARGET_PATH"} = $ENV{"_NT386TREE"}."\\dump";
$ENV{"_BuildVer"} = "Verify";

# Edit the english ini file to point to our build
# This is to pick up generated VS Project files and
# DSP Image files
my @output = `sd edit xdk.ini`;
print @output;
open INFILE, "<xdk.ini" or die "Can't read xdk.ini\n";
my @lines = <INFILE>;
close INFILE;
open OUTFILE, ">xdk.ini" or die "Can't write xdk.ini\n";
foreach(@lines)
{
#    s/UnpackerTemplate=\%Pdrive\%\\free\\dump\\unpacker.exe/UnpackerTemplate=$ENV{"_NT386TREE"}\\dump\\unpacker.exe/g;
#    s/UnpackerNewExe=\%Pdrive\%\\XDKSetup\%_BuildVer\%.exe/UnpackerNewExe=$ENV{"_NT386TREE"}\\dump\\XDKSetup.exe/g;
    s/XBOXBINSRETAIL=\%Pdrive\%\\free/XBOXBINSRETAIL=$ENV{"_NT386TREE"}/g;
    print OUTFILE $_;
}
close OUTFILE;

# Run the packer in test mode
my @packer = `$ENV{"_NT386TREE"}\\dump\\xpacker -t xdk.ini`;

# Check for any line containing ATG, which likely means you messed up
my $missedfiles = "";
foreach (@packer)
{
    if (/atg\\samples/)
    {
        $missedfiles = $missedfiles."**> $_\n";
    }
}

# Revert the ini file
@output = `sd revert xdk.ini`;
print @output;

# Display output from the packer
print "\n***\nPacker output:\n@packer";

# Print out anything with atg\samples in it
print "\n\n$missedfiles";
