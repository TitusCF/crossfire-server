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
/* Note that it has a lot of Win32-Specific code; this is because Windows    */
/* uses specific function calls to handle dynamic libraries (the DLLs), where*/
/* UNIX-based systems use the dlxxx functions.                               */
/* Note that initPlugins() also got a specific Win32 version, not because of */
/* the library-handling, but because scandir does not exist on Win32.        */
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
            RTNCmd.time = *(float *)(RTNValue->Value[2]);
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
int findPlugin(char* id)
{
    int i;
    for(i=0; i<PlugNR; i++)
        if(!strcmp(id,PlugList[i].id))
            return i;
    return -1;
};

#ifdef WIN32
/*****************************************************************************/
/* WIN32 Plugins initialization. Browses the plugins directory and call      */
/* initOnePlugin for each file found.                                        */
/*****************************************************************************/
void initPlugins(void)
{
    struct dirent *currentfile;
    DIR *plugdir;
    int n;
    char buf[MAX_BUF];
    char buf2[MAX_BUF];

    LOG(llevInfo,"Now initializing plugins\n");
    strcpy(buf,LIBDIR);
    strcat(buf,"/plugins/");
    LOG(llevInfo,"Plugins directory is %s\n",buf);

    plugdir = opendir(buf);

    n = 0;

    while(currentfile = readdir(plugdir))
    {
        if (strcmp(currentfile->d_name,".."))
        {
            if (strcmp(currentfile->d_name,"."))
            {
                strcpy(buf2,buf);
                strcat(buf2,currentfile->d_name);
                LOG(llevInfo,"Registering plugin %s\n",currentfile->d_name);
                initOnePlugin(buf2);
            }
        }
    }
};

