/*
    CrossFire, A Multiplayer game for X-windows

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

    The author can be reached via e-mail to mark@pyramid.com.
*/

/**
 * @file
 * Sound-related defines. Mostly unused.
 */

#ifndef SOUNDS_H
#define SOUNDS_H

#define SOUND_NEW_PLAYER	0
#define SOUND_FIRE_ARROW	1
#define SOUND_LEARN_SPELL	2
#define SOUND_FUMBLE_SPELL	3
#define SOUND_WAND_POOF		4
#define SOUND_OPEN_DOOR		5
#define SOUND_PUSH_PLAYER	6
#define SOUND_PLAYER_HITS1	7
#define SOUND_PLAYER_HITS2	8
#define SOUND_PLAYER_HITS3	9
#define SOUND_PLAYER_HITS4	10
#define SOUND_PLAYER_IS_HIT1	11
#define SOUND_PLAYER_IS_HIT2	12
#define SOUND_PLAYER_IS_HIT3	13
#define SOUND_PLAYER_KILLS	14
#define SOUND_PET_IS_KILLED	15
#define SOUND_PLAYER_DIES	16
#define SOUND_OB_EVAPORATE	17
#define SOUND_OB_EXPLODE	18
#define SOUND_CLOCK		19
#define SOUND_TURN_HANDLE	20
#define SOUND_FALL_HOLE		21
#define SOUND_DRINK_POISON     	22
#define SOUND_CAST_SPELL_0	23
/* ... + other sounds for spells : SOUND_CAST_SPELL_0 + spell type */

/* NROF_SOUNDS is defined in "defines.h".  Don't forget to change this number
 * if you add or remove any sound.
 */

/**
 * Those flags are for the 'socket.sound' field.
 */
#define SND_EFFECTS     1       /**< Client wands regular sounds. */
#define SND_MUSIC       2       /**< Client wants background music info. */
#define SND_MUTE        64      /**< Don't sent anything for now. */

#endif /* SOUNDS_H */
