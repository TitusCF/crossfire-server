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

#include <global.h>
#include <loader.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif
#include <version.h>

/* global weathermap */
weathermap_t **weathermap;

void set_logfile(char *val) { settings.logfilename=val; }
void call_version() { version(NULL); exit(0); }
void showscores() { display_high_score(NULL,9999,NULL); exit(0); }
void set_debug() { settings.debug=llevDebug; }
void unset_debug() { settings.debug=llevInfo; }
void set_mondebug() { settings.debug=llevMonster; }
void set_dumpmon1() {settings.dumpvalues=1; }
void set_dumpmon2() {settings.dumpvalues=2; }
void set_dumpmon3() {settings.dumpvalues=3; }
void set_dumpmon4() {settings.dumpvalues=4; }
void set_dumpmon5() {settings.dumpvalues=5; }
void set_dumpmon6() {settings.dumpvalues=6; }
void set_dumpmon7() {settings.dumpvalues=7; }
void set_dumpmon8() {settings.dumpvalues=8; }
void set_dumpmon9() {settings.dumpvalues=9; }
void set_dumpmont(char *name) {settings.dumpvalues=10; settings.dumparg=name; }
void set_daemon() {settings.daemonmode=1; }
void set_datadir(char *path) { settings.datadir=path; }
void set_confdir(char *path) { settings.confdir=path; }
void set_localdir(char *path) { settings.localdir=path; }
void set_mapdir(char *path) { settings.mapdir=path; }
void set_archetypes(char *path) { settings.archetypes=path; }
void set_treasures(char *path) { settings.treasures=path; }
void set_uniquedir(char *path) { settings.uniquedir=path; }
void set_playerdir(char *path) { settings.playerdir=path; }
void set_tmpdir(char *path) { settings.tmpdir=path; }

void showscoresparm(char *data) { 
    display_high_score(NULL,9999,data); 
    exit(0); 
}

void set_csport(char *val)
{ 
    settings.csport=atoi(val);
#ifndef WIN32 /* ***win32: set_csport: we remove csport error secure check here, do this later */
    if (settings.csport<=0 || settings.csport>32765 ||
	(settings.csport<1024 && getuid()!=0)) {
	fprintf(stderr,"%d is an invalid csport number.\n",settings.csport);
	exit(1);
    }
#endif /* win32 */
}

/* Most of this is shamelessly stolen from XSysStats.  But since that is
 * also my program, no problem.
 */
struct Command_Line_Options {
    char    *cmd_option;    /* how it is called on the command line */
    uint8   num_args;	    /* Number or args it takes */
    uint8   pass;           /* What pass this should be processed on. */
    void    (*func)();      /* function to call when we match this.
			     * if num_args is true, than that gets passed
			     * to the function, otherwise nothing is passed
			     */
};

/* The way this system works is pretty simple - parse_args takes
 * the options passed to the program and a pass number.  If an option
 * matches both in name and in pass (and we have enough options),
 * we call the associated function.  This makes writing a multi
 * pass system very easy, and it is very easy to add in new options.
 */
struct Command_Line_Options options[] = {

/* Pass 1 functions - STuff that can/should be called before we actually
 * initialize any data.
 */
{"-h", 0, 1, help},
/* Honor -help also, since it is somewhat common */
{"-help", 0, 1, help},
{"-v", 0, 1, call_version},
{"-d", 0, 1, set_debug},
{"+d", 0, 1, unset_debug},
{"-mon", 0, 1, set_mondebug},
{"-data",1,1, set_datadir},
{"-conf",1,1, set_confdir},
{"-local",1,1, set_localdir},
{"-maps", 1, 1, set_mapdir},
{"-arch", 1, 1, set_archetypes},
{"-playerdir", 1, 1, set_playerdir},
{"-treasures", 1, 1, set_treasures},
{"-uniquedir", 1, 1, set_uniquedir},
{"-tmpdir", 1, 1, set_tmpdir},
{"-log", 1, 1, set_logfile},

/* Pass 2 functions.  Most of these could probably be in pass 1, 
 * as they don't require much of anything to bet set up.
 */
{"-csport", 1, 2, set_csport},
{"-detach", 0, 2, set_daemon},

/* Start of pass 3 information. In theory, by pass 3, all data paths
 * and defaults should have been set up. 
 */
{"-o", 0, 3, compile_info},
{"-m", 0, 3, set_dumpmon1},
{"-m2", 0, 3, set_dumpmon2},
{"-m3", 0, 3, set_dumpmon3},
{"-m4", 0, 3, set_dumpmon4},
{"-m5", 0, 3, set_dumpmon5},
{"-m6", 0, 3, set_dumpmon6},
{"-m7", 0, 3, set_dumpmon7},
{"-m8", 0, 3, set_dumpmon8},
{"-m9", 0, 3, set_dumpmon9},
{"-mt", 1, 3, set_dumpmont},
{"-mexp", 0, 3, dump_experience},
{"-s", 0, 3, showscores},
{"-score", 1, 3, showscoresparm}
};


