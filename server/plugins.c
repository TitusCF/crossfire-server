/*
 * static char *rcsid_plugins_c =
 *   "$Id$";
 */

/*****************************************************************************/
/*  CrossFire, A Multiplayer game for X-windows                              */
/*                                                                           */
/*  Copyright (C) 2000 Mark Wedel                                            */
/*  Copyright (C) 1992 Frank Tore Johansen                                   */
/*                                                                           */
/*  This program is free software; you can redistribute it and/or modify     */
/*  it under the terms of the GNU General Public License as published by     */
/*  the Free Software Foundation; either version 2 of the License, or        */
/*  (at your option) any later version.                                      */
/*                                                                           */
/*  This program is distributed in the hope that it will be useful,          */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the            */
/*  GNU General Public License for more details.                             */
/*                                                                           */
/*  You should have received a copy of the GNU General Public License        */
/*  along with this program; if not, write to the Free Software              */
/*  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.                */
/*                                                                           */
/*****************************************************************************/
/* This is the server-side plugin management part.                           */
/*****************************************************************************/
/* Original code by Yann Chachkoff (yann.chachkoff@mailandnews.com).         */
/* Special thanks to:                                                        */
/* David Delbecq                   (david.delbecq@mailandnews.com);          */
/* Joris Bontje                    (jbontje@suespammers.org);                */
/* Philip Currlin                  (?);                                      */
/*****************************************************************************/

/*****************************************************************************/
/* First, the headers. We only include plugin.h, because all other includes  */
/* are done into it, and plugproto.h (which is used only by this file).      */
/*****************************************************************************/
#include <plugin.h>

#ifndef __CEXTRACT__
#include <sproto.h>
#endif

#include <plugproto.h>
CFPlugin PlugList[32];
int PlugNR = 0;

/*****************************************************************************/
/* Tries to find if a given command is handled by a plugin.                  */
/* Note that find_plugin_command is called *before* the internal commands are*/
/* checked, meaning that you can "overwrite" them.                           */
/*****************************************************************************/
CommArray_s *find_plugin_command(char *cmd, object *op)
{
    CFParm CmdParm;
    CFParm* RTNValue;
    int i;
    char cmdchar[10];
    static CommArray_s RTNCmd;

    strcpy(cmdchar,"command?");
    CmdParm.Value[0] = cmdchar;
    CmdParm.Value[1] = cmd;
    CmdParm.Value[2] = op;

    for(i=0;i<PlugNR;i++)
    {
        RTNValue = (PlugList[i].propfunc(&CmdParm));
        if (RTNValue!=NULL)
        {
            RTNCmd.name = (char *)(RTNValue->Value[0]);
            RTNCmd.func = (CommFunc)(RTNValue->Value[1]);
            RTNCmd.time = *(double *)(RTNValue->Value[2]);
            printf("RTNCMD: name %s, time %f\n", RTNCmd.name, RTNCmd.time);
            return &RTNCmd;
        };
    };
    return NULL;
};

/*****************************************************************************/
/* Displays a list of loaded plugins (keystrings and description) in the     */
/* game log window.                                                          */
/*****************************************************************************/
void displayPluginsList(object *op)
{
    char buf[MAX_BUF];
    int i;

    new_draw_info (NDI_UNIQUE, 0, op, "List of loaded plugins:");
    new_draw_info (NDI_UNIQUE, 0, op, "-----------------------");
    for (i=0;i<PlugNR;i++)
    {
        strcpy(buf, PlugList[i].id);
        strcat(buf, ", ");
        strcat(buf, PlugList[i].fullname);
        new_draw_info (NDI_UNIQUE, 0, op, buf);
    };
};

/*****************************************************************************/
/* Searches in the loaded plugins list for a plugin with a keyname of id.    */
/* Returns the position of the plugin in the list if a matching one was found*/
/* or -1 if no correct plugin was detected.                                  */
/*****************************************************************************/
int findPlugin(const char* id)
{
    int i;
    for(i=0; i<PlugNR; i++)
        if(!strcmp(id,PlugList[i].id))
            return i;
    return -1;
};

static LIBPTRTYPE cf_dlopen(char *fname)
{
#ifdef WIN32
    return(LoadLibrary(fname));
#else
    return(dlopen(fname, RTLD_NOW|RTLD_GLOBAL));
#endif
}

static const char *cf_dlerror(void)
{
#ifdef WIN32
    static char buf[256];
    DWORD err;
    char *p;

    err = GetLastError();
    if (FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, err, 0, buf, sizeof(buf), NULL) == 0)
	snprintf(buf, sizeof(buf), "error %lu", err);

    p = strchr(buf, '\0');
    while(p > buf && (p[-1] == '\r' || p[-1] == '\n'))
	p--;
    *p = '\0';

    return(buf);
#else
    return(dlerror());
#endif
}

static void cf_dlclose(LIBPTRTYPE lib)
{
#ifdef WIN32
    FreeLibrary(lib);
#else
    dlclose(lib);
#endif
}

static f_plugin cf_dlsym(LIBPTRTYPE lib, const char *name)
{
#ifdef WIN32
    return((f_plugin)GetProcAddress(lib, name));
#else
    return((f_plugin)dlsym(lib, name));
#endif
}

/*****************************************************************************/
/* Plugins initialization. Browses the plugins directory and call            */
/* initOnePlugin for each file found.                                        */
/* Returns 0 if at least one plugin was successfully loaded, -1 if not       */
/*****************************************************************************/
int initPlugins(void)
{
    struct dirent *currentfile;
    DIR *plugdir;
    int l;
    char buf[MAX_BUF];
    char buf2[MAX_BUF];
    int result;

    LOG(llevInfo,"Initializing plugins\n");
    strcpy(buf,LIBDIR);
    strcat(buf,"/plugins/");
    LOG(llevInfo,"Plugins directory is %s\n",buf);

    plugdir = opendir(buf);
    if (plugdir == NULL)
	return -1;

    result = -1;
    while((currentfile = readdir(plugdir)) != NULL)
    {
	l = strlen(currentfile->d_name);
	if (l > strlen(PLUGIN_SUFFIX))
	{
	    if (strcmp(currentfile->d_name+l-strlen(PLUGIN_SUFFIX), PLUGIN_SUFFIX) == 0)
	    {
		strcpy(buf2,buf);
		strcat(buf2,currentfile->d_name);
		LOG(llevInfo," -> Loading plugin : %s\n",currentfile->d_name);
		if (initOnePlugin(buf2) == 0)
		{
		    result = 0;
		}
	    }
	}
    }

    closedir(plugdir);
    return result;
}

