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
void play_sound_player_only(player *pl, int soundnum,  int x, int y)
{
    int soundtype;
    SockList sl;

    if (!pl->socket.sound) return;
    /* Do some quick conversion to the sound type we want. */
    if (soundnum>=SOUND_CAST_SPELL_0) {
	soundtype=SOUND_SPELL;
	soundnum -=SOUND_CAST_SPELL_0;
    }
    else soundtype=SOUND_NORMAL;

    sl.buf=malloc(MAXSOCKBUF);
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
void play_sound_map(mapstruct *map, int x, int y, int sound_num)
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
		play_sound_player_only(pl, sound_num, x-pl->ob->x, y-pl->ob->y);
	    }
	}
    }
}
