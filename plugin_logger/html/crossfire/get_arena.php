<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>Arena result</TITLE>
</HEAD><?php if ( (isset($playername) ) &&
           (isset($moment))  &&
           (!isset($subframe)) ){

    echo '<frameset rows="85%,*">';
    echo "<!-- $PHP_SELF -->";
    $playername=urlencode($playername);
    $momentname=urlencode($moment);
    echo '<FRAME NAME="shoot" SRC="';
    echo $PHP_SELF."?playername=$playername&moment=$momentname&subframe=1";
    echo '" MARGINHEIGHT=0 MARGINWIDTH=0 FRAMEBORDER=2>';
    echo "<FRAME NAME=\"infos\" SRC=\"get_infos.php\" MARGINHEIGHT=0 MARGINWIDTH=0 FRAMEBORDER=2>\n\n";
}else{
?>
<BODY>
<?php
if ((!isset($playername)) || (!isset($moment))){
?>
<u><b><FONT color="red"><FONT size=+3>Error:</FONT></FONT></b></u><p>
<FONT size=+1>You should not call this page direcly.<BR>
Instead, go to <A HREF="players.php">players list</A>.</FONT>
<?php
}else{
    require_once ("database.php");
    $arena_result = @database_exec ("select moment,winner,looser,winner_comment,looser_comment,screenshoot from arena where ((winner='$playername') or (looser='$playername')) AND moment='$moment' ");
    $rowcount= @database_numrows ($arena_result);
    if ($rowcount<1){?>

        <u><b><FONT color="red"><FONT size=+3>Error:</FONT></FONT></b></u><p>
        <FONT size=+1>We were unable to get to achieve your request for <?=$playername?>.<BR>
        Please go back to <A HREF="players.php">players list</A>.</FONT>
<?php
    }else{
        $array=database_fetch_array($arena_result,0);
        $moment=strtotime ($array[0]);
        $winner=$array[1];
        $looser=$array[2];
        $winner_comment=$array[3];
        $looser_comment=$array[4];
        $screenshoot=$array[5];
        require_once ('screenshoot.php');
        echo '<TABLE BORDER="2" align="center">';
        echo "<TR><TD align=\"center\" colspan=\"2\">$winner&nbsp;";
        echo '<FONT size=+4>VS</FONT>';
        echo "&nbsp;&nbsp;$looser</TD></TR>";
        echo '<TR><TD COLSPAN="2" align="center">';
        $screenshoot_result= @database_exec ("select shootfile from screenshoot where id='$screenshoot'");
        $array2=database_fetch_array($screenshoot_result,0);
        $shootfile=$array2[0];
        put_screenshoot ($shootfile);
        echo '</TD></TR><TR><TD colspan="2" align="center"><b><font size="+1">1-0</font></b></TD></TR>';
        echo "<TR><TD valign=\"top\" width=\"50%\"><u>The winner's comment:</u><BR>";
        echo nl2br($winner_comment);;
        echo "</TD><TD valign=\"top\" width=\"50%\"><u>The looser's comment:</u><BR>";
        echo nl2br($looser_comment);
        echo '</TD></TR></TABLE>';
    }
}
} /*End of frame maker!*/
?>
</BODY>
</HTML>