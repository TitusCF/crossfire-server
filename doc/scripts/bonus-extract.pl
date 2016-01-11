#
# Extracts the bonuses from the stat_bonus file.
#
# Author: Daniel Hawkins
#

use strict;

# Make sure enough arguments were passed to the program.
if ($#ARGV lt 2 or ($ARGV[1] ne "html" and $ARGV[1] ne "latex")){
	print "Usage: bonus_extract.pl [stat_bonus_file] [html|latex] [output_file]\n";
	die("Not or invalid enough arguments");
}

# Make sure the output file can be written to.
open(OUT, "> $ARGV[2]") or die("Cannot open $ARGV[2] to write results");
open(IN, "< $ARGV[0]") or die("Cannot open $ARGV[0] to read bonuses");

# Declare variables
my $max_stat, my @hp_bonus, my @sp_bonus, my @grace_bonus, my @shop_bonus,
my @dex_bonus, my @speed_bonus, my @damage_bonus, my @hit_bonus, my @weight_limit,
my @learn_spell, my @cleric_chance, my @turn_bonus, my @fear_bonus,
my $cur_ref, my @temp;

$cur_ref = undef;
while(<IN>){
	# Ignore comment lines and blank lines.
	next if (/^\s*$/ or /^\s*#/);
	if ($cur_ref ne undef){
		@temp = split(/,\s*/);
		for (@temp){
			chomp;
			# Make sure we don't have leading spaces, then add to the array.
			push(@$cur_ref, $1) if (/^\s*([^{}]-?(\d*)(\.\d+)?)/);
		}
		$cur_ref = undef if (/^\s*}/);
	}
	elsif (/^max_stat\s+(\d+)/){
		$max_stat = $1;
	}
	#
	# Since we only fill one array at a time, just figure out what array
	# we're in and then through the values in when we get the next lines.
	#
	# Why use references, you ask? Because it makes the actual extraction of values easy.
	#

	#
	# Also, I'm extracting a few values that, as of now, aren't used.
	# I intend to implement them in the output soon.
	#
	# Daniel Hawkins 2015-12-11
	#
	elsif (/^con_hp_bonus/){
		$cur_ref = \@hp_bonus;
	}
	elsif (/^pow_int_sp_bonus/){
		$cur_ref = \@sp_bonus;
	}
	elsif (/^wis_pow_grace_bonus/){
		$cur_ref = \@grace_bonus;
	}
#
#	Not used in curent implementation of shop buy/sell calculations
#	Current is r20215 at the time of last inspection.
#
#	Daniel Hawkins 2016-01-11
#
#	elsif (/^cha_shop_bonus/){
#		$cur_ref = \@shop_bonus;
#	}
	elsif (/^dex_bonus/){
		$cur_ref = \@dex_bonus;
	}
	elsif (/^dex_speed_bonus/){
		$cur_ref = \@speed_bonus;
	}
	elsif (/^str_damage_bonus/){
		$cur_ref = \@damage_bonus;
	}
	elsif (/^str_hit_bonus/){
		$cur_ref = \@hit_bonus;
	}
	elsif (/^str_weight_limit/){
		$cur_ref = \@weight_limit;
	}
	elsif (/^int_wis_learn_spell/){
		$cur_ref = \@learn_spell;
	}
	elsif (/^wis_cleric_chance/){
		$cur_ref = \@cleric_chance;
	}
	elsif (/^wis_turn_bonus/){
		$cur_ref = \@turn_bonus;
	}
	elsif (/^cha_fear_bonus/){
		$cur_ref = \@fear_bonus;
	}
}
close(IN);

#
# Current implementation has Cha only affect buy prices
#
# # Now we calculate the buy/sell values
# my @buy, my @sell;
# for (@shop_bonus){
#	push(@buy, $_ + 100);
#	push(@sell, 100 - $_);
# }

# Do shop buy calculations.
for (0..$max_stat){
    # 1.06 and 0.38 are taken from shop_buy_multiplier in server/shop.c
    my $val = 100.0 / (0.38 * 1.06 ** $_);
    # shop.c also defines the multiplier limits to be .5 and 2.0
    if ($val < 50){
	$val = 50;
    }
    elsif ($val > 200){
	$val = 200;
    }
    push(@shop_bonus, $val);
}

# Proceed to output -- This works since the values in the stat_bonus file have index 0, but it is not needed.
if ($ARGV[1] eq "html"){
	for (1..$max_stat){
		print OUT "<tr><td>$_</td><td>$hp_bonus[$_]</td><td>$sp_bonus[$_]</td><td>$dex_bonus[$_]</td><td>$damage_bonus[$_]</td><td>$hit_bonus[$_]</td>
		<td>$weight_limit[$_]</td><td>$speed_bonus[$_]</td><td>$learn_spell[$_]</td>"#<td>$buy[$_]%/$sell[$_]%</td></tr>\n";
		."<td>$shop_bonus[$_]%</td></tr>\n";
	}
}
elsif ($ARGV[1] eq "latex"){
	for (1..$max_stat){
		print OUT "$_ & $hp_bonus[$_] & $sp_bonus[$_] & $dex_bonus[$_] & $damage_bonus[$_] & $hit_bonus[$_]
		& $weight_limit[$_] & $speed_bonus[$_] & $learn_spell[$_] & "#$buy[$_]\\\%/$sell[$_]\\\% \\\\ \n";
		."$shop_bonus[$_]\\\% \\\\ \n";
	}
}

close(OUT);