/*****************************************************************************/
/* Plugin initialization. Initializes a plugin known by its filename.        */
/* The initialization process has several stages:                            */
/* - Loading of the DLL itself;                                              */
/* - Basical plugin information request;                                     */
/* - CF-Plugin specific initialization tasks (call to initPlugin());         */
/* - Hook bindings;                                                          */
/* Returns 0 if the plugin was successfully loaded, -1 if not                */
/*****************************************************************************/
int initOnePlugin(char* pluginfile)
{
    int i;
    int j;
    LIBPTRTYPE ptr;
    CFParm HookParm;
    CFParm *InitParm;

    if (PlugNR >= sizeof(PlugList)/sizeof(*PlugList))
    {
	LOG(llevError, "Error while trying to load %s: %s\n", pluginfile, "too many plugins");
	return -1;
    }

    ptr = cf_dlopen(pluginfile);
    if (ptr == NULL)
    {
	LOG(llevError, "Error while trying to load %s: %s\n", pluginfile, cf_dlerror());
	return -1;
    }
    PlugList[PlugNR].libptr = ptr;
    PlugList[PlugNR].initfunc = cf_dlsym(ptr, "initPlugin");
    if (PlugList[PlugNR].initfunc==NULL)
    {
	LOG(llevError,"Plugin init error: %s\n", cf_dlerror());
	cf_dlclose(ptr);
	return -1;
    }

    InitParm = PlugList[PlugNR].initfunc(NULL);
    LOG(llevInfo,"Plugin name: %s, known as %s\n",
	(char *)(InitParm->Value[1]),
	(char *)(InitParm->Value[0])
    );
    strcpy(PlugList[PlugNR].id,(char *)(InitParm->Value[0]));
    strcpy(PlugList[PlugNR].fullname,(char *)(InitParm->Value[1]));
    if (findPlugin(PlugList[PlugNR].id) >= 0)
    {
	LOG(llevError, "Error while trying to load %s: plugin %s already loaded\n", pluginfile, PlugList[PlugNR].id);
	cf_dlclose(ptr);
	return -1;
    }
    PlugList[PlugNR].hookfunc = cf_dlsym(ptr, "registerHook");
    PlugList[PlugNR].eventfunc = cf_dlsym(ptr, "triggerEvent");
    PlugList[PlugNR].pinitfunc = cf_dlsym(ptr, "postinitPlugin");
    PlugList[PlugNR].propfunc = cf_dlsym(ptr, "getPluginProperty");
    if (PlugList[PlugNR].hookfunc==NULL)
    {
	LOG(llevError, "registerHook error: %s\n", cf_dlerror());
	cf_dlclose(ptr);
	return -1;
    }
    if (PlugList[PlugNR].eventfunc==NULL)
    {
	LOG(llevError,"triggerEvent error: %s\n", cf_dlerror());
	cf_dlclose(ptr);
	return -1;
    }
    if (PlugList[PlugNR].pinitfunc==NULL)
    {
	LOG(llevError,"postinitPlugin error: %s\n", cf_dlerror());
	cf_dlclose(ptr);
	return -1;
    }
    if (PlugList[PlugNR].propfunc==NULL)
    {
	LOG(llevError,"getPluginProperty error: %s\n", cf_dlerror());
	cf_dlclose(ptr);
	return -1;
    }

    for(i=0;i<NR_EVENTS;i++)
	PlugList[PlugNR].gevent[i] = 0;

    for(j=1; j<NR_OF_HOOKS;j++)
    {
	int tmp = j;
	HookParm.Value[0] = &tmp;
	HookParm.Value[1] = HookList[j];
	PlugList[PlugNR].hookfunc(&HookParm);
    }
    PlugNR++;
    PlugList[PlugNR-1].pinitfunc(NULL);
    LOG(llevInfo,"[Done]\n");

    return 0;
}

/*****************************************************************************/
/* Removes one plugin from memory. The plugin is identified by its keyname.  */
/* Returns 0 if the plugin was removed, -1 if not.                           */
/*****************************************************************************/
int removeOnePlugin(char *id)
{
    int plid;
    int j;
    plid = findPlugin(id);
    if (plid<0)
	return -1;
    /* We unload the library... */
    cf_dlclose(PlugList[plid].libptr);
    /* Then we copy the rest on the list back one position */
    PlugNR--;
    for (j=plid+1;j<sizeof(PlugList)/sizeof(*PlugList);j++)
    {
	PlugList[j-1] = PlugList[j];
    }

    return 0;
}

/*****************************************************************************/
/* Hook functions. Those are wrappers to crosslib functions, used by plugins.*/
/* Remember : NEVER call crosslib functions directly from a plugin if a hook */
/* exists.                                                                   */
/*****************************************************************************/

/*****************************************************************************/
/* LOG wrapper                                                               */
/*****************************************************************************/
/* 0 - Level of logging;                                                     */
/* 1 - Message.                                                              */
/*****************************************************************************/
CFParm* CFWLog(CFParm* PParm)
{
        LOG(*(int *)(PParm->Value[0]),(char *)(PParm->Value[1]));
        return NULL;
};

/*****************************************************************************/
/* new_info_map wrapper.                                                     */
/*****************************************************************************/
/* 0 - Color information;                                                    */
/* 1 - Map where the message should be heard;                                */
/* 2 - Message.                                                              */
/*****************************************************************************/
CFParm* CFWNewInfoMap(CFParm* PParm)
{
    new_info_map(*(int *)(PParm->Value[0]), (struct mapdef *)(PParm->Value[1]),
        (char*)(PParm->Value[2]));
    return NULL;
};

