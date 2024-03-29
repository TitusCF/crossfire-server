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
#include "logger.h"

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
 * Initialize or create a BufferReader from a file path.
 * The whole file is read into memory.
 * @param br buffer to initialize, if NULL then a new BufferReader is created.
 * @param filepath path of the file to read.
 * @param failureMessage message to LOG() in case of open failure.
 * Must contain 2 placeholders '%s' for the filepath and the error message.
 * @param failureLevel log level to use in case of failure.
 * @return BufferReader with the file contents, NULL in case of error, in which case
 * a LOG() is emitted.
 */
BufferReader *bufferreader_init_from_file(BufferReader *br, const char *filepath, const char *failureMessage, LogLevel failureLevel);

/**
 * Initialize a BufferReader from a tar file entry.
 * @param br buffer to initialize.
 * @param tar tar file pointer.
 * @param h header in the tar file to initialize for.
 */
void bufferreader_init_from_tar_file(BufferReader *br, mtar_t *tar, mtar_header_t *h);

/**
 * Initialize or create a BufferReader from a memory block.
 * @param br buffer to initialize, if NULL then a new BufferReader is created.
 * @param data data to initialize the reader to.
 * @param length length of data.
 * @return BufferReader with a copy of data.
 */
BufferReader *bufferreader_init_from_memory(BufferReader *br, const char *data, size_t length);

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

