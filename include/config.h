/*
 * static char *rcsid_config_h =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2000 Mark Wedel
    Copyright (C) 1992 Frank Tore Johansen

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    The author can be reached via e-mail to mwedel@scurz.net
*/

/* This file contains various #defines that select various options.
 * Some may not be desirable, and some just may not work.
 *
 * There are some options that are not selectable in this file which
 * may not always be undesirable.  An example would be certain
 * treasures that you may not want to have available.  To remove the
 * activation code would make these items worthless - instead remove
 * these from the treasure file.  Some things to look for are:
 *
 * prepare_weapon, improve_*: Allow characters to enchant their own
 *	weapons
 * ench_armour: Allow characters to enchant their armor.
 *
 * In theory, most of the values here should just be defaults, and 
 * everything here should just be selectable by different run time
 * flags  However, for some things, that would just be too messy.
 */

/* To be removed soon (setable by player) */
/* IF this is set, then the range type will not switch to skill when
 * you use a melee weapon - this better emulates pre-skill code.
 */

#define NO_AUTO_SKILL_SWITCH

/* Removed for 0.94.2:
 * CHRFONT - removed - not useful in client/server, or maybe even at all
 * NEW_IMPROVE_WEAPON - now standard on all releases.
 * NO_LOG - removed - can't see why anyone would want to enable it.
 * ONE_PLAYER_PR_UID - removed from option list - not useful in client/server
 *  model
 * SIMPLE_PARTY_SYSTEM - now standard part of game
 */
/* 0.94.2 - rearrange this file some.  There are 4 main sections -
 * Section 1 is feature selection (enabling/disabling certain features)
 *
 * Section 2 is compiler/machine dependant section (stuff that just
 *     makes the program compile and run properly, but don't change the
 *     behavior)
 *
 * Section 3 is location of certain files and other defaults.  Things in
 *     this section generally do not need to be changed, and generally do
 *     not alter the play as perceived by players.  However, you may
 *     have your own values you want to set here.
 *
 * Section 4 deals with save file related options.
 */

 /*******************************************************************
 * SECTION 1 - FEATURES
 *
 * You don't have to change anything here to get a working program, but
 * you may want to on personal preferance.  Items are arranged
 * alphabetically.
 *
 * Short list of features, and what to search for:
 * ALCHEMY - enables alchemy code
 * ALLOW_SKILLS - enables skills code.
 * BALANCED_STAT_LOSS - Based death stat depletion on level etc?
 * CASTING_TIME - makes spells take time to cast
 * CS_LOGSTATS - log various new client/server data.
 * DEBUG - more verbose message logging?
 * EXPLORE_MODE - add an explore mode method of play?
 * FULL_RING_DESCRIPTION - makes rings abilities show in inventory window
 * MAX_TIME - how long an internal tick is in microseconds
 * MANY_CORES - generate core dumps on gross errors instead of continuing?
 * MULTIPLE_GODS - adds numerous gods to the game, with different powers
 * NO_CONE_PROPOGATE - makes cone spells stop at the first monster
 * NOT_PERMADEATH - should death be the final end or not?
 * PARTY_KILL_LOG - stores party kill information
 * RANDOM_ENCOUNTERS - add in special encounter maps?
 * REAL_WIZ - changes wiz stuff to be more mudlike.
 * RECYCLE_TMP_MAPS - use tmp maps across multiple runs?
 * RESURRECTION - lets players bring other dead players back to life.
 * SEARCH_ITEMS - let players search for items on the ground
 * SECURE - Allow overriding values with run time flags?
 * SET_TITLE - lets players set their title
 * SIMPLE_EXP - uses simpler experience system
 * SPELL_* - various spell related options
 * STAT_LOSS_ON_DEATH - toggle between stat loss or stat depletion
 * USE_LIGHTING - enable light/darkness & light sources
 * USE_PERMANENT_EXPERIENCE - allow players to accumulate permanent experience?
 * USE_SWAP_STATS - allows stat swapping for new characters
 * WATCHDOG - allows use of an external watchdog program
 *
 ***********************************************************************/

/* Enables Brian Thomas's new alchemy code.  Might unbalance the game - lets
 * characters make potions and other special items.
 * 0.94.2 note - I think enough work has been done, and even some maps
 * changed that this really should probably be a standard feature now.
 */

#define ALCHEMY


/* Uncommenting the following line allows the new skill code to be used
 * (look at skills.h to see some of the skill names.)  Allowing skills
 * can really change the balance of the game, and thus might be undesirable.
 * A few notes:  The following toggle just makes it so that the player can
 * not use skills (hopefully it will work).  But the skills will still exist,
 * so skills could still be gained with this selected.  In fact, some races
 * start with skills.
 * 0.94.2: This should probably also be standard part of the game.
 */

