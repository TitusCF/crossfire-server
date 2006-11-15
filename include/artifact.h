#ifndef ARTIFACT_H
#define ARTIFACT_H

typedef struct artifactstruct {
	object	*item;
	uint16	chance;
	uint8	difficulty;
	struct artifactstruct *next;
	linked_char *allowed;
} artifact;

typedef struct artifactliststruct {
	uint8	type;		/* Object type that this list represents */
	uint16	total_chance;	/* sum of chance for are artifacts on this list */
	struct artifactliststruct *next;
	struct artifactstruct *items;
} artifactlist;

#endif /* ARTIFACT_H */
