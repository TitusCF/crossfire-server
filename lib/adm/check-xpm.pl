#!/usr/local/bin/perl
#
# This checks to make sure the esrv_xpm.eric file is up to date, and
# also checks its consistency.
#
open(BMAPS,"bmaps.paths") || die("nobmaps\n");
open(ESRV,"esrv_xpm.eric") || die("noesrv\n");
$_ = <BMAPS>;
while(<ESRV>) {
    next unless /^ESRV_XPM (\d+) (.+)$/o;
    $num = $1;
    $place = $2;
    chop($_ = <BMAPS>);
    die("Bad line '$_'\n") unless /^\\(\d+)\s+(.+)$/o;
    $xnum = $1;
    $xplace = $2;
    die("($num,$place) != ($xnum,$xplace)\n") 
	unless $num eq $xnum && $place eq $xplace;
}
$_ = <BMAPS>;
die("Too much on bmaps '$_'\n") unless /^$/o;

