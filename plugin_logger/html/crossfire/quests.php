<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>Available quests</TITLE>
</HEAD>
<BODY bgcolor="#DDDDDD"><CENTER>
  <U><B><font size="+1">List of available quests:
<?php
  if (isset ($quest_name))
      echo "quests corresponding to $quest_name";
  else
      echo "last 20 map reset";
?>
  </font></B></U>
  <P><TABLE BORDER="0" cellpadding=3 cellspacing=0>
  <?php
  require_once ('database.php');
  database_connect();
  if (isset ($quest_name)){
      $quest_name="%".str_replace ("%","%%",strtoupper($quest_name))."%";
      $result =database_exec ("select path,name,state from map_state
                               where upper(path) LIKE '$quest_name' OR upper(name) LIKE '$quest_name'
                               ORDER BY moment DESC");
  }else
      $result =database_exec ("select path,name,state from map_state where state=0 ORDER BY moment DESC");
  $rowcount = database_numrows ($result);
  if (($rowcount>20) && (!isset($quest_name))) $rowcount=20;
  $colors=array("#EEEEEE","#CCCCCC");
  for ($i=0;$i<$rowcount;$i++){
      list ($path,$name,$state)=database_fetch_array ($result,$i);
      $bgcolor=$colors[$i%2];
      if ($name) echo "<TR><TD bgcolor=\"$bgcolor\">$name</TD>\n";
      else echo "<TR><TD bgcolor=\"$bgcolor\">$path</TD>\n";
      if ($state==1) $state_string="Quest is done or someone is doing it";
      else $state_string="Quest is available";
      echo "<TD bgcolor=\"$bgcolor\">$state_string</TD></TR>\n";
  }
  ?>
  </TABLE>
<P>
<FORM action="quests.php" method="GET">
Find a quest: <INPUT type="text" name="quest_name" value="Castle"><INPUT type="submit" value="Find">
</FORM>
</CENTER>
</BODY>
</HTML>