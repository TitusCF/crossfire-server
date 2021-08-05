/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 1999-2014 Mark Wedel and the Crossfire Development Team
 * Copyright (c) 1992 Frank Tore Johansen
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

/**
 * @file
 * Contains the definition for all in-game commands a player can issue.
 */

extern "C" {
#include "global.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "commands.h"
#include "sproto.h"
#include "assert.h"
}
#include <map>
#include <vector>
#include <string>
#include <algorithm>

struct registered_command {
    command_registration registration;
    command_array_struct *command;
    uint8_t type;
};

static command_registration next_registration = 1;
static std::map<std::string, std::vector<registered_command *> > registered_commands;

/**
 * Normal game commands.
 */
static command_array_struct Commands[] = {
    { "afk", command_afk,                0.0 },
    { "apply", command_apply,            1.0 },   /* should be variable */
    { "applymode", command_applymode,    1.0 },   /* should be variable */
    { "body", command_body,              0.0 },
    { "bowmode", command_bowmode,        0.0 },
    { "brace", command_brace,            0.0 },
    { "cast", command_cast,              0.2 },   /* Is this right? */
    { "delete", command_delete,          0.0 },
    { "disarm", command_disarm,          1.0 },
    { "dm", command_dm,                  0.0 },
    { "dmhide", command_dmhide,          0.0 }, /* Like dm, but don't tell a dm arrived, hide player */
    { "drop", command_drop,              1.0 },
    { "dropall", command_dropall,        1.0 },
    { "empty", command_empty,            1.0 },
    { "examine", command_examine,        0.5 },
    { "fix_me", command_fix_me,          0.0 },
    { "follow", command_follow,          0.0 },
    { "forget_spell", command_forget_spell, 0.0 },
    { "get", command_take,               1.0 },
    { "help", command_help,              0.0 },
    { "hiscore", command_hiscore,        0.0 },
    { "inventory", command_inventory,    0.0 },
    { "invoke", command_invoke,          1.0 },
    { "killpets", command_kill_pets,     0.0 },
    { "language", command_language,      0.0 },
    { "listen", command_listen,          0.0 },
    { "lock", command_lock_item,         0.0 },
    { "maps", command_maps,              0.0 },
    { "mapinfo", command_mapinfo,        0.0 },
    { "mark", command_mark,              0.0 },
    { "motd", command_motd,              0.0 },
    { "news", command_news,              0.0 },
    { "party", command_party,            0.0 },
    { "party_rejoin", command_party_rejoin, 0.0 },
    { "passwd", command_passwd,          0.0 },
    { "peaceful", command_peaceful,      0.0 },
    { "petmode", command_petmode,        0.0 },
    { "pickup", command_pickup,          1.0 },
    { "prepare", command_prepare,        1.0 },
    { "printlos", command_printlos,      0.0 },
    { "quit", command_quit,              0.0 },
    { "ready_skill", command_rskill,     1.0 },
    { "rename", command_rename_item,     0.0 },
    { "resistances", command_resistances, 0.0 },
    { "rotateshoottype", command_rotateshoottype, 0.0 },
    { "rules", command_rules,            0.0 },
    { "save", command_save,              0.0 },
    { "skills", command_skills,          0.0 },   /* shows player list of skills */
    { "use_skill", command_uskill,       1.0 },
    { "search", command_search,          1.0 },
    { "search-items", command_search_items, 0.0 },
    { "showpets", command_showpets,      1.0 },
    { "sound", command_sound,            0.0 },
    { "statistics", command_statistics,  0.0 },
    { "take", command_take,              1.0 },
    { "throw", command_throw,            1.0 },
    { "time", command_time,              0.0 },
    { "title", command_title,            0.0 },
    { "use", command_use,                1.0 },
    { "usekeys", command_usekeys,        0.0 },
    { "whereabouts", command_whereabouts, 0.0 },
    { "whereami", command_whereami,      0.0 },
    { "unarmed_skill", command_unarmed_skill, 0.0 },
#ifdef DEBUG_MALLOC_LEVEL
    { "verify", command_malloc_verify,   0.0 },
#endif
    { "version", command_version,        0.0 },
    { "wimpy", command_wimpy,            0.0 },
    { "who", command_who,                0.0 },
    /*
     * directional commands
     */
    { "stay", command_stay,              1.0 }, /* 1.0 because it is used when using a
                                               *  skill on yourself */
    { "north", command_north,            1.0 },
    { "east", command_east,              1.0 },
    { "south", command_south,            1.0 },
    { "west", command_west,              1.0 },
    { "northeast", command_northeast,    1.0 },
    { "southeast", command_southeast,    1.0 },
    { "southwest", command_southwest,    1.0 },
    { "northwest", command_northwest,    1.0 },
    { "up",        command_up,           1.0 },
    { "down",      command_down,         1.0 },
    { "run", command_run,                1.0 },
    { "run_stop", command_run_stop,      0.0 },
    { "fire", command_fire,              1.0 },
    { "fire_stop", command_fire_stop,    0.0 },
    { "face", command_face,              0.0 },
    { "quest", command_quest,            0.0 },
    { "knowledge", command_knowledge,    0.0 },
    { NULL, NULL, 0.0 }
};

