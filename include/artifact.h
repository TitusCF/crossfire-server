/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2002-2006 Mark Wedel & Crossfire Development Team
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
 * Artifact-related structures.
 *
 * See the @ref page_artifact "page on artifacts" for more information.
 */

#ifndef ARTIFACT_H
#define ARTIFACT_H

/**
 * This is one artifact, ie one special item.
 */
typedef struct artifactstruct {
    object *item;                   /**< Special values of the artifact. Note that this object is malloc() ed. */
    uint16 chance;                  /**< Chance of the artifact to happen. */
    uint8 difficulty;               /**< Minimum map difficulty for the artifact to happen. */
    struct artifactstruct *next;    /**< Next artifact in the list. */
    linked_char *allowed;           /**< List of archetypes the artifact can affect. */
    int allowed_size;               /**< Length of allowed, for faster computation. */
} artifact;

/**
 * This represents all archetypes for one particular object type.
 */
typedef struct artifactliststruct {
    uint8 type;                         /**< Object type that this list represents. */
    uint16 total_chance;                /**< Sum of chance for are artifacts on this list. */
    struct artifactliststruct *next;    /**< Next list of artifacts. */
    struct artifactstruct *items;       /**< Artifacts for this type. Will never be NULL. */
} artifactlist;

#endif /* ARTIFACT_H */
