#!/usr/bin/perl
#
# this script goes through and collects the various image sets into crossfire-x.png
# files, where x is the numbers from the image_info file.  This
# collects all the images at once, whether or not they have
# been changed or not.

# Currently, there are no command line options for this script.

use FileHandle;

die("No arch directory - will not rebuild $mode image file") if (! -e "arch");

open(IMAGEINFO,"image_info") || die("Can't open image_info file: $!\n");
while (<IMAGEINFO>) {
    # Ignore lines that start with comments or just empty lines
    next if /^#/;
    next if /^\s*$/;
    ($setnum, $ext, @rest) = split /:/;
    # We don't actually need that data in this script, but may as well do sanity
    # checking.
    if ($#rest != 4) {
	print STDERR "image_info: line is corrupt:\n$_";
    }
    if ($extension[$setnum]) {
	print STDERR "Warning: set $setnum is duplicated in image_info file\n";
    }
    $extension[$setnum] = $ext;
}
close(IMAGEINFO);


for ($count=0; $count<=$#extension; $count++) {
    $ESRV[$count] = new FileHandle;
    $fh = $ESRV[$count];
    open($fh, ">crossfire.$count") ||
	die("Can't open crossfire.$count for write: $!\n");
}

open(BMAPS,"bmaps.paths") || die("Can't open bmaps.paths: $!\n");
$_ = <BMAPS>;
while(<BMAPS>) {
    chop;

    # we need to insert the extension for the images between the name
    # and the number (.171 or whatever) extension, so split on that.

    die("Unknown line: '$_'\n") unless /^\\(\d{5})\s+(\S+)\.(\w\w\w)$/o;
    $num = $1;
    $file = $2;
    $file1 = $3;

    print "$num $file\n" if ($num % 50) == 0 ;
    # This probably isn't the most efficient way to do this if a 
    # large number of images are added, as we try to open each
    # instance.
    # OTOH, we are doing one directory
    # at a time, so we should be hitting the DNLC at a very high
    # rate.

    for ($count=0; $count<=$#extension; $count++) {
	$filename = "$file.$extension[$count].$file1.png";
	$fh = $ESRV[$count];

	$length = -s "$filename";
	if (open(FILE,"$filename")) {
	    print $fh "IMAGE $num $length $file.$file1\n";
	    print "Error reading file $filename" if (!read(FILE, $buf, $length));
	    print $fh $buf;
	    close(FILE);
	}
	elsif ($count==0) {
	    # set 0 should have all the sets
	    print "Error: Image $filename not found for set 0!\n";
	}
	
    }
}
for ($count=0; $count<=$#extension; $count++) {
    close($ESRV[$count]);
}
close(BMAPS);
