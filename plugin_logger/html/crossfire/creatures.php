<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>Most in danger creature</TITLE>
</HEAD><BODY bgcolor="#DDDDDD">
<CENTER>
  <U><B><font size="+1">20 most in danger creatures</font></B></U>
  <TABLE BORDER="0" cellpadding=3><TR><TD><U>name</U></TD><TD><U>score</U></TD></TR>
  <?php
  require_once ('database.php');
  database_connect();
  $result= database_exec ("select count from creature_death order by count DESC");
  list ($maxdeath)=database_fetch_array($result,0);
  $result= database_exec ("select name,count from creature_death order by count DESC");
  $num_rows=database_numrows ($result);
  $maxresults=100;
  $colors=array("#EEEEEE","#CCCCCC");
  for ($i=0;($i<$num_rows) && ($i<$maxresults);$i++){
      list ($name,$count)=database_fetch_array ($result,$i);
      $relative=round (100*$count/$maxdeath,2);
      if ($relative<10) $relative=round (100*$count/$maxdeath,3);
      else if ($relative<1) $relative=round (100*$count/$maxdeath,4);
      $bgcolor=$colors[$i%2];
      echo "<TR><TD bgcolor=\"$bgcolor\">$name</TD>\n";
      echo "<TD align=\"left\" bgcolor=\"$bgcolor\">$relative</TD></TR>\n";
  };
  ?>
  </TABLE></CENTER>
</BODY>
</HTML>