<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>Most popular maps</TITLE>
</HEAD>
<BODY bgcolor="#DDDDDD">
  <CENTER><U><B><font size="+1">50 most popular maps</font></B></U>
  <TABLE BORDER="0" cellpadding=5><TR><TD align="right"><U>Path</U></TD><TD><U>name</U></TD><TD><U>score</U></TD></TR>
  <?php
  require_once ('database.php');
  database_connect();
  $result= database_exec ("select visittime from map_visits order by visittime DESC");
  list ($maxtime)=database_fetch_array($result,0);
  $result= database_exec ("select mappath,mapname,visittime from map_visits order by visittime DESC");
  $num_rows=database_numrows ($result);
  $maxresults=50;
  $colors=array("#EEEEEE","#CCCCCC");
  for ($i=0;($i<$num_rows) && ($i<$maxresults);$i++){
      list ($path,$name,$visittime)=database_fetch_array ($result,$i);
      $relative=round (100*$visittime/$maxtime,2);
      if ($relative<10) $relative=round (100*$visittime/$maxtime,3);
      else if ($relative<1) $relative=round (100*$visittime/$maxtime,4);
      $bgcolor=$colors[$i%2];
      echo "<TR><TD align=\"right\" bgcolor=\"$bgcolor\">$path</TD>\n";
      echo "<TD bgcolor=\"$bgcolor\">$name&nbsp;</TD>\n";
      echo "<TD align=\"left\" bgcolor=\"$bgcolor\">$relative</TD></TR>\n";
  };
  ?>
  </TABLE>
  </CENTER>
</BODY>
</HTML>