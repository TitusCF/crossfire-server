/**
 * @file
 * Image-related structures.
 */

#ifndef IMAGE_H
#define IMAGE_H

/** Actual image data the client will display. */
typedef struct face_info {
    uint8_t *data;            /**< Image data. */
    uint16_t datalen;         /**< Length of data. */
    uint32_t checksum;        /**< Checksum of face data. */
} face_info;

/** Information about one face set */
typedef struct face_sets {
    int id; /**< @todo remove */
    char    *prefix;    /**< Faceset short name, used in pictures names (base, clsc). */
    char    *fullname;  /**< Full faceset name. */
    struct face_sets   *fallback;   /**< Faceset to use when an image is not found in this faceset. */
    char    *size;      /**< Human-readable set size. */
    char    *extension; /**< Supplementary description. */
    char    *comment;   /**< Human-readable comment for this set. */
    size_t  allocated;  /**< Allocated size of faces. */
    face_info   *faces; /**< images in this faceset */
} face_sets;

extern face_sets *find_faceset(int id);

#define MAX_IMAGE_SIZE 10000

#endif /* IMAGE_H */