/* Note since this may be called before the library has been set up,
 * we don't use any of crossfires built in logging functions.
 */
static void parse_args(int argc, char *argv[], int pass)
{
    int i, on_arg=1;

    while (on_arg<argc) {
	for (i=0; i<sizeof(options)/sizeof(struct Command_Line_Options); i++) {
	    if (!strcmp(options[i].cmd_option, argv[on_arg])) {
		/* Found a matching option, but should not be processed on
		 * this pass.  Just skip over it
		 */
		if (options[i].pass != pass) {
		    on_arg += options[i].num_args+1;
		    break;
		}
		if (options[i].num_args) {
		    if ((on_arg+options[i].num_args)>=argc) {
			fprintf(stderr,"%s requires an argument.\n", options[i].cmd_option);
			exit(1);
		    }
		    else {
			if (options[i].num_args==1)
				options[i].func(argv[on_arg+1]);
			if (options[i].num_args==2)
				options[i].func(argv[on_arg+1],argv[on_arg+2]);
			on_arg +=options[i].num_args+1;
		    }
		}
		else { /* takes no args */
			options[i].func();
			on_arg++;
		}
		break;
	    }
	}
	if (i==sizeof(options)/sizeof(struct Command_Line_Options)) {
		fprintf(stderr,"Unknown option: %s\n", argv[on_arg]);
		usage();
		exit(1);
	}
    }
}

materialtype_t *materialt;

static materialtype_t *get_empty_mat() {
    materialtype_t *mt;
    int i;

    mt = (materialtype_t *)malloc(sizeof(materialtype_t));
    if (mt == NULL)
	fatal(OUT_OF_MEMORY);
    mt->name = NULL;
    mt->description = NULL;
    for (i=0; i < NROFATTACKS; i++) {
	mt->save[i] = 0;
	mt->mod[i] = 0;
    }
    mt->chance = 0;
    mt->difficulty = 0;
    mt->magic = 0;
    mt->damage = 0;
    mt->wc = 0;
    mt->ac = 0;
    mt->sp = 0;
    mt->weight = 100;
    mt->value = 100;
    mt->next = NULL;
    return mt;
}

