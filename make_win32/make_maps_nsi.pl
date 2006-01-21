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
print NSI "!include \"MUI.nsh\"
Name \"Crossfire Server - $mapset\"

CRCCheck On

OutFile \"$output.exe\"

InstallDir \"\$PROGRAMFILES\\Crossfire Server\"

!define MUI_ABORTWARNING

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE \"..\\COPYING\"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE \"English\"

DirText \"Please select the folder below\"

CompletedText \"Installation complete\"

UninstallText \"This will uninstall Crossfire Server $mapset from your system\"

SetOverwrite IfNewer

";

my $startdir = $ARGV[ 1 ];
chdir( $startdir );
$startdir = cwd( );

my $currentdir = "";
my $currentinstdir = "";

my $outdir = "\$INSTDIR\\share\\maps";
my $data = "";
my $remove = "";
my $mode = 0;

print "Startdir: $startdir\n";

find( \&handleFind, $startdir );

print NSI "Section \"$mapset\" maps
  SectionIn RO
$data
  WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Crossfire Server $mapset\" \"DisplayName\" \"Crossfire Server $mapset (remove only)\"
  WriteRegStr HKLM \"Software\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Crossfire Server $mapset\" \"UninstallString\" \"\$INSTDIR\\UninstMaps.exe\"
  WriteUninstaller \"UninstMaps.exe\"
SectionEnd
";

print NSI "

Section \"un.$mapset\"
$remove
  Delete \"\$INSTDIR\\UninstMaps.exe\"
  DeleteRegKey HKEY_LOCAL_MACHINE \"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Uninstall\\Crossfire Server $mapset\"
SectionEnd

";

#Unlinked maps
do_maps( "Unlinked maps", "unlinked", 1 );

# test maps
do_maps( "Test maps", "test", 2 );

# Python scripts
do_maps( "Python scripts", "python", 3 );

print NSI "
!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
  !insertmacro MUI_DESCRIPTION_TEXT \${maps} \"The main game maps. Required.\"
  !insertmacro MUI_DESCRIPTION_TEXT \${unlinked} \"Maps that can't be accessed from the main maps.\"
  !insertmacro MUI_DESCRIPTION_TEXT \${test} \"Game test maps.\"
  !insertmacro MUI_DESCRIPTION_TEXT \${python} \"Python scripts. Require the server to have the Python plugin installed.\"
!insertmacro MUI_FUNCTION_DESCRIPTION_END
";

print "Done.\n";

exit;

sub handleFind
  {
  my $base = $_;
  my $foundFile = $File::Find::name;
  my $dir = $File::Find::dir;
  if ($dir =~ m/\/CVS$/) { return 1; }
  if (($mode != 1) && ($dir =~ m/$startdir\/unlinked/ )) { return 1; }
  if (($mode != 2) && ($dir =~ m/$startdir\/test/ )) { return 1; }
  if (($mode != 3) && ($dir =~ m/$startdir\/python/ )) { return 1; }
  if ($foundFile =~ m/\/CVS$/) { return 1; }
  if ($foundFile =~ m/\.pyc$/) { return 1; }

  # Temp
#  if (( $mode == 0 ) && ($dir !=~ m/\/world\// )) { return 1; }
  
    if ($currentdir ne $dir)
      {
      $currentdir = $dir;
      
      $dir =~ s/$startdir//ig;
      $dir =~ s/\//\\/ig;

      $currentinstdir = $outdir.$dir;
      
      $data .= "  CreateDirectory \"$currentinstdir\"\n";
      $data .= "  SetOutPath \"$currentinstdir\"\n";
      
      # Remove statement
      $remove = "  RmDir \"$currentinstdir\"\n" . $remove;
      }

  if (!(-d $foundFile))
    {
    $foundFile =~ s/\//\\/gi;
    $data .= "  File \"" . $foundFile . "\"\n";
    # Remove statement
    $remove = "  Delete \"$currentinstdir\\$base\"\n" . $remove;
    if ( $foundFile =~ /.*py$/ )
      {
      $remove = "  Delete \"$currentinstdir\\" . $base . "c\"\n" . $remove;
      }
    }
  }

sub help( )
  {
  print "Syntax error.\n";
  print "Read instructions at top of this script.\n";
  exit;
  }

sub do_maps( )
  {
  my $name = shift( );
  my $path = shift( );
  my $m = shift( );

  $data = "";
  $remove = "";
  $mode = $m;
  print "$name from " . $startdir . "/$path/\n";

  find( \&handleFind, $startdir . "/$path/" );

  print NSI "Section \"$name\" $path
$data
SectionEnd

Section \"un.$name\"
$remove
SectionEnd

";

  }