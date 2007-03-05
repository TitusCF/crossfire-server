#ifndef GOD_H
#define GOD_H

/**
 * Used to link together the gods.
 */
typedef struct glnk {
  const char *name;     /**< Name of this god. */
  struct archt *arch;   /**< Pointer to the archetype of this god. */
  int id;               /**< Id of the god. */
  const char *pantheon; /**< The name of the group this god belongs to. Unused. @todo remove? */
  struct glnk *next;    /**< Next god. */
} godlink; 

#endif /* GOD_H */