/*****************************************************************************/
/* WIN32 Plugin initialization. Initializes a plugin known by its filename.  */
/* The initialization process has several stages:                            */
/* - Loading of the DLL itself;                                              */
/* - Basical plugin information request;                                     */
/* - CF-Plugin specific initialization tasks (call to initPlugin());         */
/* - Hook bindings;                                                          */
/*****************************************************************************/
void initOnePlugin(char* pluginfile)
{
    int i=0;
    HMODULE DLLInstance;
    void *ptr = NULL;
    CFParm* HookParm;

    if ((DLLInstance = LoadLibrary(pluginfile))==NULL)
    {
        LOG(llevError,"Error while trying to load %s\n",pluginfile);
        return;
    }
    PlugList[PlugNR].libptr = DLLInstance;
    PlugList[PlugNR].initfunc = (f_plugin)(GetProcAddress(DLLInstance,"initPlugin"));
    if (PlugList[PlugNR].initfunc==NULL)
    {
        LOG(llevError,"Plugin init error\n");
        FreeLibrary(ptr);
        return;
    }
    else
    {
        CFParm* InitParm;
        InitParm = PlugList[PlugNR].initfunc(NULL);
        LOG(llevInfo,"Plugin name: %s, known as %s\n",
            (char *)(InitParm->Value[1]),
            (char *)(InitParm->Value[0])
            );
        strcpy(PlugList[PlugNR].id,(char *)(InitParm->Value[0]));
        strcpy(PlugList[PlugNR].fullname,(char *)(InitParm->Value[1]));
    }
    PlugList[PlugNR].hookfunc = (f_plugin)(GetProcAddress(DLLInstance,"registerHook"));
    PlugList[PlugNR].eventfunc = (f_plugin)(GetProcAddress(DLLInstance,"triggerEvent"));
    PlugList[PlugNR].pinitfunc = (f_plugin)(GetProcAddress(DLLInstance,"postinitPlugin"));
    PlugList[PlugNR].propfunc = (f_plugin)(GetProcAddress(DLLInstance,"getPluginProperty"));
    if (PlugList[PlugNR].pinitfunc==NULL)
    {
        LOG(llevError,"Plugin postinit error\n");
        FreeLibrary(ptr);
        return;
    }

    for(i=0;i<NR_EVENTS;i++)
        PlugList[PlugNR].gevent[i] = 0;
    if (PlugList[PlugNR].hookfunc==NULL)
    {
        LOG(llevError, "Plugin hook error\n");
        FreeLibrary(ptr);
        return;
    }
    else
    {
        int j;
        i = 0;
        HookParm = (CFParm *)(malloc(sizeof(CFParm)));
        HookParm->Value[0]=(int *)(malloc(sizeof(int)));

        for(j=1; j<=NR_OF_HOOKS;j++)
        {
            memcpy(HookParm->Value[0], &j, sizeof(int));
            HookParm->Value[1] = HookList[j];
            /*switch(j)
            {
            case HOOK_NONE:
                break;
            case HOOK_LOG:
                HookParm->Value[1] = &CFWLog;
                break;
            case HOOK_NEWINFOMAP:
                HookParm->Value[1] = &CFWNewInfoMap;
                break;
            case HOOK_SPRINGTRAP:
                HookParm->Value[1] = &CFWSpringTrap;
                break;
            case HOOK_CASTSPELL:
                HookParm->Value[1] = &CFWCastSpell;
                break;
            case HOOK_CMDRSKILL:
                HookParm->Value[1] = &CFWCmdRSkill;
                break;
            case HOOK_BECOMEFOLLOWER:
                HookParm->Value[1] = &CFWBecomeFollower;
                break;
            case HOOK_PICKUP:
                HookParm->Value[1] = &CFWPickup;
                break;
            case HOOK_GETMAPOBJECT:
                HookParm->Value[1] = &CFWGetMapObject;
                break;
            case HOOK_ESRVSENDITEM:
                HookParm->Value[1] = &CFWESRVSendItem;
                break;
            case HOOK_FINDPLAYER:
                HookParm->Value[1] = &CFWFindPlayer;
                break;
            case HOOK_MANUALAPPLY:
                HookParm->Value[1] = &CFWManualApply;
                break;
            case HOOK_CMDDROP:
                HookParm->Value[1] = &CFWCmdDrop;
                break;
            case HOOK_CMDTAKE:
                HookParm->Value[1] = &CFWCmdTake;
                break;
            case HOOK_CMDTITLE:
                HookParm->Value[1] = &CFWCmdTitle;
                break;
            case HOOK_TRANSFEROBJECT:
                HookParm->Value[1] = &CFWTransferObject;
                break;
            case HOOK_KILLOBJECT:
                HookParm->Value[1] = &CFWKillObject;
                break;
            case HOOK_LEARNSPELL:
                HookParm->Value[1] = &CFWDoLearnSpell;
                break;
            case HOOK_FORGETSPELL:
                HookParm->Value[1] = &CFWDoForgetSpell;
                break;
            case HOOK_CHECKFORSPELL:
                HookParm->Value[1] = &CFWCheckSpellKnown;
                break;
            case HOOK_ESRVSENDINVENTORY:
                HookParm->Value[1] = &CFWESRVSendInventory;
                break;
            case HOOK_CREATEARTIFACT:
                HookParm->Value[1] = &CFWCreateArtifact;
                break;
            case HOOK_GETARCHETYPE:
                HookParm->Value[1] = &CFWGetArchetype;
                break;
            case HOOK_UPDATESPEED:
                HookParm->Value[1] = &CFWUpdateSpeed;
                break;
            case HOOK_UPDATEOBJECT:
                HookParm->Value[1] = &CFWUpdateObject;
                break;
            case HOOK_FINDANIMATION:
                HookParm->Value[1] = &CFWFindAnimation;
                break;
            case HOOK_GETARCHBYOBJNAME:
                HookParm->Value[1] = &CFWGetArchetypeByObjectName;
                break;
            case HOOK_INSERTOBJECTINMAP:
                HookParm->Value[1] = &CFWInsertObjectInMap;
                break;
            case HOOK_READYMAPNAME:
                HookParm->Value[1] = &CFWReadyMapName;
                break;
            case HOOK_ADDEXP:
                HookParm->Value[1] = &CFWAddExp;
                break;
            case HOOK_DETERMINEGOD:
                HookParm->Value[1] = &CFWDetermineGod;
                break;
            case HOOK_FINDGOD:
                HookParm->Value[1] = &CFWFindGod;
                break;
            case HOOK_DUMPOBJECT:
                HookParm->Value[1] = &CFWDumpObject;
                break;
            case HOOK_LOADOBJECT:
                HookParm->Value[1] = &CFWLoadObject;
                break;
            case HOOK_REGISTEREVENT:
                HookParm->Value[1] = &RegisterGlobalEvent;
                break;
            case HOOK_UNREGISTEREVENT:
                HookParm->Value[1] = &UnregisterGlobalEvent;
                break;
            case HOOK_REMOVEOBJECT:
                HookParm->Value[1] = &CFWRemoveObject;
                break;
            case HOOK_ADDSTRING:
                HookParm->Value[1] = &CFWAddString;
                break;
            case HOOK_ADDREFCOUNT:
                HookParm->Value[1] = &CFWAddRefcount;
                break;
            case HOOK_FREESTRING:
                HookParm->Value[1] = &CFWFreeString;
                break;
            case HOOK_GETFIRSTMAP:
                HookParm->Value[1] = &CFWGetFirstMap;
                break;
            case HOOK_GETFIRSTPLAYER:
                HookParm->Value[1] = &CFWGetFirstPlayer;
                break;
            case HOOK_GETFIRSTARCHETYPE:
                HookParm->Value[1] = &CFWGetFirstArchetype;
                break;
            case HOOK_QUERYCOST:
                HookParm->Value[1] = &CFWQueryCost;
                break;
            case HOOK_QUERYMONEY:
                HookParm->Value[1] = &CFWQueryMoney;
                break;
            case HOOK_PAYFORITEM:
                HookParm->Value[1] = &CFWPayForItem;
                break;
            case HOOK_PAYFORAMOUNT:
                HookParm->Value[1] = &CFWPayForAmount;
                break;
            case HOOK_NEWDRAWINFO:
                HookParm->Value[1] = &CFWNewDrawInfo;
                break;
            case HOOK_MOVEPLAYER:
                HookParm->Value[1] = &CFWMovePlayer;
                break;
            case HOOK_MOVEOBJECT:
                HookParm->Value[1] = &CFWMoveObject;
                break;
            };*/

/*  Serious bug, fix this in all local copies */
/*            HookParm->dparm = 2044; */
            PlugList[PlugNR].hookfunc(HookParm);
        };
        free(HookParm->Value[0]);
        free(HookParm);
    };
    if (PlugList[PlugNR].eventfunc==NULL)
    {
        LOG(llevError,"Event plugin error\n");
        FreeLibrary(ptr);
        return;
    };
    PlugNR++;
    PlugList[PlugNR-1].pinitfunc(NULL);
    LOG(llevInfo,"Done\n");
};