#define ALLOW_SKILLS



/* Use balanced stat loss code?
 * This code is a little more merciful with repeated stat loss at lower
 * levels. Basically, the more stats you have lost, the less likely that
 * you will lose more. Additionally, lower level characters are shown
 * a lot more mercy (there are caps on how much of a stat you can lose too).
 * On the nasty side, if you are higher level, you can lose mutiple stats
 * _at_once_ and are shown less mercy when you die. But when you're higher
 * level, it is much easier to buy back your stats with potions.
 * Turn this on if you want death-based stat loss to be more merciful
 * at low levels and more cruel at high levels.
 * Only works when stats are depleted rather than lost. This option has
 * no effect if you are using genuine stat loss.
 *
 * The BALSL_.. values control this behaviour.
 * BALSL_NUMBER_LOSSES_RATIO determines the number of stats to lose.
 * the character level is divided by that value, and that is how many
 * stats are lost.
 *
 * BALSL_MAX_LOSS_RATIO puts the upper limit on depletion of a stat - 
 * basically, level/max_loss_ratio is the most a stat can be depleted.
 *
 * BALSL_LOSS_CHANCE_RATIO controls how likely it is a stat is depleted.
 * The chance not to lose a stat is 
 * depleteness^2 / (depletedness^2+ level/ratio).
 * ie, if the stats current depleted value is 2 and the character is level
 * 15, the chance not to lose the stat is 4/(4+3) or 4/7.  The higher the
 * level, the more likely it is a stat can get really depleted, but
 * this gets more offset as the stat gets more depleted.
 * 
 */
/* GD */

#define BALANCED_STAT_LOSS FALSE
#define BALSL_LOSS_CHANCE_RATIO    4
#define BALSL_NUMBER_LOSSES_RATIO  6
#define BALSL_MAX_LOSS_RATIO       2

/* casting times for spells, if you have this defined then it takes a
 * specific amount of time for a spell to go off. You may attack or
 * be hit during this time, and this will cause you to lose the spell.
 * I commented this out on my copy - I don't like the 'double clutch'
 * needed to cast it (first direction key starts casting, the
 * next actually puts the spell into effect.)  It could perhaps
 * be changed so that the initial direction is where it goes, and it
 * is cast automatically in that direction (so only 1 key is needed.)
 * But this could be undesirable as various things move while you cast it.
 */
/*
#define CASTING_TIME
*/

/* CS_LOGSTATS will cause the server to log various usage stats
 * (number of connections, amount of data sent, amount of data received,
 * and so on.)  This can be very useful if you are trying to measure
 * server/bandwidth usage.  It will periodially dump out information 
 * which contains usage stats for the last X amount of time.
 * CS_LOGTIME is how often it will print out stats.
 */
#define CS_LOGSTATS
#define CS_LOGTIME  600

/* DEBUG generates copious amounts of output.  I tend to change the CC options
 * in the crosssite.def file if I want this.  By default, you probably
 * dont want this defined.
 */
#ifndef DEBUG
#define DEBUG
#endif


/* if EXPLORE_MODE is defined, it allows players to enter explore mode,
 * where they can not die.  Unlike other games (nethack for instance) explore
 * modes, the only thing Crossfire explore mode does is prevent death.
 * Characters in explore mode will not be able to go on the scoreboard.  Also,
 * once explore mode is entered, there is no return.  Explore mode can
 * only be entered if the person doing so is the only player in the
 * game, and once done, that person can not add new players to the
 * game.  To get into explore mode, type 'explore
 * Explore mode code added by Mark Wedel (mark@pyramid.com)
 * It's a good idea to turn off EXPLORE_MODE if you will only be running
 * on server, since it would block new players.
 */
/*
#define EXPLORE_MODE
*/


/* FULL_RING_DESCRIPTION has crossfire prints out the abilities of
 * rings or amulets in the appropriate windows.  So instead of just
 * have 'ring' get printed in your inventory, ring (Cha+1) or
 * amulet (reflect missiled), etc, will be printed out.  This also applies
 * to items laying on the ground.
 * No more information will be gathered than if you left click the
 * item (so, if the item is not identified, you will see nothing
 * except the item name.)  Tastes may vary on using this option.  It
 * can make the name excessively long, especially for unpaid items,
 * causing the (unpaid) to be cut off.  On the other hand, it can save
 * a lot of time, especially if you have several rings, or are looking
 * for one in a shop with lots of rings.
 * 0.94.2 - this will likely be removed in the not too distant future - 
 * client should really deal/emulate the short names if that is desired.
 */

#define FULL_RING_DESCRIPTION


