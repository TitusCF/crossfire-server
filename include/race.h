#ifndef RACE_H
#define RACE_H

typedef struct ralnk {  /* Used to link the race lists together */
  const char *name;		/* name of this race entry */ 
  int nrof;		/* nrof things belonging to this race */
  struct oblnk *member; /* linked object list of things belonging to this race */
  struct ralnk *next;
} racelink;

#endif /* RACE_H */
