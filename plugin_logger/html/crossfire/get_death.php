<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>A day like other ones</TITLE>
</HEAD>
<?php if ( (isset($playername) ) &&
           (isset($playershoot)) &&
           (isset($screenshoot))  &&
           (!isset($subframe)) ){

    echo '<frameset rows="80%,*">';
    echo "<!-- $PHP_SELF -->";
    echo '<FRAME NAME="shoot" SRC="';
    $playername=urlencode($playername);
    $playershoot=urlencode($playershoot);
    $screenshoot=urlencode($screenshoot);
    echo $PHP_SELF."?playername=$playername&playershoot=$playershoot&screenshoot=$screenshoot&subframe=1";
    echo '" MARGINHEIGHT=0 MARGINWIDTH=0 NORESIZE FRAMEBORDER=2>';
    echo '<FRAME NAME="infos" SRC="get_infos.php" MARGINHEIGHT=0 MARGINWIDTH=0 NORESIZE FRAMEBORDER=2>';
    echo '</FRAMESET>';
}
else{
?>
<BODY bgcolor="#FFFFFF">
<?php
if ( (!isset($playername) ) ||
     (!isset($playershoot)) ||
     (!isset($screenshoot)) )
                die ("Should not be invoked directly");
require_once ('database.php');
database_connect();
$screenshootresult = @database_exec ("select shootfile from screenshoot where id='$screenshoot'");
$count=@database_numrows($screenshootresult);
if ( !($screenshootresult) ||
     ($count<1) )
   die ("Error occured while trying to load screenshoot");
$screenshootarray=database_fetch_array($screenshootresult,0);
$screenshootfile=$screenshootarray[0];
$playershootresult=database_exec ("select title, inventoryfile, face,
     agilitylvl, agilityexp, mentallvl, mentalexp,
     physiquelvl, physiqueexp, magiclvl, magicexp,
     wisdomlvl, wisdomexp, personalitylvl, personalityexp,
     generallvl, generalexp, god FROM playershoot
     WHERE id='$playershoot'");
$count=@database_numrows($playershootresult);
if ( !($playershootresult) ||
     ($count<1) )
  die ("Error occured while trying to load player stats");
list ($title, $inventoryfile, $face,
      $agilitylvl, $agilityexp, $mentallvl, $mentalexp,
      $physiquelvl, $physiqueexp, $magiclvl, $magicexp,
      $wisdomlvl, $wisdomexp, $personalitylvl, $personalityexp,
      $generallvl, $generalexp, $god)=database_fetch_array($playershootresult,0);
require ("screenshoot.php");

function put_stat($statlvl,$statexp,$statname,$god="")
  {
  if ($statexp>0){
     echo ucfirst($statname).":";
     echo "<blockquote>level:&nbsp;$statlvl<BR>exp:&nbsp;".number_format($statexp);
     if ($god) echo "<BR>Worship&nbsp;$god";
     echo "</blockquote>";
  }
  else echo "No experience in $statname";
  }

function dump_inventory($filename)
  {
 // echo $filename;
  $handle=fopen ($filename,"r");
  echo '<table border=0  bgcolor="#FFFFFF" style="border-width: medium;  border-style: inset;">';
  while (!feof ($handle)){
     $buffer=fgets ($handle,4096); /*Should be enough*/
     list ($first,$description)=explode (" ",$buffer,2);
     if (!$first) continue;
     $arch=substr ($first,1,strlen($first)-2); /*clear []*/
     list ($archname,$active)= explode ("|",$arch,2);
     $description=rtrim($description);
     echo "\t\t<TR><TD nowrap><IMG SRC=\"png/$archname.png\"></TD><TD>$description";
     if (strcmp ($active,"Active")==0) echo (" (Ready)");
     echo "</TD></TR>\n";
     //echo $buffer;
  }
  fclose ($handle);
  echo "</table>";
  }

?>



   <TABLE cellpadding=4 cellspacing=0 align="center" bgcolor="#C0C0C0" border="0" width="100%"
   style="border-width: medium   ; border-style: outset   ; border-color: darkGray   ;">
     <TR>
       <TD colspan=3 style="border-style: inset"><center>Crossfire - <B><?=$playername?></B> the <I><?=$title?></I></center></TD>
     </TR>
     <TR>
       <TD rowspan=2 valign="top" style="border-width: thin; border-style: groove;">
       <u>Inventory:</u><BR>
       <?php
       dump_inventory ($crossfirebasedir.$inventoryfile);
       ?></TD>
       <TD align="center" style="border-width: thin; border-style: groove;">
       <TABLE BORDER=0 style="border-style: none;"><TR>
       <TD><?php echo "<IMG SRC=\"png/$face.png\">";?></TD>
       <TD>Level:<BR>Experience:</TD>
       <TD><?=$generallvl?><BR><?=number_format($generalexp)?></TD>
       </TR></TABLE>
       </TD>
       <TD rowspan=2 valign="top" style="border-width: thin; border-style: groove;">
       <u>Statistics:</u><BR>
       <table border=0  bgcolor="#FFFFFF" style="border-width: medium;  border-style: inset;"><TR><TD>
       <?php put_stat ($physiquelvl, $physiqueexp, "physique");?><BR>
       <?php put_stat ($magiclvl   , $magicexp   , "magic"   );?><BR>
       <?php put_stat ($wisdomlvl  , $wisdomexp  , "wisdom"  ,$god);?><BR>
       <?php put_stat ($mentallvl  , $mentalexp  , "mental"  );?><BR>
       <?php put_stat ($agilitylvl , $agilityexp , "agility" );?><BR>
       <?php put_stat ($personalitylvl ,$personalityexp ,"personality");?><BR>
       </TD></TR></TABLE></TD>
     </TR>

     <TR>
       <TD valign="top" align="center" style="border-width: thin; border-style: groove;"><?php put_screenshoot($screenshootfile);?></TD>
     </TR>
   </TABLE>
</BODY>
</HTML>
<?php
}
?>


