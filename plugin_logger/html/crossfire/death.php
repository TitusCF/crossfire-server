<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>Player deaths statistics</TITLE>
</HEAD>
<BODY>
<TABLE border=0 align="center"><TR><TD>
  <?php if (!isset ($maxresults)) $maxresults=20?>
  <u><b>Last <?=$maxresults?> deaths:</b></u><P>
  <?php require_once ("database.php");
  database_connect();
  $death_result=database_exec ("select player, killer, moment, playershoot, screenshoot from death order by moment DESC, counter DESC");
  $numrows=database_numrows($death_result);
  if ($numrows<1){
?>
  <b>Sorry,</b> no death result available.<BR>
<?php
  }else{
    echo '<TABLE BORDER="0">';
    for ($i=0;($i<$numrows) && ($i<$maxresults);$i++){
    $array=database_fetch_array($death_result,$i);
    $player=$array[0];
    $killer=$array[1];
    $moment=$array[2];
    $playershoot=$array[3];
    $screenshoot=$array[4];
    echo '<TR><TD align="right"><I>';
    echo date ("D, j F Y H:i T",strtotime ($moment));
    echo '</I></TD><TD align="right">';
    echo "<b><a HREF=\"get_player.php?playername=$player\">$player</a></b>";
    echo '</TD><TD align="left">';
    $request="?playername=$player";
    $request.="&playershoot=".urlencode ($playershoot);
    $request.="&screenshoot=".urlencode ($screenshoot);
    echo " was ";
    echo "<b><a HREF=\"get_death.php$request\">";
    echo " killed by $killer.";
    echo "</a></b>";
    echo "</TD></TR>\n";
    }
    echo '</TABLE>';
?>
    <P><HR><P>
    <?="<FORM action=\"$PHP_SELF\" method=\"GET\">"?>

      <SELECT name="maxresults">
          <OPTION VALUE="20">Show last 20 deaths</OPTION>
          <OPTION VALUE="30">Show last 30 deaths</OPTION>
          <OPTION VALUE="40">Show last 40 deaths</OPTION>
          <OPTION VALUE="50">Show last 50 deaths</OPTION>
          <OPTION VALUE="75">Show last 75 deaths</OPTION>
          <OPTION VALUE="100">Show last 100 deaths</OPTION>
          <OPTION VALUE="150">Show last 150 deaths</OPTION>
      </SELECT>
      <INPUT type="submit" value="Show">
    </FORM>
    <BR>
<?php
  echo "<A HREF=\"$PHP_SELF?maxresults=".($maxresults*2);
  echo '">Show more results...</A>';
  }
  ?>
</TD></TR></TABLE>
</BODY>
</HTML>