/*
 * This option creates more core files.  In some areas, there are certain
 * checks done to try and make the program more stable (ie, check
 * parameter for null, return if it is).  These checks are being done
 * for things that should not happen (ie, being supplied a null parameter).
 * What MANY_CORES does, is if one of these checks is true, it will
 * dump core at that time, allowing for fairly easy tracking down of the
 * problem.  Better to fix problems than create thousands of checks.
 */

#define MANY_CORES


/*
 * If you feel the game is too fast or too slow, change MAX_TIME.
 * You can experiment with the 'speed <new_max_time> command first.
 * The length of a tick is MAX_TIME microseconds.  During a tick,
 * players, monsters, or items with speed 1 can do one thing.
 */

#define MAX_TIME	120000


/* Following allows characters to worshsip different gods.  Has no effect
 * if ALLOW_SKILLS (above) is not defined.
 * 0.94.2 - this probably also needs to be standard part of the game.
 */

#define MULTIPLE_GODS


/* This is a modification to the cone spells.  If a cone spell hits
 * a monster, the spell stops progressing (same is true for parties
 * of players).  This weakens the cones spells a bit.  If it is not
 * define, you get the old behaviour (cone spells cover every square
 * up to their range in a 90 degree arc.
 */
/*
#define NO_CONE_PROPOGATE
*/

/* NOT_PERMADEATH by Charles Henrich (henrich@crh.cl.msu.edu), April 9, 1993
 *
 * Makes death non permanent.  If enabled and you die, you lose a bunch of
 * exp, a random stat, and go back to starting town.
 * If not defined, if you die, you are dead.  If an a multiplayer server,
 * resurrection may be possible
 *
 * This option changes the game significantly one way or the other - it makes
 * adventuring right at the edge of death something more reasonable to do
 * (death still hurts here).  On the other hand, it certainly makes the
 * game a bit safer and easier.
 */

#define NOT_PERMADEATH


/* This determine how many entries are stored in the kill log.  You
 * can see this information with the 'party kills' command.  More entries
 * mean slower performance and more memory.  IF this is not defined, then
 * this feature is disabled.
 */
/*
#define PARTY_KILL_LOG 20
*/

/* RANDOM_ENCOUNTERS determines if random encounter maps should be
 * done.  What this means is that if you are in natural terrain
 * (grass, brush, trees, jungle, etc), there is a chance as you step into
 * that terrain that a 3x3 area will become a random encounter.  You
 * automatically enter this encounter map, monsters are placed in it,
 * and you remain in it until you get to the outside of the encounter
 * map.  Reasons for having it active might be to make things a little
 * more interesting (players can no longer be absolutely safe bringing
 * stuff back to town), and it gives low level characters a place to beat
 * up an endless supply of some monsters.  Reasons for disabling is because
 * they might just be a nuisance, or you just don't like them.  It
 * may also be broken (I have gotten one report of it putting characters
 * back to the wrong map.).  I personally don't recommend its use because
 * you will get random encounters in many maps where you really should
 * not get them (any maps with appropriate terrain may get a random
 * encounter, and this includes things like dungeons or adventure maps that
 * actually have a few spaces of the terrain).  For random encounters to
 * be useful, it needs to be changed so that by default maps do not have
 * random encounters unless that map is explicit set for it to be OK.
 */
/*
#define RANDOM_ENCOUNTERS
*/

/* Calling this REAL_WIZ is probably not really good.  Something like
 * MUD_WIZ might be a better name.
 *
 * Basically, if REAL_WIZ is define then the WIZ/WAS_WIZ flags for objects
 * are not set - instead, wizard created/manipulated objects appear as
 * normal objects.  This makes the wizard a little more mudlike, since
 * object manipulations will be usable for other objects.
 */

#define REAL_WIZ


/*
 * Set this if you want the temporary maps to be saved and reused across
 * crossfire runs.  This can be especially useful for single player
 * servers, but even holds use for multiplayer servers.  The file used
 * is updated each time a temp map is updated.
 * Note that the file used to store this information is stored in
 * the LIB directory.  Running multiple crossfires with the same LIB
 * directory will cause serious problems, simply because in order for
 * this to really work, the filename must be constant so the next run
 * knows where to find the information.
 */
/*
#define RECYCLE_TMP_MAPS
*/

/* define RESURECTION if you want to let players bring other players
 * back to life via some spells.  If RESURRECTION is undefined, then a
 * death is permanent.  NOTE: RESURRECTION (either defined or undefined)
 * has no meaning if NOT_PERMADEATH is defined.  SAVE_PLAYER also needs
 * to be defined for this to have any use
 */

#define RESURRECTION


