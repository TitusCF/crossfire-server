<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>dialog</TITLE>
</HEAD>
<BODY>
  <TABLE cellpadding="3">
  <?php
  require_once ('database.php');
  database_connect();
  $result =database_exec ("select player,title,saywhat,dialogtype,moment from dialog order by moment DESC, counter DESC");
  $rowcount = database_numrows ($result);
  $maxresults=50;
  if ($rowcount>$maxresults) $rowcount=$maxresults;
  for ($i=$rowcount-1;$i>=0;$i--){
        $array=database_fetch_array($result,$i);
        list ($player,$title,$saywhat,$dialogtype,$moment)=$array;
        if ($player=='SERVER') $title="";
        else $title="(the $title)";
        switch ($dialogtype){
            case 1:
                $fgcolor="#0000C0";
                $fgcolor2="#000000";
                $starttag="";
                $endtag="";
                break;
            case 2:
                $fgcolor="#FF0000";
                $fgcolor2="#FF0000";
                $starttag="";
                $endtag="";
                break;
            case 3:
                $fgcolor="#707070";
                $fgcolor2="#707070";
                $starttag="<I>";
                $endtag="</I>";
                break;
        }
        //$when= date ("D, d F Y H:i:s T",strtotime($moment));
        $when= date ("H:i:s",database_strtotime($moment));
        $day= date ("d F Y",database_strtotime($moment));
        $time=database_strtotime ($moment);
        if ($day!=$oldday){
            $fullday= date ("D, d F Y H:i:s T",database_strtotime($moment));
            echo "<TR><TD align=\"center\"><B>$fullday</B></TD><TD>&nbsp;</TD></TR>";
            $oldday=$day;
        }
        else if (($previoustime) && (abs($time-$previoustime)>300)){
            echo "<TR><TD align=\"center\"><B>...</B></TD><TD>&nbsp;</TD></TR>";
        }
        $previoustime=$time;
        echo "<TR><TD>[$when]</TD><TD>";
        echo "$starttag<Font Color=\"$fgcolor\"><I><B>$player</B> $title<FONT SIZE=\"+1\">&gt;</FONT></I>&nbsp;&nbsp;";
        echo "</FONT><Font Color=\"$fgcolor2\">$saywhat</font> $endtag";
        echo "</TR></TD>";
  }
  ?>
  </TABLE>
</BODY>
</HTML>