================
Crossfire Server
================
:Author: Crossfire Development Team <crossfire@metalforge.org>
:Website: http://crossfire.real-time.com/

Crossfire is a free, open-source, cooperative multi-player RPG and adventure
game. Since its initial release, Crossfire has grown to encompass over 150
monsters, 3000 areas to explore, an elaborate magic system, 13 races, 15
character classes, and many powerful artifacts scattered far and wide. Set
in a fantastical medieval world, it blends the style of Gauntlet, NetHack,
Moria, and Angband.

.. note:: You do not need the server to play Crossfire. Crossfire Server is
  only for those who wish to run their own server.

Installation
------------
Dependencies
~~~~~~~~~~~~
Crossfire should run on any machine with an ANSI C compiler installed. It
should compile on most POSIX compliant systems. To date, it has been known to
work on Linux, BSD, Windows, Solaris, and Mac.

In addition, there are a number of optional dependencies that make the game
even more fun. These include:

* Support for dynamically loaded libraries. Most modern operating systems
  should have it. This is needed for server plugins.

* Python 2.6 or later - this is needed for the Crossfire Python plugin
  (CFPython) and is *highly* recommended. Most maps, quests, and NPCs will
  not work without it.

* SQLite3 - for cflogger and cfnewspaper. These plugins are still under
  development and aren't very useful at the moment.

* Check - for unit tests. This is required for plugin developers, but mostly
  useless to normal users.

Support Files
~~~~~~~~~~~~~
In order to build the server, a copy of the game archetypes must be unpacked
or linked in the "lib/arch" directory. This can be deleted once the build is
finished, but keeping a copy around won't hurt. Perl is required to generate
the final archetype files, but is not needed after the build is complete.

In order to run the server, a copy of the game maps is also required.

Compiling
~~~~~~~~~
Crossfire Server uses autoconf/automake. Briefly::

  $ ./configure
  $ make
  $ make install

Generic instructions for using autoconf/automake are in `INSTALL <INSTALL>`_.

An older, step-by-step guide is available on the wiki:
http://wiki.metalforge.net/doku.php/server:server_compiling

Final Steps
~~~~~~~~~~~
- Configure your server (see *$PREFIX/etc/crossfire/*)
- Make maps available in *$PREFIX/share/crossfire/maps/*


Usage
-----
You should now be able to run the server::

  $ crossfire-server

Getting Help
------------
Website
~~~~~~~
Official Website:
http://crossfire.real-time.com/

Crossfire Wiki:
http://wiki.metalforge.net/doku.php

SourceForge Downloads Page:
http://sourceforge.net/projects/crossfire/files/

IRC
~~~
It's possible to link up with a small community of Crossfire players and
developers on irc.freenode.net in the #crossfire channel. Though activity
waxes and wanes, it's often possible to get live help there.

Mailing Lists
~~~~~~~~~~~~~
For the latest subscription for crossfire mailing lists, go to:
http://crossfire.real-time.com/mailinglists/index.html

There you can subscribe/unsubscribe any of the mailing lists.  Links to
archives of the mailing lists can be found there, as well as at:
http://gmane.org/find.php?list=crossfire

Contributing
------------
Reporting Bugs
~~~~~~~~~~~~~~
Bugs should be filed in the Crossfire bug tracker on SourceForge:
http://sourceforge.net/p/crossfire/bugs/

When reporting bugs, make sure you include the following:

* If you have a sourceforge login, make sure you are logged in.  This way,
  if there are questions about the bug, you will be notified about the
  request for more information.
* What version of crossfire did you use?
* What type of computer did you use (CPU type)
* What is the version of the OS?
* What compiler (and its version) did you use (ie, gcc, icc, etc)?
* Which flags did you give it?
* If the bug happens when compiling crossfire, send an EXACT copy of the
  compiler line, as well as the errors it puts out.  Sending bugs of the
  sort 'it failed with a line like ...' doesn't help at all.
* If you are using any of the plugins, what version(s) of Python and SQLite
  do you have installed?
* If the bug happened while running crossfire:

  - Include any output before to the bug.
  - Give a description of what you did before the bug occurred.  The better
    detailed the description, the better chance we have of figuring out
    where the bug happened, or how we can recreate the bug.
  - If possible, a stack trace from gdb (or other debugger) is very helpful.

The more information provided, the better chance of the bug being fixed in
a timely fashion.

Further debugging information can be found at
http://crossfire.real-time.com/debugging_tips.html

Submitting Patches
~~~~~~~~~~~~~~~~~~
See the `<doc/style-guide.txt>`_ file.

Patches should be sent to the Crossfire patch tracker on SourceForge:
http://sourceforge.net/p/crossfire/patches/

Copyright
---------
::

  Copyright (c) 2000, 2006 Mark Wedel
  Copyright (c) 1992 Frank Tore Johansen

  This program is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the Free
  Software Foundation; either version 2 of the License, or (at your option)
  any later version.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
  for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  675 Mass Ave, Cambridge, MA 02139, USA.

  The author can be reached via e-mail to crossfire-devel@real-time.com

See `<COPYING>`_ for more details.