static void load_materials()
{
    char buf[MAX_BUF], filename[MAX_BUF], *cp, *next;
    FILE *fp;
    materialtype_t *mt;
    int i, value;

    sprintf(filename, "%s/materials", settings.datadir);
    LOG(llevDebug, "Reading material type data from %s...", filename);
    if ((fp = fopen(filename, "r")) == NULL) {
        LOG(llevError, "Cannot open %s for reading\n", filename);
        mt = get_empty_mat();
        mt->next = NULL;
        materialt = mt;
        return;
    }
    mt = get_empty_mat();
    materialt = mt;
    while (fgets(buf, MAX_BUF, fp) != NULL) {
	if (*buf=='#')
	    continue;
	if ((cp=strchr(buf, '\n'))!=NULL)
	    *cp='\0';
	cp=buf;
	while(*cp==' ') /* Skip blanks */
	    cp++;
	if (!strncmp(cp, "name", 4)) {
	    /* clean up the previous entry */
	    if (mt->next != NULL) {
		if (mt->description == NULL)
		    mt->description = add_string(mt->name);
		mt = mt->next;
	    }
	    mt->next = get_empty_mat();
	    mt->name = add_string(strchr(cp, ' ') + 1);
	} else if (!strncmp(cp, "description", 11)) {
	    mt->description = add_string(strchr(cp, ' ') + 1);
	} else if (sscanf(cp, "material %d", &value)) {
	    mt->material = value;
	} else if (!strncmp(cp, "saves", 5)) {
	    cp = strchr(cp, ' ') + 1;
	    for (i=0; i < NROFATTACKS; i++) {
		if (cp == NULL) {
		    mt->save[i] = 0;
		    continue;
		}
		if ((next=strchr(cp,',')) != NULL)
		    *(next++) = '\0';
		sscanf(cp, "%hd", &mt->save[i]);
		cp = next;
	    }
	} else if (!strncmp(cp, "mods", 4)) {
	    	cp = strchr(cp, ' ') + 1;
		for (i=0; i < NROFATTACKS; i++) {
		    if (cp == NULL) {
			mt->save[i] = 0;
			continue;
		    }
		    if ((next=strchr(cp,',')) != NULL)
			*(next++) = '\0';
		    sscanf(cp, "%hd", &mt->mod[i]);
		    cp = next;
		}
	} else if (sscanf(cp, "chance %d\n", &value)) {
	    mt->chance = (sint8)value;
	} else if (sscanf(cp, "diff %d\n", &value)) {
	    mt->difficulty = (sint8)value;
	} else if (sscanf(cp, "magic %d\n", &value)) {
	    mt->magic = (sint8)value;
	} else if (sscanf(cp, "damage %d\n", &value)) {
	    mt->damage= (sint8)value;
	} else if (sscanf(cp, "wc %d\n", &value)) {
	    mt->wc = (sint8)value;
	} else if (sscanf(cp, "ac %d\n", &value)) {
	    mt->ac = (sint8)value;
	} else if (sscanf(cp, "sp %d\n", &value)) {
	    mt->sp = (sint8)value;
	} else if (sscanf(cp, "weight %d\n", &value)) {
	    mt->weight = value;
	} else if (sscanf(cp, "value %d\n", &value)) {
	    mt->value = value;
	}
    }
    mt->next = NULL;
    LOG(llevDebug, "Done.\n");
    fclose(fp);
}

/* This loads the settings file.  There could be debate whether this should
 * be here or in the common directory - but since only the server needs this
 * information, having it here probably makes more sense.
 */
