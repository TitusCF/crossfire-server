#!/usr/bin/perl
#
# perl seems like a better language to do this in than C
# Eventually, it would be nice for this to replace the C version.  For now,
# all it is really good for is to make sure all the xpm files are using
# proper colors.

# Most colors that we can really support in output format
$MAX_COLOR=91;
# Ascii value of the starting color
$START_COLOR=35;
# Template file to use to get color information
$XPM_TEMPLATE="arch/dev/xpm.template";



$filenum=1;

#if ($#ARGV!=0) {
#	die("Usage: $0 (filename)\n");
#}

read_template();

open(BMAPS,"<bmaps.paths") || die ("Can not open bmaps.paths for reading.\n");
#open(OUT,">$ARGV[0].$filenum") || die ("Can not open $ARGV[0].$filenum for writing.\n");

while (<BMAPS>) {
    next if (!/\\/);
    chomp;
    ($num, $name) = split;
    $count++;
#    print "processing $name\n" if (!($count % 100));
    load_xpm($name);
}
close(BMAPS);

# reads the template file to get color information.  We get the
# real color names, and store the symbolic name in an associative array.
#
sub read_template{
    open(TEMPLATE,"<$XPM_TEMPLATE") || die("Can not open $XPM_TEMPLATE for reading\n");
    while (<TEMPLATE>) {
	if (/"24 24 ([0-9]*) 1"/,) {
	    $num_colors=$1;
	    print "found $num_colors colors in template\n";
	    last;
	}
    }
    $on_color=1;
    while (<TEMPLATE>) {
	# Skip comment lines
	next if (m#^/\*.*\*/#);
	# Next few lines are pretty disgusting looking regex.  In all cases,
	# we make the assumptio that the real color is only 1 word.
	# First case is one work symbolic
	if (/".\s*(s\s\w+)\s*(c\s\w+)"/) {
	    ($rc = $2) =~ tr/A-Z/a-z/;
	    ($sc=$1) =~ tr/A-Z/a-z/;
	    $color{$rc}=$sc;
	    $on_color++;
	}
	# Second case - symbolic name is 2 words
	elsif (/".\s*(s \w+ \w+)\s*(c \w+)"/) {
	    ($rc = $2) =~ tr/A-Z/a-z/;
	    ($sc=$1) =~ tr/A-Z/a-z/;
	    $color{$rc}=$sc;
	    $on_color++;
	}
	# Third case - no symbolic name
	elsif (/".\s*(c \w+)"/) {
	    ($rc = $1) =~ tr/A-Z/a-z/;
	    $color{$rc}=$1;
	    $on_color++;
	}
	else { print "did not find match for $_"; }
	last if ($on_color > $num_colors);
    }
    close(TEMPLATE);
}


# reads the template file to get color information.  We get the
# real color names, and store the symbolic name in an associative array.
#
sub load_xpm {
    $file=shift;

    open(TEMPLATE,"<$file.xpm") || die("Can not open $file for reading\n");
    while (<TEMPLATE>) {
	if (/"24 24 ([0-9]*) 1"/,) {
	    $num_colors=$1;
	    last;
	}
    }
    $on_color=1;
    while (<TEMPLATE>) {
	# Skip comment lines
	next if (m#^/\*.*\*/#);
	# Next few lines are pretty disgusting looking regex.  In all cases,
	# we make the assumptio that the real color is only 1 word.
	# First case is one work symbolic
	if (/".\s*(s\s\w+)\s*(c\s\w+)"/) {
	    ($rc = $2) =~ tr/A-Z/a-z/;
	    ($sc=$1) =~ tr/A-Z/a-z/;
	    $on_color++;
	}
	# Second case - symbolic name is 2 words
	elsif (/".\s*(s \w+ \w+)\s*(c \w+)"/) {
	    ($rc = $2) =~ tr/A-Z/a-z/;
	    ($sc=$1) =~ tr/A-Z/a-z/;
	    $on_color++;
	}
	# Third case - symbolic name is 2 words, monochrome name also supplied
	elsif (/".\s*(s \w+ \w+)\s*m\s([wW]hite|[bB]lack)\s*(c \w+)"/) {
	    ($rc = $3) =~ tr/A-Z/a-z/;
	    ($sc=$1) =~ tr/A-Z/a-z/;
	    $on_color++;
	}
	# Fourth case - 1 word symbolic, monochrome supplied
	elsif (/".\s*(s \w+)\s*m\s([wW]hite|[bB]lack)\s*(c \w+)"/) {
	    ($rc = $3) =~ tr/A-Z/a-z/;
	    ($sc=$1) =~ tr/A-Z/a-z/;
	    $on_color++;
	}	# Fifth case - no symbolic name, but do have monochrome
	elsif (/".\s*m\s([wW]hite|[bB]lack)\s*(c \w+)"/) {
	    ($rc = $2) =~ tr/A-Z/a-z/;
	    $sc="";
	    $on_color++;
	}	# Third case - no symbolic name
	elsif (/".\s*(c \w+)"/) {
	    ($rc = $1) =~ tr/A-Z/a-z/;
	    $sc="";
	    $on_color++;
	}
	else { 
		print "$file.xpm: did not find match for $_"; 
		$on_color++;
		next;
	}
        if ($color{$rc} eq "") {
		print "could not find color '$rc' in $file.xpm\n";
        }
	elsif ($sc ne "" && $color{$rc} ne $sc) {
		print "symbolic color for $rc does match match ($sc ne $color{$rc}) in file $file.xpm\n";
	}
	last if ($on_color > $num_colors);
    }
    close(TEMPLATE);
}

	
