#!/usr/local/bin/perl

open(BMAPS,"bmaps.paths") || die("Can't open bmaps.paths: $!\n");
open(ESRV,">crossfire.xpm") || die("Can't open crossfire.xpm for write: $!\n");
$_ = <BMAPS>;
while(<BMAPS>) {
    chop;
    die("Unknown line: '$_'\n") unless /^\\(\d{5})\s+(\S+)$/o;
    $num = $1;
    $file = $2;
    print "$num $file\n" if ($num % 50) == 0 ;
    open(FILE,"$file\.xpm") || die("Unable to open $file\.xpm for read: $!\n");
    print ESRV "ESRV_XPM $num $file\n";
    while(<FILE>) {
	print ESRV $_;
    }
    close(FILE);
    print ESRV "ESRV_XPM_END\n";
}
close(ESRV);
close(BMAPS);
