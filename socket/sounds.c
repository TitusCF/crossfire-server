/*
 * static char *rcsid_sound_c =
 *   "$Id$";
 */

/* Send bug reports to Raphael Quinet (quinet@montefiore.ulg.ac.be) */

/**
 * \file
 * Sound-related functions.
 *
 * \date 2003-12-02
 */

#include <global.h>
#include <sproto.h>
#include <sounds.h>

/**
 * Maximum distance a player may hear a sound from.
 * This is only used for new client/server sound.  If the sound source
 * on the map is farther away than this, we don't sent it to the client.
 */
#define MAX_SOUND_DISTANCE 10

/**
 * Plays a sound for specified player only
 */
void play_sound_player_only(player *pl, short soundnum,  sint8 x, sint8 y)
{
    char soundtype;
    SockList sl;

    if (pl->socket.sound & SND_MUTE || !(pl->socket.sound & SND_EFFECTS)) return;
    /* Do some quick conversion to the sound type we want. */
    if (soundnum>=SOUND_CAST_SPELL_0) {
	soundtype=SOUND_SPELL;
	soundnum -=SOUND_CAST_SPELL_0;
    }
    else soundtype=SOUND_NORMAL;

    sl.buf=malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf, "sound ");
    sl.len=strlen((char*)sl.buf);
    SockList_AddChar(&sl, x);
    SockList_AddChar(&sl, y);
    SockList_AddShort(&sl, soundnum);
    SockList_AddChar(&sl, soundtype);
    Send_With_Handling(&pl->socket, &sl);
    free(sl.buf);
}

#define POW2(x) ((x) * (x))

/** Plays some sound on map at x,y.  */
void play_sound_map(const mapstruct *map, int x, int y, short sound_num)
{
    player *pl;

    if (sound_num >= NROF_SOUNDS) {
	LOG(llevError,"Tried to play an invalid sound num: %d\n", sound_num);
	return;
    }

    for (pl = first_player; pl; pl = pl->next) {
	if (pl->ob->map == map) {
	    int distance=isqrt(POW2(pl->ob->x - x) + POW2(pl->ob->y - y));

	    if (distance<=MAX_SOUND_DISTANCE) {
		play_sound_player_only(pl, sound_num, ( sint8 )( x-pl->ob->x ), ( sint8 )( y-pl->ob->y ));
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
void send_background_music(player* pl, const char* music) {
    SockList sl;

    if (pl->socket.sound & SND_MUTE || !(pl->socket.sound & SND_MUSIC)) return;

    sl.buf=malloc(MAXSOCKSENDBUF);
    strcpy((char*)sl.buf, "music ");
    sl.len=strlen((char*)sl.buf);
    if (music)
        SockList_AddString(&sl, music);
    else
        SockList_AddString(&sl, "NONE");

    Send_With_Handling(&pl->socket, &sl);
    free(sl.buf);
}
