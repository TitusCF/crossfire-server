/*
 * Crossfire -- cooperative multi-player graphical RPG and adventure game
 *
 * Copyright (c) 2021 the Crossfire Development Team
 *
 * Crossfire is free software and comes with ABSOLUTELY NO WARRANTY. You are
 * welcome to redistribute it under certain conditions. For details, please
 * see COPYING and LICENSE.
 *
 * The authors can be reached via e-mail at <crossfire@metalforge.org>.
 */

#include <ctype.h>
#include <stdlib.h>
#include <string.h>
#include "global.h"
#include "compat.h"
#include "bufferreader.h"

struct BufferReader {
    char *buf;              /**< Data buffer. */

    size_t allocated_size;  /**< Allocated size of buf. */
    size_t buffer_length;   /**< Used size of buf. */

    char *current_line;     /**< Pointer to the next line of the buffer. */
    size_t line_index;      /**< Current line index. */
};

BufferReader *bufferreader_create() {
    BufferReader *buffer = calloc(1, sizeof(*buffer));
    if (!buffer) {
        fatal(OUT_OF_MEMORY);
    }

    return buffer;
}

void bufferreader_destroy(BufferReader *br) {
    free(br->buf);
    free(br);
}

/**
 * Ensure the buffer can hold the specified length, initialize all fields.
 * @param br buffer to initialize.
 * @param length lenfth to initialize for.
 */
static void bufferreader_init_for_length(BufferReader *br, size_t length) {
    size_t needed = length + 1;
    if (needed > br->allocated_size) {
        br->buf = realloc(br->buf, needed);
        if (!br->buf) {
            fatal(OUT_OF_MEMORY);
        }
        br->allocated_size = needed;
    }
    br->buffer_length = length;
    br->buf[br->buffer_length] = '\0';
    br->current_line = br->buffer_length > 0 ? br->buf : NULL;
    br->line_index = 0;
}

void bufferreader_init_from_file(BufferReader *br, FILE *file) {
    fseek(file, 0L, SEEK_END);
    bufferreader_init_for_length(br, ftell(file));
    fseek(file, 0, SEEK_SET);

    size_t actual = fread(br->buf, 1, br->buffer_length, file);
    if (actual != br->buffer_length) {
        LOG(llevError, "Expected to read %ld bytes, only read %ld!\n", br->buffer_length, actual);
        br->buffer_length = actual;
    }
}

void bufferreader_init_from_tar_file(BufferReader *br, mtar_t *tar, mtar_header_t *h) {
    bufferreader_init_for_length(br, h->size);
    mtar_read_data(tar, br->buf, h->size);
}

char *bufferreader_next_line(BufferReader *br) {
    if (!br->current_line) {
        return NULL;
    }

    br->line_index++;
    char *newline = strchr(br->current_line, '\n');
    char *curr = br->current_line;
    if (newline) {
        br->current_line = newline + 1;
        (*newline) = '\0';
    } else {
        br->current_line = NULL;
    }
    return curr;
}

char *bufferreader_get_line(BufferReader *br, char *buffer, size_t length) {
    if (!br->current_line) {
        return NULL;
    }

    br->line_index++;
    char *newline = strchr(br->current_line, '\n');
    char *curr = br->current_line;
    if (newline) {
        size_t cp = MIN(length, newline - curr + 1);
        br->current_line = newline + 1;
        strncpy(buffer, curr, cp);
        buffer[cp] = '\0';
    } else {
        strncpy(buffer, curr, length);
        br->current_line = NULL;
    }

    return buffer;
}

size_t bufferreader_current_line(BufferReader *br) {
    return br->line_index;
}

size_t bufferreader_data_length(BufferReader *br) {
    return  br->buffer_length;
}

char *bufferreader_data(BufferReader *br) {
    return br->buf;
}
