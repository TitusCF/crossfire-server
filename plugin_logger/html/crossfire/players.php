<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>List of know crossfire players</TITLE>
</HEAD>
<BODY>
<?php
  require_once ("database.php");
  database_connect();
  $result =database_exec ("select player from playerlog group by player");
  $rowcount = database_numrows ($result);
?>
<FONT size=+3><b>Select player you want infos on</b></FONT>
<FORM action="get_player.php" method="GET">
<SELECT name="playername">

<?php
  for ($i=0;$i<$rowcount;$i++){
        $array=database_fetch_array($result,$i);
        $player = $array[0];
        echo "<OPTION VALUE=\"$player\">$player</OPTION>";
  }
?>
</SELECT>
<INPUT type="submit" value="Show on!">
</FORM>
</BODY>
</HTML>