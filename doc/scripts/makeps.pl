#!/usr/bin/perl
eval 'exec perl -S $0 "$@"'
    if $running_under_some_shell;
			# this emulates #! processing on NIH machines.
			# (remove #! line above if indigestible)

eval '$'.$1.'$2;' while $ARGV[0] =~ /^([A-Za-z_0-9]+=)(.*)/ && shift;
			# process any FOO=bar switches

# makeps - make Postscript-files of the archetypes listed in text file whose
# filename is passed in 'input'
# Variables passed in:
#   archdir - root of crossfire-src, with a trailing slash
#   libdir  - where archetypes etc. is found

$, = ' ';		# set output field separator
$\ = "\n";		# set output record separator

$size=0.4;
$IMAGE_SIZE=32;		# Size of PNG images

if ($output eq "tex") { $BG="\\#ffffff"; }
else { $BG="\\#ab0945"; }

# Set colour to 1 if you want colour postscript.
$colour = 0;
# IF you have giftrans installed and want transparent gifs, set
# appropriately.  IT looks much nicer if you can do it.
$giftrans = 0;

# An array listing which archetypes files need fixing, the value
# is the file where it is used. There must be at least one character
# between the ~~spec~~'s.


open(IN,"<".$input) || die("can not open $input\n");
while (<IN>) {
    @flds = split(/~~/);
    $work_todo = 1;
    $i = 1;
    while ($flds[$i] ne "") {
	        $makeps{$flds[$i]} = 0;
		$i += 2;
    }
}
close(IN);

if ($output ne "png") {
    # An array to reduce the size of the bitmap exponentially.
    # A 4x8 bitmap will be reduced to 60% of its full size.
    if ($work_todo) {
	$size_mul{1} = 1;
	for ($i = 1; $i <= 11; $i++) {# Max input is 12x12, a *large* bitmap ;-)
	    $size_mul{$i} = $size_mul{$i} * 0.9;
        }
    }
}

# Make the image directory for storing retrieved arch faces if it does not exist
unless (-d "images") {
    `mkdir images`;
}

$More = 0;
print STDERR "starting to process $inarch\n";
open(IN,"<".$inarch) || die("could not open $inarch\n");
line: while (<IN>) {
    chomp;	# strip record separator
    @Fld = split(/ /, $_, 2);
    if ($Fld[0] eq 'Object') {
	if ($interesting) {
	    if (!$More && $makeps{$obj} != 1) {
		$makeps{$obj} = &assemble();
	    }
	}

	# Get ready for next archetype
	if (!$More) {
	    $xmin = $xmax = $ymin = $ymax = 0;
	    $obj = $Fld[1];
	    $interesting = defined $makeps{$obj};
	}
	$X = $Y = 0;
	$More = 0;
    }

    if ($Fld[0] eq 'face') {
	$face = $Fld[1];
    }
    if ($Fld[0] eq 'x') {
	$X = $Fld[1];
	if ($X > $xmax) {	#???
	    $xmax = $X;
	}
	elsif ($X < $xmin) {	#???
	    $xmin = $X;
	}
    }
    if ($Fld[0] eq 'y') {
	$Y = $Fld[1];
	if ($Y > $ymax) {	#???
	    $ymax = $Y;
	}
	elsif ($Y < $ymin) {	#???
	    $ymin = $Y;
	}
    }
    if ($Fld[0] eq 'More') {
	$More = 1;
    }
    if ($Fld[0] eq 'msg') {
	do {
	    $_ = <IN>;
	    @Fld = split;
	}
	while ($Fld[0] ne 'endmsg');
    }
}
close(IN);

# Remember to check the last archetype also...
if ($interesting) {
    if ($makeps{$obj} != 1) {
	$makeps{$obj} = &assemble();
    }
}

system('rm -f work.pbm tmp.pbm empty.pbm');
# clean up a little
system("pbmmake -white $IMAGE_SIZE $IMAGE_SIZE > empty.pbm");

# We've created a number of Postscript-files - now we need to
# patch the filenames and sizes into the TeX-files.

$, = '';
open(IN,"<".$input);
while (<IN>) {
    @Fld = split(/~~/);
    if ($#Fld > 1) {
	for ($i = 1; $i <= $#Fld-1; $i += 2) {
	    if (defined $makeps{$Fld[$i]}) {
    			$Fld[$i] = $makeps{$Fld[$i]};
	    }
	}
    }
    print @Fld;
}
close(IN);


sub assemble {
    local($w, $h, $ppm, $buff, $i, $j, $bmap_file, $ps_file) = @_;

    # All the image are big image anymore, so we don't really need to handle split pieces.
    $face = get_image("../../lib/crossfire.tar", $face);
    $bmap_file = "images/".$face.".png";
    if ($output eq "tex") {$ps_file = "images/".$face . '.ps';     }
    elsif ($output eq "png") { $ps_file = "images/".$face . '.png'; }
    elsif ($output eq "pdf") {
        $tmp = $face;
        $tmp =~ s/\./-/gi;
        $ps_file = $tmp . '.png';
    } else { $ps_file = $faces{0, 0} . '.gif'; }

    $ps_file =~ s/[_ ]/-/g;

    # We don't need to manipulate the files, so just do hard links - much
    # faster, and also doesn't use space.
    if (($output eq "png") || ($output eq "pdf")) {
	link($bmap_file, $ps_file);
	if ($output eq "png") {
	   $ps = "<img src=$ps_file>";
        } else {
            $ps = "\\includegraphics[scale=0.5]{" . $ps_file . "}";
        }
	return $ps;
    }

    $w = $xmax - $xmin + 1;
    $h = $ymax - $ymin + 1;

    if (! -e $ps_file) {
	# Maybe ln -s instead?
	if ($output eq "tex") {
	    if ($colour) {	system("pngtopnm -mix -background $BG $bmap_file | pnmtops -noturn -nosetpage > $ps_file"); }
	    else {	system("pngtopnm -mix -background $BG $bmap_file | pnmdepth 255 | ppmtopgm | pnmtops -noturn -nosetpage> $ps_file"); }
	}
	elsif ($giftrans) {
	    system("pngtopnm -mix -background $BG $bmap_file | ppmtogif | giftrans -t $BG $ppm > $ps_file");
	} else {
	    system("pngtopnm -mix -background $BG $bmap_file | ppmtogif > $ps_file");
	}
    }
    $mul = $size_mul{int(sqrt($w * $h))} * $size;
    if ($output eq "tex") {
	if ($h == 1) {
	    $ps = sprintf "\\psfig{figure=$ps_file,width=%0.2fcm,height=%0.2fcm}", $w * $mul, $h * $mul;
	} else {
	    $ps = sprintf "\\raisebox{-%0.2fcm}{\\psfig{figure=$ps_file,width=%0.2fcm,height=%0.2fcm}}", ($h-1) * $mul, $w * $mul, $h * $mul;
	}
    } else {
	    $ps = "<img src=$ps_file>";
    }
    $ps;
}

# Tarball will probably be ../lib/crossfire.tar
sub get_image {
    local($tarball, $face) = @_;
    # Substitute in the base face set into the name.
    $face =~ s/\./\.base\./;
    # Next we get the image fire from the tarball and load it.
    my $output = `tar --extract --file='$tarball' $face.png`;
    rename("$face.png", "images/$face.png");
    return $face
}
