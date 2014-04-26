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
typedef struct {
    char    *prefix;    /**< Faceset short name, used in pictures names (base, clsc). */
    char    *fullname;  /**< Full faceset name. */
    uint8_t   fallback;   /**< Faceset to use when an image is not found in this faceset, index in facesets. */
    char    *size;      /**< Human-readable set size. */
    char    *extension; /**< Supplementary description. */
    char    *comment;   /**< Human-readable comment for this set. */
    face_info   *faces; /**< images in this faceset */
} face_sets;
#define MAX_FACE_SETS   20  /**< Maximum number of image sets the program will handle */

extern face_sets facesets[MAX_FACE_SETS];

extern unsigned int nrofpixmaps;

#define MAX_IMAGE_SIZE 10000

#endif /* IMAGE_H */
