#!/usr/bin/perl

if ($ARGV[0] eq "-xpm") {$mode ="xpm";}
elsif ($ARGV[0] eq "-xbm") {$mode ="xbm";}
elsif ($ARGV[0] eq "-png") {$mode ="png";}
else {die("No option/invalid option specified\n");}

die("No arch directory - will not rebuild $mode image file") if (! -e "arch");

open(BMAPS,"bmaps.paths") || die("Can't open bmaps.paths: $!\n");
open(ESRV,">crossfire.$mode") || die("Can't open crossfire.$mode for write: $!\n");
$_ = <BMAPS>;
while(<BMAPS>) {
    chop;
    die("Unknown line: '$_'\n") unless /^\\(\d{5})\s+(\S+)$/o;
    $num = $1;
    $file = $2;
    print "$num $file\n" if ($num % 50) == 0 ;
    if ($mode eq "xbm") {
	# the bitmaps files are little special - we take the data and
	# convert it to binary - saves space, is faster in the end
	# 
	print ESRV "IMAGE $num 72 $file\n";
	open(FILE,"<$file") || die("Unable to open $file for read: $!\n");
	$len=0;
	while (<FILE>) {
	    if (/^\s*0x[0-9a-f][0-9a-f]/i) {
		@vals = split(/[,}]/);
		for ($i=0; $i<$#vals; $i++) {
		    $dest[$i]=oct($vals[$i]);
		}
		$numel = $#vals;
		$dest = pack "C$numel", @dest;
		$len += $numel;
		print ESRV $dest;
	    }
	}
	close(FILE);
	if ($len != 72) {
	    die("Image $file datelen: $len != 72\n") 
	}
    }
    else {
	$length = -s "$file.$mode";
	open(FILE,"$file\.$mode") || print("Unable to open $file\.$mode for read: $!\n");
        @name2 = split("/",$file);
        $name3 = $name2[$#name2];
        $name4 = "$name3\.$mode";
#        print "name4:", $name4, "\n";
        if(open(FILE2,"./alternate_images/$name4") ) {
             print "Alternate image found: ",$name4,"\n";
             $length = -s "./alternate_images/$name4";
             print ESRV "IMAGE $num $length $file\n";
             while(<FILE2>) {
               print ESRV $_;
	     }
             close(FILE2);
        } else {
	   print ESRV "IMAGE $num $length $file\n";
	   while(<FILE>) {
	       print ESRV $_;
	   }
	   close(FILE);
        }
    }
}
close(ESRV);
close(BMAPS);
