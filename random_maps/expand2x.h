/*
 * Expands a layout by 2x in each dimension.
 * (Header file)
 * H. S. Teoh
 * --------------------------------------------------------------------------
 * $Id$
 */

#ifndef EXPAND2X_H
#define EXPAND2X_H

/* Expands a layout by 2x in each dimension.
 * Resulting layout is actually (2*xsize-1)x(2*ysize-1). (Because of the cheesy
 * algorithm, but hey, it works).
 *
 * Don't forget to free the old layout after this is called (it does not
 * presume to do so itself).
 */
char **expand2x(char **layout, int xsize, int ysize);

#endif /* EXPAND2X_H */
