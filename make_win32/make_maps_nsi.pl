# This is a Perl script used to generate Nullsoft's Installer
# scripts for Crossfire maps.
# NullSoft installer can be found at http://nsis.sourceforge.net
# (note: this script is probably relevant to Windows platform only)

# Created by Nicolas 'Ryo'
# Released in public domain

# Based on http://nsis.sourceforge.net/archive/nsisweb.php?page=93&instances=0,8

# Usage: make_maps_nsi.pl <map set name> <path to search>
# <map set name> is for instance 'Bigworld Maps' or 'SmallWorld Maps'
# <path to search> is, well, the path to search for maps

# The install script will have the following features:
# * its name is 'CrossfireServer<map set name without spaces>.nsi'
# * generated .exe name 'CrossfireServer<map set name without spaces>.exe'
# * all files except CVS\* are grabbed
# * default install path %program files%\Crossfire Server\share\maps
# * uninstall entry 'Crossfire Server - <map set name>' in control panel
# * uninstaller called 'UnistMaps.exe' in %program files%\Crossfire Server (to avoid
#    conflits with Crossfire Server's uninstaller)

# Notes:
# * the licence the .nsi will use is supposed to be ..\COPYING
#  the idea is that the .nsi is ran from make_win32 subdirectory

use File::Find;
use File::Spec;
use Cwd;

use strict;

my $Crossfire = "Crossfire Server";

&help unless @ARGV == 2;

my $mapset = $ARGV[ 0 ];

my $output = $Crossfire.$mapset;
$output =~ s/ //g;

print "Output: $output\n";

open( NSI, ">".$output.".nsi" );

# Let's write the header

print NSI ";Application title\n";
print NSI "Name \"Crossfire Server - $mapset\"\n";
print NSI "\n";
print NSI ";Do A CRC Check\n";
print NSI "CRCCheck On\n";
print NSI "\n";
print NSI ";Output File Name\n";
print NSI "OutFile \"$output.exe\"\n";
print NSI "\n";
print NSI ";License Page Introduction\n";
print NSI "LicenseText \"You must agree to this license before installing.\"\n";
print NSI "\n";
print NSI ";License Data\n";
print NSI "LicenseData \"..\\COPYING\"\n";
print NSI "\n";
print NSI ";The Default Installation Directory\n";
print NSI "InstallDir \"\$PROGRAMFILES\\Crossfire Server\"\n";
print NSI "\n";
print NSI ";The text to prompt the user to enter a directory\n";
print NSI "DirText \"Please select the folder below\"\n";
print NSI "\n";
print NSI "CompletedText \"Installation complete\"\n";
print NSI "\n";
print NSI "Section \"Install\"\n";

my $startdir = $ARGV[ 1 ];
chdir( $startdir );
$startdir = cwd( );

my $currentdir = "";
my $currentinstdir = "";

my $outdir = "\$INSTDIR\\share\\maps";

my $remove = "";
my $remove = "";

print "Startdir: $startdir\n";

find( \&handleFind, $startdir );

# Some reg keys for uninstall
print NSI "  WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Crossfire Server $mapset\" \"DisplayName\" \"Crossfire Server $mapset (remove only)\"\n";
print NSI "  WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Crossfire Server $mapset\" \"UninstallString\" \"\$INSTDIR\\UninstMaps.exe\"\n";
print NSI "  WriteUninstaller \"UninstMaps.exe\"\n";

# Finish: the remove statements
print NSI "SectionEnd\n";
print NSI "\n";
print NSI "UninstallText \"This will uninstall Crossfire Server $mapset from your system\"\n";
print NSI "\n";
print NSI "Section Uninstall\n";
print NSI "$remove";
print NSI "  Delete \"\$INSTDIR\\UninstMaps.exe\"\n";
print NSI "  DeleteRegKey HKEY_LOCAL_MACHINE \"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Crossfire Server $mapset\"\n";
print NSI "SectionEnd\n";

print "Terminé.\n";

exit;

sub handleFind
  {
  my $base = $_;
  my $foundFile = $File::Find::name;
  my $dir = $File::Find::dir;
  if ($dir =~ m/\/CVS$/) { return 1; }
  if ($foundFile =~ m/\/CVS$/) { return 1; }
  
    if ($currentdir ne $dir)
      {
      $currentdir = $dir;
      
      $dir =~ s/$startdir//ig;
      $dir =~ s/\//\\/ig;

      $currentinstdir = $outdir.$dir;
      
      print NSI "  CreateDirectory \"$currentinstdir\"\n";
      print NSI "  SetOutPath \"$currentinstdir\"\n";
      
      # Remove statement
      $remove = "  RmDir \"$currentinstdir\"\n" . $remove;
      }

  if (!(-d $foundFile))
    {
    $foundFile =~ s/\//\\/gi;
    print NSI "  File \"" . $foundFile . "\"\n";
    # Remove statement
    $remove = "  Delete \"$currentinstdir\\$base\"\n" . $remove;
    }
  }

sub help( )
  {
  print "Syntax error.\n";
  print "Read instructions at top of this script.\n";
  exit;
  }

