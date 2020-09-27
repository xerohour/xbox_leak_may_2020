my %FileHash;
my @foo = `sd opened -l ...`;

foreach (@foo)
{
    my ($name, $rest) = split /#/;
    my ($version, $rest2) = split / - /,$rest;
    my ($status) = split / /,$rest2;

    $FileHash{lc($name)} = [ (lc($name), $version, $status) ];
}

for $file (keys(%FileHash))
{
#    print "File: $file\n\tVersion: $FileHash{$file}[1]\tStatus: $FileHash{$file}[2]\n";
}

my @diroutput = `dir /a-r-d /s /b`;
my @dirfiles;
foreach (@diroutput)
{
    next if /\\Debug\\/i;
    next if /\\Release\\/i;
    next if /\\obj\\/i;
    next if /\.dsp$/i;
    next if /\.dsw$/i;
    next if /\.ncb$/i;
    next if /\.opt$/i;
    next if /\.plg$/i;
    next if /\.tmp$/i;
    next if /\.cod$/i;
    next if /\\build\.log$/i;
    next if /\\build\.err$/i;
    next if /\\build\.wrn$/i;
    next if /\\Resource\.err$/i;
    chomp;
    push (@dirfiles, lc($_));
}
print "Difference:\n";

foreach (@dirfiles)
{
    next if $FileHash{$_};
    print "File: $_ not in SD\n";
}