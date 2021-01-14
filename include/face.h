/**
 * @file
 * Face-related structures.
 */

#ifndef FACE_H
#define FACE_H

/**
 * New face structure - this enforces the notion that data is face by
 * face only - you can not change the color of an item - you need to instead
 * create a new face with that color.
 */
typedef struct Face {
    uint16_t number;              /**< This is the image unique identifier. */
    uint8_t visibility;
    uint8_t magicmap;             /**< Color to show this in magic map */
    struct Face *smoothface;  /**< Smoothed face for this, NULL for none. */
    const char *name;             /**< Face name, as used by archetypes and such. */
} Face;

/**
 * This represents one animation. It points to different faces.
 */
typedef struct animations_struct {
    sstring name;             /**< Name of the animation sequence. */
    uint8_t num_animations;   /**< How many different faces to animate, size of the faces array. */
    uint8_t facings;          /**< How many facings (1,2,4,8). */
    uint16_t num;             /**< Where we are in the array. */
    const Face **faces; /**< The actual faces for the animation. */
} Animations;

#endif /* FACE_H */
