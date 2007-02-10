/*
 * static char *rcsid_init_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002 Mark Wedel & Crossfire Development Team
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

    The authors can be reached via e-mail at crossfire-devel@real-time.com
*/

/**
 * @file common/init.c
 * Basic initialization for the common library.
 */

#define EXTERN
#define INIT_C
#include <global.h>
#include <object.h>

/** You unforunately need to looking in include/global.h to see what these
 * correspond to.
 */
struct Settings settings = {
    LOGFILE,   /* Logfile */
    CSPORT,				/* Client/server port */

    /* Debug level */
#ifdef DEBUG
    llevDebug,
#else
    llevInfo,
#endif
    0, NULL, 0,    /* dumpvalues, dumparg, daemonmode */
    0, /* argc */
    NULL, /* argv */
    CONFDIR,
    DATADIR, 
    LOCALDIR,
    PLAYERDIR, MAPDIR, ARCHETYPES,REGIONS,TREASURES, 
    UNIQUE_DIR, TEMPLATE_DIR,
    TMPDIR,
    STAT_LOSS_ON_DEATH,
    PK_LUCK_PENALTY,
    PERMANENT_EXPERIENCE_RATIO,
    DEATH_PENALTY_RATIO,
    DEATH_PENALTY_LEVEL,
    BALANCED_STAT_LOSS,
    NOT_PERMADETH,
    SIMPLE_EXP,
    RESET_LOCATION_TIME,
    SET_TITLE,
    RESURRECTION,
    SEARCH_ITEMS,
    SPELL_ENCUMBRANCE,
    SPELL_FAILURE_EFFECTS,
    CASTING_TIME,
    REAL_WIZ,
    RECYCLE_TMP_MAPS,
    EXPLORE_MODE,
    SPELLPOINT_LEVEL_DEPEND,
    SET_FRIENDLY_FIRE,
    "", /* Who format specifier */
    "", /* who wiz format specifier */
    MOTD,
    "rules",
    "news",
    "",		/* DM_MAIL */
    0,		/* This and the next 3 values are metaserver values */
    "",
    "",
    0,
    "",
    0,0,0,0,0,0,0,0,  /* worldmap settings*/
    EMERGENCY_MAPPATH, EMERGENCY_X, EMERGENCY_Y,
    0,
    1.0,
    /* Armor enchantment stuff */
    ARMOR_MAX_ENCHANT,
    ARMOR_WEIGHT_REDUCTION,
    ARMOR_WEIGHT_LINEAR,
    ARMOR_SPEED_IMPROVEMENT,
    ARMOR_SPEED_LINEAR,
    1, /* no_player_stealing */
    0, /* create_home_portals */
    5000000, /* pk_max_experience */
    10, /* pk_max_experience_percent */

};

/**
 * Perhaps not the best place for this, but needs to be
 * in some file in the common area so that standalone
 * programs, like the random map generator, can be built.
 */
const char* const spellpathnames[NRSPELLPATHS] = {
 "Protection",
 "Fire",
 "Frost",
 "Electricity",
 "Missiles",
 "Self",
 "Summoning",
 "Abjuration",
 "Restoration",
 "Detonation",
 "Mind",
 "Creation",
 "Teleportation",
 "Information",
 "Transmutation",
 "Transferrence",
 "Turning",
 "Wounding",
 "Death",
 "Light"
};


/**
 * This loads the emergency map information from a
 * .emergency file in the map directory.  Doing this makes
 * it easier to switch between map distributions (don't need
 * to recompile.  Note that there is no reason I see that
 * this could not be re-loaded during play, but it seems
 * like there should be little reason to do that.
 *
 * @note
 * If file doesn't exist, will not do anything.
 */
static void init_emergency_mappath(void)
{
    char filename[MAX_BUF], tmpbuf[MAX_BUF];
    FILE    *fp;
    int online=0;

    /* If this file doesn't exist, not a big deal */
    sprintf(filename,"%s/%s/.emergency",settings.datadir, settings.mapdir);
    if ((fp = fopen(filename, "r"))!=NULL) {
        while (fgets(tmpbuf, MAX_BUF-1, fp)) {
            if (tmpbuf[0] == '#') continue; /* ignore comments */

            if (online == 0) {
                tmpbuf[strlen(tmpbuf)-1] = 0;	/* kill newline */
                settings.emergency_mapname = strdup_local(tmpbuf);
            }
            else if (online == 1) {
                settings.emergency_x = atoi(tmpbuf);
            }
            else if (online == 2) {
                settings.emergency_y = atoi(tmpbuf);
            }
            online++;
            if (online>2) break;
        }
        fclose(fp);
        if (online<=2)
            LOG(llevError,"Online read partial data from %s\n", filename);
        LOG(llevDebug,"Emergency mappath reset to %s (%d, %d)\n", settings.emergency_mapname,
            settings.emergency_x, settings.emergency_y);
    }
}


/**
 * It is vital that init_library() is called by any functions
 * using this library.
 * If you want to lessen the size of the program using the library,
 * you can replace the call to init_library() with init_globals() and
 * init_function_pointers().  Good idea to also call init_vars and
 * init_hash_table if you are doing any object loading.
 */