/*****************************************************************************/
/* spring_trap wrapper.                                                      */
/*****************************************************************************/
/* 0 - Trap;                                                                 */
/* 1 - Victim.                                                               */
/*****************************************************************************/
CFParm* CFWSpringTrap(CFParm* PParm)
{
    spring_trap((object *)(PParm->Value[0]),(object *)(PParm->Value[1]));
    return NULL;
};

/*
 * type of firing express how the dir parameter was parsed
 *  if type is FIRE_DIRECTIONAL, it is a value from 0 to 8 corresponding to a direction
 *  if type is FIRE_POSITIONAL, the 16bits dir parameters is separated into 2 signed shorts
 *       8 lower bits : signed, relative x value from caster
 *       8 higher bits: signed, relative y value from caster
 *   use the following macros defined in <define.h> with FIRE_POSITIONAL:
 *       GET_X_FROM_DIR(dir)  extract the x value
 *       GET_Y_FROM_DIR(dir)  extract they value
 *       SET_DIR_FROM_XY(X,Y) get dir from x,y values
 */
/*****************************************************************************/
/* cast_spell wrapper.                                                       */
/*****************************************************************************/
/* 0 - op;                                                                   */
/* 1 - caster;                                                               */
/* 2 - direction;                                                            */
/* 3 - spell object                                                          */
/* 4 - optional args;                                                        */
/*****************************************************************************/
CFParm* CFWCastSpell(CFParm* PParm)
{
    static int val;
    CFParm *CFP;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = cast_spell((object *)(PParm->Value[0]),(object *)(PParm->Value[1]),
        *(int*)(PParm->Value[2]), (object*)(PParm->Value[3]),
        (char *)(PParm->Value[4]));
    CFP->Value[0] = (void *)(&val);
    return CFP;
};

/*****************************************************************************/
/* command_rskill wrapper.                                                   */
/*****************************************************************************/
/* 0 - player;                                                               */
/* 1 - parameters.                                                           */
/*****************************************************************************/
CFParm* CFWCmdRSkill(CFParm* PParm)
{
    static int val;
    CFParm *CFP;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = command_rskill((object *)(PParm->Value[0]),
        (char *)(PParm->Value[1]));
    CFP->Value[0] = (void *)(&val);
    return CFP;
};

/*****************************************************************************/
/* become_follower wrapper.                                                  */
/*****************************************************************************/
/* 0 - object to change;                                                     */
/* 1 - new god object.                                                       */
/*****************************************************************************/
CFParm* CFWBecomeFollower(CFParm* PParm)
{
    become_follower((object *)(PParm->Value[0]),(object *)(PParm->Value[1]));
    return NULL;
};

/*****************************************************************************/
/* pick_up wrapper.                                                          */
/*****************************************************************************/
/* 0 - picker object;                                                        */
/* 1 - picked object.                                                        */
/*****************************************************************************/
CFParm* CFWPickup(CFParm* PParm)
{
    pick_up((object *)(PParm->Value[0]),(object *)(PParm->Value[1]));
    return NULL;
};

/*****************************************************************************/
/* pick_up wrapper.                                                          */
/*****************************************************************************/
/* 0 - map                                                                   */
/* 1 - x position                                                            */
/* 2 - y position                                                            */
/*****************************************************************************/
CFParm* CFWGetMapObject(CFParm* PParm)
{
    object* val;
    CFParm *CFP;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = get_map_ob(
        (mapstruct *)(PParm->Value[0]),
        *(int *)(PParm->Value[1]),
        *(int *)(PParm->Value[2])
    );
    CFP->Value[0] = (void *)(val);
    return CFP;
};

/*****************************************************************************/
/* esrv_send_item wrapper.                                                   */
/*****************************************************************************/
/* 0 - Player object;                                                        */
/* 1 - Object to update.                                                     */
/*****************************************************************************/
CFParm* CFWESRVSendItem(CFParm* PParm)
{
    esrv_send_item(
        (object *)(PParm->Value[0]),
        (object *)(PParm->Value[1])
    );
    return(NULL);
};

/*****************************************************************************/
/* find_player wrapper.                                                      */
/*****************************************************************************/
/* 0 - name of the player to find.                                           */
/*****************************************************************************/
CFParm* CFWFindPlayer(CFParm* PParm)
{
    player *pl;
    CFParm *CFP;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    pl = find_player((char *)(PParm->Value[0]));
    CFP->Value[0] = (void *)(pl);
    return CFP;
};

/*****************************************************************************/
/* manual_apply wrapper.                                                     */
/*****************************************************************************/
/* 0 - object applying;                                                      */
/* 1 - object to apply;                                                      */
/* 2 - apply flags.                                                          */
/*****************************************************************************/
CFParm* CFWManualApply(CFParm* PParm)
{
    CFParm *CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = manual_apply((object *)(PParm->Value[0]),
        (object *)(PParm->Value[1]),*(int *)(PParm->Value[2]));
    CFP->Value[0] = &val;
    return CFP;
};

/*****************************************************************************/
/* command_drop wrapper.                                                     */
/*****************************************************************************/
/* 0 - player;                                                               */
/* 1 - parameters string.                                                    */
/*****************************************************************************/
CFParm* CFWCmdDrop(CFParm* PParm)
{
    CFParm *CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = command_drop((object *)(PParm->Value[0]),(char *)(PParm->Value[1]));
    CFP->Value[0] = &val;
    return CFP;
};

/*****************************************************************************/
/* command_take wrapper.                                                     */
/*****************************************************************************/
/* 0 - player;                                                               */
/* 1 - parameters string.                                                    */
/*****************************************************************************/
CFParm* CFWCmdTake(CFParm* PParm)
{
    CFParm *CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = command_take((object *)(PParm->Value[0]),(char *)(PParm->Value[1]));
    CFP->Value[0] = &val;
    return CFP;
};

/*****************************************************************************/
/* transfer_ob wrapper.                                                      */
/*****************************************************************************/
/* 0 - object to transfer;                                                   */
/* 1 - x position;                                                           */
/* 2 - y position;                                                           */
/* 3 - random param;                                                         */
/* 4 - originator object;                                                    */
/*****************************************************************************/
CFParm* CFWTransferObject(CFParm* PParm)
{
    CFParm *CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = transfer_ob(
        (object *)(PParm->Value[0]),
        *(int *)(PParm->Value[1]),
        *(int *)(PParm->Value[2]),
        *(int *)(PParm->Value[3]),
        (object *)(PParm->Value[4])
        );
    CFP->Value[0] = &val;
    return CFP;
};

