#! /usr/local/bin/perl

$default_X_size = 16;
$default_Y_size = 16;

$expand = 2;
$help = 0;
$input_map = $ARGV[$#ARGV];

# argv loop
foreach $i (0 .. $#ARGV) {
  if($ARGV[$i] =~ "-h") { $help = 1; }
  if($ARGV[$i] =~ "-e") { $factor = $ARGV[++$i]; }
}

# various help/runtime messages
if(!$expand||!$input_map) {
  print "USAGE: $0 -e factor <input map> > <output map> \n" ;
  exit 0;
}
if($help) {
  print "\n$0 options:\n" ;
  print "-e\t Factor by which to expand map x,y dimensions.\n";
  print "-h\t This help message. \n";
  exit 0;
}

#Read in input map
open(FILE, $input_map) && (@mapdata=<FILE>) && close(FILE)
  || print "FATAL: file $input_map not found!\n";

# convert map data into objects
while ($i<=$#mapdata) {
  local(@datum) = split (' ',$mapdata[$i]);
  if($datum[0] eq "arch") { $name[$objnum] = $datum[1]; }
  elsif($datum[0] eq "end") { $objnum++; }
  elsif($datum[0] eq "x") { $x[$objnum] = $datum[1]; } 
  elsif($datum[0] eq "y") { $y[$objnum] = $datum[1]; } 
  else {
    push(@otherline,$mapdata[$i]); $olines_in_obj[$objnum]++; 
  }
  $i++; 
}

# Object 0 is the map header info, dont
# expand it, so just print it out
&expand_obj("0 1 0");
$bufline = $olines_in_obj[0];

#Expand the map objects 1 to $objnum 
while ($j++<$objnum) {
  &expand_obj("$j $expand $bufline");
  $bufline += $olines_in_obj[$j];
}

# SUBROUTINES

sub expand_obj {
  local($data) = @_;
  local(@temp) = split(' ',$data);
  local($obj) = $temp[0];
  local($factor) = $temp[1];
  local($end_buf) = $temp[2] + $olines_in_obj[$obj];
  local($start_x) = $x[$obj] * $factor;
  local($start_y) = $y[$obj] * $factor;
  local($end_x) = $start_x + $factor;
  local($end_y) = $start_y + $factor;

  while($start_x<$end_x) {
    while($start_y<$end_y) { 
        local($start_buf) = $temp[2];
        if($name[$obj]) { printf("arch %s\n",$name[$obj]); } 
        else { return; }

        if($obj==0) { 
          if($x[0]) { printf("x %d\n",$x[0]*$expand); } 
          else { printf("x %d\n",$default_X_size*$expand); } 
        } elsif ($x[$obj]) { printf("x %d\n",$start_x); } 

        if($obj==0) { 
          if($y[0]) { printf("y %d\n",$y[0]*$expand); }
          else { printf("y %d\n",$default_Y_size*$expand); }
        } elsif ($y[$obj]) { printf("y %d\n",$start_y); }

        while ($start_buf<$end_buf) {
          print "$otherline[$start_buf]"; 
          $start_buf++;
        }
        print"end\n";
        $start_y++;
    }
    $start_x++;
    $start_y = $y[$obj] * $factor;
  }
}
