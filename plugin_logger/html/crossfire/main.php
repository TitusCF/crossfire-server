<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.0//EN">
<HTML>
<HEAD>
  <TITLE>Crossfire Server</TITLE>
</HEAD>
<BODY>
  <div align="center"><FONT size=+2><b>Welcome to crossfire server running on localhost</b></FONT></div>
  <BR>
  The current status of the server is<B>
<?php
  require_once ("database.php");
  if (server_active()) echo "active";
  else echo "down or asleep";
?></B>
</BODY>
</HTML>