/*****************************************************************************/
/* command_title wrapper.                                                    */
/*****************************************************************************/
/* 0 - object;                                                               */
/* 1 - params string.                                                        */
/*****************************************************************************/
CFParm* CFWCmdTitle(CFParm* PParm)
{
    CFParm *CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = command_title((object *)(PParm->Value[0]),(char *)(PParm->Value[1]));
    CFP->Value[0] = &val;
    return CFP;
};

/*****************************************************************************/
/* kill_object wrapper.                                                      */
/*****************************************************************************/
/* 0 - killed object;                                                        */
/* 1 - damage done;                                                          */
/* 2 - killer object;                                                        */
/* 3 - type of killing.                                                      */
/*****************************************************************************/
CFParm* CFWKillObject(CFParm* PParm)
{
    CFParm *CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = kill_object(
        (object *)(PParm->Value[0]),
        *(int *)(PParm->Value[1]),
        (object *)(PParm->Value[2]),
        *(int *)(PParm->Value[3])
        );
    CFP->Value[0] = &val;
    return CFP;
};

/*****************************************************************************/
/* do_forget_spell wrapper.                                                  */
/*****************************************************************************/
/* 0 - object to affect;                                                     */
/* 1 - spell name to forget.                                                 */
/*****************************************************************************/
CFParm* CFWDoForgetSpell(CFParm* PParm)
{
    do_forget_spell(
        (object *)(PParm->Value[0]),
        (char *)(PParm->Value[1])
    );
    return NULL;
};

/*****************************************************************************/
/* do_learn_spell wrapper.                                                   */
/*****************************************************************************/
/* 0 - object to affect;                                                     */
/* 1 - spell index to learn;                                                 */
/* 2 - special prayer integer flag.                                          */
/*****************************************************************************/
CFParm* CFWDoLearnSpell(CFParm* PParm)
{
    do_learn_spell(
        (object *)(PParm->Value[0]),
        (object *)(PParm->Value[1]),
        *(int *)(PParm->Value[2])
    );
    return NULL;
};

/*****************************************************************************/
/* check_spell_known wrapper.                                                */
/*****************************************************************************/
/* 0 - object to check;                                                      */
/* 1 - spell name to search.                                                 */
/*****************************************************************************/
CFParm* CFWCheckSpellKnown(CFParm* PParm)
{
    CFParm *CFP;
    object *val;

    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = check_spell_known(
        (object *)(PParm->Value[0]),
        (char *)(PParm->Value[1])
        );
    CFP->Value[0] = val;
    return CFP;
};

/*****************************************************************************/
/* esrv_send_inventory wrapper.                                              */
/*****************************************************************************/
/* 0 - player object.                                                        */
/* 1 - updated object.                                                       */
/*****************************************************************************/
CFParm* CFWESRVSendInventory(CFParm* PParm)
{
    esrv_send_inventory(
        (object *)(PParm->Value[0]),
        (object *)(PParm->Value[1])
    );
    return NULL;
};

/*****************************************************************************/
/* create_artifact wrapper.                                                  */
/*****************************************************************************/
/* 0 - op;                                                                   */
/* 1 - name of the artifact to create.                                       */
/*****************************************************************************/
CFParm* CFWCreateArtifact(CFParm* PParm)
{
    CFParm *CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = create_artifact(
        (object *)(PParm->Value[0]),
        (char *)(PParm->Value[1])
    );
    CFP->Value[0] = &val;
    return CFP;
};

/*****************************************************************************/
/* get_archetype wrapper.                                                    */
/*****************************************************************************/
/* 0 - Name of the archetype to search for.                                  */
/*****************************************************************************/
CFParm* CFWGetArchetype(CFParm* PParm)
{
    CFParm *CFP;
    object* val;

    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = get_archetype( (char *)(PParm->Value[0]) );

    CFP->Value[0] = (void *)(val);
    return CFP;
};

/*****************************************************************************/
/* update_ob_speed wrapper.                                                  */
/*****************************************************************************/
/* 0 - object to update.                                                     */
/*****************************************************************************/
CFParm* CFWUpdateSpeed(CFParm* PParm)
{
    update_ob_speed(
        (object *)(PParm->Value[0])
    );
    return NULL;
};

/*****************************************************************************/
/* update_object wrapper.                                                    */
/*****************************************************************************/
/* 0 - object to update.                                                     */
/*****************************************************************************/
CFParm* CFWUpdateObject(CFParm* PParm)
{
    update_object(
        (object *)(PParm->Value[0]),
        *(int *)(PParm->Value[1])
    );
    return NULL;
};

/*****************************************************************************/
/* find_animation wrapper.                                                   */
/*****************************************************************************/
/* 0 - name of the animation to find.                                        */
/*****************************************************************************/
CFParm* CFWFindAnimation(CFParm* PParm)
{
    CFParm *CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    LOG(llevInfo,"CFWFindAnimation: %s\n",(char *)(PParm->Value[0]));
    val = find_animation((char *)(PParm->Value[0]));
    LOG(llevInfo,"Returned val: %i\n",val);
    CFP->Value[0] = (void *)(&val);
    return CFP;
};

/*****************************************************************************/
/* get_archetype_by_object_name wrapper                                      */
/*****************************************************************************/
/* 0 - name to search for.                                                   */
/*****************************************************************************/
CFParm* CFWGetArchetypeByObjectName(CFParm* PParm)
{
    CFParm *CFP;
    object* val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = get_archetype_by_object_name(
        (char *)(PParm->Value[0])
    );
    CFP->Value[0] = (void *)(val);
    return CFP;
};

/*****************************************************************************/
/* insert_ob_in_map wrapper.                                                 */
/*****************************************************************************/
/* 0 - object to insert;                                                     */
/* 1 - map;                                                                  */
/* 2 - originator of the insertion;                                          */
/* 3 - integer flags.                                                        */
/*****************************************************************************/
CFParm* CFWInsertObjectInMap(CFParm* PParm)
{
    CFParm *CFP;
    object* val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = insert_ob_in_map(
        (object *)(PParm->Value[0]),
        (mapstruct *)(PParm->Value[1]),
        (object *)(PParm->Value[2]),
        *(int *)(PParm->Value[3])
    );
    CFP->Value[0] = (void *)(val);
    return CFP;
};