/*
 * Enables the 'search-item command; a method to find equipment
 * in shops. 
 * This is somewhat useful is FULL_RING_DESCRIPTION is enabled (above.)
 * It matches by substrings of the name of the item as it appears in
 * the look window.  Therefor, if FULL_RING_DESCRIPTION is not enabled,
 * you can never match by ring abilities, as these are then not
 * displayed in the look window.
 * Seems like it now works, and doesn't cause the game to hang.
 * 0.94.2 - I think this really should be a client issue - after all, the
 * client has all the info the player does.
 */

#define SEARCH_ITEMS


/*
 * If SECURE is defined, crossfire will not accept enviromental variables
 * as changes to the LIBDIR and other defines pointing to specific
 * directories.  The only time this should really be an issue if you are
 * running crossfire setuid/setgid and someone else could change the
 * values and get special priveledges.  If you are running with normal uid
 * privelidges, then this is not any more/less secure (user could compile
 * there own version pointing wherever they want.)  However, having nonsecure
 * can make debugging or other developement much easier, because you can
 * change the paths without forcing a recompile.
 *
 * NOTE: Prior to 0.93.2, the default was for this to be defined (ie,
 * could not change values on the command line/through environmental
 * variables.)
 */
/*
#define SECURE
*/

/*
 * Define the following if you wish to allow players to set their title
 * and to save it on the disk
 * 0.94.2 - this will probably become a default option sometime unless
 * someone has a good reason why it shouldn't
 */

#define SET_TITLE

/*
 * Set to true if you want to use the new (0.95.3) experience system.
 * The system has the following changes:
 *
 * 1) 2000 exp need for second level.  All additional levels are shifted
 *  accordingly.
 * 2) Your stats in your skill categories don't affect the amount of
 *  exp you get.
 * 3) Your level difference doesn't affect the exp you get for killing
 *  monsters.
 *
 * If this is set to FALSE out, you get the old system.
 *
 * This can also be set by using -simple-exp/+simple-exp as a run
 * time option.
 */

#define SIMPLE_EXP TRUE

/*  SPELLPOINT_LEVEL_DEPEND  --  Causes the spellpoint cost
 *  of spells to vary with their power.  Spells that become very
 *  powerful at high level cost more.  The damage/time of
 *  characters increases though.
 */

#define SPELLPOINT_LEVEL_DEPEND


/* SPELL_ENCUMBRANCE -- If you're carrying a weapon or wearing heavy armour,
  you have a chance of fumbling your spellcasting with this on.  Formula is:
  encumbrance = 3*weapon_weight + armour_weight

  failure if roll from 1-200 is less than encumbrance + sp->level -
  op->level - 35
*/

#define SPELL_ENCUMBRANCE

/* SPELL_FAILURE_EFFECTS only has meaing if SPELL_ENCUMBRANCE is defined.
 * What it does, is that when the player fails a spell, various effects
 * will happen (player is paralyzed, confused, wonder spell is cast, etc.)
 * I disabled it because I think it would make life much too hazardous
 * for low level casters.  They either need to wear light armor (which
 * means that they will get pounded on by monsters), or will get
 * confused/paralyzed/other effects often.  High level casters would
 * be mostly unaffected, since they would be casting spells that are
 * below their level.
 * Note-  it seems that you still get some failur effects even with this
 * not defined - most notably when reading scrolls and fail to read
 * them properly.
 */

/* #define SPELL_FAILURE_EFFECTS */


/* Set this to FALSE if you don't want characters to loose a random stat when
 * they die - instead, they just get deplete.
 * Setting it to TRUE keeps the old behaviour.  This can be
 * changed at run time via -stat_loss_on_death or +stat_loss_on_death.
 * In theory, this can be changed on a running server, but so glue code
 * in the wiz stuff would need to be added for that to happen.
 */

#define STAT_LOSS_ON_DEATH FALSE

/*
 * The following enables the new light/darkness code.  Basically, darkness
 * affects line of sight, depending how dark the actual map is.  There are
 * also objects that create light (torches, spells, etc), which counteract
 * this effect.
 * 0.94.2 - as per alchemy, this should probably be standard now
 */

#define USE_LIGHTING

