<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>About</TITLE>
</HEAD>
<BODY bgcolor="#FFFFFF">
  <?php
  if (isset($text)){

      $array=explode ("|",stripslashes($text));
      echo "<u>There you see:</u>";
      if (!$array) echo "Nothing\n";
      else{
          echo '<table border="0" width="100%"><tr><td valign="top"><ul>';
          $stop1=round(count($array)/3);
          $stop2=round((count($array)*2)/3);
          $cmpt=1;
          foreach ($array as $object){
              if (chop($object))echo "<li> $object<BR>\n";
              if ($cmpt==$stop1) echo "\t</ul></td>\n\t<td valign=\"top\"><ul>\n";
              if ($cmpt==$stop2) echo "\t</ul></td>\n\t<td valign=\"top\"><ul>\n";
              $cmpt++;
          }
          echo '</ul></td></tr></table>';
      }

  }
  ?>
</BODY>
</HTML>