void init_library(void) {
    init_environ();
    init_globals();
    init_hash_table();
    init_objects();
    init_vars();
    init_block();
    read_bmap_names ();
    read_smooth();
    init_anim();    /* Must be after we read in the bitmaps */
    init_archetypes();	/* Reads all archetypes from file */
    init_dynamic ();
    init_attackmess();
    init_clocks();
    init_emergency_mappath();
    init_experience();
}


/**
 * Initializes values from the environmental variables.
 * it needs to be called very early, since command line options should
 * overwrite these if specified.
 */
void init_environ(void) {
    char *cp;

    cp=getenv("CROSSFIRE_LIBDIR");
    if (cp) settings.datadir=cp;
    cp=getenv("CROSSFIRE_LOCALDIR");
    if (cp) settings.localdir=cp;
    cp=getenv("CROSSFIRE_PLAYERDIR");
    if (cp) settings.playerdir=cp;
    cp=getenv("CROSSFIRE_MAPDIR");
    if (cp) settings.mapdir=cp;
    cp=getenv("CROSSFIRE_ARCHETYPES");
    if (cp) settings.archetypes=cp;
    cp=getenv("CROSSFIRE_TREASURES");
    if (cp) settings.treasures=cp;
    cp=getenv("CROSSFIRE_UNIQUEDIR");
    if (cp) settings.uniquedir=cp;
    cp=getenv("CROSSFIRE_TEMPLATEDIR");
    if (cp) settings.templatedir=cp;
    cp=getenv("CROSSFIRE_TMPDIR");
    if (cp) settings.tmpdir=cp;
}


/**
 * Initialises all global variables.
 * Might use environment-variables as default for some of them.
 *
 * Setups logfile, and such variables.
 */
void init_globals(void) {
    if (settings.logfilename[0] == 0) {
        logfile = stderr;
    }
    else if ((logfile=fopen(settings.logfilename, "a"))==NULL) {
        fprintf(stderr,"Unable to open %s as the logfile - will use stderr instead\n",
            settings.logfilename);
        logfile = stderr;
    }
    else {
        setvbuf(logfile, NULL, _IOLBF, 0);
    }
    exiting = 0;
    first_player=NULL;
    first_friendly_object=NULL;
    first_map=NULL;
    first_treasurelist=NULL;
    first_artifactlist=NULL;
    first_archetype=NULL;
    *first_map_ext_path=0;
    warn_archetypes=0;
    nroftreasures = 0;
    nrofartifacts = 0;
    nrofallowedstr=0;
    ring_arch = NULL;
    amulet_arch = NULL;
    staff_arch = NULL;
    undead_name = add_string("undead");
    trying_emergency_save = 0;
    num_animations=0;
    animations=NULL;
    animations_allocated=0;
    init_defaults();
}

/**
 * Cleans all memory allocated for global variables.
 *
 * Will clear:
 *  * attack messages
 *  * emergency map settings
 *  * friendly list
 *  * experience
 *  * regions
 */
void free_globals()
{
    int msg, attack;
    objectlink* friend;
    region* reg;

    FREE_AND_CLEAR_STR(undead_name);
    attack_mess[NROFATTACKMESS][MAXATTACKMESS];
    for (msg = 0; msg < NROFATTACKMESS; msg++)
        for (attack = 0; attack < MAXATTACKMESS; attack++) {
            free(attack_mess[msg][attack].buf1);
            free(attack_mess[msg][attack].buf2);
            free(attack_mess[msg][attack].buf3);
        }

    free(settings.emergency_mapname);

    while (first_friendly_object) {
        friend = first_friendly_object->next;
        FREE_AND_CLEAR(first_friendly_object);
        first_friendly_object = friend;
    }

    free_experience();

    while (first_region) {
        reg = first_region->next;
        FREE_AND_CLEAR(first_region->name);
        FREE_AND_CLEAR(first_region->parent_name);
        FREE_AND_CLEAR(first_region->jailmap);
        FREE_AND_CLEAR(first_region->msg);
        FREE_AND_CLEAR(first_region->longname);
        FREE_AND_CLEAR(first_region);
        first_region = reg;
    }
}

/**
 * Sets up and initialises the linked list of free and used objects.
 * Allocates a certain chunk of objects and puts them on the free list.
 * Called by init_library();
 */
void init_objects(void) {
    int i;
    /* Initialize all objects: */
    objects=NULL;
    active_objects = NULL;

#ifdef MEMORY_DEBUG
    free_objects=NULL;
#else
    free_objects=objarray;
    objarray[0].prev=NULL,
    objarray[0].next= &objarray[1],
    SET_FLAG(&objarray[0], FLAG_REMOVED);
    SET_FLAG(&objarray[0], FLAG_FREED);
    for(i=1;i<STARTMAX-1;i++) {
        objarray[i].next= &objarray[i+1];
        objarray[i].prev= &objarray[i-1];
        SET_FLAG(&objarray[i], FLAG_REMOVED);
        SET_FLAG(&objarray[i], FLAG_FREED);
    }
    objarray[STARTMAX-1].next=NULL;
    objarray[STARTMAX-1].prev= &objarray[STARTMAX-2];
    SET_FLAG(&objarray[STARTMAX-1], FLAG_REMOVED);
    SET_FLAG(&objarray[STARTMAX-1], FLAG_FREED);
#endif
}

