<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>The gravestones collection</TITLE>
</HEAD>
<BODY bgcolor="#DDDDDD"><CENTER>
</font></B></U>
<?php
  require_once ('database.php');
  database_connect();
  if (isset ($playername))
      $deathresult = database_exec ("select moment,player,killer,playershoot,screenshoot
                                     from death where player='$playername'
                                     order by moment DESC,counter DESC");
  else
      $deathresult = database_exec ("select moment,player,killer,playershoot,screenshoot
                                     from death order by moment DESC,counter DESC");
  if (!isset($maxresult)) $maxresult=10;
  if (!isset($startresult)) $startresult=0;
  $rowcount=database_numrows ($deathresult);
  $realstart=$startresult+1;
  $realend=($startresult+$maxresult);
  if ($realend>$rowcount) $realend=$rowcount;
  echo '<U><B><font size="+1">';
  $string="";
  if ($startresult==0) $string.="last $realend ";
  $string.="deaths ";
  if (isset ($playername)) $string.="for $playername";
  echo ucfirst($string);
  echo "</font></B></U>";
  echo "<P>results $realstart to $realend";
  echo "<TABLE cellpadding=3 cellspacing=0>";
  $colors=array("#EEEEEE","#CCCCCC");
  for ($i=$startresult;($i<$rowcount)&&($i<($maxresult+$startresult));$i++){
        $array=database_fetch_array($deathresult,$i);
        $moment=database_strtotime ($array[0]);
        $player=$array[1];
        $killer=$array[2];
        $playershoot=$array[3];
        $screenshoot=$array[4];
        $bgcolor=$colors[$i%2];
        echo "<TR><TD bgcolor=\"$bgcolor\">";
        echo "<IMG SRC=\"png/gravestone2.111.png\">";
        echo "</TD><TD bgcolor=\"$bgcolor\">";
        echo date ("D, j F Y H:i:s T",$moment);
        echo "</TD><TD bgcolor=\"$bgcolor\">$player was killed by $killer";
        echo "</TD><TD bgcolor=\"$bgcolor\">";
        echo "<FORM action= \"get_death.php\" method=\"GET\">";
        echo "<input type=\"hidden\" name=\"playername\" value=\"$player\">";
        echo "<input type=\"hidden\" name=\"playershoot\" value=\"$playershoot\">";
        echo "<input type=\"hidden\" name=\"screenshoot\" value=\"$screenshoot\">";
        echo "<input type=\"submit\" value=\"Show details\">";
        echo "</FORM>";
        echo "</TD></TR>";
    }
    echo "</TABLE>";
    if ($startresult>0){
      $newstart=$startresult-$maxresult;
      if ($newstart<0) $newstart=0;
      if (isset($playername))
        echo "<A HREF=\"$PHP_SELF?playername=$playername&maxresult=$maxresult&startresult=$newstart\">";
      else
        echo "<A HREF=\"$PHP_SELF?maxresult=$maxresult&startresult=$newstart\">";
      echo "...previous</A>&nbsp;&nbsp;";
    }
    if ($rowcount>($maxresult+$startresult)){
      $newstart=$startresult+$maxresult;
      if (isset($playername))
        echo "<A HREF=\"$PHP_SELF?playername=$playername&maxresult=$maxresult&startresult=$newstart\">";
      else
        echo "<A HREF=\"$PHP_SELF?maxresult=$maxresult&startresult=$newstart\">";
      echo "next...</A>";
    }

?>
</BODY>
</HTML>