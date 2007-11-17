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
 * @file
 * i18n support definitions
 */
#ifndef LANGUAGES_H
#define LANGUAGES_H

#define NUM_LANGUAGES 5
#define NUM_I18N_STRINGS   52
extern const char* language_codes[]; /**< Language codes ("en", "fr", etc.) table */
extern const char* language_names[]; /**< Language names ("English", "French", etc.) table */
extern const char* i18n_strings[NUM_LANGUAGES][NUM_I18N_STRINGS]; /**< Translation table */

#define I18N_MSG_CMISC_000 0
#define I18N_MSG_CMISC_001 1
#define I18N_MSG_CMISC_002 2
#define I18N_MSG_CMISC_003 3
#define I18N_MSG_CMISC_004 4
#define I18N_MSG_CMISC_005 5
#define I18N_MSG_CMISC_006 6
#define I18N_MSG_CMISC_007 7
#define I18N_MSG_CMISC_008 8
#define I18N_MSG_CMISC_009 9
#define I18N_MSG_CMISC_010 10
#define I18N_MSG_CMISC_011 11
#define I18N_MSG_CMISC_012 12
#define I18N_MSG_CMISC_013 13
#define I18N_MSG_CMISC_014 14
#define I18N_MSG_CMISC_015 15
#define I18N_MSG_CMISC_016 16
#define I18N_MSG_CMISC_017 17
#define I18N_MSG_CMISC_018 18
#define I18N_MSG_CMISC_019 19
#define I18N_MSG_CMISC_020 20
#define I18N_MSG_CMISC_021 21
#define I18N_MSG_CMISC_022 22
#define I18N_MSG_CMISC_023 23
#define I18N_MSG_CMISC_024 24
#define I18N_MSG_CMISC_025 25
#define I18N_MSG_CMISC_026 26
#define I18N_MSG_CMISC_027 27
#define I18N_MSG_CMISC_028 28
#define I18N_MSG_CMISC_029 29
#define I18N_MSG_CMISC_030 30
#define I18N_MSG_CMISC_031 31
#define I18N_MSG_CMISC_032 32
#define I18N_MSG_CMISC_033 33
#define I18N_MSG_CMISC_034 34
#define I18N_MSG_CMISC_035 35
#define I18N_MSG_CMISC_036 36
#define I18N_MSG_CMISC_037 37
#define I18N_MSG_CMISC_038 38
#define I18N_MSG_CMISC_039 39
#define I18N_MSG_CMISC_040 40
#define I18N_MSG_CMISC_041 41
#define I18N_MSG_CMISC_042 42
#define I18N_MSG_CMISC_043 43
#define I18N_MSG_CMISC_044 44
#define I18N_MSG_CMISC_045 45
#define I18N_MSG_CMISC_046 46
#define I18N_MSG_CMISC_047 47
#define I18N_MSG_CMISC_048 48
#define I18N_MSG_CMISC_049 49
#define I18N_MSG_CMISC_050 50
#define I18N_MSG_CMISC_051 51
#endif /* LANGUAGES_H */