/*****************************************************************************/
/* ready_map_name wrapper.                                                   */
/*****************************************************************************/
/* 0 - name of the map to ready;                                             */
/* 1 - integer flags.                                                        */
/*****************************************************************************/
CFParm* CFWReadyMapName(CFParm* PParm)
{
    CFParm* CFP;
    mapstruct* val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = ready_map_name(
        (char *)(PParm->Value[0]),
        *(int *)(PParm->Value[1])
    );
    CFP->Value[0] = (void *)(val);
    return CFP;
};

/*****************************************************************************/
/* add_exp wrapper.                                                          */
/*****************************************************************************/
/* 0 - object to increase experience of.                                     */
/* 1 - amount of experience to add.                                          */
/* 2 - skill name to add it to						     */
/* 3 - flags for this							     */
/*****************************************************************************/
CFParm* CFWAddExp(CFParm* PParm)
{
    change_exp(
        (object *)(PParm->Value[0]),
        *(sint64 *)(PParm->Value[1]),
	(char*)(PParm->Value[2]),
	*(int *)(PParm->Value[3])
    );
    return(NULL);
};

/*****************************************************************************/
/* determine_god wrapper.                                                    */
/*****************************************************************************/
/* 0 - object to determine the god of.                                       */
/*****************************************************************************/
CFParm* CFWDetermineGod(CFParm* PParm)
{
    CFParm* CFP;
    char* val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = (char*)determine_god(
        (object *)(PParm->Value[0])
    );
    CFP->Value[0] = (void *)(val);
    return CFP;
};

/*****************************************************************************/
/* find_god wrapper.                                                         */
/*****************************************************************************/
/* 0 - Name of the god to search for.                                        */
/*****************************************************************************/
CFParm* CFWFindGod(CFParm* PParm)
{
    CFParm* CFP;
    object* val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = find_god(
        (char *)(PParm->Value[0])
    );
    CFP->Value[0] = (void *)(val);
    return CFP;
};

/*****************************************************************************/
/* dump_me wrapper.                                                          */
/*****************************************************************************/
/* 0 - object to dump;                                                       */
/*****************************************************************************/
CFParm* CFWDumpObject(CFParm* PParm)
{
    CFParm* CFP;
    static char val[10240];
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    dump_me((object *)(PParm->Value[0]),val);
    CFP->Value[0] = (void *)(val);
    return CFP;
};

/*****************************************************************************/
/* load_object_str wrapper.                                                  */
/*****************************************************************************/
/* 0 - object dump string to load.                                           */
/*****************************************************************************/
CFParm* CFWLoadObject(CFParm* PParm)
{
    CFParm* CFP;
    object* val;

    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = load_object_str((char *)(PParm->Value[0]));
    CFP->Value[0] = (void *)(val);
    return CFP;
};

/*****************************************************************************/
/* remove_ob wrapper.                                                        */
/*****************************************************************************/
/* 0 - object to remove.                                                     */
/*****************************************************************************/
CFParm* CFWRemoveObject(CFParm* PParm)
{
    remove_ob((object *)(PParm->Value[0]));
    return NULL;
};

/*****************************************************************************/
/* add_string wrapper.                                                       */
/*****************************************************************************/
/* 0 - string to add.                                                        */
/*****************************************************************************/
CFParm* CFWAddString(CFParm* PParm)
{
    CFParm* CFP;
    char* val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = (char *)(PParm->Value[0]);
    CFP->Value[0] = (void*) add_string (val);
    return CFP;
};

/*****************************************************************************/
/* add_refcount wrapper.                                                     */
/*****************************************************************************/
/* 0 - string to increment refcount.                                         */
/*****************************************************************************/
CFParm* CFWAddRefcount(CFParm* PParm)
{
    CFParm* CFP;
    char* val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = (char *)(PParm->Value[0]);
    CFP->Value[0] = (void*) add_refcount (val);
    return CFP;
};

/*****************************************************************************/
/* free_string wrapper.                                                      */
/*****************************************************************************/
/* 0 - string to free.                                                       */
/*****************************************************************************/
CFParm* CFWFreeString(CFParm* PParm)
{
    char* val;
    val = (char *)(PParm->Value[0]);
    free_string (val);
    return NULL;
};

/*****************************************************************************/
/* first_map wrapper.                                                        */
/*****************************************************************************/
CFParm* CFWGetFirstMap(CFParm* PParm)
{
    CFParm* CFP;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    CFP->Value[0] = (void*)(first_map) ;
    return CFP;
};

/*****************************************************************************/
/* first_player wrapper.                                                     */
/*****************************************************************************/
CFParm* CFWGetFirstPlayer(CFParm* PParm)
{
    CFParm* CFP;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    CFP->Value[0] = (void*)(first_player) ;
    return CFP;
};

/*****************************************************************************/
/* first_archetype wrapper.                                                  */
/*****************************************************************************/
CFParm* CFWGetFirstArchetype(CFParm* PParm)
{
    CFParm* CFP;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    CFP->Value[0] = (void*)(first_archetype) ;
    return CFP;
};

/*****************************************************************************/
/* query_cost wrapper.                                                       */
/*****************************************************************************/
/* 0 - object to evaluate.                                                   */
/* 1 - who tries to sell of buy it                                           */
/* 2 - F_SELL F_BUY or F_TRUE                                                */
/*****************************************************************************/
CFParm* CFWQueryCost(CFParm* PParm)
{
    CFParm* CFP;
    object* whatptr;
    object* whoptr;
    int flag;
    static uint64 val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    whatptr = (object *)(PParm->Value[0]);
    whoptr = (object *)(PParm->Value[1]);
    flag = *(int*)(PParm->Value[2]);
    val=query_cost (whatptr,whoptr,flag);
    CFP->Value[0] = (void*) &val;
    return CFP;
};