/* Use permanent experience code?
 * This code allows players to build up a small amount of 'permanent
 * experience' which reduces the effect of large experience drains, such as
 * death. This makes multiple frequent deaths less devastating, and also
 * ensures that any character will make some gradual progress even if they
 * die all of the time.
 * A nice option if your keep dying due to massive client/server lags despite
 * playing well... or you like to swim well outside of your depth. :)
 *
 * The PERM_EXP values adjust the behaviour of this option - if
 * USE_PERMAMENT_EXPERIENCE if off, these values have no meaning.  If it
 * is on, the minimum ratio is the minimum amount of permanent exp relative
 * to the total exp in the skill (ie, at a default of .25, if you had 100
 * experience, at least 25 of it would be permanent).  The gain ratio
 * is how much of experienced experience goes to the permanent value.
 * This does not detract from total exp gain (ie, if you gained 100 exp,
 * 100 would go to the skill total and 10 to the permanent value).
 * the loss ratio is the maximum amount of experience that can be lost
 * in any one hit - this is calculated as total exp - perm exp * loss ratio.
 *
 * A few thoughts on these default value (by MSW)
 * gain ratio is pretty much meaningless until exp has been lost, as until
 * that poin, the minimum ratio will be used.
 * It is also impossible for the exp to actually be reduced to the permanent
 * exp ratio - since the loss ratio is .5, it will just get closer and
 * closer.  However, after about half a dozen hits, pretty much all the
 * exp that can be lost has been lost, and after that, only minor loss
 * will occur.
 */
/* GD */

#define USE_PERMANENT_EXPERIENCE FALSE
#define PERM_EXP_MINIMUM_RATIO        0.25f
#define PERM_EXP_GAIN_RATIO           0.10f
#define PERM_EXP_MAX_LOSS_RATIO       0.50f

/*
 * SWAP_STATS is fixed now - rgg.
 *
 * Swap stats allows players to swap the values for their stats around
 * in the "Roll again" phase of the character selection.  This should
 * hopefully make character selection quicker, allowing for quicker
 * game start ups. 
 * Bugs:    c.blackwood@rdt.monash.edu.au
 * SORT_ROLLED_STATS - only useful with USE_SWAP_STATS - will order
 * stats from lowest to highest on initial roll.
 */ 

#define	USE_SWAP_STATS
#define SORT_ROLLED_STATS

/*
 * WATCHDOG lets sends datagrams to port 13325 on localhost
 * in (more-or-less) regular intervals, so an external watchdog
 * program can kill the server if it hangs (for whatever reason).
 * It shouldn't hurt anyone if this is defined but you don't
 * have an watchdog program.
 */

#define WATCHDOG

/***********************************************************************
 * SECTION 2 - Machine/Compiler specific stuff.
 *
 * Short list of items:
 * COMPRESS_SUFFIX - selection of compression programs
 * O_NDELAY - If you don't have O_NDELAY, uncomment it.
 *
 ***********************************************************************/

/*
 * If you compress your files to save space, set the COMPRESS_SUFFIX below
 * to the compression suffix you want (.Z, .gz, .bz2).  The autoconf
 * should already find the program to use.  If you set the suffix to
 * something that autoconf did not find, you are likely to have serious
 * problems, so make sure you have the appropriate compression tool installed
 * before you set this.  You can look at the autoconf.h file to see
 * what compression tools it found (search for COMPRESS).
 * Note that this is used when saving files.  Crossfire will search all
 * methods when loading a file to see if it finds a match
 */

#ifndef COMPRESS_SUFFIX
/* #define COMPRESS_SUFFIX ".Z" */
#endif

/* If you get a complaint about O_NDELAY not being known/undefined, try
 * uncommenting this.
 * This may cause problems - O_NONBLOCK will return -1 on blocking writes
 * and set error to EAGAIN.  O_NDELAY returns 0.  This is only if no bytes
 * can be written - otherwise, the number of bytes written will be returned
 * for both modes.
 */

/*
#define O_NDELAY O_NONBLOCK
*/


/***********************************************************************
 * Section 3
 *
 * General file and other defaults that don't need to be changed, and
 * do not change gameplay as percieved by players much.  Some options
 * may affect memory consumption however.
 *
 * Values:
 *
 * BANFILE - ban certain users/hosts.
 * CSPORT - port to use for new client/server
 * DMFILE - file with dm/wizard access lists
 * DM_MAIL - address of server administrator
 * DUMP_SWITCHES - enable -m? flags for spoiler generation
 * LIBDIR - location of archetypes & other data.
 * LOGFILE - where to log if using -daemon option
 * MAP_ - various map timeout and swapping parameters
 * MAX_OBJECTS - how many objects to keep in memory.
 * MAX_OBJECTS_LWM - only swap maps out if below that value
 * MOTD - message of the day - printed each time someone joins the game
 * PERM_FILE - limit play times
 * SHUTDOWN - used when shutting down the server
 * SOCKETBUFSIZE - size of buffer used internally by the server for storing
 *    backlogged messages.
 * TMPDIR - directory to use for temp files
 * UNIQUE_DIR - directory to put unique item files into
 * USE_CALLOC for some memory requests
 ***********************************************************************
 */

/*
 * BANFILE - file used to ban certain sites from playing.  See the example
 * ban_file for examples.
 */

#ifndef BANFILE
#define BANFILE         "ban_file"
#endif


