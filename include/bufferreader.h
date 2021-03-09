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

/** @file
 *
 * Implements a buffer reader.
 *
 * Usage is:
 * <code>
 * BufferReader *reader = bufferreader_create();
 * bufferreader_init_from_file(reader, opened_file);
 * char *buf:
 * while ((buf = bufferreader_next_line(reader)) != NULL) {
 *   (...)
 * }
 * bufferreader_destroy(reader);
 * </code>
 *
 * Alternative is to access the buffer's full data through bufferreader_data()
 * and bufferreader_data_length().
 *
 * Buffer may and should be reused to avoid allocating/freeing memory all the time.
 *
 * No function ever fails. In case not enough memory is available, {@link
 * fatal()} is called.
 */

#ifndef BUFFERREADER_H
#define BUFFERREADER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "microtar.h"

/**
 * The reader state.
 */
typedef struct BufferReader BufferReader;

/**
 * Create a new BufferReader.
 * @return new buffer, never NULL.
 */
BufferReader *bufferreader_create();

/**
 * Destroy a BufferReader.
 * @param br buffer to destroy, pointer becomes invalid after the call.
 */
void bufferreader_destroy(BufferReader *br);

/**
 * Initialize a BufferReader from a file pointer.
 * The whole file is read into memory.
 * @param br buffer to initialize.
 * @param file opened file to load, must be opened in binary mode.
 */
void bufferreader_init_from_file(BufferReader *br, FILE *file);

/**
 * Initialize a BufferReader from a tar file entry.
 * @param br buffer to initialize.
 * @param tar tar file pointer.
 * @param h header in the tar file to initialize for.
 */
void bufferreader_init_from_tar_file(BufferReader *br, mtar_t *tar, mtar_header_t *h);

/**
 * Return the next line in the buffer, as separated by a newline.
 * @param br buffer to get the line from.
 * @return NULL if end of buffer is reached, else a null-terminated string.
 */
char *bufferreader_next_line(BufferReader *br);

/**
 * Copy the next line in the buffer, keeping the newline.
 * @param br buffer to get the line from.
 * @param buffer where to put the line.
 * @param length maximum length of buffer.
 * @return NULL if no more data is available, buffer else.
 */
char *bufferreader_get_line(BufferReader *br, char *buffer, size_t length);

/**
 * Return the index of the last line returned by bufferreader_next_line().
 * @param br buffer to get the line index of.
 * @return line index, starting from 1.
 */
size_t bufferreader_current_line(BufferReader *br);

/**
 * Return the length of the buffer data.
 * @param br buffer.
 * @return buffer length.
 */
size_t bufferreader_data_length(BufferReader *br);

/**
 * Get the whole buffer, independently of the calls to bufferreader_next_line().
 * @param br buffer.
 * @return buffer data, may be NULL.
 */
char *bufferreader_data(BufferReader *br);


#ifdef __cplusplus
}
#endif

#endif /* BUFFERREADER_H */

