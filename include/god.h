typedef struct glnk { /* Used to link together the gods */
  char *name;       		/* name of this god */
  struct archt *arch;      	/* pointer to the archetype of this god */
  int id;               	/* id of the god */ 
  char *pantheon;      		/* the name of the group this god belongs to */
  struct glnk *next;    
} godlink; 