/** Chat/shout related commands. */
static command_array_struct CommunicationCommands [] = {
    { "tell", command_tell,              0.1 },
    { "reply", command_reply,            0.0 },
    { "say", command_say,                0.1 },
    { "gsay", command_gsay,              1.0 },
    { "shout", command_shout,            0.1 },
    { "chat", command_chat,              0.1 },
    { "me", command_me,                  0.1 },
    { "cointoss", command_cointoss,      0.0 },
    { "orcknuckle", command_orcknuckle,  0.0 },
    /*
     * begin emotions
     */
    { "nod", command_nod,                0.0 },
    { "dance", command_dance,            0.0 },
    { "kiss", command_kiss,              0.0 },
    { "bounce", command_bounce,          0.0 },
    { "smile", command_smile,            0.0 },
    { "cackle", command_cackle,          0.0 },
    { "laugh", command_laugh,            0.0 },
    { "giggle", command_giggle,          0.0 },
    { "shake", command_shake,            0.0 },
    { "puke", command_puke,              0.0 },
    { "growl", command_growl,            0.0 },
    { "scream", command_scream,          0.0 },
    { "sigh", command_sigh,              0.0 },
    { "sulk", command_sulk,              0.0 },
    { "hug", command_hug,                0.0 },
    { "cry", command_cry,                0.0 },
    { "poke", command_poke,              0.0 },
    { "accuse", command_accuse,          0.0 },
    { "grin", command_grin,              0.0 },
    { "bow", command_bow,                0.0 },
    { "clap", command_clap,              0.0 },
    { "blush", command_blush,            0.0 },
    { "burp", command_burp,              0.0 },
    { "chuckle", command_chuckle,        0.0 },
    { "cough", command_cough,            0.0 },
    { "flip", command_flip,              0.0 },
    { "frown", command_frown,            0.0 },
    { "gasp", command_gasp,              0.0 },
    { "glare", command_glare,            0.0 },
    { "groan", command_groan,            0.0 },
    { "hiccup", command_hiccup,          0.0 },
    { "lick", command_lick,              0.0 },
    { "pout", command_pout,              0.0 },
    { "shiver", command_shiver,          0.0 },
    { "shrug", command_shrug,            0.0 },
    { "slap", command_slap,              0.0 },
    { "smirk", command_smirk,            0.0 },
    { "snap", command_snap,              0.0 },
    { "sneeze", command_sneeze,          0.0 },
    { "snicker", command_snicker,        0.0 },
    { "sniff", command_sniff,            0.0 },
    { "snore", command_snore,            0.0 },
    { "spit", command_spit,              0.0 },
    { "strut", command_strut,            0.0 },
    { "thank", command_thank,            0.0 },
    { "twiddle", command_twiddle,        0.0 },
    { "wave", command_wave,              0.0 },
    { "whistle", command_whistle,        0.0 },
    { "wink", command_wink,              0.0 },
    { "yawn", command_yawn,              0.0 },
    { "beg", command_beg,                0.0 },
    { "bleed", command_bleed,            0.0 },
    { "cringe", command_cringe,          0.0 },
    { "think", command_think,            0.0 },
    { NULL, NULL, 0.0 }
};