/*****************************************************************************/
/* query_money wrapper.                                                      */
/*****************************************************************************/
/* 0 - object we are looking for solvability at.                             */
/*****************************************************************************/
CFParm* CFWQueryMoney(CFParm* PParm)
{
    CFParm* CFP;
    object* whoptr;
    static uint64 val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    whoptr = (object *)(PParm->Value[0]);
    val=query_money (whoptr);
    CFP->Value[0] = (void*) &val;
    return CFP;
};

/*****************************************************************************/
/* pay_for_item wrapper.                                                     */
/*****************************************************************************/
/* 0 - object to pay.                                                        */
/* 1 - who tries to buy it                                                   */
/*****************************************************************************/
CFParm* CFWPayForItem(CFParm* PParm)
{
    CFParm* CFP;
    object* whatptr;
    object* whoptr;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    whatptr = (object *)(PParm->Value[0]);
    whoptr = (object *)(PParm->Value[1]);
    val= pay_for_item (whatptr,whoptr);
    CFP->Value[0] = (void*) &val;
    return CFP;
};

/*****************************************************************************/
/* pay_for_amount wrapper.                                                   */
/*****************************************************************************/
/* 0 - amount to pay.                                                        */
/* 1 - who tries to pay it                                                   */
/*****************************************************************************/
CFParm* CFWPayForAmount(CFParm* PParm)
{
    CFParm* CFP;
    uint64 amount;
    object* whoptr;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    amount = *(uint64 *)(PParm->Value[0]);
    whoptr = (object *)(PParm->Value[1]);
    val= pay_for_amount (amount,whoptr);
    CFP->Value[0] = (void*) &val;
    return CFP;
};

/*****************************************************************************/
/* new_draw_info wrapper.                                                    */
/*****************************************************************************/
/* 0 - flags                                                                 */
/* 1 - pri                                                                   */
/* 2 - pl                                                                    */
/* 3 - buf                                                                   */
/*****************************************************************************/
CFParm* CFWNewDrawInfo(CFParm* PParm)
{
    new_draw_info(*(int *)(PParm->Value[0]),
                  *(int *)(PParm->Value[1]),
                  (object *)(PParm->Value[2]),
                  (char *)(PParm->Value[3]));
    return NULL;
};

/*****************************************************************************/
/* move_player wrapper.                                                      */
/*****************************************************************************/
/* 0 - player to move                                                        */
/* 1 - direction of move                                                     */
/*****************************************************************************/
CFParm* CFWMovePlayer (CFParm* PParm)
{
    CFParm* CFP;
    static int val;
    val=move_player ((object*)PParm->Value[0],
                     *(int*)PParm->Value[1]);
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    CFP->Value[0] = (void*) &val;
    return(CFP);
}

/*****************************************************************************/
/* move_object wrapper.                                                      */
/*****************************************************************************/
/* 0 - object to move                                                        */
/* 1 - direction of move                                                     */
/* 2 - originator                                                            */
/*****************************************************************************/
CFParm* CFWMoveObject (CFParm* PParm)
{
    CFParm* CFP;
    static int val;
    val=move_ob ((object*)PParm->Value[0],
                 *(int*)PParm->Value[1],
                 (object*)PParm->Value[2]);
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    CFP->Value[0] = (void*) &val;

    return(CFP);
}

/*****************************************************************************/
/* send_plugin_custom_message wrapper.                                       */
/*****************************************************************************/
/* 0 - player to send command to                                             */
/* 1 - command to send                                                       */
/*****************************************************************************/
CFParm* CFWSendCustomCommand(CFParm* PParm)
{
    send_plugin_custom_message((object *)(PParm->Value[0]),(char *)(PParm->Value[1]));
    return NULL;
};

/*****************************************************************************/
/* cftimer_create wrapper.                                                   */
/*****************************************************************************/
/* 0 - id                                                                    */
/* 1 - delay                                                                 */
/* 2 - object                                                                */
/* 3 - mode                                                                  */
/*****************************************************************************/
CFParm* CFWCFTimerCreate(CFParm* PParm)
{
    CFParm* CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = cftimer_create(*(int *)(PParm->Value[0]),
                         *(long *)(PParm->Value[1]),
                         (object *)(PParm->Value[2]),
                         *(int *)(PParm->Value[3]));
    CFP->Value[0] = (void *)(&val);
    return CFP;
};

/*****************************************************************************/
/* cftimer_destroy wrapper.                                                  */
/*****************************************************************************/
/* 0 - id                                                                    */
/*****************************************************************************/
CFParm* CFWCFTimerDestroy(CFParm* PParm)
{
    CFParm* CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = cftimer_destroy(*(int *)(PParm->Value[0]));
    CFP->Value[0] = (void *)(&val);
    return CFP;
};

/*****************************************************************************/
/* SET_ANIMATION wrapper.                                                    */
/*****************************************************************************/
/* 0 - object                                                                */
/* 1 - face                                                                  */
/*****************************************************************************/
CFParm* CFWSetAnimation (CFParm* PParm)
{
    object* op=(object*)PParm->Value[0];
    int face=*(int*)PParm->Value[1];
    if (face!=-1)
        {
        SET_ANIMATION (op,face);
        }
    update_object(op, UP_OBJ_FACE);
    return(NULL);
}

/*****************************************************************************/
/* communicate wrapper.                                                      */
/*****************************************************************************/
/* 0 - object                                                                */
/* 1 - string                                                                */
/*****************************************************************************/
CFParm* CFWCommunicate (CFParm* PParm)
{
    char buf[MAX_BUF];
    object* op=(object*)PParm->Value[0];
    char* string=(char*)PParm->Value[1];
    if ((!op) || (!string)) return NULL;
    sprintf(buf, "%s says: ",op->name);
    strncat(buf, string, MAX_BUF - strlen(buf)-1);
    buf[MAX_BUF-1]=0;
    new_info_map(NDI_WHITE,op->map, buf);
    communicate (op,string);
    return NULL;
}

