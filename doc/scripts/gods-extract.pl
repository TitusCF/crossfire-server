#!/usr/bin/env perl

# Script to extract the god information from the server.
#
# Outputs the results as either html or LaTex.
#
# Author: Daniel Hawkins

use strict;

if ($#ARGV lt 2){
	print "Usage: gods-extract.pl [server command] [html|latex] [output file]\n";
	die("Invalid number of arguments provided");
}

# Make meaningful variables for argv[1] and argv[2]
my $output_type = $ARGV[1];
my $file = $ARGV[2];

# We have a command to run, so run it.
my $output = `$ARGV[0] 2>&1`;
# Separate the lines
my @lines = split(/\n/, $output);

# Make sure we can open the file before we begin parsing.
open(OUTFILE, "> $file") or die("Could not open $file for output");

# set up the collections and other variables
my @gift, my @name, my @enemy, my @aligned, my @erace, my @atype, my $god = 0,
	my @immune, my @prot, my @vuln, my @attuned, my @repelled, my @denied, my @desc,
	my @tmp_immune, my @tmp_prot, my @tmp_vuln, my @tmp_gifts, my $need_priest_gifts = 0;
# Parse each line
for (@lines){
	# If we are in the priest gifts, then try to parse those first
	if ($need_priest_gifts eq 1){
		unless (/^\s*$/){ # Next empty line indicates end of gifts
			chomp;
			push(@tmp_gifts, $_);
		}
		else{
			# Clean up temporary information for god.
			$need_priest_gifts = 0;
			&finish_god;
		}
	}
	next if (/^\s*$/); # Skip empty lines
	if (/GOD:\s*(\w+)/){
		++$god;
		$gift[$god] = 0;
		$name[$god] = ucfirst $1;
	}
	next if ($god == 0); # Don't parse noise at the beginning of the output.
	if (/enemy:\s*(\w+)/){
		$enemy[$god] = ucfirst $1;
	}
	elsif (/aligned_race\(s\):\s*(\w+(,\s*\w+)*)/){
		my $fixed = $1;
		$fixed =~ s/_/ /g;
		$aligned[$god] = ucfirst $fixed;
	}
	elsif (/enemy_race\(s\):\s*(\w+(,\s*\w+)*)/){
		my $fixed = $1;
		$fixed =~ s/_/ /g;
		$erace[$god] = ucfirst $fixed;
	}
	elsif (/Attacks:\s*(\w+(\s\w+)*(,\s*\w+(\s\w+)*)*)/){
		$atype[$god] = ucfirst $1;
	}
	elsif (/resist\s*([A-z]+(\s[A-z]+)*\s*(-?\d{1,3}))/){
		if ($3 lt "0"){
			push(@tmp_vuln, $1);
		}
		elsif ($3 lt "100"){
			push(@tmp_prot, $1);
		}
		elsif ($3 eq "100"){
			push(@tmp_immune, $1);
		}
	}
	elsif (/Attuned:\s*(\w+(,\s*\w+)*)/){
		$attuned[$god] = ucfirst $1;
	}
	elsif (/Repelled:\s*(\w+(,\s*\w+)*)/){
		$repelled[$god] = ucfirst $1;
	}
	elsif (/Denied:\s*(\w+(,\s*\w+)*)/){
		$denied[$god] = ucfirst $1;
	}
	# Its just easier to take the dscription verbatim
	elsif (/Desc:\s*(.+)$/){
		my @parts = split(",", $1);
		$desc[$god] = ucfirst $parts[0];
	}
	elsif (/Priest gifts\/limitations:\s*(NONE)?/){
		if ($1 eq "NONE"){
			&finish_god;
		}
		else{
			$need_priest_gifts = 1;
		}
	}
}

