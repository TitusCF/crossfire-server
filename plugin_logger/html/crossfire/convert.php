<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>pictures massa convertion</TITLE>
</HEAD>
<BODY>
  <?php
  $sourceshoot="png/monk.111.png";
  $destinationfilename="gif/monk.111.gif";
  $im=imagecreatefrompng ($sourcefilename);
  $result=imagejpeg($im,$destinationfilename);
  ?>
</BODY>
</HTML>