/* CSPORT is the port used for the new client/server code.  Change
 * if desired.  Only of relevance if ERIC_SERVER is set above
 */

#define CSPORT 13327 /* old port + 1 */


/*
 * DMFILE
 * A file containing valid names that can be dm, one on each line.  See
 * example dm_file for syntax help.
 */

#ifndef DMFILE
#define DMFILE "dm_file"
#endif


/*
 * Where to send error-reports.  If you're hacking at the source, define
 * this as your mail-address.  You do not need to define this - you can
 * leave it undefined if you don't want the information available from this
 * and instead use some other mean (motd for example)
 */

#ifndef DM_MAIL
/* #define DM_MAIL "mark@pyramid.com" */
#endif

/*
 * If you want to regenerate the spoiler.ps file, you have to recompile
 * the game with DUMP_SWITCHES defined.  If defined, it turns on the
 * -m -m2 -m3 -m4 switches.  There is probably no reason not to define
 * this
 */

#define DUMP_SWITCHES

/* LOGFILE specifies which file to log to when playing with the
 * -daemon option.
 */

#ifndef LOGFILE
#define LOGFILE "/tmp/cross.log"
#endif

/*
 * MAP_MAXTIMEOUT tells the maximum of ticks until a map is swapped out
 * after a player has left it.  If it is set to 0, maps are
 * swapped out the instant the last player leaves it.
 * If you are low on memory, you should set this to 0.
 * Note that depending on the map timeout variable, the number of
 * objects can get quite high.  This is because depending on the maps,
 * a player could be having the objects of several maps in memory
 * (the map he is in right now, and the ones he left recently.)
 * Each map has it's own TIMEOUT value and value field and it is
 * defaulted to 300
 *
 * Having a nonzero value can be useful: If a player leaves a map (and thus
 * is on a new map), and realizes they want to go back pretty quickly, the
 * old map is still in memory, so don't need to go disk and get it.
 *
 * MAP_MINTIMEOUT is used as a minimum timeout value - if the map is set
 * to swap out in less than that many ticks, we use the MINTIMEOUT value
 * velow.  If MINTIMEOUT > MAXTIMEOUT, MAXTIMEOUT will be used for all
 * maps.
 */

/* How many ticks till maps are swapped out */
#define MAP_MAXTIMEOUT	1000
/* At least that many ticks before swapout */
#define MAP_MINTIMEOUT  500

/*
 * MAP_RESET tells whether map is reset after some time.  If it is defined,
 * the game uses weight variable of map object to tell, after how many seconds
 * the map will be reset.  If MAP_RESET is undefined, maps will never reset.
 *
 * MAP_MAXRESET is the maximum time a map can have before being reset.  It
 * will override the time value set in the map, if that time is longer than
 * MAP_MAXRESET.  This value is in seconds.  If you are low on space on the
 * TMPDIR device, set this value to somethign small.  The default
 * value in the map object is 7200 (2 hours)
 * I personally like 1 hour myself, for solo play.  It is long enough that
 * maps won't be resetting as a solve a quest, but short enough that some
 * maps (like shops and inns) will be reset during the time I play.
 * Comment out MAP_MAXRESET time if you always want to use the value
 * in the map archetype.
 */

#define MAP_RESET
#define MAP_MAXRESET	7200

/* CHECK_ACTIVE_MAPS is in certain functions to determine how often
 * the process_active maps function should be called.  In general, a simple
 * count variable is used (ie, every CHECK_ACTIVE_MAPS objects loaded/saved
 * or pixmaps built, call the function.  The process_maps function will
 * return without doing any processing if not enough time has elapsed.
 * In general, a too low value for CHECK_ACTIVE_MAPS will cause some
 * loss in performance, a too high value will mean that the function is
 * not called enough, causing uneven movement.
 * If CHECK_ACTIVE_MAPS is 0, then the function is never called.
 * Also see PROCESS_WHILE_LOADING below.
 */

#define CHECK_ACTIVE_MAPS 25

/* This option will cause process_maps to be called while maps are
 * being loaded and saved.  This has the advantage that other
 * players (the one that did not enter a new map) can do whatever they
 * want while the map is loading, and are not frozen.  Same is true
 * when a map is being frozen.
 *
 * If you are on a sufficiently fast machine, the map loading and
 * saving time may be small enough, that the players don't notice
 * anything, so setting this might not be desirable.
 *
 * Also, there used to be bugs associated with this - players entering maps
 * to find that all the exits are closed, appearing the middle the
 * ocean as the exit a map, etc.  Hopefully, I have fixed those bugs,
 * but it is difficult to be sure.  If you set this, and find those
 * things happening, turn this option off, and please let me know.
 *
 * Apparantly, there are still some bugs associated with this option, so
 * it is better to leave commented out.
 *
 * Mark Wedel (mark@pyramid.com)
 */
