#!/usr/bin/perl

require "util.pl";

$root = $ARGV[0];
$archetypes = "archetypes";

### main
&info ("examining $archetypes ...");

open (ARCH,"< $archetypes") || &die ("cannot open $archetypes");
&checkarch;
close (ARCH);

exit 0;


sub checkarch {
	$warnings = 0;
	$more = 0;
line:	while(<ARCH>) {
	    chop;
	    ($var,@values) = split;
	    if ($var eq "More") {
		$more = 1;
		next line;
	    }
	    if ($var eq "Object") {
		$arch = $values[0];
		$is_alive = 0;
		$level = 0;
		$is_not_head = $more;
		$more = 0;
		next line;
	    }
	    $more = 0;
	    if ($var eq "end") {
		if ( ! $is_not_head && $is_alive && $level <= 0) {
		    &warn ("arch $arch is alive, but doesn't have level");
		    $warnings++;
		}
		next line;
	    }
	    if ($var eq "alive") {
		$is_alive = $values[0];
		next line;
	    }
	    if ($var eq "level") {
		$level = $values[0];
		next line;
	    }
    }
    &info ("$warnings problems found");
}
