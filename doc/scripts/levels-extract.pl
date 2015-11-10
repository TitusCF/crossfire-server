#!/usr/bin/env perl

# Read the experience table for the server,
# then output the first ten levels
# for either html or LaTex.
#
# Author: Daniel Hawkins

use strict;

# Ensure proper use of arguments
if ($#ARGV lt 2){
	print "Usage: levels-extract.pl [exp_table_path] [html|latex] [output_file]\n";
	die("Invalid number of arguments provided.\n");
}

my $exp_path = $ARGV[0];
my $output_type = $ARGV[1];
my $out_file = $ARGV[2];
my @exp_array;

open(EXP_FILE, "< $exp_path") or die("Failed to load experience table file at $exp_path");
open(OUT_FILE, "> $out_file") or die("Failed to open output file for experience table at $out_file");
while (<EXP_FILE>){
	# This would normally go in the while statement, but it would reset $_.
	if ($#exp_array gt 10){
		last;
	}
	#Ignore comment and blank lines, as well as max level definition
	next if (/^#/ || /^\s*$/ || /^max_level/);
	# We have some levels, split them up
	my @array = split(/,\s*/, $_);
	# Still only acquire the first ten levels. Technically, we get 11, but 0 is not needed
	for (my $cur = 0; $cur <= $#array and $#exp_array <= 10; ++$cur){
		# Remove whitespace.
		chomp($array[$cur]);
		# Shove the value onto the experience array.
		push(@exp_array, $array[$cur]);
	}
}
close EXP_FILE;

my $out_at = 1;
# Print each of the results -- only output in one format.
if ($output_type eq "html"){
	for my $elem (@exp_array){
		print OUT_FILE "<tr><td align=center>".$out_at++."</td><td align=center>$elem</td></tr>\n";
	}
}
elsif ($output_type == "latex"){
	for my $elem (@exp_array){
		print OUT_FILE $out_at++." & $elem \\\\ \n";
	}
}
close OUT_FILE;