/** Wizard commands. */
static command_array_struct WizCommands [] = {
    { "abil", command_abil,                      0.0 },
    { "accountpasswd", command_accountpasswd,    0.0 },
    { "addexp", command_addexp,                  0.0 },
    { "arrest", command_arrest,                  0.0 },
    { "banish", command_banish,                  0.0 },
    { "create", command_create,                  0.0 },
    { "debug", command_debug,                    0.0 },
    { "diff", command_diff,                      0.0 },
    { "dmtell", command_dmtell,                  0.0 },
    { "dump", command_dump,                      0.0 },
    { "dumpabove", command_dumpabove,            0.0 },
    { "dumpbelow", command_dumpbelow,            0.0 },
    { "dumpfriendlyobjects", command_dumpfriendlyobjects, 0.0 },
    { "dumpallarchetypes", command_dumpallarchetypes, 0.0 },
    { "dumpallmaps", command_dumpallmaps,        0.0 },
    { "dumpallobjects", command_dumpallobjects,  0.0 },
    { "dumpmap", command_dumpmap,                0.0 },
    { "free", command_free,                      0.0 },
    { "freeze", command_freeze,                  0.0 },
    { "goto", command_goto,                      0.0 },
    { "hide", command_hide,                      0.0 },
    { "insert_into", command_insert_into,        0.0 },
    { "invisible", command_invisible,            0.0 },
    { "kick", command_kick,                      0.0 },
    { "learn_special_prayer", command_learn_special_prayer, 0.0 },
    { "learn_spell", command_learn_spell,        0.0 },
    { "malloc", command_malloc,                  0.0 },
    { "nodm", command_nowiz,                     0.0 },
    { "nowiz", command_nowiz,                    0.0 },
    { "patch", command_patch,                    0.0 },
    { "players", command_players,                0.0 },
    { "plugin", command_loadplugin,              0.0 },
    { "pluglist", command_listplugins,           0.0 },
    { "plugout", command_unloadplugin,           0.0 },
    { "purge_quest_state", command_purge_quest,  0.0 },
    { "purge_quests", command_purge_quest_definitions,  0.0 },
    { "remove", command_remove,                  0.0 },
    { "reset", command_reset,                    0.0 },
    { "set_god", command_setgod,                 0.0 },
    { "settings", command_settings,              0.0 },
    { "server_speed", command_speed,             0.0 },
    { "shutdown", command_shutdown,              0.0 },
    { "ssdumptable", command_ssdumptable,        0.0 },
    { "stack_clear", command_stack_clear,        0.0 },
    { "stack_list", command_stack_list,          0.0 },
    { "stack_pop", command_stack_pop,            0.0 },
    { "stack_push", command_stack_push,          0.0 },
    { "stats", command_stats,                    0.0 },
    { "strings", command_strings,                0.0 },
    { "style_info", command_style_map_info,      0.0 },        /* Costly command, so make it wiz only */
    { "summon", command_summon,                  0.0 },
    { "teleport", command_teleport,              0.0 },
    { "toggle_shout", command_toggle_shout,      0.0 },
    { "wizpass", command_wizpass,                0.0 },
    { "wizcast", command_wizcast,                0.0 },
    { "overlay_save", command_overlay_save,      0.0 },
    { "overlay_reset", command_overlay_reset,    0.0 },
/*    { "possess", command_possess, 0.0 }, */
    { "mon_aggr", command_mon_aggr,              0.0 },
    { "loadtest", command_loadtest,              0.0 },
    { NULL, NULL, 0.0 }
};

/**
 * Register a player-issued command. The only cause of failure is trying to
 * override an existing command with one having a different type.
 * @param name command name.
 * @param type type of the command, one of COMMAND_TYPE_xxx.
 * @param func function to call for the command.
 * @param time how long the command takes.
 * @return identifier to unregister the command, 0 if adding failed.
 */
command_registration command_register(const char *name, uint8_t type, command_function func, float time) {
    auto existing = registered_commands.find(name);
    if (existing != registered_commands.end()) {
        assert(!existing->second.empty());
        if (existing->second.back()->type != type) {
            return 0;
        }
    }

    registered_command *add = new registered_command();
    add->registration = next_registration;
    next_registration++;
    add->type = type;
    add->command = new command_array_struct();
    add->command->func = func;
    add->command->time = time;
    registered_commands[name].push_back(add);
    return add->registration;
}

/**
 * Utility function calling command_register on all commands in the array until the name is NULL.
 * @param commands commands to register.
 * @param type type of the commands.
 */
static void command_add(command_array_struct *commands, uint8_t type) {
    for (int i = 0; commands[i].name; i++) {
        command_register(commands[i].name, type, commands[i].func, commands[i].time);
    }
}