/*****************************************************************************/
/* Removes one plugin from memory. The plugin is identified by its keyname.  */
/*****************************************************************************/
void removeOnePlugin(char *id)
{
    int plid;
    int j;
    LOG(llevDebug,"Warning - removeOnePlugin non-canon under Win32\n");
    plid = findPlugin(id);
    if (plid<0)
        return;
    /* We unload the library... */
    FreeLibrary(PlugList[plid].libptr);
    /* Then we copy the rest on the list back one position */
    PlugNR--;
    if (plid==31) return;
    for (j=plid+1;j<32;j++)
    {
        PlugList[j-1] = PlugList[j];
    };
};

#else

#ifndef HAVE_SCANDIR

extern int alphasort( struct dirent **a, struct dirent **b);
#endif

/*****************************************************************************/
/* UNIX Plugins initialization. Browses the plugins directory and call       */
/* initOnePlugin for each file found.                                        */
/*****************************************************************************/
void initPlugins(void)
{
        struct dirent **namelist=NULL;
        int n,l;
        char buf[MAX_BUF];
        char buf2[MAX_BUF];

        LOG(llevInfo,"Initializing plugins :\n");
        strcpy(buf,LIBDIR);
        strcat(buf,"/plugins/");
        n = scandir(buf, &namelist, 0, alphasort);
        if (n < 0)
            perror("scandir");
        else
            while(n--)
            {
		l=strlen(namelist[n]->d_name);
		if (l>strlen(PLUGIN_SUFFIX))
		{
		    if (!strcmp(namelist[n]->d_name+l-strlen(PLUGIN_SUFFIX),PLUGIN_SUFFIX))
                    {
                        strcpy(buf2,buf);
                        strcat(buf2,namelist[n]->d_name);
                        LOG(llevInfo," -> Loading plugin : %s\n",namelist[n]->d_name);
                        initOnePlugin(buf2);
                    };
                }
            };
        if (namelist != NULL) free(namelist);
};

