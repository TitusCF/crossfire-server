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
 * \file
 * Sound-related functions.
 *
 * \date 2003-12-02
 */

#include "global.h"

#include <assert.h>
#include <stdlib.h>
#include <string.h>

#include "sounds.h"
#include "sproto.h"

/**
 * Maximum distance a player may hear a sound from.
 * This is only used for new client/server sound.  If the sound source
 * on the map is farther away than this, we don't sent it to the client.
 */
#define MAX_SOUND_DISTANCE 10

/**
 * Plays a sound for specified player only.
 *
 * @param pl
 * player to play sound to.
 * @param sound_type
 * sound type, see @ref Soundtypes "the sound types".
 * @param emitter
 * object emitting a sound.
 * @param dir
 * direction the sound is moving into.
 * @param action
 * sound name to play.
 */
void play_sound_player_only(player *pl, int8_t sound_type, object *emitter, int dir, const char *action) {
    SockList sl;
    int volume = 50;
    sstring name;
    object *source;

    if (pl->socket.sound&SND_MUTE || !(pl->socket.sound&SND_EFFECTS))
        return;
    if (pl->socket.sounds_this_tick >= MAX_SOUNDS_TICK)
        return;
    if (!emitter->map && !(emitter->env && emitter->env->map))
        return;

    source = emitter->map ? emitter : emitter->env;

    // Approximate the distance to the emitter from the source
    int dx = FABS(source->x-pl->ob->x),
        dy = FABS(source->y-pl->ob->y);
    int distance = (MIN(dx, dy) * 3 + FABS(dx-dy) * 2) / 2;
    // Make the sound dissipation more gradual.
    distance >>= 1;
    // Downscale the volume by distance
    volume = distance ? volume / distance : volume;

    pl->socket.sounds_this_tick = 0;

    name = emitter->type == PLAYER ? emitter->race : emitter->name;
    if (name == NULL) {
        return;
    }

    SockList_Init(&sl);
    SockList_AddString(&sl, "sound2 ");
    SockList_AddChar(&sl, (int8_t)(source->x-pl->ob->x));
    SockList_AddChar(&sl, (int8_t)(source->y-pl->ob->y));
    SockList_AddChar(&sl, dir);
    SockList_AddChar(&sl, volume);
    SockList_AddChar(&sl, sound_type);
    SockList_AddLen8Data(&sl, action, strlen(action));
    SockList_AddLen8Data(&sl, name, strlen(name));
    Send_With_Handling(&pl->socket, &sl);
    SockList_Term(&sl);
}

#define POW2(x) ((x)*(x))

/**
 * Plays a sound on a map.
 * @param sound_type
 * sound type, see @ref Soundtypes "the sound types".
 * @param emitter
 * object emitting the sound. Must be on a map.
 * @param dir
 * direction the sound is moving.
 * @param action
 * sound name to play.
 */
void play_sound_map(int8_t sound_type, object *emitter, int dir, const char *action) {
    player *pl;
    object *source;

    if (!emitter->map && !(emitter->env && emitter->env->map))
        return;

    source = emitter->map ? emitter : emitter->env;

    for (pl = first_player; pl; pl = pl->next) {
        if (pl->ob->map == emitter->map) {
            int distance = isqrt(POW2(pl->ob->x-source->x)+POW2(pl->ob->y-source->y));

            if (distance <= MAX_SOUND_DISTANCE) {
                play_sound_player_only(pl, sound_type, emitter, dir, action);
            }
        }
    }
}

/**
 * Sends background music to client.
 *
 * @param pl
 * player
 * @param music
 * background music name. Can be NULL.
 */
void send_background_music(player *pl, const char *music) {
    SockList sl;

    if (pl->socket.sound&SND_MUTE || !(pl->socket.sound&SND_MUSIC))
        return;

    SockList_Init(&sl);
    SockList_AddString(&sl, "music ");
    SockList_AddString(&sl, music == NULL ? "NONE" : music);
    Send_With_Handling(&pl->socket, &sl);
    SockList_Term(&sl);
}

static char const* pick_bg_music(mapstruct map[static 1]) {
    if (map->background_music != NULL) {
        return map->background_music;
    }
    return get_name_of_region_for_map(map);
}

void player_update_bg_music(object player[static 1]) {
    assert(player->contr);
    assert(player->type == PLAYER);
    send_background_music(player->contr, pick_bg_music(player->map));
}

