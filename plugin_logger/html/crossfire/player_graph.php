<?php
if (isset ($playername)){
$width=400;
$height=150;
$im=Imagecreate(400,200);
$white = ImageColorAllocate ($im,255,255,255);
$linex=0;
$liney=0;

function linereset($x,$y)
{
    global $linex;
    global $liney;
    $linex=$x;
    $liney=$y;
}

function lineto($x,$y,$col)
{
    global $linex;
    global $liney;
    global $im;
    if ($linex==0) $liney=$y;
    imageline ($im,$linex,$liney,$x,$y,$col);
    $linex=$x;
    $liney=$y;
}

function timeline ($values,$moments, $maxvalue, $color)
{
    global $width;
    global $height;
    $last_moment=$moments[0];
    $first_moment=$moments[sizeof ($moments)-1];
    $xfactor=$width/($last_moment-$first_moment+1);
    $yfactor=$height/($maxvalue+1);
    //echo "$xfactor,$yfactor<BR>";
    global $im;
    $colorind=ImageColorAllocate ($im,$color[0],$color[1],$color[2]);
    linereset (0,$height);
    $i=0;
    while (sizeof($values)>0){
        $valueitem=array_pop ($values);
        $momentitem=array_pop ($moments);
        $x=($momentitem-$first_moment)*$xfactor;
        $y=$height-($valueitem*$yfactor);
        //echo ("$x -> $y ($valueitem,$momentitem) <BR>");
        lineto ($x,$y,$colorind);
    }
}
  require_once ("database.php");
  database_connect ();
  $result=database_exec ("select player,title,agilitylvl,agilityexp,mentallvl,mentalexp,
                          physiquelvl, physiqueexp,magiclvl,magicexp,wisdomlvl,wisdomexp,
                          personalitylvl,personalityexp,generallvl,generalexp,moment from playershoot
                          where player='$playername'
                          ORDER by moment desc, counter DESC");
  $numrows=database_numrows ($result);
  $precision=$numrows/10;
  $Vagilitylvl=array();
  $Vagilityexp=array();
  $Vmentallvl=array();
  $Vmentalexp=array();
  $Vphysiquelvl=array();
  $Vphysiqueexp=array();
  $Vmagiclvl=array();
  $Vmagicexp=array();
  $Vwisdomlvl=array();
  $Vwisdomexp=array();
  $Vpersonalitylvl=array();
  $Vpersonalityexp=array();
  $Vgenerallvl=array();
  $Vgeneralexp=array();
  $Vmoment=array();
  for ($i=0;$i<$numrows;$i++){
      list ($player,$title,$agilitylvl,$agilityexp,$mentallvl,$mentalexp,
            $physiquelvl, $physiqueexp,$magiclvl,$magicexp,$wisdomlvl,$wisdomexp,
            $personalitylvl,$personalityexp,$generallvl,$generalexp, $moment)=
            database_fetch_array($result,$i);
      array_push ($Vagilitylvl,    $agilitylvl);
      array_push ($Vagilityexp,    $agilityexp);
      array_push ($Vmentallvl,     $mentallvl);
      array_push ($Vmentalexp,     $mentalexp);
      array_push ($Vphysiquelvl,   $physiquelvl);
      array_push ($Vphysiqueexp,   $physiqueexp);
      array_push ($Vmagiclvl,      $magiclvl);
      array_push ($Vmagicexp,      $magicexp);
      array_push ($Vwisdomlvl,     $wisdomlvl);
      array_push ($Vwisdomexp,     $wisdomexp);
      array_push ($Vpersonalitylvl,$personalitylvl);
      array_push ($Vpersonalityexp,$personalityexp);
      array_push ($Vgenerallvl,    $generallvl);
      array_push ($Vgeneralexp,    $generalexp);
      array_push ($Vmoment, database_strtotime($moment));
      if ($maxagilitylvl     <$agilitylvl     ) $maxagilitylvl      = $agilitylvl     ;
      if ($maxagilityexp     <$agilityexp     ) $maxagilityexp      = $agilityexp     ;
      if ($maxmentallvl      <$mentallvl      ) $maxmentallvl       = $mentallvl      ;
      if ($maxmentalexp      <$mentalexp      ) $maxmentalexp       = $mentalexp      ;
      if ($maxphysiquelvl    <$physiquelvl    ) $maxphysiquelvl     = $physiquelvl    ;
      if ($maxphysiqueexp    <$physiqueexp    ) $maxphysiqueexp     = $physiqueexp    ;
      if ($maxmagiclvl       <$magiclvl       ) $maxmagiclvl        = $magiclvl       ;
      if ($maxmagicexp       <$magicexp       ) $maxmagicexp        = $magicexp       ;
      if ($maxwisdomlvl      <$wisdomlvl      ) $maxwisdomlvl       = $wisdomlvl      ;
      if ($maxwisdomexp      <$wisdomexp      ) $maxwisdomexp       = $wisdomexp      ;
      if ($maxpersonalitylvl <$personalitylvl ) $maxpersonalitylvl  = $personalitylvl ;
      if ($maxpersonalityexp <$personalityexp ) $maxpersonalityexp  = $personalityexp ;
      if ($maxgenerallvl     <$generallvl     ) $maxgenerallvl      = $generallvl     ;
      if ($maxgeneralexp     <$generalexp     ) $maxgeneralexp      = $generalexp     ;
  }
  $verymax=$maxagilityexp;
  if ($verymax<$maxmentalexp ) $verymax=$maxmentalexp ;
  if ($verymax<$maxphysiqueexp ) $verymax=$maxphysiqueexp ;
  if ($verymax<$maxmagicexp ) $verymax= $maxmagicexp;
  if ($verymax<$maxwisdomexp ) $verymax= $maxwisdomexp;
  if ($verymax<$maxpersonalityexp ) $verymax= $maxpersonalityexp;
  timeline ($Vagilityexp,$Vmoment,$verymax,array (255,0,0));
  timeline ($Vmentalexp,$Vmoment,$verymax,array (0,255,0));
  timeline ($Vphysiqueexp,$Vmoment,$verymax,array (0,0,255));
  timeline ($Vmagicexp,$Vmoment,$verymax,array (128,0,128));
  timeline ($Vwisdomexp,$Vmoment,$verymax,array (128,128,0));
  timeline ($Vpersonalityexp,$Vmoment,$verymax,array (0,128,128));
  timeline ($Vgeneralexp,$Vmoment,$maxgeneralexp,array (0,0,0));
  /*timeline ($Vagilityexp,$Vmoment,$maxagilityexp,array (255,0,0));
  timeline ($Vmentalexp,$Vmoment,$maxmentalexp,array (0,255,0));
  timeline ($Vphysiqueexp,$Vmoment,$maxphysiqueexp,array (0,0,255));
  timeline ($Vmagicexp,$Vmoment,$maxmagicexp,array (255,0,255));
  timeline ($Vwisdomexp,$Vmoment,$maxwisdomexp,array (255,255,0));
  timeline ($Vpersonalityexp,$Vmoment,$maxpersonalityexp,array (0,255,255));
  timeline ($Vgeneralexp,$Vmoment,$maxgeneralexp,array (0,0,0));*/
  }
Header("Content-type: image/png");
imagePNG ($im);
?>