/**
 * Initialises global variables which can be changed by options.
 * Called by init_library().
 */

void init_defaults(void) {
  editor=0;
  nroferrors=0;
}

/**
 * Initializes first_map_path from the archetype collection.
 *
 * Must be called after archetypes have been initialized.
 *
 * @note
 * will call exit() if no MAP archetype was found.
 */
void init_dynamic (void) {
    archetype *at = first_archetype;
    while (at) {
        if (at->clone.type == MAP) {
            if (at->clone.race) {
                strcpy (first_map_ext_path, at->clone.race);
            }
            if (EXIT_PATH (&at->clone)) {
                strcpy (first_map_path, EXIT_PATH (&at->clone));
                return;
            }
        }
        at = at->next;
    }
    LOG(llevDebug,"You Need a archetype called 'map' and it have to contain start map\n");
    exit (-1);
}

/** Ingame time */
unsigned long todtick;

/**
 * Write out the current time to the file so time does not
 * reset every time the server reboots.
 */
void write_todclock(void)
{
    char filename[MAX_BUF];
    FILE *fp;

    sprintf(filename, "%s/clockdata", settings.localdir);
    if ((fp = fopen(filename, "w")) == NULL) {
        LOG(llevError, "Cannot open %s for writing\n", filename);
        return;
    }
    fprintf(fp, "%lu", todtick);
    fclose(fp);
}

/**
 * Initializes the gametime and TOD counters
 * Called by init_library().
 */
void init_clocks(void)
{
    char filename[MAX_BUF];
    FILE *fp;
    static int has_been_done=0;

    if (has_been_done)
        return;
    else
        has_been_done = 1;

    sprintf(filename, "%s/clockdata", settings.localdir);
    LOG(llevDebug, "Reading clockdata from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Can't open %s.\n", filename);
        todtick = 0;
        write_todclock();
        return;
    }
    fscanf(fp, "%lu", &todtick);
    LOG(llevDebug, "todtick=%lu\n", todtick);
    fclose(fp);
}

/** Attack messages the player gets when hitting/getting hit. */
attackmess_t attack_mess[NROFATTACKMESS][MAXATTACKMESS];

/**
 * Initializes the attack messages.
 * Called by init_library().
 *
 * Memory will be cleared by free_globals().
 */
void init_attackmess(void){
    char buf[MAX_BUF];
    char filename[MAX_BUF];
    char *cp, *p;
    FILE *fp;
    static int has_been_done=0;
    int mess, level, comp;
    int mode=0, total=0;

    if (has_been_done)
        return;
    else
        has_been_done = 1;

    sprintf(filename, "%s/attackmess", settings.datadir);
    LOG(llevDebug, "Reading attack messages from %s...", filename);
    if ((fp = open_and_uncompress(filename, 0, &comp)) == NULL) {
        LOG(llevError, "Can't open %s.\n", filename);
        return;
    }

    level = 0;
    while (fgets(buf, MAX_BUF, fp)!=NULL) {
        if (*buf=='#') continue;
        if((cp=strchr(buf,'\n'))!=NULL)
            *cp='\0';
        cp=buf;
        while(*cp==' ') /* Skip blanks */
            cp++;

        if (strncmp(cp, "TYPE:", 5)==0) {
            p = strtok(buf, ":");
            p = strtok(NULL, ":");
            if (mode == 1) {
                attack_mess[mess][level].level = -1;
                attack_mess[mess][level].buf1 = NULL;
                attack_mess[mess][level].buf2 = NULL;
                attack_mess[mess][level].buf3 = NULL;
            }
            level = 0;
            mess = atoi(p);
            mode = 1;
            continue;
        }
        if (mode==1) {
            p = strtok(buf, "=");
            attack_mess[mess][level].level = atoi(buf);
            p = strtok(NULL, "=");
            if (p != NULL)
                attack_mess[mess][level].buf1 = strdup_local(p);
            else
                attack_mess[mess][level].buf1 = strdup_local("");
            mode = 2;
            continue;
        } else if (mode==2) {
            p = strtok(buf, "=");
            attack_mess[mess][level].level = atoi(buf);
            p = strtok(NULL, "=");
            if (p != NULL)
                attack_mess[mess][level].buf2 = strdup_local(p);
            else
                attack_mess[mess][level].buf2 = strdup_local("");
            mode = 3;
            continue;
        } else if (mode==3) {
            p = strtok(buf, "=");
            attack_mess[mess][level].level = atoi(buf);
            p = strtok(NULL, "=");
            if (p != NULL)
                attack_mess[mess][level].buf3 = strdup_local(p);
            else
                attack_mess[mess][level].buf3 = strdup_local("");
            mode = 1;
            level++;
            total++;
            continue;
        }
    }
    LOG(llevDebug, "got %d messages in %d categories.\n", total, mess+1);
    close_and_delete(fp, comp);
}