/**
 * Init standard commands.
 */
void commands_init(void) {
    command_add(Commands, COMMAND_TYPE_NORMAL);
    command_add(CommunicationCommands, COMMAND_TYPE_COMMUNICATION);
    command_add(WizCommands, COMMAND_TYPE_WIZARD);
}

/**
 * Clear all registered commands.
 */
void commands_clear() {
    for (auto cmd = registered_commands.begin(); cmd != registered_commands.end(); cmd++) {
        for (auto h = cmd->second.begin(); h != cmd->second.end(); h++) {
            free((*h)->extra);
            delete *h;
        }
        cmd->second.clear();
    }
    registered_commands.clear();

    next_registration = 1;
}

/**
 * Helper function to display commands.
 *
 * @param op
 * player asking for information.
 * @param ap
 * commands to display.
 * @param legend
 * banner to print before the commands.
 */
static void show_commands(object *op, uint8_t type, const char *legend) {
    char line[HUGE_BUF];

    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, legend);

    line[0] = '\0';
    std::for_each(registered_commands.begin(), registered_commands.end(), [&type, &line] (auto cmd) {
        if (cmd.second.back()->type == type) {
            strcat(line, cmd.first.c_str());
            strcat(line, " ");
        }
    });
    draw_ext_info(NDI_UNIQUE, 0, op, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, line);
}

/**
 * Display the list of commands to a player.
 *
 * @param pl player asking for commands.
 * @param is_dm true if the player is a DM, false else.
 */
void command_list(object *pl, bool is_dm) {
    show_commands(pl, COMMAND_TYPE_NORMAL, "      Commands:");
    draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "\n");
    show_commands(pl, COMMAND_TYPE_COMMUNICATION, "      Communication commands:");
    if (is_dm) {
        draw_ext_info(NDI_UNIQUE, 0, pl, MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_INFO, "\n");
        show_commands(pl, COMMAND_TYPE_WIZARD, "      Wiz commands:");
    }
}

/**
 * Find a command by its name.
 * @param name command name.
 * @param is_dm whether the player is DM or not.
 * @return command, NULL if no match found.
 */
command_array_struct *command_find(const char *name, bool is_dm) {
    auto existing = registered_commands.find(name);
    if (existing == registered_commands.end()) {
        return NULL;
    }
    if (!is_dm && existing->second.back()->type == COMMAND_TYPE_WIZARD) {
        return NULL;
    }
    return existing->second.back()->command;
}

/**
 * Handle a player-issued command.
 *
 * @param pl
 * player who is issuing the command
 * @param command
 * the actual command with its arguments. Will be modified in-place.
 */
void command_execute(object *pl, char *command) {
    command_array_struct *csp, sent;
    char *cp, *low;

    pl->contr->has_hit = 0;

    /*
     * remove trailing spaces from commant
     */
    cp = command+strlen(command)-1;
    while ((cp >= command) && (*cp == ' ')) {
        *cp = '\0';
        cp--;
    }
    cp = strchr(command, ' ');
    if (cp) {
        *(cp++) = '\0';
        while (*cp == ' ')
            cp++;
    } else {
        cp = strchr(command, '\0');
    }

    for (low = command; *low; low++)
        *low = tolower(*low);

    csp = find_plugin_command(command, &sent);
    if (!csp)
        csp = command_find(command, QUERY_FLAG(pl, FLAG_WIZ));

    if (csp == NULL) {
        draw_ext_info_format(NDI_UNIQUE, 0, pl,
                             MSG_TYPE_COMMAND, MSG_TYPE_COMMAND_ERROR,
                             "'%s' is not a valid command.",
                             command);
        return;
    }

    pl->speed_left -= csp->time;

    /* A character time can never exceed his speed (which in many cases,
     * if wearing armor, is less than one.)  Thus, in most cases, if
     * the command takes 1.0, the player's speed will be less than zero.
     * it is only really an issue if time goes below -1
     * Due to various reasons that are too long to go into here, we will
     * actually still execute player even if his time is less than 0,
     * but greater than -1.  This is to improve the performance of the
     * new client/server.  In theory, it shouldn't make much difference.
     */

    if (csp->time && pl->speed_left < -2.0) {
        LOG(llevDebug, "execute_newclient_command: Player issued command that takes more time than he has left.\n");
    }
    csp->func(pl, cp);
}