/*****************************************************************************/
/* Removes one plugin from memory. The plugin is identified by its keyname.  */
/*****************************************************************************/
void removeOnePlugin(char *id)
{
    int plid;
    int j;
    plid = findPlugin(id);
    if (plid<0)
        return;
    /* We unload the library... */
    dlclose(PlugList[plid].libptr);
    /* Then we copy the rest on the list back one position */
    PlugNR--;
    if (plid==31) return;
    LOG(llevInfo,"plid=%i, PlugNR=%i\n",plid,PlugNR);
    for (j=plid+1;j<32;j++)
    {
        PlugList[j-1] = PlugList[j];
    };
};

/*****************************************************************************/
/* UNIX Plugin initialization. Initializes a plugin known by its filename.   */
/* The initialization process has several stages:                            */
/* - Loading of the DLL itself;                                              */
/* - Basical plugin information request;                                     */
/* - CF-Plugin specific initialization tasks (call to initPlugin());         */
/* - Hook bindings;                                                          */
/*****************************************************************************/
void initOnePlugin(char* pluginfile)
{
        int i=0;
        void *ptr = NULL;
        CFParm* HookParm;
        if ((ptr=dlopen(pluginfile,RTLD_NOW|RTLD_GLOBAL))==NULL)
        {
                LOG(llevInfo,"Plugin error: %s\n", dlerror());
                return;
        };
        PlugList[PlugNR].libptr = ptr;
        PlugList[PlugNR].initfunc = (f_plugin)(dlsym(ptr,"initPlugin"));
        if (PlugList[PlugNR].initfunc==NULL)
        {
                LOG(llevInfo,"Plugin init error: %s\n", dlerror());
        }
        else
        {
            CFParm* InitParm;
            InitParm = PlugList[PlugNR].initfunc(NULL);
            LOG(llevInfo,"    Plugin %s loaded under the name of %s\n",
                (char *)(InitParm->Value[1]),
                (char *)(InitParm->Value[0])
            );
            strcpy(PlugList[PlugNR].id,(char *)(InitParm->Value[0]));
            strcpy(PlugList[PlugNR].fullname,(char *)(InitParm->Value[1]));
        };
        PlugList[PlugNR].hookfunc = (f_plugin)(dlsym(ptr,"registerHook"));
        PlugList[PlugNR].eventfunc = (f_plugin)(dlsym(ptr,"triggerEvent"));
        PlugList[PlugNR].pinitfunc = (f_plugin)(dlsym(ptr,"postinitPlugin"));
        PlugList[PlugNR].propfunc = (f_plugin)(dlsym(ptr,"getPluginProperty"));
        LOG(llevInfo,"Done\n");
        if (PlugList[PlugNR].pinitfunc==NULL)
        {
                LOG(llevInfo,"Plugin postinit error: %s\n", dlerror());
        }

        for(i=0;i<NR_EVENTS;i++)
        {
            PlugList[PlugNR].gevent[i] = 0;
        };
        if (PlugList[PlugNR].hookfunc==NULL)
        {
                LOG(llevInfo,"Plugin hook error: %s\n", dlerror());
        }
        else
        {
                int j;
                i = 0;
                HookParm = (CFParm *)(malloc(sizeof(CFParm)));
                HookParm->Value[0]=(int *)(malloc(sizeof(int)));

                for(j=1; j<=NR_OF_HOOKS;j++)
                {
                    memcpy(HookParm->Value[0], &j, sizeof(int));
                    HookParm->Value[1] = HookList[j];
                    /*switch(j)
                    {
                        case HOOK_NONE:
                            break;
                        case HOOK_LOG:
                            HookParm->Value[1] = &CFWLog;
                            break;
                        case HOOK_NEWINFOMAP:
                            HookParm->Value[1] = &CFWNewInfoMap;
                            break;
                        case HOOK_SPRINGTRAP:
                            HookParm->Value[1] = &CFWSpringTrap;
                            break;
                        case HOOK_CASTSPELL:
                            HookParm->Value[1] = &CFWCastSpell;
                            break;
                        case HOOK_CMDRSKILL:
                            HookParm->Value[1] = &CFWCmdRSkill;
                            break;
                        case HOOK_BECOMEFOLLOWER:
                            HookParm->Value[1] = &CFWBecomeFollower;
                            break;
                        case HOOK_PICKUP:
                            HookParm->Value[1] = &CFWPickup;
                            break;
                        case HOOK_GETMAPOBJECT:
                            HookParm->Value[1] = &CFWGetMapObject;
                            break;
                        case HOOK_ESRVSENDITEM:
                            HookParm->Value[1] = &CFWESRVSendItem;
                            break;
                        case HOOK_FINDPLAYER:
                            HookParm->Value[1] = &CFWFindPlayer;
                            break;
                        case HOOK_MANUALAPPLY:
                            HookParm->Value[1] = &CFWManualApply;
                            break;
                        case HOOK_CMDDROP:
                            HookParm->Value[1] = &CFWCmdDrop;
                            break;
                        case HOOK_CMDTAKE:
                            HookParm->Value[1] = &CFWCmdTake;
                            break;
                        case HOOK_CMDTITLE:
                            HookParm->Value[1] = &CFWCmdTitle;
                            break;
                        case HOOK_TRANSFEROBJECT:
                            HookParm->Value[1] = &CFWTransferObject;
                            break;
                        case HOOK_KILLOBJECT:
                            HookParm->Value[1] = &CFWKillObject;
                            break;
                        case HOOK_LEARNSPELL:
                            HookParm->Value[1] = &CFWDoLearnSpell;
                            break;
                        case HOOK_FORGETSPELL:
                            HookParm->Value[1] = &CFWDoForgetSpell;
                            break;
                        case HOOK_CHECKFORSPELL:
                            HookParm->Value[1] = &CFWCheckSpellKnown;
                            break;
                        case HOOK_ESRVSENDINVENTORY:
                            HookParm->Value[1] = &CFWESRVSendInventory;
                            break;
                        case HOOK_CREATEARTIFACT:
                            HookParm->Value[1] = &CFWCreateArtifact;
                            break;
                        case HOOK_GETARCHETYPE:
                            HookParm->Value[1] = &CFWGetArchetype;
                            break;
                        case HOOK_UPDATESPEED:
                            HookParm->Value[1] = &CFWUpdateSpeed;
                            break;
                        case HOOK_UPDATEOBJECT:
                            HookParm->Value[1] = &CFWUpdateObject;
                            break;
                        case HOOK_FINDANIMATION:
                            HookParm->Value[1] = &CFWFindAnimation;
                            break;
                        case HOOK_GETARCHBYOBJNAME:
                            HookParm->Value[1] = &CFWGetArchetypeByObjectName;
                            break;
                        case HOOK_INSERTOBJECTINMAP:
                            HookParm->Value[1] = &CFWInsertObjectInMap;
                            break;
                        case HOOK_READYMAPNAME:
                            HookParm->Value[1] = &CFWReadyMapName;
                            break;
                        case HOOK_ADDEXP:
                            HookParm->Value[1] = &CFWAddExp;
                            break;
                        case HOOK_DETERMINEGOD:
                            HookParm->Value[1] = &CFWDetermineGod;
                            break;
                        case HOOK_FINDGOD:
                            HookParm->Value[1] = &CFWFindGod;
                            break;
                        case HOOK_DUMPOBJECT:
                            HookParm->Value[1] = &CFWDumpObject;
                            break;
                        case HOOK_LOADOBJECT:
                            HookParm->Value[1] = &CFWLoadObject;
                            break;
                        case HOOK_REGISTEREVENT:
                            HookParm->Value[1] = &RegisterGlobalEvent;
                            break;
                        case HOOK_UNREGISTEREVENT:
                            HookParm->Value[1] = &UnregisterGlobalEvent;
                            break;
                        case HOOK_REMOVEOBJECT:
                            HookParm->Value[1] = &CFWRemoveObject;
                            break;
                        case HOOK_ADDSTRING:
                            HookParm->Value[1] = &CFWAddString;
                            break;
                        case HOOK_ADDREFCOUNT:
                            HookParm->Value[1] = &CFWAddRefcount;
                            break;
                        case HOOK_FREESTRING:
                            HookParm->Value[1] = &CFWFreeString;
                            break;
                        case HOOK_GETFIRSTMAP:
                            HookParm->Value[1] = &CFWGetFirstMap;
                            break;
                        case HOOK_GETFIRSTPLAYER:
                            HookParm->Value[1] = &CFWGetFirstPlayer;
                            break;
                        case HOOK_GETFIRSTARCHETYPE:
                            HookParm->Value[1] = &CFWGetFirstArchetype;
                            break;
                        case HOOK_QUERYCOST:
                            HookParm->Value[1] = &CFWQueryCost;
                            break;
                        case HOOK_QUERYMONEY:
                            HookParm->Value[1] = &CFWQueryMoney;
                            break;
                        case HOOK_PAYFORITEM:
                            HookParm->Value[1] = &CFWPayForItem;
                            break;
                        case HOOK_PAYFORAMOUNT:
                            HookParm->Value[1] = &CFWPayForAmount;
                            break;
                        case HOOK_NEWDRAWINFO:
                            HookParm->Value[1] = &CFWNewDrawInfo;
                            break;
                        case HOOK_SENDCUSTOMCOMMAND:
                            HookParm->Value[1] = &CFWSendCustomCommand;
                            break;
                    };*/
                    PlugList[PlugNR].hookfunc(HookParm);
                };
                free(HookParm->Value[0]);
                free(HookParm);
        };
        if (PlugList[PlugNR].eventfunc==NULL)
        {
                LOG(llevError,"Event plugin error %s\n", dlerror());
        };
        PlugNR++;
        PlugList[PlugNR-1].pinitfunc(NULL);
        LOG(llevInfo,"[Done]\n");
};
#endif /*WIN32*/

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
/* 3 - type of casting;                                                      */
/* 4 - is it an ability or a wizard spell ?                                  */
/* 5 - spelltype;                                                            */
/* 6 - optional args;                                                        */
/* 7 - type of firing;                                                       */
/*                                                                           */
/*****************************************************************************/
CFParm* CFWCastSpell(CFParm* PParm)
{
    static int val;
    CFParm *CFP;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
/*int cast_spell(object *op, object *caster, int dir, int type, int ability, */
/*SpellTypeFrom item, char *stringarg); */
    val = cast_spell((object *)(PParm->Value[0]),(object *)(PParm->Value[1]),
        *(int *)(PParm->Value[2]),*(int *)(PParm->Value[3]),
        *(int *)(PParm->Value[4]),*(SpellTypeFrom *)(PParm->Value[5]),
        (char *)(PParm->Value[6])/*,*(int *) (PParm->Value[7])*/);
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
/* 0 - picker object;                                                        */
/* 1 - picked object.                                                        */
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
    return(PParm);
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
/* 1 - spell index to forget.                                                */
/*****************************************************************************/
CFParm* CFWDoForgetSpell(CFParm* PParm)
{
    do_forget_spell(
        (object *)(PParm->Value[0]),
        *(int *)(PParm->Value[1])
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
        *(int *)(PParm->Value[1]),
        *(int *)(PParm->Value[2])
    );
    return NULL;
};

/*****************************************************************************/
/* check_spell_known wrapper.                                                */
/*****************************************************************************/
/* 0 - object to check;                                                      */
/* 1 - spell index to search.                                                */
/*****************************************************************************/
CFParm* CFWCheckSpellKnown(CFParm* PParm)
{
    CFParm *CFP;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = check_spell_known(
        (object *)(PParm->Value[0]),
        *(int *)(PParm->Value[1])
        );
    CFP->Value[0] = &val;
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
    object* val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = create_artifact(
        (object *)(PParm->Value[0]),
        (char *)(PParm->Value[1])
    );
    CFP->Value[0] = (void *)(val);
    return CFP;
};

/*****************************************************************************/
/* get_archetype wrapper.                                                    */
/*****************************************************************************/
/* 0 - Name of the archetype to search for.                                  */
/*****************************************************************************/
CFParm* CFWGetArchetype(CFParm* PParm)
{
    /*object* get_archetype(char* name); */
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
/*****************************************************************************/
CFParm* CFWAddExp(CFParm* PParm)
{
    add_exp(
        (object *)(PParm->Value[0]),
        *(int *)(PParm->Value[1])
    );
    return(PParm);
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
    val = determine_god(
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
    char*   val;
/*    object* ob; not used */
    val = (char *)(malloc(sizeof(char)*10240));
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
    LOG(llevDebug,"CFWLoadObject: %s\n",query_name(val));
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
CFParm* CFWAddString(CFParm* PParm)
{
    CFParm* CFP;
    char* val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = (char *)(PParm->Value[0]);
    CFP->Value[0] = (void*) add_string (val);
    return CFP;
};

CFParm* CFWAddRefcount(CFParm* PParm)
{
    CFParm* CFP;
    char* val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    val = (char *)(PParm->Value[0]);
    CFP->Value[0] = (void*) add_refcount (val);
    return CFP;
};
CFParm* CFWFreeString(CFParm* PParm)
{
/*  CFParm* CFP; not used */
    char* val;
    val = (char *)(PParm->Value[0]);
    free_string (val);
    return NULL;
};

CFParm* CFWGetFirstMap(CFParm* PParm)
{
    CFParm* CFP;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    CFP->Value[0] = (void*)(first_map) ;
    return CFP;
};

CFParm* CFWGetFirstPlayer(CFParm* PParm)
{
    CFParm* CFP;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    CFP->Value[0] = (void*)(first_player) ;
    return CFP;
};

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
    static int val;
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
    static int val;
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
    int amount;
    object* whoptr;
    static int val;
    CFP = (CFParm*)(malloc(sizeof(CFParm)));
    amount = *(int *)(PParm->Value[0]);
    whoptr = (object *)(PParm->Value[1]);
    val= pay_for_amount (amount,whoptr);
    CFP->Value[0] = (void*) &val;
    return CFP;
};

/*new_draw_info(int flags, int pri, object *pl, const char *buf); */
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

CFParm* CFWSendCustomCommand(CFParm* PParm)
{
    send_plugin_custom_message((object *)(PParm->Value[0]),(char *)(PParm->Value[1]));
    return NULL;
};

CFParm* CFWCFTimerCreate(CFParm* PParm)
{
/*int cftimer_create(int id, long delay, object* ob, int mode) */
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

CFParm* CFWCFTimerDestroy(CFParm* PParm)
{
/*int cftimer_destroy(int id) */
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
    return(PParm);    
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
/* ObjectCreateClone object_copy wrapper.                                    */
/*****************************************************************************/
/* 0 - object                                                                */
/* 1 - type 0 = clone with inventory                                         */
/*          1 = only duplicate the object without it's content and op->more  */
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
/* Sets a variable in a given object.                                        */
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
/* teleport an object to another map                                         */
/*****************************************************************************/
/* 0 - object                                                                */
/* 1 - mapname we use for destination                                        */
/* 2 - mapx                                                                  */
/* 3 - mapy                                                                  */
/* 4 - unique?                                                               */
/* 5 - msg (used for random maps entering. May be NULL)                      */
/*****************************************************************************/
CFParm* CFWTeleportObject (CFParm* PParm)
{
    object* current;
/*    char * mapname; not used
    int mapx;
    int mapy;
    int unique; not used */
    current=get_object();
    EXIT_PATH(current)=add_string ((char*)PParm->Value[1]);
    EXIT_X(current)=*(int*)PParm->Value[2];
    EXIT_Y(current)=*(int*)PParm->Value[3];
    if (*(int*)PParm->Value[4]) SET_FLAG(current,FLAG_UNIQUE);
    if (PParm->Value[5]) current->msg=add_string ((char*)PParm->Value[5]);
    enter_exit ((object*) PParm->Value[0],current);
    free_object (current);
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
/* Concerns events: BORN, QUIT, LOGIN, LOGOUT, SHOUT for now.                */
/*****************************************************************************/
/*                                                                           */
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