/*****************************************************************************/
/* find_best_object_match wrapper.                                           */
/*****************************************************************************/
/* 0 - object to find object in inventory                                    */
/* 1 - name                                                                  */
/*****************************************************************************/
CFParm* CFWFindBestObjectMatch (CFParm* PParm)
{
    CFParm* CFP;
    object* op=(object*)PParm->Value[0];
    char* param=(char*)PParm->Value[1];
    object* result;
    result=(object*)find_best_object_match(op,param);
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    CFP->Value[0] = (void*) result;

    return(CFP);
}

/*****************************************************************************/
/* player_apply_below wrapper.                                               */
/*****************************************************************************/
/* 0 - object player                                                         */
/*****************************************************************************/
CFParm* CFWApplyBelow (CFParm* PParm)
{
    object* op=(object*)PParm->Value[0];
    if (!op) return NULL;
    player_apply_below (op);
    return NULL;
}

/*****************************************************************************/
/* free_object wrapper.                                                      */
/*****************************************************************************/
/* 0 - object                                                                */
/*****************************************************************************/
CFParm* CFWFreeObject (CFParm* PParm)
{
    object* op=(object*)PParm->Value[0];
    if (op) free_object(op);
    return NULL;
}

/*****************************************************************************/
/* fix_player wrapper.                                                      */
/*****************************************************************************/
/* 0 - object                                                                */
/*****************************************************************************/
CFParm* CFWFixPlayer (CFParm* PParm)
{
    object* op=(object*)PParm->Value[ 0 ];
    if (op) fix_player(op);
    return NULL;
}

/*****************************************************************************/
/* free wrapper.                                                             */
/*****************************************************************************/
/* 0 - pointer                                                               */
/*****************************************************************************/
CFParm* CFWFreeMemory(CFParm* PParm)
{
    void* op = PParm->Value[ 0 ];
    if (op) free(op);
    return NULL;
}

/*****************************************************************************/
/* ObjectCreateClone object_copy wrapper.                                    */
/*****************************************************************************/
/* 0 - object                                                                */
/* 1 - type 0 = clone with inventory                                         */
/*          1 = only duplicate the object without its content and op->more   */
/*****************************************************************************/
CFParm* CFWObjectCreateClone (CFParm* PParm)
{
    CFParm* CFP=(CFParm*)malloc(sizeof (CFParm));
    if (*(int*)PParm->Value[1]==0)
        CFP->Value[0]=ObjectCreateClone ((object*)PParm->Value[0]);
    else if (*(int*)PParm->Value[1]==1)
        {
        object* tmp;
        tmp = get_object();
        copy_object((object*)PParm->Value[0],tmp);
        CFP->Value[0]=tmp;
        }
    return CFP;
}

/*****************************************************************************/
/* set_variable wrapper -- Sets a variable in a given object.                */
/*****************************************************************************/
/* 0 - object to change                                                      */
/* 1 - string describing the change to make.                                 */
/*****************************************************************************/
CFParm* CFWSetVariable(CFParm* PParm)
{
    set_variable((object*)(PParm->Value[0]), (char *)(PParm->Value[1]));
    return NULL;
}

/*****************************************************************************/
/* decrease_ob_nr wrapper.                                                   */
/*****************************************************************************/
/* 0 - object to change                                                      */
/* 1 - nrof                                                                  */
/*****************************************************************************/
CFParm* CFWDecreaseObjectNR(CFParm* PParm)
{
    CFParm* CFP=(CFParm*)malloc(sizeof (CFParm));
    object* op=(object*)PParm->Value[0];
    int i=*(int*)PParm->Value[1];
    object* ob;

    ob = decrease_ob_nr (op, i);
    CFP->Value[0] = ob;
    return CFP;
}

/*****************************************************************************/
/* teleport an object to another map                                         */
/*****************************************************************************/
/* 0 - object                                                                */
/* 1 - map structure                                                         */
/* 2 - mapx                                                                  */
/* 3 - mapy                                                                  */
/* return value:                                                             */
/*  0: ok                                                                    */
/*  1: can't find a free spot                                                */
/*  2: out of map                                                            */
/*****************************************************************************/
CFParm* CFWTeleportObject (CFParm* PParm)
{
    mapstruct* map;
    int x, y;
    object* who;
    CFParm* CFP=(CFParm*)malloc(sizeof (CFParm));
    static int result;

    who = ( object* )PParm->Value[ 0 ];
    map = ( mapstruct* )PParm->Value[ 1 ];
    x = *( int* )PParm->Value[ 2 ];
    y = *( int* )PParm->Value[ 3 ];

    CFP->Value[ 0 ] = (void *)&result;
    if ( ( out_of_map( map,x,y ) ) == 0 )
    {
        int k;
        object *tmp;
        k = find_first_free_spot(who->arch,map,x,y);
        if (k==-1)
        {
            result = 1;
            return CFP;
        }

        remove_ob( who );

        for(tmp=who;tmp!=NULL;tmp=tmp->more)
            tmp->x=x+freearr_x[k]+(tmp->arch==NULL?0:tmp->arch->clone.x),
            tmp->y=y+freearr_y[k]+(tmp->arch==NULL?0:tmp->arch->clone.y);

        insert_ob_in_map( who, map, NULL, 0 );
        result = 0;
    };

    return CFP;
}

/*****************************************************************************/
/* check_trigger wrapper.                                                    */
/*****************************************************************************/
/* 0 - item to check                                                         */
/* 1 - cause                                                                 */
/*****************************************************************************/
CFParm* CFWCheckTrigger(CFParm* PParm)
{
    check_trigger( ( object* )( PParm->Value[ 0 ] ), ( object* )( PParm->Value[ 1 ] ) );
    return NULL;
};

/*****************************************************************************/
/* out_of_map wrapper.                                                       */
/*****************************************************************************/
/* 0 - map                                                                   */
/* 1 - x                                                                     */
/* 2 - y                                                                     */
/*****************************************************************************/
CFParm* CFWOutOfMap(CFParm* PParm)
{
    static int val;
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));
    val = out_of_map( ( mapstruct* )PParm->Value[ 0 ], *( int* )PParm->Value[ 1 ], *( int* )PParm->Value[ 2 ] );
    CFP->Value[ 0 ] = ( void* )&val;
    return CFP;
};