/*#define PROCESS_WHILE_LOADING*/

/*
 * MAX_OBJECTS is no hard limit.  If this limit is exceeded, crossfire
 * will look for maps which are already scheldued for swapping, and
 * promptly swap them out before new maps are being loaded.
 * If playing only by yourself, this number can probably be as low as
 * 3000.  If in server mode, probably figure about 1000-2000 objects per
 * active player (if they typically play on different maps), for some guess
 * on how many to define.  If it is too low, maps just get swapped out
 * immediately, causing a performance hit.  If it is too high, the program
 * consumes more memory.  If you have gobs of free memory, a high number
 * might not be a bad idea.  Each object is around 300 bytes right now.
 */

#define MAX_OBJECTS	6000

/*
 * Max objects low water mark (lwm).  If defined, the map swapping strategy
 * is a bit different:
 * 1) We only start swapping maps if the number of objects in use is
 *    greater than MAX_OBJECTS above.
 * 2) We keep swapping maps until there are no more maps to swap or the number
 *    of used objects drop below this low water mark value.
 *
 * If this is not defined, maps are swapped out on the timeout value above,
 * or if the number of objects used is greater than MAX_OBJECTS above.
 *
 * Note:  While this will prevent the pauses noticed when saving maps, there
 * can instead be cpu performance penalties - any objects in memory get
 * processed.  So if there are 4000 objects in memory, and 1000 of them
 * are living objects, the system will process all 1000 objects each tick.
 * With swapping enable, maybe 600 of the objects would have gotten swapped
 * out.  This is less likely a problem with a smaller number of MAX_OBJECTS
 * than if it is very large.
 * Also, the pauses you do get can be worse, as if you enter a map with
 * a lot of new objects and go above MAX_OBJECTS, it may have to swap out
 * many maps to get below the low water mark.
 */

/*#define MAX_OBJECTS_LWM	MAX_OBJECTS/2*/

/*
 * If you want to have a Message Of The Day file, define MOTD to be
 * the file with the message.  If the file doesn't exist or if it
 * is empty, no message will be displayed.
 * (It resides in the LIBDIR directory)
 */

#ifndef MOTD
#define MOTD "motd"
#endif

/*
 * You can restrict playing in certain times by creating a PERMIT_FILE
 * in LIBDIR. See teh sample for usabe notes.
 */

#define PERM_FILE "forbid"

/*
 * If you want to take the game down while installing new versions, or
 * for other reasons, put a message into the SHUTDOWN_FILE file.
 * Remember to delete it when you open the game again.
 * (It resides in the LIBDIR directory)
 */

#ifndef SHUTDOWN_FILE
#define SHUTDOWN_FILE "shutdown"
#endif


/*
 * SOCKETBUFSIZE is the size of the buffer used internally by the server for
 * storing backlogged messages for the client.  This is not operating system
 * buffers or the like.  This amount is used per connection (client).
 * This buffer is in addition to OS buffers, so it may not need to be very
 * large.  When the OS buffer and this buffer is exhausted, the server
 * will drop the client connection for falling too far behind.  So if
 * you have very slow client connections, a larger value may be 
 * warranted.
 */

#define SOCKETBUFSIZE 128*1024

/*
 * Your tmp-directory should be large enough to hold the uncompressed
 * map-files for all who are playing.
 * It ought to be locally mounted, since the function used to generate
 * unique temporary filenames isn't guaranteed to work over NFS or AFS
 * On the other hand, if you know that only one crossfire server will be
 * running using this temporary directory, it is likely to be safe to use
 * something that is NFS mounted (but performance may suffer as NFS is
 * slower than local disk)
 */

/*#define TMPDIR "/home/hugin/a/crossfire/crossfire/tmp"*/
#define TMPDIR "/tmp"


/* Directory to use for unique items. This is placed into the 'lib' 
 * directory.  Changing this will cause any old unique items file
 * not to be used.
 */
#define UNIQUE_DIR "unique-items"

/*
 * If undefined, malloc is always used.
 * It looks like this can be oboleted.  However, it can be useful to
 * track down some bugs, as it will make sure that the entire data structure
 * is set to 0, at the expense of speed.
 * Rupert Goldie has run Purify against the code, and if this is disabled,
 * apparantly there are a lot of uninitialized memory reads - I haven't
 * seen any problem (maybe the memory reads are copies, and the destination
 * doesn't actually use the garbage values either?), but the impact on speed
 * of using this probably isn't great, and should make things more stable.
 * Msw 8-9-97
 */
#define USE_CALLOC


