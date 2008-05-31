/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002,2006 Mark Wedel & Crossfire Development Team
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
 * i18n support routines.
 * @warning
 * This file is encoded in UTF-8, take care to not mess the language_names array.
 */

#include <global.h>

/** List of available language codes. */
const char* language_codes[] =
{
    "en",
    "fr",
    "nl",
    "it",
    "de"
};

/** Language names. */
const char* language_names[] =
{
    "English",
    "Français",
    "Nederlands",
    "Italiano",
    "Deutsch"
};
/** All translated strings. */
const char* i18n_strings[NUM_LANGUAGES][NUM_I18N_STRINGS];

/**
 * Returns the i18n language index associated with the given object.
 * This only has a meaning for players.
 * @param op The player object to get the language of
 * @return The language numerical code. If none is associated, get_language returns 0
 */
int get_language(object* op)
{
    if (!op->contr)
        return 0;
    if (op->contr->language < 0 || op->contr->language >= NUM_LANGUAGES)
        return 0;
    return op->contr->language;
}

/**
 * Translates a given i18n string into the provided language.
 * @param language The language numerical code to translate the string to
 * @param id The i18n string identifier
 * @return The translated string, or NULL if an error occured.
 */
const char* i18n_translate(int language, int id)
{
    if (language >= NUM_LANGUAGES)
        return NULL;
    else if (id >= NUM_I18N_STRINGS)
        return NULL;

    if (i18n_strings[language][id]==NULL)
        return i18n_strings[0][id];
    else
        return i18n_strings[language][id];
}


/**
 * Replaces '\n' by a newline char.
 *
 * Since we are replacing 2 chars by 1, no overflow should happen.
 *
 * @param line
 * text to replace into.
 */
static void convert_newline(char* line) {
    char* next;
    char buf[MAX_BUF];

    while ((next = strstr(line, "\\n")) != NULL) {
        *next = '\n';
        *(next + 1) = '\0';
        snprintf(buf, MAX_BUF, "%s%s", line, next + 2);
        strcpy(line, buf);
    }
}

/**
 * Initializes the i18n subsystem.
 */
void i18n_init(void)
{
    char filename[MAX_BUF], line[HUGE_BUF];
    int i, entry;
    FILE* fp;
    char* token;
    int counter;
    char* buffer;
    for(i=0;i<NUM_LANGUAGES;i++)
    {
        snprintf(filename, sizeof(filename), "%s/i18n/messages.%s", settings.datadir, language_codes[i]);
        if ((fp=fopen(filename, "r")) == NULL)
        {
            LOG(llevError, "Cannot open i18n file %s: %s\n", 
                filename, strerror_local(errno, line, sizeof(line)));
            if(i==0)
                exit(1);
        }
        else
        {
            counter = 0;
            while(fgets(line, MAX_BUF, fp))
            {
                if (strstr(line,"#")!=line)
                {
                    line[strlen(line) - 1] = '\0'; /* erase the final newline that messes things. */
                    token = strtok(line, "|");
                    entry = atoi(token);
                    token = strtok(NULL, "|");
                    buffer = malloc(sizeof(char)*(strlen(token)+1));
                    strcpy(buffer,token);
                    convert_newline(buffer);
                    i18n_strings[i][entry]=buffer;
                }
                counter++;
            }
            LOG(llevDebug, "Read %i strings for language: %s\n",
                counter, language_codes[i]);
            fclose(fp);
        }
    }
}
