# read args

if (scalar(@ARGV) < 4 || scalar(@ARGV) > 6) {
	print <<EOF;
Usage: scriptevent.pl segment script routine time [offset] [flags]
   segment: filename of segment to insert the event into
   script:  filename of script
   routine: name of routine to call
   time:    music time to make call
   flags:   2 for queue time, 4 for at time
EOF

	exit;
}

$segment = shift;
$script = shift;
$routine = shift;
$time = shift;
$offset = shift;
$offset = 0 if !defined $offset;
$flags = shift;
$flags = 0 if !defined $flags;

# get the segment in text (rifftool) form

$segtext = `%d%\\tools\\riffto~1\\rifftool -e $segment`;

# make one that has a script event track

if ($segtext =~ /LISTscrt/) {
	# we already have a script event track
	$segwithtrack = $segtext;
}
else {
	# add one
	foreach $_ (split("\n", $segtext)) {
		$segwithtrack .= "$_\n";
		if (/LISTtrkl/) {
			$segwithtrack .= <<EOF;
  \$D'RIFFDMTK'
   \$D'trkh'
   4108FA85 3586 11d3 8B D7 00 60 08 93 B1 B6          // Script track GUID
   00 00 00 00 01 00 00 00 00 00 00 00 \"scrt\"
   \$A
   \$D'LISTscrt'                                        // Track info
    \$D'LISTscrl'                                       // List of events
    \$A
   \$A
  \$A
EOF
		}
	}
}

# add the script event and write back to the segment
open SAVE, "| %d%\\tools\\riffto~1\\rifftool -i $segment";

foreach $_ (split("\n", $segwithtrack)) {
	print SAVE "$_\n";
	if (/LISTscrl/) {
		$flags = sprintf "%08x", ($flags);
		$ltime = sprintf "%08x", ($time);
		$ptime = sprintf "%08x", ($time + $offset);

		$widescript = "0000";
		while ($c = chop($script)) {
			$widescript = "\"$c\" 00 " . $widescript;
		}

		$wideroutine = "0000";
		while ($c = chop($routine)) {
			$wideroutine = "\"$c\" 00 " . $wideroutine;
		}

		print SAVE <<EOF;
     \$D'LISTscre'                                      // script event
      \$D'scrh'
      $flags                                         // flag for At time
      $ltime                                         // logical time
      $ptime                                         // physical time
      \$A
      \$D'LISTDMRF'
       \$D'refh'
       810B5013 E88D 11d2 8B C1 00 60 08 93 B1 B6          // CLSID_DirectMusicScript
       16 00 00 00                                         // valid data
       \$A
       \$D'file'
       $widescript // Script filename
       \$A
      \$A
      \$D'scrn'
      $wideroutine // Script routine name
      \$A
     \$A
EOF
	}
}
