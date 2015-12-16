#!/usr/bin/env perl

#
# Takes the server spell list and parses them into text documents.
# Designed to use the server dump of spells, so it should be easier than parsing arches.
#
# Author: Daniel Hawkins
#

use strict;

if ($#ARGV lt 2){
	print "Usage: spell-lists.pl [server command] [spell list file] [prayer list file]\n";
	die("Not enough arguments");
}

# Declare my lists for storing information
my @prayer_list_name, my @prayer_list_level;
my @spell_list_name, my @spell_list_level;

my $output = `$ARGV[0] 2>&1`;
my @results = split("\n", $output);

for (@results){
	# Make sure we are handling a spell information piece
	next unless(/^(\w+(\s\w+)*):\w+:\w+:(\w+):(\d+)$/);
	if ($3 eq "praying"){
		push(@prayer_list_name, $1);
		push(@prayer_list_level, $4);
	}
	# There's at least one spell that uses ??? for skill. It should be skipped.
	elsif ($3 eq "sorcery" or $3 eq "summoning" or $3 eq "pyromancy" or $3 eq "evocation"){
		push(@spell_list_name, $1);
		push(@spell_list_level, $4);
	}
}

my $i;
# Sort each list by spell level.
my @sorted_spells = &sort(\@spell_list_level, \@spell_list_name);
my @sorted_prayers = &sort(\@prayer_list_level, \@prayer_list_name);

open(SPELLS, "> $ARGV[1]") or die("Failed to open $ARGV[1] for output");
for ($i = 0; $i < $#sorted_spells; ++$i){
	if ($sorted_spells[$i]){
		print SPELLS "level $i\n";
		print SPELLS "==========\n";
		print SPELLS "$sorted_spells[$i]\n";
	}
}
close(SPELLS);
open(PRAYERS, "> $ARGV[2]") or die("Failed to open $ARGV[2] for output");
for ($i = 0; $i < $#sorted_prayers; ++$i){
	if ($sorted_prayers[$i]){
		print PRAYERS "level $i\n";
		print PRAYERS "=========\n";
		print PRAYERS "$sorted_prayers[$i]\n";
	}
}
close(PRAYERS);


# Use a bin sort here to sort them.
#
# @param $level
# The reference to the array of levels for spells.
# @param $name
# The reference to the array of names for spells.
sub sort{
	# These are both array references
	my $level = shift;
	my $name = shift;
	# Now, declare the array of array references to sort the spells into.
	my @level_array;
	# Iterate through the spells
	for ($i = 0; $i < $#$level; ++$i){
		# Since push on array references is experimental, I'll just concat strings.
		$level_array[$$level[$i]] .= $$name[$i]."\n";
	}
	return @level_array;
}