static void load_settings()
{
    char buf[MAX_BUF],*cp;
    int	has_val,comp;
    FILE    *fp;

    sprintf(buf,"%s/settings",settings.confdir);
    /* We don't require a settings file at current time, but down the road,
     * there will probably be so many values that not having a settings file
     * will not be a good thing.
     */
    if ((fp = open_and_uncompress(buf, 0, &comp)) == NULL) {
	LOG(llevError,"Warning: No settings file found\n");
	return;
    }
    while (fgets(buf, MAX_BUF-1, fp) != NULL) {
	if (buf[0] == '#') continue;
	/* eliminate newline */
	if ((cp=strrchr(buf,'\n'))!=NULL) *cp='\0';

	/* Skip over empty lines */
	if (buf[0] == 0) continue;

	/* Skip all the spaces and set them to nulls.  If not space,
	 * set cp to "" to make strcpy's and the like easier down below.
	 */
	if ((cp = strchr(buf,' '))!=NULL) {
	    while (*cp==' ') *cp++=0;
	    has_val=1;
	} else {
	    cp="";
	    has_val=0;
	}

	if (!strcasecmp(buf,"metaserver_notification")) {
	    if (!strcasecmp(cp,"on") || !strcasecmp(cp,"true")) {
		settings.meta_on=TRUE;
	    } else if (!strcasecmp(cp,"off") || !strcasecmp(cp,"false")) {
		settings.meta_on=FALSE;
	    } else {
		LOG(llevError,"load_settings: Unkown value for metaserver_notification: %s\n",
		    cp);
	    }
	} else if (!strcasecmp(buf,"metaserver_server")) {
	    if (has_val) strcpy(settings.meta_server, cp);
	    else 
		LOG(llevError,"load_settings: metaserver_server must have a value.\n");
	} else if (!strcasecmp(buf,"motd")) {
	    if (has_val)
		strcpy(settings.motd, cp);
	    else 
		LOG(llevError,"load_settings: motd must have a value.\n");
	} else if (!strcasecmp(buf,"dm_mail")) {
	    if (has_val)
		strcpy(settings.dm_mail, cp);
	    else 
		LOG(llevError,"load_settings: dm_mail must have a value.\n");
	} else if (!strcasecmp(buf,"metaserver_host")) {
	    if (has_val) strcpy(settings.meta_host, cp);
	    else 
		LOG(llevError,"load_settings: metaserver_host must have a value.\n");
	} else if (!strcasecmp(buf,"metaserver_port")) {
	    int port = atoi(cp);

	    if (port<1 || port>65535)
		LOG(llevError,"load_settings: metaserver_port must be between 1 and 65535, %d is invalid\n",
		    port);
	    else settings.meta_port = port;
	} else if (!strcasecmp(buf,"metaserver_comment")) {
	    strcpy(settings.meta_comment, cp);
	} else if (!strcasecmp(buf, "worldmapstartx")) {
	    int size = atoi(cp);

	    if (size < 0)
		LOG(llevError, "load_settings: worldmapstartx must be at least"
		    "0, %d is invalid\n", size);
	    else
		settings.worldmapstartx = size;
	} else if (!strcasecmp(buf, "worldmapstarty")) {
	    int size = atoi(cp);

	    if (size < 0)
		LOG(llevError, "load_settings: worldmapstarty must be at least"
		    "0, %d is invalid\n", size);
	    else
		settings.worldmapstarty = size;
	} else if (!strcasecmp(buf, "worldmaptilesx")) {
	    int size = atoi(cp);

	    if (size < 1)
		LOG(llevError, "load_settings: worldmaptilesx must be greater"
		    "than 1, %d is invalid\n", size);
	    else
		settings.worldmaptilesx = size;
	} else if (!strcasecmp(buf, "worldmaptilesy")) {
	    int size = atoi(cp);

	    if (size < 1)
		LOG(llevError, "load_settings: worldmaptilesy must be greater"
		    "than 1, %d is invalid\n", size);
	    else
		settings.worldmaptilesy = size;
	} else if (!strcasecmp(buf, "worldmaptilesizex")) {
	    int size = atoi(cp);

	    if (size < 1)
		LOG(llevError, "load_settings: worldmaptilesizex must be"
		    "greater than 1, %d is invalid\n", size);
	    else
		settings.worldmaptilesizex = size;
	} else if (!strcasecmp(buf, "worldmaptilesizey")) {
	    int size = atoi(cp);

	    if (size < 1)
		LOG(llevError, "load_settings: worldmaptilesizey must be"
		    "greater than 1, %d is invalid\n", size);
	    else
		settings.worldmaptilesizey = size;
	} else if (!strcasecmp(buf, "dynamiclevel")) {
	    int lev = atoi(cp);

	    if (lev < 0)
		LOG(llevError, "load_settings: dynamiclevel must be"
		    "at least 0, %d is invalid\n", lev);
	    else
		settings.dynamiclevel = lev;
	} else if (!strcasecmp(buf, "fastclock")) {
	    int lev = atoi(cp);

	    if (lev < 0)
		LOG(llevError, "load_settings: fastclock must be at least 0"
		    ", %d is invalid\n", lev);
	    else
		settings.fastclock = lev;
	} else if (!strcasecmp(buf, "not_permadeth")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.not_permadeth=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.not_permadeth=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for not_permadeth"
		    ": %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "resurrection")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.resurrection=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.resurrection=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for resurrection"
		    ": %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "set_title")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.set_title=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.set_title=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for set_title"
		    ": %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "search_items")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.search_items=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.search_items=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for search_items"
		    ": %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "spell_encumbrance")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.spell_encumbrance=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.spell_encumbrance=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for "
		    "spell_encumbrance: %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "spell_failure_effects")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.spell_failure_effects=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.spell_failure_effects=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for"
		    "spell_failure_effects: %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "casting_time")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.casting_time=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.casting_time=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for"
		    "casting_time: %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "real_wiz")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.real_wiz=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.real_wiz=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for"
		    "real_wiz: %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "recycle_tmp_maps")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.recycle_tmp_maps=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.recycle_tmp_maps=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for"
		    "recycle_tmp_maps: %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "explore_mode")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.explore_mode=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.explore_mode=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for"
		    "explore_mode: %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "spellpoint_level_depend")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.spellpoint_level_depend=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.spellpoint_level_depend=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for"
		    "spellpoint_level_depend: %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "stat_loss_on_death")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.stat_loss_on_death=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.stat_loss_on_death=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for"
		    "stat_loss_on_death: %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "use_permanent_experience")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.use_permanent_experience=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.use_permanent_experience=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for"
		    "use_permanent_experience: %s\n", cp);
	    }
	} else if (!strcasecmp(buf, "balanced_stat_loss")) {
	    if (!strcasecmp(cp, "on") || !strcasecmp(cp, "true")) {
		settings.balanced_stat_loss=TRUE;
	    } else if (!strcasecmp(cp, "off") || !strcasecmp(cp, "false")) {
		settings.balanced_stat_loss=FALSE;
	    } else {
		LOG(llevError, "load_settings: Unkown value for"
		    "balanced_stat_loss: %s\n", cp);
	    }
	} else if (!strcasecmp(buf,"simple_exp")) {
	    if (!strcasecmp(cp,"on") || !strcasecmp(cp,"true")) {
		settings.simple_exp=TRUE;
	    } else if (!strcasecmp(cp,"off") || !strcasecmp(cp,"false")) {
		settings.simple_exp=FALSE;
	    } else {
		LOG(llevError,"load_settings: Unkown value for simple_exp: %s\n",
		    cp);
	    }
	} else {
	    LOG(llevError,"Unknown value in settings file: %s\n", buf);
	}
    }
    close_and_delete(fp, comp);
}


