<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>Official Arena annoucements</TITLE>
</HEAD>
<BODY bgcolor="#DDDDDD">
<TABLE border=0 align="center"><TR><TD>

<?php if (!isset ($maxresults)) $maxresults=20?>
  <u><b>Last <?=$maxresults?> arena results<?php
  if (isset($playername)) echo " for $playername";
  ?>:</b></u><P>
  <?php require_once ("database.php");
  database_connect();
  if (isset ($playername))
      $arena_result=database_exec ("select winner, looser, moment from arena
                                    where winner='$playername' or looser='$playername'
                                    order by moment DESC, counter DESC");
  else
      $arena_result=database_exec ("select winner, looser, moment from arena order by moment DESC, counter DESC");
  $numrows=database_numrows($arena_result);
  if ($numrows<1){
?>
  <b>Sorry</b>, no arena result available.<BR>
<?php
  }else{
    echo '<TABLE BORDER="0">';
    for ($i=0;($i<$numrows) && ($i<$maxresults);$i++){
    $array=database_fetch_array($arena_result,$i);
    $winner=$array[0];
    $looser=$array[1];
    $moment=$array[2];
    echo '<TR><TD align="right"><I>';
    echo date ("D, j F Y H:i T",strtotime ($moment));
    echo '</I></TD><TD align="right">';
    echo "<b><a HREF=\"get_player.php?playername=$winner\">$winner</a></b>";
    echo '</TD><TD> defeated ';
    echo '</TD><TD align="left">';
    echo "<b><a HREF=\"get_player.php?playername=$looser\">$looser</a></b>";
    echo '</TD><TD align="left">';
    $urlquery="?playername=".urlencode ($winner);
    $urlquery.="&moment=".urlencode($moment);
    echo "<a HREF=\"get_arena.php$urlquery\"><i>Show details....</i></A>";
    echo "</TD></TR>\n";
    }
    echo '</TABLE>';
?>
    <P><HR><P>
<?php
  echo "<A HREF=\"$PHP_SELF?maxresults=".($maxresults*2);
  echo '">Show more arena results';
  if (isset ($playername)) echo " for $playername";
  echo '...</A>';
  }
?>
    <BR><FORM action="#" method="POST">

      <SELECT name="maxresults">
          <OPTION VALUE="20">Show last 20 arena results</OPTION>
          <OPTION VALUE="30">Show last 30 arena results</OPTION>
          <OPTION VALUE="40">Show last 40 arena results</OPTION>
          <OPTION VALUE="50">Show last 50 arena results</OPTION>
          <OPTION VALUE="75">Show last 75 arena results</OPTION>
          <OPTION VALUE="100">Show last 100 arena results</OPTION>
          <OPTION VALUE="150">Show last 150 arena results</OPTION>
      </SELECT>
      <?php if (isset($playername))
      echo "<input type=\"hidden\" name=\"playername\" value=\"$playername\">"?>
      <INPUT type="submit" value="Show">
    </FORM>

</TD></TR></TABLE>
</BODY>
</HTML>