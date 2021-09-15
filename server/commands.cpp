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

/** Represents one command. */
struct registered_command {
    command_registration registration;                  /**< Identifier for unregistration. */
    command_function func_std;                          /**< Command function. */
    command_function_extra func_extra;                  /**< Command function if extra argument. */
    char *extra;                                        /**< Extra argument, if not NULL then func_extra is used, else func_std. */
    float time;                                         /**< How long it takes to execute this command. */
    uint8_t type;                                       /**< Command type, one of COMMAND_TYPE_xxx. */
};

/** Next identifier for a command registration. */
static command_registration next_registration = 1;
/** All registered commands, key is the name, vector is all registered handlers in registration order (last one is used). */
static std::map<std::string, std::vector<registered_command *> > registered_commands;

/**
 * Register a player-issued command. The only cause of failure is trying to
 * override an existing command with one having a different type.
 * If extra is null, then func_std must not be null, else func_extra must not be null.
 * @param name command name.
 * @param type type of the command, one of COMMAND_TYPE_xxx.
 * @param func_std function to call for the command.
 * @param func_extra function to call for the command with an extra argument.
 * @param extra extra argument to give to func_extra.
 * @param time how long the command takes.
 * @return identifier to unregister the command, 0 if adding failed.
 */
static command_registration do_register(const char *name, uint8_t type, command_function func_std, command_function_extra func_extra, const char *extra, float time) {
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
    add->func_std = func_std;
    add->func_extra = func_extra;
    add->extra = extra ? strdup(extra) : nullptr;
    add->time = time;
    registered_commands[name].push_back(add);
    return add->registration;
}

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
    return do_register(name, type, func, nullptr, nullptr, time);
}

/**
 * Init standard commands.
 */
