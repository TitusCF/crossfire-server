/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2020 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include "MessageLoader.h"
#include "Messages.h"

extern "C" {
#include "global.h"
#include "compat.h"
#include "string.h"
}

MessageLoader::MessageLoader(Messages* messages) : m_messages(messages) {
}

void MessageLoader::processFile(FILE *file, const std::string &filename) {
    char buf[MAX_BUF], msgbuf[HUGE_BUF], *cp;
    int text = 0, nrofmsg = 0;

    LOG(llevDebug, "Reading messages from %s...\n", filename.c_str());

    GeneralMessage *tmp = NULL;
    int lineno;
    int error_lineno;

    error_lineno = 0;
    for (lineno = 1; fgets(buf, MAX_BUF, file) != NULL; lineno++) {
        if (*buf == '#')
            continue;
        cp = strchr(buf, '\n');
        if (cp != NULL) {
            // Remove trailing whitespace
            while (cp > buf && (cp[-1] == ' ' || cp[-1] == '\t'))
                cp--;
            /* If we make sure there is a newline here,
             * we can avoid the auto-append of it and make long
             * blocks of text not get split.
             * But only do that if we are getting the message.
             * Everywhere else we do not want the newline.
             * Daniel Hawkins 2018-10-24
             */
            if (text)
            {
                *cp = '\n';
                // to have found a newline means we have room for a null terminator, too
                *(++cp)= '\0';
            }
            else
                *cp = '\0';
        }
        if (tmp != NULL) {
            if (text && strncmp(buf, "ENDMSG", 6) == 0) {
                if (strlen(msgbuf) > BOOK_BUF) {
                    LOG(llevDebug, "Warning: this string exceeded max book buf size:\n");
                    LOG(llevDebug, "  %s\n", msgbuf);
                }
                tmp->message = add_string(msgbuf);
                if (tmp->identifier[0] != '\n' && tmp->title == NULL) {
                    LOG(llevError, "Error: message can't have identifier without title, file %s on line %d\n", filename.c_str(), error_lineno);
                }
                m_messages->define(tmp->identifier, tmp);
                nrofmsg++;
                tmp = NULL;
                text = 0;
            } else if (text) {
                if (!buf_overflow(msgbuf, buf, HUGE_BUF-1)) {
                    strcat(msgbuf, buf);
                    // If there is a newline in the text, it will be included in the output where it belongs
                    // We should avoid really long lines of text getting split up this way.
                } else if (error_lineno != 0) {
                    LOG(llevInfo, "Warning: truncating book at %s, line %d\n", filename.c_str(), error_lineno);
                }
            } else if (strcmp(buf, "TEXT") == 0) {
                text = 1;
            } else if (strncmp(buf, "CHANCE ", 7) == 0) {
                tmp->chance = atoi(buf + 7);
            } else if (strncmp(buf, "TITLE ", 6) == 0) {
                tmp->title = add_string(buf + 6);
            } else if (strncmp(buf, "QUEST ", 6) == 0) {
                tmp->quest_code = add_string(buf + 6);
            } else if (strncmp(buf, "FACE ", 5) == 0) {
                const Face *face = find_face(buf + 5);
                tmp->face = face;
            } else if (error_lineno != 0) {
                LOG(llevInfo, "Warning: unknown line %s, in file %s line %d\n", buf, filename.c_str(), error_lineno);
            }
        } else if (strncmp(buf, "MSG", 3) == 0) {
            error_lineno = lineno;
            tmp = (GeneralMessage *)calloc(1, sizeof(GeneralMessage));
            tmp->face = NULL;
            if (buf[3] == ' ') {
                int i = 4;
                while (buf[i] == ' ' && buf[i] != '\0')
                    i++;
                if (buf[i] != '\0') {
                    tmp->identifier = add_string(buf + i);
                }
            }
            /* We need an identifier, so generate one from filename and line, that should be unique enough! */
            if (!tmp->identifier) {
                snprintf(msgbuf, sizeof(msgbuf), "\n%s\n%d", filename.c_str(), lineno);
                tmp->identifier = add_string(msgbuf);
            }
            strcpy(msgbuf, ""); /* reset msgbuf for new message */
        } else {
            LOG(llevInfo, "Warning: syntax error at %s, line %d\n", filename.c_str(), lineno);
        }
    }

    if (tmp != NULL) {
        LOG(llevError, "Invalid file %s", filename.c_str());
        fatal(SEE_LAST_ERROR);
    }

    LOG(llevDebug, "done messages %s, found %d messages.\n", filename.c_str(), nrofmsg);
}
