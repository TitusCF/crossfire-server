(This is a quick-and-dirty documentation for the plugin interface - expect a better one soon).

List of supported events.
=========================

Local events
------------
Those can be attached to a specific object in the game.

APPLY
Tag: event_apply
This event is generated whenever the object is applied or unapplied.

ATTACK
Tag: event_attack
This event is used in two cases:
- bound to a weapon, it is triggered each time the weapon is used to slay
  something; this can typically be used to generate special effects when
  you hit a monster;
- bound to a monster, it is triggered when the monster is attacked.

CLOSE
Tag: event_close
Generated when a container is closed.

DEATH
Tag: event_death
Generated when the object dies.

DROP
Tag: event_drop
Generated when the object is dropped, either on the floor or in a container.

PICKUP
Tag: event_pickup
Generated when the object is picked up.

SAY
Tag: event_say
Generated when someone says something around the object.

STOP
Tag: event_stop
Generated for a thrown object, when the object is stopped for some reason.

TIME
Tag: event_time
Generated each time the object gets an opportunity to move.

THROW
Tag: event_throw
Generated when the object is thrown.

TRIGGER
Tag: event_trigger
Used for various objects, like traps, teleporters or triggers. Generated when
those objects are used (for example, when a player passes through a teleporter)

Global events
-------------
Those concern the game as a whole or can't be bound to a specific object.
Those events may be "registered" by a plugin (it means that the plugin requests
to get a message each time one of those events happens).

BORN
Generated when a new character is created.

CLOCK
Generated at each game loop.
Warning: When no player is logged, the loop "stops", meaning that clock events
are not generated anymore !

CRASH
Generated when a server crash does occur. It is not a recursive event, so if a
crash occur from *inside* the crash event handling, it is not called a second
time, preventing infinite loops to occur.
Note: This event is not implemented for now.

GDEATH
Generated whenever someone dies.

GKILL
Generated whenever something/someone is killed.

LOGIN
Generated whenever a player logs into the game.

LOGOUT
Generated whenever a player logs out the game.

MAPENTER
Generated whenever someone enters a map.

MAPLEAVE
Generated whenever someone leaves a map.

MAPRESET
Generated each time a map is reset.

REMOVE
Generated when a player character is removed from the game ("quit" command).

SHOUT
Generated whenever someone shouts something.

The case of Python
==================

Here is how CFPython handles events:
- local events are managed using the event_... tags. The event_..._plugin tag
  should be Python. the event_... specifies the name of the Python script to
  run. This path is relative to the crossfire map subdirectory.

- global events are all registered by CFPython. If you want to react to one of
  them, you need to create script files in a python/ subdirectory of your
  crossfire map directory. Those files should be called:
  python_shout.py : for the shout global event;
  python_mapenter.py : for the mapenter global event;
  python_remove.py : for the remove global event;
  and so on.
  Some special values are also passed to CFPython for each global event. Those
  are:
  BORN : WhoIsActivator = The object representing the new character.
  LOGIN: WhoAmI and WhoIsActivator = the object representing the character.
         WhatIsMessage = The IP address of the player.
  LOGOUT: Same as for LOGIN.
  REMOVE: WhoIsActivator = The object representing the deleted char.
  SHOUT: WhoIsActivator = The object that shouted something.
         WhatIsMessage = The message shout.
  MAPENTER: WhoIsActivator = The object that entered the new map.
  MAPLEAVE: WhoIsActivator = The object that is leaving the map.
  MAPRESET: WhatIsMessage = The path name of the map reset.
  