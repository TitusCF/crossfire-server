<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<?php if (!isset ($playername))
      {
      die ("This page should not be used directly!");
      }
?>

<HTML>
<HEAD>
  <TITLE>Informations about <?php echo $playername?></TITLE>
</HEAD>
<BODY bgcolor="#DDDDDD">
<?php
  require_once('database.php');
  database_connect();
  $status_result= database_exec ("select logmessage,moment from playerlog
           where player='$playername' and (logmessage='LOGIN' or logmessage='LOGOUT')
           order by moment DESC, counter DESC");
  $rowcount=@database_numrows ($status_result);
  if ($rowcount>0){
      list ($logmessage,$moment_log)=database_fetch_array ($status_result,0);
      $server_result=database_exec ("select event from server_events where moment>'$moment_log' and event=1");
      if (($logmessage=="LOGIN") && (database_numrows($server_result)<=0) && server_active())
          $status='<B><FONT SIZE="+2">online</FONT></B>';
      else $status='offline';
      echo "$playername is currently $status.<BR>";
  }
?>
<FONT size=+4><u><b>1. The birth</b></u></FONT>
<P>
<B>This all started there</B><P>
<?php

  function a_an ($name)
    {
    $char= strtolower ($name{0});
    if ( ($char=="a") || ($char=="e")|| ($char=="i")|| ($char=="o")|| ($char=="u") )
       return "an ".$name;
    else
       return "a ".$name;
    }
  $birthresult = @database_exec ("select moment,race,species,path from birth where player='$playername'");
  $rowcount=@database_numrows ($birthresult);
  if ($rowcount<1){
    echo "On a unknown Day, $playername decided to come to world. ";
    echo "He was poor and his parents died a few times later. ";
    echo "So nobody is there anymore to tell the story of $playername's youth. ";
    echo "<BR>That's all I knew about it at the moment I wrote those lines.";
  }
  else{
    $array=database_fetch_array($birthresult,0);
    $moment=database_strtotime ($array[0]);
    $race=$array[1];
    $species=$array[2];
    $path=$array[3];
    echo "<TABLE><TR><TD>";
    echo "It was the ". date ("jS F Y",$moment);
    echo ", a " . date ("l",$moment) . ". ";
    echo "At ". date ("H:i:s",$moment) . " $playername ";
    echo "opened his eyes for the first time.<BR>He was ".a_an($species).", like his parents.";
    if ($race != "(null)")
    {
        echo "<BR>For those who don't know, ".a_an($species)." is ". a_an(str_replace ("_"," ",$race)) ." creature.";
    }
    echo "<P>";
    if ($path!=''){
      echo "Quickly, $playername followed his own path. ";
      echo "He decided to become ".a_an($path).".<BR>";
    }
    echo "</TD><TD><IMG src=\"jpg/".$species.".jpg\" border=1></TD></TR></TABLE>";
  }
?>
<P>
<FONT size=+4><u><b>2. Wins and defeats</b></u></FONT>
<P>
<?php
  $colors=array("#EEEEEE","#CCCCCC");
  $arena_result=database_exec ("select winner, looser, moment from arena
                                where winner='$playername' or looser='$playername'
                                order by moment DESC, counter DESC");
  $numrows=database_numrows($arena_result);
  if ($numrows<1){
      echo ("Sorry, no arena combat for <B>$playername</B> in database!<BR>");
  }else{
      $maxresults=10;
      echo "<U>Last 10 arena result for $playername:</U><BR>";
      echo '<TABLE BORDER="0" cellpadding=3 cellspacing=0>';
      for ($i=0;($i<$numrows) && ($i<$maxresults);$i++){
          $array=database_fetch_array($arena_result,$i);
          $winner=$array[0];
          $looser=$array[1];
          $moment=$array[2];
          $bgcolor=$colors[$i%2];
          echo "<TR><TD align=\"right\" bgcolor=\"$bgcolor\"><I>";
          echo date ("D, j F Y H:i T",database_strtotime ($moment));
          echo "</I></TD><TD align=\"right\" bgcolor=\"$bgcolor\">";
          if ($winner==$playername) echo "<B>$winner</B>";
          else echo "<a HREF=\"get_player.php?playername=$winner\">$winner</a>";
          echo "</TD><TD bgcolor=\"$bgcolor\"> defeated ";
          echo "</TD><TD align=\"left\" bgcolor=\"$bgcolor\">";
          if ($looser==$playername) echo "<B>$looser</B>";
          else echo "<a HREF=\"get_player.php?playername=$looser\">$looser</a>";
          echo "</TD><TD align=\"left\" bgcolor=\"$bgcolor\">";
          $urlquery="?playername=".urlencode ($winner);
          $urlquery.="&moment=".urlencode($moment);
          echo "<a HREF=\"get_arena.php$urlquery\">Show details....</A>";
          echo "</TD></TR>\n";
      }
      echo "</TABLE>\n";
      if ($numrows>$maxresults){
          echo "<A HREF=\"arena.php?maxresults=20&playername=";
          echo urlencode($playername);
          echo "\">More results...</A>";
      }
  }
?>
<P>
<FONT size=+4><u><b>3. The cycle of life</b></u></FONT>
<P>
<?php
  echo "<IMG SRC=\"player_graph.php?playername=$playername\"><BR>";
  $deathresult = @database_exec ("select moment,killer,playershoot,screenshoot from death where player='$playername' order by moment DESC,counter DESC");
  $rowcount=@database_numrows ($deathresult);
  if ($rowcount<1){
    echo "The life of $playername is full of successes. ";
    echo "<BR><B><FONT COLOR=\"#FF0000\">He didn't die yet!</FONT></B>";
    echo "<BR>Well, at least as far as i can remember<P>";
  }else{
    echo "<TABLE cellpadding=3 cellspacing=0>";
    for ($i=0;$i<$rowcount;$i++){
        $array=database_fetch_array($deathresult,$i);
        $moment=database_strtotime ($array[0]);
        $killer=$array[1];
        $playershoot=$array[2];
        $screenshoot=$array[3];
        $bgcolor=$colors[$i%2];
        echo "<TR><TD bgcolor=\"$bgcolor\">";
        echo "<IMG SRC=\"png/gravestone2.111.png\">";
        echo "</TD><TD bgcolor=\"$bgcolor\">";
        echo date ("D, j F Y H:i:s T",$moment);
        echo "</TD><TD bgcolor=\"$bgcolor\">$playername was killed by $killer";
        echo "</TD><TD bgcolor=\"$bgcolor\">";
        echo "<FORM action= \"get_death.php\" method=\"GET\">";
        echo "<input type=\"hidden\" name=\"playername\" value=\"$playername\">";
        echo "<input type=\"hidden\" name=\"playershoot\" value=\"$playershoot\">";
        echo "<input type=\"hidden\" name=\"screenshoot\" value=\"$screenshoot\">";
        echo "<input type=\"submit\" value=\"Show details\">";
        echo "</FORM>";
        echo "</TD></TR>";
    }
    echo "</TABLE>";
  }
?>
</BODY>
</HTML>