void commands_init(void) {
#define RN(name, func, time) command_register(name, COMMAND_TYPE_NORMAL, func, time)
#define RC(name, func, time) command_register(name, COMMAND_TYPE_COMMUNICATION, func, time)
#define RW(name, func, time) command_register(name, COMMAND_TYPE_WIZARD, func, time)

    /* Normal game commands. */
    RN("afk", command_afk,                0.0);
    RN("apply", command_apply,            1.0);   /* should be variable */
    RN("applymode", command_applymode,    1.0);   /* should be variable */
    RN("body", command_body,              0.0);
    RN("bowmode", command_bowmode,        0.0);
    RN("brace", command_brace,            0.0);
    RN("cast", command_cast,              0.2);   /* Is this right? */
    RN("delete", command_delete,          0.0);
    RN("disarm", command_disarm,          1.0);
    RN("dm", command_dm,                  0.0);
    RN("dmhide", command_dmhide,          0.0); /* Like dm, but don't tell a dm arrived, hide player */
    RN("drop", command_drop,              1.0);
    RN("dropall", command_dropall,        1.0);
    RN("empty", command_empty,            1.0);
    RN("examine", command_examine,        0.5);
    RN("fix_me", command_fix_me,          0.0);
    RN("follow", command_follow,          0.0);
    RN("forget_spell", command_forget_spell, 0.0);
    RN("get", command_take,               1.0);
    RN("help", command_help,              0.0);
    RN("hiscore", command_hiscore,        0.0);
    RN("inventory", command_inventory,    0.0);
    RN("invoke", command_invoke,          1.0);
    RN("killpets", command_kill_pets,     0.0);
    RN("language", command_language,      0.0);
    RN("listen", command_listen,          0.0);
    RN("lock", command_lock_item,         0.0);
    RN("maps", command_maps,              0.0);
    RN("mapinfo", command_mapinfo,        0.0);
    RN("mark", command_mark,              0.0);
    RN("motd", command_motd,              0.0);
    RN("news", command_news,              0.0);
    RN("party", command_party,            0.0);
    RN("party_rejoin", command_party_rejoin, 0.0);
    RN("passwd", command_passwd,          0.0);
    RN("peaceful", command_peaceful,      0.0);
    RN("petmode", command_petmode,        0.0);
    RN("pickup", command_pickup,          1.0);
    RN("prepare", command_prepare,        1.0);
    RN("printlos", command_printlos,      0.0);
    RN("quit", command_quit,              0.0);
    RN("ready_skill", command_rskill,     1.0);
    RN("rename", command_rename_item,     0.0);
    RN("resistances", command_resistances, 0.0);
    RN("rotateshoottype", command_rotateshoottype, 0.0);
    RN("rules", command_rules,            0.0);
    RN("save", command_save,              0.0);
    RN("skills", command_skills,          0.0);   /* shows player list of skills */
    RN("use_skill", command_uskill,       1.0);
    RN("search", command_search,          1.0);
    RN("search-items", command_search_items, 0.0);
    RN("showpets", command_showpets,      1.0);
    RN("sound", command_sound,            0.0);
    RN("statistics", command_statistics,  0.0);
    RN("take", command_take,              1.0);
    RN("throw", command_throw,            1.0);
    RN("time", command_time,              0.0);
    RN("title", command_title,            0.0);
    RN("use", command_use,                1.0);
    RN("usekeys", command_usekeys,        0.0);
    RN("whereabouts", command_whereabouts, 0.0);
    RN("whereami", command_whereami,      0.0);
    RN("unarmed_skill", command_unarmed_skill, 0.0);
#ifdef DEBUG_MALLOC_LEVEL
    RN("verify", command_malloc_verify,   0.0);
#endif
    RN("version", command_version,        0.0);
    RN("wimpy", command_wimpy,            0.0);
    RN("who", command_who,                0.0);
    /*
     * directional commands
     */
    RN("stay", command_stay,              1.0); /* 1.0 because it is used when using a
                                               *  skill on yourself */
    RN("north", command_north,            1.0);
    RN("east", command_east,              1.0);
    RN("south", command_south,            1.0);
    RN("west", command_west,              1.0);
    RN("northeast", command_northeast,    1.0);
    RN("southeast", command_southeast,    1.0);
    RN("southwest", command_southwest,    1.0);
    RN("northwest", command_northwest,    1.0);
    RN("run", command_run,                1.0);
    RN("run_stop", command_run_stop,      0.0);
    RN("fire", command_fire,              1.0);
    RN("fire_stop", command_fire_stop,    0.0);
    RN("face", command_face,              0.0);
    RN("quest", command_quest,            0.0);
    RN("knowledge", command_knowledge,    0.0);

    /* Chat/shout related commands. */
    RC("tell", command_tell,              0.1);
    RC("reply", command_reply,            0.0);
    RC("say", command_say,                0.1);
    RC("gsay", command_gsay,              1.0);
    RC("shout", command_shout,            0.1);
    RC("chat", command_chat,              0.1);
    RC("me", command_me,                  0.1);
    RC("cointoss", command_cointoss,      0.0);
    RC("orcknuckle", command_orcknuckle,  0.0);
     /*
      * begin emotions
      */
    RC("nod", command_nod,                0.0);
    RC("dance", command_dance,            0.0);
    RC("kiss", command_kiss,              0.0);
    RC("bounce", command_bounce,          0.0);
    RC("smile", command_smile,            0.0);
    RC("cackle", command_cackle,          0.0);
    RC("laugh", command_laugh,            0.0);
    RC("giggle", command_giggle,          0.0);
    RC("shake", command_shake,            0.0);
    RC("puke", command_puke,              0.0);
    RC("growl", command_growl,            0.0);
    RC("scream", command_scream,          0.0);
    RC("sigh", command_sigh,              0.0);
    RC("sulk", command_sulk,              0.0);
    RC("hug", command_hug,                0.0);
    RC("cry", command_cry,                0.0);
    RC("poke", command_poke,              0.0);
    RC("accuse", command_accuse,          0.0);
    RC("grin", command_grin,              0.0);
    RC("bow", command_bow,                0.0);
    RC("clap", command_clap,              0.0);
    RC("blush", command_blush,            0.0);
    RC("burp", command_burp,              0.0);
    RC("chuckle", command_chuckle,        0.0);
    RC("cough", command_cough,            0.0);
    RC("flip", command_flip,              0.0);
    RC("frown", command_frown,            0.0);
    RC("gasp", command_gasp,              0.0);
    RC("glare", command_glare,            0.0);
    RC("groan", command_groan,            0.0);
    RC("hiccup", command_hiccup,          0.0);
    RC("lick", command_lick,              0.0);
    RC("pout", command_pout,              0.0);
    RC("shiver", command_shiver,          0.0);
    RC("shrug", command_shrug,            0.0);
    RC("slap", command_slap,              0.0);
    RC("smirk", command_smirk,            0.0);
    RC("snap", command_snap,              0.0);
    RC("sneeze", command_sneeze,          0.0);
    RC("snicker", command_snicker,        0.0);
    RC("sniff", command_sniff,            0.0);
    RC("snore", command_snore,            0.0);
    RC("spit", command_spit,              0.0);
    RC("strut", command_strut,            0.0);
    RC("thank", command_thank,            0.0);
    RC("twiddle", command_twiddle,        0.0);
    RC("wave", command_wave,              0.0);
    RC("whistle", command_whistle,        0.0);
    RC("wink", command_wink,              0.0);
    RC("yawn", command_yawn,              0.0);
    RC("beg", command_beg,                0.0);
    RC("bleed", command_bleed,            0.0);
    RC("cringe", command_cringe,          0.0);
    RC("think", command_think,            0.0);

    /** Wizard commands. */
    RW("abil", command_abil,                      0.0);
    RW("accountpasswd", command_accountpasswd,    0.0);
    RW("addexp", command_addexp,                  0.0);
    RW("arrest", command_arrest,                  0.0);
    RW("banish", command_banish,                  0.0);
    RW("create", command_create,                  0.0);
    RW("debug", command_debug,                    0.0);
    RW("diff", command_diff,                      0.0);
    RW("dmtell", command_dmtell,                  0.0);
    RW("dump", command_dump,                      0.0);
    RW("dumpabove", command_dumpabove,            0.0);
    RW("dumpbelow", command_dumpbelow,            0.0);
    RW("dumpfriendlyobjects", command_dumpfriendlyobjects, 0.0);
    RW("dumpallarchetypes", command_dumpallarchetypes, 0.0);
    RW("dumpallmaps", command_dumpallmaps,        0.0);
    RW("dumpallobjects", command_dumpallobjects,  0.0);
    RW("dumpmap", command_dumpmap,                0.0);
    RW("free", command_free,                      0.0);
    RW("freeze", command_freeze,                  0.0);
    RW("goto", command_goto,                      0.0);
    RW("hide", command_hide,                      0.0);
    RW("insert_into", command_insert_into,        0.0);
    RW("invisible", command_invisible,            0.0);
    RW("kick", command_kick,                      0.0);
    RW("learn_special_prayer", command_learn_special_prayer, 0.0);
    RW("learn_spell", command_learn_spell,        0.0);
    RW("malloc", command_malloc,                  0.0);
    RW("nodm", command_nowiz,                     0.0);
    RW("nowiz", command_nowiz,                    0.0);
    RW("patch", command_patch,                    0.0);
    RW("players", command_players,                0.0);
    RW("plugin", command_loadplugin,              0.0);
    RW("pluglist", command_listplugins,           0.0);
    RW("plugout", command_unloadplugin,           0.0);
    RW("purge_quest_state", command_purge_quest,  0.0);
    RW("purge_quests", command_purge_quest_definitions,  0.0);
    RW("remove", command_remove,                  0.0);
    RW("reset", command_reset,                    0.0);
    RW("set_god", command_setgod,                 0.0);
    RW("settings", command_settings,              0.0);
    RW("server_speed", command_speed,             0.0);
    RW("shutdown", command_shutdown,              0.0);
    RW("ssdumptable", command_ssdumptable,        0.0);
    RW("stack_clear", command_stack_clear,        0.0);
    RW("stack_list", command_stack_list,          0.0);
    RW("stack_pop", command_stack_pop,            0.0);
    RW("stack_push", command_stack_push,          0.0);
    RW("stats", command_stats,                    0.0);
    RW("strings", command_strings,                0.0);
    RW("style_info", command_style_map_info,      0.0);        /* Costly command, so make it wiz only */
    RW("summon", command_summon,                  0.0);
    RW("teleport", command_teleport,              0.0);
    RW("toggle_shout", command_toggle_shout,      0.0);
    RW("wizpass", command_wizpass,                0.0);
    RW("wizcast", command_wizcast,                0.0);
    RW("overlay_save", command_overlay_save,      0.0);
    RW("overlay_reset", command_overlay_reset,    0.0);
    /*   RW("possess", command_possess, 0.0); */
    RW("mon_aggr", command_mon_aggr,              0.0);
    RW("loadtest", command_loadtest,              0.0);

#undef RN
#undef RC
#undef RW
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
static registered_command *command_find(const char *name, bool is_dm) {
    auto existing = registered_commands.find(name);
    if (existing == registered_commands.end()) {
        return NULL;
    }
    if (!is_dm && existing->second.back()->type == COMMAND_TYPE_WIZARD) {
        return NULL;
    }
    return existing->second.back();
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
    registered_command *csp;
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
    if (csp->extra) {
        csp->func_extra(pl, cp, csp->extra);
    } else {
        csp->func_std(pl, cp);
    }
}

/**
 * Register a player-issued command with an extra parameter.
 * This allow commands that need a supplemental fixed argument.
 * The only cause of failure is trying to
 * override an existing command with one having a different type.
 * @param name command name.
 * @param extra extra data to give the command, must not be null.
 * @param type type of the command, one of COMMAND_TYPE_xxx.
 * @param func function to call for the command.
 * @param time how long the command takes.
 * @return identifier to unregister the command, 0 if adding failed.
 */
command_registration command_register_extra(const char *name, const char *extra, uint8_t type, command_function_extra func, float time) {
    assert(extra);
    return do_register(name, type, nullptr, func, extra, time);
}

/**
 * Unregister a previously registered command.
 * @param command identifier returned by command_register() or command_register_extra().
 */
void command_unregister(command_registration command) {
    for (auto cmd = registered_commands.begin(); cmd != registered_commands.end(); cmd++) {
        for (auto h = cmd->second.begin(); h != cmd->second.end(); h++) {
            if ((*h)->registration == command) {
                free((*h)->extra);
                delete (*h);
                cmd->second.erase(h);
                if (cmd->second.empty()) {
                    registered_commands.erase(cmd);
                }
                return;
            }
        }
    }
}