# Okay, we have the information, now output the results
if ($output_type eq "html"){
	for (my $i = 1; $i <= $god; ++$i){
		print OUTFILE "<table border=1 cellpadding=3 width=100%><tr><th width=20%>$name[$i]</th><td>$desc[$i]</td></tr>\n";
		print OUTFILE "<tr><td>Enemy cult:</td><td>$enemy[$i]</td></tr>\n";
		print OUTFILE "<tr><td>Aligned race(s):</td><td>$aligned[$i]</td></tr>\n";
		print OUTFILE "<tr><td>Enemy race(s):</td><td>$erace[$i]</td></tr>\n";
		print OUTFILE "<tr><td>Attacktype(s):</td><td>$atype[$i]</td></tr>\n";
		print OUTFILE "<tr><td>Immunity:</td><td>$immune[$i]</td></tr>\n" if ($immune[$i]);
		print OUTFILE "<tr><td>Protected:</td><td>$prot[$i]</td></tr>\n" if ($prot[$i]);
		print OUTFILE "<tr><td>Vulnerable:</td><td>$vuln[$i]</td></tr>\n" if ($vuln[$i]);
		print OUTFILE "<tr><td>Attuned:</td><td>$attuned[$i]</td></tr>\n" if ($attuned[$i]);
		print OUTFILE "<tr><td>Repelled:</td><td>$repelled[$i]</td></tr>\n" if ($repelled[$i]);
		print OUTFILE "<tr><td>Denied:</td><td>$denied[$i]</td></tr>\n" if ($denied[$i]);
		print OUTFILE "<tr><td>Added gifts/limits:</td><td>$gift[$i]</td></tr>\n" if ($gift[$i]);
		print OUTFILE "</table><br />\n";
	}
}
elsif($output_type eq "latex"){
	for (my $i = 1; $i <= $god; ++$i){
		print OUTFILE "\{\\bf $name[$i] \} & $desc[$i] \\\\\\hline\n";
		print OUTFILE "Enemy cult: & $enemy[$i] \\\\ \n";
		print OUTFILE "Aligned race(s): & ".break_in_threes($aligned[$i])." \\\\ \n";
		print OUTFILE "Enemy race(s): & ".break_in_threes($erace[$i])." \\\\ \n";
		print OUTFILE "Attacktype(s): & ".break_in_threes($atype[$i])." \\\\ \n";
		print OUTFILE "Immunity: & ".break_in_threes($immune[$i])." \\\\ \n" if ($immune[$i]);
		print OUTFILE "Protected: & ".break_in_threes($prot[$i])." \\\\ \n" if ($prot[$i]);
		print OUTFILE "Vulnerable: & ".break_in_threes($vuln[$i])." \\\\ \n" if ($vuln[$i]);
		print OUTFILE "Attuned: & ".break_in_threes($attuned[$i])." \\\\ \n" if ($attuned[$i]);
		print OUTFILE "Repelled: & ".break_in_threes($repelled[$i])." \\\\ \n" if ($repelled[$i]);
		print OUTFILE "Denied: & ".break_in_threes($denied[$i])." \\\\ \n" if ($denied[$i]);
		# Print each gift on its own line
		if ($gift[$i]){
			$gift[$i] =~ s/,/ \\\\ \n &/g;
			print OUTFILE "Added gifts/limits: & \\\\ \n & $gift[$i] \\\\ \n";
		}
		print OUTFILE "\\hline \n \\sngc\{ \} \\\\ \n \\sngc\{ \} \\\\" if ($i ne $god);
		print OUTFILE " \\hline \n";
	}
}
close(OUTFILE);

sub finish_god {	
	# Put immunities, vulnerabilities, and protections into the correct arrays.
	$immune[$god] = ucfirst join(", ", @tmp_immune);
	$prot[$god] = ucfirst join(", ", @tmp_prot);
	$vuln[$god] = ucfirst join(", ", @tmp_vuln);

	# Put gifts into the array.
	$gift[$god] = join(", ", @tmp_gifts);

	# Clear the temp arrays
	@tmp_immune = ();
	@tmp_prot = ();
	@tmp_vuln = ();
	@tmp_gifts = ();
}

#
# Breaks LaTex output into groups of three.
# Every third comma gets a new table line.
#
# @param $string
# The list needing to be separated for adequate LaTex output
#
# @return the substituted string
#
sub break_in_threes{
	my $string = shift or die("break_in_threes expected an argument");
	$string =~ s/(,[^,]+,[^,]+,)/$1 \\\\ \n &/g;
	return $string;
}