/*
 * init() is called only once, when starting the program.
 */

void init(int argc, char **argv) {

    (void) umask(0);	/* We don't want to be affected by players' umask */

    init_done=0;		/* Must be done before init_signal() */
    logfile=stderr;
    parse_args(argc, argv, 1);	/* First arg pass - right now it does
				 * nothing, but in future specifying the
				 * LibDir in this pass would be reasonable*/

    init_library();	/* Must be called early */
    load_settings();	/* Load the settings file */
    init_weather();
    read_supplydb();    /* read the supply and demand database */
    load_materials();
    parse_args(argc, argv, 2);
    fprintf(logfile,"Welcome to CrossFire, v%s\n",VERSION);
    fprintf(logfile,"Copyright (C) 1994 Mark Wedel.\n");
    fprintf(logfile,"Copyright (C) 1992 Frank Tore Johansen.\n");

    if (strcmp(settings.dm_mail, "") != 0) {
	fprintf(logfile,"Maintained locally by: %s\n", settings.dm_mail);
	fprintf(logfile,"Questions and bugs should be mailed to above address.\n");
    }
    SRANDOM(time(NULL));

    init_startup();	/* Write (C), check shutdown/forbid files */
    init_signals();	/* Sets up signal interceptions */
    setup_library();	/* Set up callback function pointers */
    init_commands();	/* Sort command tables */
    read_map_log();	/* Load up the old temp map files */

    parse_args(argc, argv, 3);

#ifndef WIN32 /* ***win32: no BecomeDaemon in windows */
    if (settings.daemonmode)
	logfile = BecomeDaemon(settings.logfilename[0]=='\0'?"logfile":settings.logfilename);
#endif

    init_beforeplay();
    init_ericserver();
    metaserver_init();
    reset_sleep();
    init_done=1;
}

void usage() {
  (void) fprintf(logfile,
	"Usage: crossfire [-h] [-<flags>]...\n");
}

void help() {
/* The information in usage is redundant with what is given below, so why call it? */
/*    usage();*/
    printf("Flags:\n");
    printf(" -csport <port> Specifies the port to use for the new client/server code.\n");
    printf(" -d          Turns on some debugging.\n");
    printf(" +d          Turns off debugging (useful if server compiled with debugging\n");
    printf("             as default).\n");
    printf(" -detach     The server will go in the background, closing all\n");
    printf("             connections to the tty.\n");
    printf(" -h          Display this information.\n");
    printf(" -log <file> Specifies which file to send output to.\n");
    printf("             Only has meaning if -detach is specified.\n");
    printf(" -mon        Turns on monster debugging.\n");
    printf(" -o          Prints out info on what was defined at compile time.\n");
    printf(" -s          Display the high-score list.\n");
    printf(" -score <name or class> Displays all high scores with matching name/class.\n");
    printf(" -v          Print version and contributors.\n");
    printf(" -data       Sets the lib dir (archetypes, treasures, etc.)\n");
    printf(" -local      Read/write local data (hiscore, unique items, etc.)\n");
    printf(" -maps       Sets the directory for maps.\n");
    printf(" -arch       Sets the archetype file to use.\n");
    printf(" -playerdir  Sets the directory for the player files.\n");
    printf(" -treasures	 Sets the treasures file to use.\n");
    printf(" -uniquedir  Sets the unique items/maps directory.\n");
    printf(" -tmpdir     Sets the directory for temporary files (mostly maps.)\n");
    printf(" -m          Lists out suggested experience for all monsters.\n");
    printf(" -m2         Dumps out abilities.\n");
    printf(" -m3         Dumps out artifactt information.\n");
    printf(" -m4         Dumps out spell information.\n");
    printf(" -m5         Dumps out skill information.\n");
    printf(" -m6         Dumps out race information.\n");
    printf(" -m7         Dumps out alchemy information.\n");
    printf(" -m8         Dumps out gods information.\n");
    printf(" -m9         Dumps out more alchemy information (formula checking).\n");
    printf(" -mt <name>  Dumps out list of treasures for a monster.\n");
    exit(0);
}