/*****************************************************************************/
/* query_name wrapper.                                                       */
/*****************************************************************************/
/* 0 - item                                                                   */
/*****************************************************************************/
CFParm* CFWQueryName(CFParm* PParm)
{
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));
    CFP->Value[ 0 ] = query_name( ( object* )PParm->Value[ 0 ] );
    return CFP;
};

/*****************************************************************************/
/* query_base_name wrapper.                                                  */
/*****************************************************************************/
/* 0 - item                                                                  */
/* 1 - plural                                                                */
/*****************************************************************************/
CFParm* CFWQueryBaseName(CFParm* PParm)
{
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));
    CFP->Value[ 0 ] = (char*)query_base_name( ( object* )PParm->Value[ 0 ], *( int* )PParm->Value[ 1 ] );
    return CFP;
};

/*****************************************************************************/
/* insert_ob_ino_b wrapper.                                                  */
/*****************************************************************************/
/* 0 - item                                                                  */
/* 1 - plural                                                                */
/*****************************************************************************/
CFParm* CFWInsertObInOb(CFParm* PParm)
{
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));
    CFP->Value[ 0 ] = ( void* )insert_ob_in_ob( ( object* )PParm->Value[ 0 ], ( object* )PParm->Value[ 1 ] );
    return CFP;
};

/*****************************************************************************/
/* Returns the settings variable.                                            */
/*****************************************************************************/
/* no parameter                                                              */
/*****************************************************************************/
CFParm* CFWGetSettings(CFParm* PParm)
{
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));
    CFP->Value[ 0 ] = ( void* )&settings;
    return CFP;
};


CFParm* CFWGetMapFlags(CFParm* PParm)
    {
    static sint16 nx, ny;
    static mapstruct* newmap;
    static val;
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));

    val = get_map_flags(
        ( mapstruct* )PParm->Value[ 0 ],
        &newmap,
        *( sint16* )PParm->Value[ 1 ],
        *( sint16* )PParm->Value[ 2 ],
        &nx,
        &ny );

    CFP->Value[ 0 ] = ( void* )&val;
    CFP->Value[ 1 ] = ( void* )&newmap;
    CFP->Value[ 2 ] = ( void* )&nx;
    CFP->Value[ 3 ] = ( void* )&ny;
    return CFP;
    }

CFParm* CFWReCmp(CFParm* PParm)
    {
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));
    CFP->Value[ 0 ] = ( void* )re_cmp( PParm->Value[ 0 ], PParm->Value[ 1 ] );
    return CFP;
    }

CFParm* CFWStrdupLocal(CFParm* PParm)
    {
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));
    CFP->Value[ 0 ] = strdup_local( ( char* )PParm->Value[ 0 ] );
    return CFP;
    }

CFParm* CFWCreatePathname(CFParm* PParm)
    {
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));
    CFP->Value[ 0 ] = ( void* )create_pathname( ( char* )PParm->Value[ 0 ] );
    return CFP;
    }

CFParm* CFWUpdateObSpeed(CFParm* PParm)
    {
    update_ob_speed( ( object* )PParm->Value[ 0 ] );
    return NULL;
    }

CFParm* CFWPresentArchByName(CFParm* PParm)
    {
    int x, y;
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));
    x = *( int* )PParm->Value[ 2 ];
    y = *( int* )PParm->Value[ 1 ];
    CFP->Value[ 0 ] = ( void* )present_arch( find_archetype( ( const char* )PParm->Value[ 0 ] ), ( mapstruct* )PParm->Value[ 1 ], x, y );
    return CFP;
    }

CFParm* CFWPresentArchNameInOb(CFParm* PParm)
    {
    CFParm* CFP = (CFParm*)malloc(sizeof (CFParm));
    CFP->Value[ 0 ] = present_arch_in_ob( find_archetype( PParm->Value[ 0 ] ), PParm->Value[ 1 ] );
    return CFP;
    }

CFParm* CFWSetDirection(CFParm* PParm)
    {
    object* who = ( object* )PParm->Value[ 0 ];
    who->direction = ( char )PParm->Value[ 1 ];
    SET_ANIMATION(who, who->direction);
    return NULL;
    }

/*****************************************************************************/
/* The following is not really a wrapper like the others are.                */
/* It is in fact used to allow the plugin to request the global events it    */
/* wants to be aware of. All events can be seen as global; on the contrary,  */
/* some events can't be used as local: for example, BORN is only global.     */
/*****************************************************************************/
/* 0 - Number of the event to register;                                      */
/* 1 - String ID of the requesting plugin.                                   */
/*****************************************************************************/
CFParm* RegisterGlobalEvent(CFParm* PParm)
{
    int PNR = findPlugin((char *)(PParm->Value[1]));
#ifdef LOG_VERBOSE
    LOG(llevDebug,"Plugin %s (%i) registered the event %i\n",(char *)(PParm->Value[1]),PNR,*(int *)(PParm->Value[0]));
#endif
    LOG(llevDebug,"Plugin %s (%i) registered the event %i\n",(char *)(PParm->Value[1]),PNR,*(int *)(PParm->Value[0]));
    PlugList[PNR].gevent[*(int *)(PParm->Value[0])] = 1;
    return NULL;
};

/*****************************************************************************/
/* The following unregisters a global event.                                 */
/*****************************************************************************/
/* 0 - Number of the event to unregister;                                    */
/* 1 - String ID of the requesting plugin.                                   */
/*****************************************************************************/
CFParm* UnregisterGlobalEvent(CFParm* PParm)
{
    int PNR = findPlugin((char *)(PParm->Value[1]));
    PlugList[PNR].gevent[*(int *)(PParm->Value[0])] = 0;
    return NULL;
};

/*****************************************************************************/
/* When a specific global event occurs, this function is called.             */
/* Concerns events: BORN, QUIT, LOGIN, LOGOUT, SHOUT, MUZZLE, KICK for now.  */
/*****************************************************************************/
/* 0 - Number of the event                                                   */
/*****************************************************************************/
void GlobalEvent(CFParm *PParm)
{
    int i;
    for(i=0;i<PlugNR;i++)
    {
        if (PlugList[i].gevent[*(int *)(PParm->Value[0])] != 0)
        {
        /* That plugin registered the event ! Then we pass it the event */
            (PlugList[i].eventfunc)(PParm);
        };
    };
};
