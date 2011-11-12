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
 * i18n support routines. See @ref page_i18n.
 * @warning
 * This file is encoded in UTF-8, take care to not mess the language_names array.
 */

#include <global.h>

/**
 * One message.
 */
typedef struct i18n_message {
    sstring code;       /**< Message code, usually the English version. */
    sstring message;    /**< Message to display. */
} i18n_message;

/**
 * One available language.
 */
typedef struct i18n_file {
    sstring code;                   /**< Language code, "message." extension. */
    sstring name;                   /**< Language's name, in its native version. */
    int count;                      /**< How many items in messages. */
    struct i18n_message *messages;   /**< Available messages for this language. */
} i18n_file;

/** Number of defined languages. */
static int i18n_count = 0;
/** Defined languages. */
static struct i18n_file *i18n_files = NULL;
/** Index of "English" in the i18nfiles array. */
static int i18n_default = -1;

static int i18n_message_compare_code(const i18n_message *a, const i18n_message *b) {
    return strcmp(a->code, b->code);
}

/**
 * Returns the i18n language index associated with the given object.
 * This only has a meaning for players.
 * @param op The player object to get the language of
 * @return The language numerical code. If none is associated, get_language returns 0
 */
int get_language(object *op) {
    if (!op->contr)
        return 0;
    if (op->contr->language < 0 || op->contr->language >= i18n_count)
        return 0;
    return op->contr->language;
}

/**
 * Translate a message in the appropriate language.
 * @param who who to translate for.
 * @param code string to translate, usually the English version.
 * @return translated message, or code if not found or who's language is invalid.
 */
const char *i18n(const object *who, const char *code) {
    i18n_message search, *found;

    if (!who || !who->contr)
        return code;

    if (who->contr->language < 0 || who->contr->language >= i18n_count)
        return code;

    search.code = add_string(code);

    found = bsearch(&search, i18n_files[who->contr->language].messages, i18n_files[who->contr->language].count, sizeof(i18n_message), (int (*)(const void *, const void *))i18n_message_compare_code);

    free_string(search.code);

    if (found)
        return found->message;

    return code;
}

/**
 * Attenmpt to find the identifier of a language from its code.
 * @param code language code.
 * @return index, -1 if not found.
 */
int i18n_find_language_by_code(const char *code) {
    int index;
    for (index = 0; index < i18n_count; index++) {
        if (strcmp(code, i18n_files[index].code) == 0)
            return index;
    }

    return -1;
}

/**
 * Find the identifier of a language from its code.
 * @param code language's code.
 * @return language's code, or the default language if code is invalid.
 */
int i18n_get_language_by_code(const char *code) {
    int try = i18n_find_language_by_code(code);
    if (try != -1)
        return try;
    return i18n_default;
}

/**
 * Return the code of a specified language.
 * @param language identifier of the language.
 * @return language's code, or default language's code if identifier is invalid.
 */
sstring i18n_get_language_code(int language) {
    if (language < 0 || language >= i18n_count)
        return i18n_files[i18n_default].code;
    return i18n_files[language].code;
}

/**
 * List all languages for who.
 * @param who who to display languages for.
 */
void i18n_list_languages(object *who) {
    for (int index = 0; index < i18n_count; index++) {
        draw_ext_info_format(NDI_UNIQUE, 0, who, MSG_TYPE_COMMAND, MSG_SUBTYPE_NONE,
            "[fixed]%s: %s",
            i18n_files[index].code,
            i18n_files[index].name
            );
    }
}

/**
 * Replaces '\n' by a newline char.
 *
 * Since we are replacing 2 chars by 1, no overflow should happen.
 *
 * @param line
 * text to replace into.
 */
static void convert_newline(char *line) {
    char *next;
    char buf[MAX_BUF];

    while ((next = strstr(line, "\\n")) != NULL) {
        *next = '\n';
        *(next+1) = '\0';
        snprintf(buf, MAX_BUF, "%s%s", line, next+2);
        strcpy(line, buf);
    }
}

/**
 * Initializes the i18n subsystem.
 * Will load all found strings.
 * If there is an error, calls fatal().
 */
void i18n_init(void) {
    char dirname[MAX_BUF], filename[MAX_BUF], line[HUGE_BUF];
    FILE *fp;
    char *token;
    DIR *dir;
    struct dirent *file;
    i18n_message code, *found;

    snprintf(dirname, sizeof(dirname), "%s/i18n/", settings.datadir);

    dir = opendir(dirname);
    if (dir == NULL) {
        LOG(llevError, "couldn't open i18n directory %s\n", dirname);
        fatal(SEE_LAST_ERROR);
    }

    code.code = add_string("LN");

    while ((file = readdir(dir)) != NULL) {
        if (strncmp(file->d_name, "messages.", 9) != 0)
            continue;

        snprintf(filename, sizeof(filename), "%s%s", dirname, file->d_name);
        if ((fp = fopen(filename, "r")) == NULL) {
            LOG(llevError, "Cannot open i18n file %s: %s\n", filename, strerror_local(errno, line, sizeof(line)));
            fatal(SEE_LAST_ERROR);
        }

        i18n_files = realloc(i18n_files, (i18n_count + 1) * sizeof(i18n_file));
        i18n_files[i18n_count].code = add_string(file->d_name + 9);
        i18n_files[i18n_count].count = 0;
        i18n_files[i18n_count].messages = NULL;

        while (fgets(line, MAX_BUF, fp)) {
            if (strstr(line, "#") != line) {
                line[strlen(line)-1] = '\0'; /* erase the final newline that messes things. */

                i18n_files[i18n_count].messages = realloc(i18n_files[i18n_count].messages, (i18n_files[i18n_count].count + 1) * sizeof(i18n_message));

                token = strtok(line, "|");
                convert_newline(token);
                i18n_files[i18n_count].messages[i18n_files[i18n_count].count].code = add_string(token);
                token = strtok(NULL, "|");
                if (token != NULL) {
                    convert_newline(token);
                    i18n_files[i18n_count].messages[i18n_files[i18n_count].count].message = add_string(token);
                } else {
                    i18n_files[i18n_count].messages[i18n_files[i18n_count].count].message = add_refcount(i18n_files[i18n_count].messages[i18n_files[i18n_count].count].code);
                }
                i18n_files[i18n_count].count++;
            }
        }
        fclose(fp);

        qsort(i18n_files[i18n_count].messages, i18n_files[i18n_count].count, sizeof(i18n_message), (int (*)(const void *, const void *))i18n_message_compare_code);
        found = bsearch(&code, i18n_files[i18n_count].messages, i18n_files[i18n_count].count, sizeof(i18n_message), (int (*)(const void *, const void *))i18n_message_compare_code);
        if (found == NULL) {
            LOG(llevError, "couldn't find language name (LN) for %s\n", filename);
            fatal(SEE_LAST_ERROR);
        }

        i18n_files[i18n_count].name = found->message;
        LOG(llevDebug, "Read %i strings for language: %s\n", i18n_files[i18n_count].count, found->message);

        if (strcmp(i18n_files[i18n_count].code, "en") == 0)
            i18n_default = i18n_count;

        i18n_count++;
    }
    closedir(dir);

    free_string(code.code);

    if (i18n_default == -1) {
        LOG(llevError, "couldn't find default language en!\n");
        fatal(SEE_LAST_ERROR);
    }
}