void init_beforeplay() {
  init_archetypes(); /* If not called before, reads all archetypes from file */
  init_artifacts();  /* If not called before, reads all artifacts from file */
  init_spells();     /* If not called before, links archtypes used by spells */
  init_archetype_pointers(); /* Setup global pointers to archetypes */
  init_races();	   /* overwrite race designations using entries in lib/races file */ 
  init_gods();	/* init linked list of gods from archs*/ 
  init_readable();	/* inits useful arrays for readable texts */
  init_formulae();  /* If not called before, reads formulae from file */
  init_new_exp_system();    /* If not called before, inits experience system */

  switch(settings.dumpvalues) {
  case 1:
    print_monsters();
    exit(0);
  case 2:
    dump_abilities();
    exit(0);
  case 3:
    dump_artifacts();
    exit(0);
  case 4:
    dump_spells();
    exit(0);
  case 5:
    dump_skills();
    exit(0);
  case 6:
    dump_races();
    exit(0);
  case 7:
    dump_alchemy();
    exit(0);
  case 8:
    dump_gods();
    exit(0);
  case 9:
    dump_alchemy_costs();
    exit(0);
  case 10:
    dump_monster_treasure(settings.dumparg);
    exit(0);
  }
}

void init_startup() {
  char buf[MAX_BUF];
  FILE *fp;
  int comp;

#ifdef SHUTDOWN_FILE
  sprintf(buf,"%s/%s",settings.confdir,SHUTDOWN_FILE);
  if ((fp = open_and_uncompress(buf, 0, &comp)) != NULL) {
    while (fgets(buf, MAX_BUF-1, fp) != NULL)
      printf("%s", buf);
    close_and_delete(fp, comp);
    exit(1);
  }
#endif

  if (forbid_play()) { /* Maybe showing highscore should be allowed? */
      LOG(llevError, "CrossFire: Playing not allowed.\n");
      exit(-1);
  }
}

/*
 * compile_info(): activated with the -o flag.
 * It writes out information on how Imakefile and config.h was configured
 * at compile time.
 */

void compile_info() {
  int i=0;
  printf("Non-standard include files:\n");
#if !defined (__STRICT_ANSI__) || defined (__sun__)
#if !defined (Mips)
  printf("<stdlib.h>\n");
  i=1;
#endif
#if !defined (MACH) && !defined (sony)
  printf("<malloc.h>\n");
  i=1;
#endif
#endif
#ifndef __STRICT_ANSI__
#ifndef MACH
  printf("<memory.h\n");
  i=1;
#endif
#endif
#ifndef sgi
  printf("<sys/timeb.h>\n");
  i=1;
#endif
  if(!i)
    printf("(none)\n");
  printf("Datadir:\t\t%s\n",settings.datadir);
  printf("Localdir:\t\t%s\n",settings.localdir);
#ifdef PERM_FILE
  printf("Perm file:\t<ETC>/%s\n",PERM_FILE);
#endif
#ifdef SHUTDOWN_FILE
  printf("Shutdown file:\t<ETC>/%s\n",SHUTDOWN_FILE);
#endif
  printf("Save player:\t<true>\n");
  printf("Save mode:\t%4.4o\n",SAVE_MODE);
  printf("Playerdir:\t<VAR>/%s\n",settings.playerdir);
  printf("Itemsdir:\t<VAR>/%s\n", settings.uniquedir);
  printf("Tmpdir:\t\t%s\n",settings.tmpdir);
  printf("Map max timeout:\t%d\n",MAP_MAXTIMEOUT);
  printf("Max objects:\t%d\n",MAX_OBJECTS);
#ifdef USE_CALLOC
  printf("Use_calloc:\t<true>\n");
#else
  printf("Use_calloc:\t<false>\n");
#endif

#ifdef X_EDITOR
  printf("Editor:\t\t%s\n",X_EDITOR);
#endif

  printf("Max_time:\t%d\n",MAX_TIME);

#ifdef WIN32 /* ***win32 compile_info(): remove execl... */
  printf("Logfilename:\t%s\n",settings.logfilename);
  exit(0);
#else
  execl("/bin/uname", "uname", "-a", NULL);
  LOG(llevError, "Opps, should't have gotten here.");
  perror("execl");
  exit(-1);
#endif
}

