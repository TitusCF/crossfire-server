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

#include "global.h"

#include <errno.h>
#include <stdlib.h>
#include <string.h>

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
 * Attempt to find the identifier of a language from its code.
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
    int index;
    for (index = 0; index < i18n_count; index++) {
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
    char dirname[MAX_BUF], filename[MAX_BUF*2], *line;
    BufferReader *br;
    char *token;
    DIR *dir;
    struct dirent *file;
    i18n_message code, *found;

    snprintf(dirname, sizeof(dirname), "%s/i18n/", settings.datadir);

    dir = opendir(dirname);
    if (dir == NULL) {
        LOG(llevError, "i18n: couldn't open %s\n", dirname);
        fatal(SEE_LAST_ERROR);
    }

    code.code = add_string("LN");

    while ((file = readdir(dir)) != NULL) {
        if (strncmp(file->d_name, "messages.", 9) != 0)
            continue;

        snprintf(filename, sizeof(filename), "%s%s", dirname, file->d_name);
        br = bufferreader_init_from_file(NULL, filename, "i18n: couldn't open %s: %s\n", llevError);
        if (!br) {
            fatal(SEE_LAST_ERROR);
        }

        i18n_files = realloc(i18n_files, (i18n_count + 1) * sizeof(i18n_file));
        if (!i18n_files) {
            LOG(llevError, "i18n: couldn't allocate memory!\n");
            fatal(OUT_OF_MEMORY);
        }
        i18n_files[i18n_count].code = add_string(file->d_name + 9);
        i18n_files[i18n_count].count = 0;
        i18n_files[i18n_count].messages = NULL;

        while ((line = bufferreader_next_line(br)) != NULL) {
            if (line[0] != '#' && line[0] != '\0') {
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
        bufferreader_destroy(br);

        qsort(i18n_files[i18n_count].messages, i18n_files[i18n_count].count, sizeof(i18n_message), (int (*)(const void *, const void *))i18n_message_compare_code);
        found = bsearch(&code, i18n_files[i18n_count].messages, i18n_files[i18n_count].count, sizeof(i18n_message), (int (*)(const void *, const void *))i18n_message_compare_code);
        if (found == NULL) {
            LOG(llevError, "i18n: no language set in %s\n", filename);
            fatal(SEE_LAST_ERROR);
        }

        i18n_files[i18n_count].name = found->message;
        LOG(llevDebug, "i18n: %d strings for %s\n",
                i18n_files[i18n_count].count, found->message);

        if (strcmp(i18n_files[i18n_count].code, "en") == 0)
            i18n_default = i18n_count;

        i18n_count++;
    }
    closedir(dir);

    free_string(code.code);

    if (i18n_default == -1) {
        LOG(llevError, "i18n: couldn't find default language (en)\n");
        fatal(SEE_LAST_ERROR);
    }
}

/**
 * Clears all i18n-related data.
 */
void i18n_free(void) {
  int file, message;

  for (file = 0; file < i18n_count; file++) {
      free_string(i18n_files[file].code); /* name is a copy of a message */
      for (message = 0; message < i18n_files[file].count; message++) {
          free_string(i18n_files[file].messages[message].code);
          free_string(i18n_files[file].messages[message].message);
      }
      free(i18n_files[file].messages);
  }
  free(i18n_files);
  i18n_files = NULL;
  i18n_count = 0;
}