/*
 * These define the players starting map and location on that map, and where
 * emergency saves are defined.  This should be left as is unless you make
 * major changes to the map.
 */

#define EMERGENCY_MAPPATH "/city/city"
#define EMERGENCY_X 15
#define EMERGENCY_Y 19


/*
 * These defines tells where, relative to LIBDIR, the maps, the map-index,
 * archetypes highscore and treaures files and directories can be found.
 */

#define MAPDIR		"maps"
#define ARCHETYPES	"archetypes"
#define HIGHSCORE	"highscore"
#define TREASURES	"treasures"


#define DEBUG_TIME		/* Enable the 'time command, and it's logs */
#define MAX_ERRORS	25	/* Bail out if more are received during tick */
#define STARTMAX        500     /* How big array of objects to start with */
#define OBJ_EXPAND      100     /* How big steps to use when expanding array */

#define HIGHSCORE_LENGTH 10	/* How many entries there are room for */

#define ARCHTABLE 5003		/* Used when hashing archetypes */
#define MAXSTRING 20

#define COMMAND_HASH_SIZE 107	/* If you change this, delete all characters :) */



/***********************************************************************
 * Section 4 - save player options.
 *
 * There are a lot of things that deal with the save files, and what
 * gets saved with them, so I put them in there own section.
 *
 ***********************************************************************/

/*
 * If you want the players to be able to save their characters between
 * games, define SAVE_PLAYER and set PLAYERDIR to the directories
 * where the player-files will be put.
 * Remember to create the directory (make install will do that though).
 *
 * If you intend to run a central server, and not allow the players to
 * start their own crossfire, you won't need to define this.
 *
 * If USE_CHECKSUM is defined, a checksum will be calculated each time
 * a player saves a character.
 *
 * If ENABLE_CHECKSUM is defined, the players will get the cheat-flag set
 * if the checksums don't match.  If your player-files don't have checksums,
 * you'll want to leave "USE_CHECKSUM" defined for a while so they all
 * get checksums, and then define "ENABLE_CHECKSUM" to utilize this.
 * ENABLE_CHECKSUM should not be needed if you are on a central server and
 * players don't have access to the save files.
 */

#define USE_CHECKSUM
/* #define ENABLE_CHECKSUM */ /* Will be default in distant future versions */

#ifndef PLAYERDIR
#define PLAYERDIR "players"
#endif

/*
 * If you have defined SAVE_PLAYER, you might want to change this, too.
 * This is the access rights for the players savefiles.
 * I think it is usefull to restrict access to the savefiles for the
 * game admin. So if you make crossfire set-uid, use 0600.
 * If you are running the game set-gid (to a games-group, for instance),
 * you must remember to make it writeable for the group (ie 0660).
 * Kjetil W. J{\o}rgensen, jorgens@pvv.unit.no
 * (Note: something should probably be done with lock-file permission)
 */
#define	SAVE_MODE	0660

/* NOTE ON SAVE_INTERVAL and AUTOSAVE:  Only one of these two really
 * needs to be selected.  You can set both, and things will work fine,
 * however, it just means that a lot more saving will be done, which
 * can slow things down some.
 */

/* How often (in seconds) the player is saved if he drops things.  If it is
 * set to 0, the player will be saved for every item he drops.  Otherwise,
 * if the player drops and item, and the last time he was saved
 * due to item drop is longer
 * the SAVE_INTERVAL seconds, he is then saved.  Depending on your playing
 * environment, you may want to set this to a higher value, so that
 * you are not spending too much time saving the characters.
 * This option should now work (Crossfire 0.90.5)
 */

/*#define SAVE_INTERVAL 300*/

/*
 * AUTOSAVE saves the player every AUTOSAVE ticks.  A value of
 * 5000 with MAX_TIME set at 120,000 means that the player will be
 * saved every 10 minutes.  Some effort should probably be made to
 * spread out these saves, but that might be more effort than it is
 * worth (Depending on the spacing, if enough players log on, the spacing
 * may not be large enough to save all of them.)  As it is now, it will
 * just set the base tick of when they log on, which should keep the
 * saves pretty well spread out (in a fairly random fashion.)
 */

#define AUTOSAVE 5000

/* Often, emergency save fails because the memory corruption that caused
 * the crash has trashed the characters too. Define NO_EMERGENCY_SAVE
 * to disable emergency saves.  This actually does
 * prevent emergency saves now (Version 0.90.5).
 */

#define NO_EMERGENCY_SAVE

/* By selecting the following, whenever a player does a backup save (with
 * the 'save command), the player will be saved at home (EMERGENCY_MAP_*
 * information that is specified later).  IF this is not set, the player
 * will be saved at his present location.
 */

/*#define BACKUP_SAVE_AT_HOME*/