/* Signal handlers: */

void rec_sigsegv(int i) {
  LOG(llevError,"\nSIGSEGV received.\n");
  fatal_signal(1, 1);
}

void rec_sigint(int i) {
  LOG(llevInfo,"\nSIGINT received.\n");
  fatal_signal(0, 1);
}

void rec_sighup(int i) {
  LOG(llevInfo,"\nSIGHUP received\n");
  if(init_done) {
    emergency_save(0);
    cleanup();
  }
  exit(0);
}

void rec_sigquit(int i) {
  LOG(llevInfo,"\nSIGQUIT received\n");
  fatal_signal(1, 1);
}

void rec_sigpipe(int i) {

/* Keep running if we receive a sigpipe.  Crossfire should really be able
 * to handle this signal (at least at some point in the future if not
 * right now).  By causing a dump right when it is received, it is not
 * doing much good.  However, if it core dumps later on, at least it can
 * be looked at later on, and maybe fix the problem that caused it to
 * dump core.  There is no reason that SIGPIPES should be fatal
 */
#if 1 && !defined(WIN32) /* ***win32: we don't want send SIGPIPE */
  LOG(llevInfo,"\nReceived SIGPIPE, ignoring...\n");
  signal(SIGPIPE,rec_sigpipe);/* hocky-pux clears signal handlers */
#else
  LOG(llevError,"\nSIGPIPE received, not ignoring...\n");
  fatal_signal(1, 1); /*Might consider to uncomment this line */
#endif
}

void rec_sigbus(int i) {
#ifdef SIGBUS
  LOG(llevError,"\nSIGBUS received\n");
  fatal_signal(1, 1);
#endif
}

void rec_sigterm(int i) {
  LOG(llevInfo,"\nSIGTERM received\n");
  fatal_signal(0, 1);
}

void fatal_signal(int make_core, int close_sockets) {
  if(init_done) {
    emergency_save(0);
    clean_tmp_files();
  }
  if(make_core)
    abort();
  exit(0);
}

void init_signals() {
#ifndef WIN32 /* init_signals() remove signals */
  signal(SIGHUP,rec_sighup);
  signal(SIGINT,rec_sigint);
  signal(SIGQUIT,rec_sigquit);
  signal(SIGSEGV,rec_sigsegv);
  signal(SIGPIPE,rec_sigpipe);
#ifdef SIGBUS
  signal(SIGBUS,rec_sigbus);
#endif
  signal(SIGTERM,rec_sigterm);
#endif /* win32 */
}

/*
 * init_library: Set up the function pointers which will point
 * back from the library into the server.
 */
void setup_library() {
  set_emergency_save(emergency_save);
  set_clean_tmp_files(clean_tmp_files);
  set_fix_auto_apply(fix_auto_apply);
  set_remove_friendly_object(remove_friendly_object);
  set_process_active_maps(process_active_maps);
  set_update_buttons(update_buttons);
  set_draw_info(new_draw_info);
  set_move_apply(move_apply);
  set_draw(draw);
  set_monster_check_apply(monster_check_apply);
  set_move_teleporter(move_teleporter);
  set_move_firewall(move_firewall);
  set_move_creator(move_creator);
  set_move_duplicator(move_duplicator);
  set_trap_adjust(trap_adjust);
  set_esrv_send_item(esrv_send_item);
  set_esrv_del_item(esrv_del_item);
  set_esrv_update_item(esrv_update_item);
/*  set_init_blocksview_players(init_blocksview_players); */
  set_info_map(new_info_map);
  set_set_darkness_map(set_darkness_map);
  set_dragon_gain_func(dragon_ability_gain);
  set_weather_effect_func(weather_effect);
}

