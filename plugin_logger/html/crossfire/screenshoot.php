<?php
$crossfirebasedir="/home/crossfire/public_html/loggerstuff/crossfire/";
function addtile ($tilename,$x,$y, $picture)
  {
  global $crossfirebasedir;
  global $tilewidth;
  global $tileheight;
  global $shoot;
  if ($tilename=="blank.111") return;
  if (file_exists($crossfirebasedir."png/".$tilename .".png"))
  {
      $im=ImageCreateFromPng ($crossfirebasedir."png/".$tilename .".png");
  }
  else
  {
      $im=ImageCreateFromPng ($crossfirebasedir."png/mint.base.111.png");
  }
  ImageCopy ($shoot,$im,$x*$tilewidth,$y*$tileheight,0,0,$tilewidth,$tileheight);
  return;
  }
function get_screenshoot ($shootfile)
  {
  global $shoot;
  global $crossfirebasedir;
  global $tilewidth;
  global $tileheight;
  global $tilewidthcount;
  global $tileheightcount;
  $outputfile=$shootfile.".jpg";
  $outputhtml=$shootfile.".map";
  if ( (file_exists ($crossfirebasedir.$outputfile)) &&
       (file_exists ($crossfirebasedir.$outputhtml)) )
      return $outputhtml;
  $handle=fopen ($crossfirebasedir.$shootfile.".sht","r");
  $handlehtml=fopen ($crossfirebasedir.$outputhtml,"wb");

  $line=fscanf ($handle,"MAP_SIZE %d,%d\n");
  list($newwidth,$newheight)=$line;
  if ($newwidth) $tilewidthcount=$newwidth;
  if ($newheight) $tileheightcount=$newheight;

  $points[0]=0;
  $points[1]=0;
  $points[2]=0;
  $points[3]=$tileheightcount;
  $points[4]=$tilewidthcount;
  $points[5]=$tileheightcount;
  $points[6]=$tilewidthcount;
  $points[7]=0;

  $shoot=imagecreate($tilewidth*$tilewidthcount,$tileheight*$tileheightcount);
  $color=imagecolorallocate ($shoot,0,0,0);
  imagefilledpolygon ($shoot,$points,4,$color);
  fwrite ($handlehtml,"<MAP NAME=\"screenshootmap\">");

  while ($fullline=fgets ($handle,2048)){
        list ($coords,$faces,$objects) = explode (" ",$fullline,3);
        list ($facex,$facey) = sscanf ($coords,"%d,%d");
        $face_array = explode ("|",$faces,3);
        addtile ($face_array[2],$facex,$facey, $shoot);
        addtile ($face_array[1],$facex,$facey, $shoot);
        addtile ($face_array[0],$facex,$facey, $shoot);
        $startx=$facex*$tilewidth;
        $starty=$facey*$tileheight;
        $endx=($facex+1)*$tilewidth;
        $endy=($facey+1)*$tileheight;
        fwrite ($handlehtml,"<AREA shape=\"rect\" coords=\"$startx,$starty,$endx,$endy\" href=\"get_infos.php?text=");
        fwrite ($handlehtml,urlencode ($objects));
        fwrite ($handlehtml,"\" target=\"infos\">\n\t");
  }
  imageJpeg ($shoot,$crossfirebasedir.$outputfile,100);
  fwrite ($handlehtml,"<IMG SRC=\"$outputfile\" usemap=\"#screenshootmap\" border=\"0\">\n\t");
  fclose ($handlehtml);
  fclose ($handle);
  return $outputhtml;
  }

function put_screenshoot ($shootname)
  {
  global $filename;
  global $crossfirebasedir;
  global $tilewidth;
  global $tileheight;
  global $tilewidthcount;
  global $tileheightcount;
  if (!isset ($shootname)){
     echo "<B>Cette page n'est pas prévue pour être accédée directement</B>";
  }else{
     $filename="screenshoot/$shootname";
     $tilewidth=32;
     $tileheight=32;
     $tilewidthcount=25;
     $tileheightcount=25;
     $imagefile=get_screenshoot ($filename);
     include ($imagefile);
  }
  }
?>