/* init_races() - reads the races file in the lib/ directory, then
 * overwrites old 'race' entries. This routine allow us to quickly
 * re-configure the 'alignment' of monsters, objects. Useful for
 * putting together lists of creatures, etc that belong to gods.
 */
 
void init_races () {
  FILE *file;
  char race[MAX_BUF], fname[MAX_BUF], buf[MAX_BUF], *cp, variable[MAX_BUF];
  archetype *mon=NULL;
  static int init_done=0;

  if (init_done) return;
  init_done=1;
  first_race=NULL;

  sprintf(fname,"%s/races",settings.datadir);
  LOG(llevDebug, "Reading races from %s...",fname);
  if(! (file=fopen(fname,"r"))) {
        perror(fname); return;
  }

  while(fgets(buf,MAX_BUF,file)!=NULL) {
    int set_race=1,set_list=1;
    if(*buf=='#') continue;
    if((cp=strchr(buf,'\n'))!=NULL)
      *cp='\0';
    cp=buf;
    while(*cp==' '||*cp=='!'||*cp=='@') { 
      if(*cp=='!') set_race=0;
      if(*cp=='@') set_list=0;
      cp++;
    }
    if(sscanf(cp,"RACE %s",variable)) { /* set new race value */
        strcpy(race,variable);
    } else {
	char *cp1;
	/* Take out beginning spaces */
	for (cp1 = cp; *cp1==' '; cp1++);
	/* Remove newline and trailing spaces */
	for (cp1 = cp + strlen(cp) -1; *cp1 == '\n' || *cp1 == ' '; cp1 --) {
		*cp1='\0';
		if (cp==cp1) break;
	}
	
	if (cp[strlen(cp)-1]=='\n') cp[strlen(cp)-1]='\0';
        /* set creature race to race value */
        if((mon=find_archetype(cp))==NULL)
           LOG(llevError,"\nCreature %s in race file lacks archetype",cp);
        else {
           if(set_race&&(!mon->clone.race||strcmp(mon->clone.race,race))) {
                if(mon->clone.race) {
                   LOG(llevDebug,"\n Resetting race to %s from %s for archetype %s",
                        race,mon->clone.race,mon->name);
                  free_string(mon->clone.race);
                }
                mon->clone.race=add_string(race);
           }
           /* if the arch is a monster, add it to the race list */
           if(set_list&&QUERY_FLAG(&mon->clone,FLAG_MONSTER))
                add_to_racelist(race, &mon->clone);
        }
    }
  }
  fclose(file);
    LOG(llevDebug,"done.\n");
}

void dump_races()
{ 
    racelink *list;
    objectlink *tmp;
    for(list=first_race;list;list=list->next) {
      fprintf(stderr,"\nRACE %s:\t",list->name); 
      for(tmp=list->member;tmp;tmp=tmp->next)
        fprintf(stderr,"%s(%d), ",tmp->ob->arch->name,tmp->ob->level);
    }
    fprintf(stderr,"\n");
}

void add_to_racelist (char *race_name, object *op) {
  racelink *race;
 
  if(!op||!race_name) return;
  race=find_racelink(race_name);
 
  if(!race) { /* add in a new race list */
    race = get_racelist();
    race->next = first_race;
    first_race = race;
    race->name=add_string(race_name);
  }
 
  if(race->member->ob) {
    objectlink *tmp = get_objectlink();
    tmp->next=race->member;
    race->member = tmp;
  }
  race->nrof++;
  race->member->ob = op;
}

racelink * get_racelist ( ) {
  racelink *list;
 
  list = (racelink *) malloc(sizeof(racelink ));
  list->name=NULL;
  list->nrof=0;
  list->member=get_objectlink();
  list->next=NULL;

  return list;
}
 
racelink * find_racelink( char *name ) {
  racelink *test=NULL;
 
  if(name&&first_race)
    for(test=first_race;test&&test!=test->next;test=test->next)
       if(!test->name||!strcmp(name,test->name)) break;
 
  return test;
}

