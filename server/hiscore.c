/*
 * static char *rcsid_hiscore_c =
 *   "$Id$";
 */

/*
    CrossFire, A Multiplayer game for X-windows

    Copyright (C) 2006 Mark Wedel & Crossfire Development Team
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

#include <global.h>
#ifndef __CEXTRACT__
#include <sproto.h>
#endif

/*
 * The score structure is used when treating new high-scores
 */

typedef struct scr {
  char name[BIG_NAME];      /* name  */
  char title[BIG_NAME];	    /* Title */
  char killer[BIG_NAME];    /* name (+ title) or "quit" */
  sint64 exp;               /* Experience */
  char maplevel[BIG_NAME];  /* Killed on what level */
  int maxhp,maxsp,maxgrace; /* Max hp, sp, grace when killed */
  int position;             /* Position in the highscore list */
} score;

/*
 * spool works mostly like strtok(char *, ":"), but it can also
 * log a specified error message if something goes wrong.
 */

static char *spool(char *bp, const char *error) {
  static char *prev_pos = NULL;
  char *next_pos;
  if (bp == NULL) {
    if (prev_pos == NULL) {
      LOG(llevError, "Called spool (%s) with NULL without previous call.\n",
          error);
      return NULL;
    }
    bp = prev_pos;
  }
  if (*bp == '\0') {
    LOG(llevError, "spool: End of line at %s\n", error);
    return NULL;
  }
  if ((next_pos = strchr(bp, ':')) != NULL) {
    *next_pos = '\0';
    prev_pos = next_pos + 1;
  } else
    prev_pos = NULL;
  return bp;
}

/*
 * Does what it says, copies the contents of the first score structure
 * to the second one.
 */

static void copy_score(const score *sc1, score *sc2) {
    strncpy(sc2->name, sc1->name, BIG_NAME);
    sc2->name[BIG_NAME - 1] = '\0';
    strncpy(sc2->title, sc1->title, BIG_NAME);
    sc2->title[BIG_NAME - 1] = '\0';
    strncpy(sc2->killer, sc1->killer, BIG_NAME);
    sc2->killer[BIG_NAME - 1] = '\0';
    sc2->exp = sc1->exp;
    strcpy(sc2->maplevel,sc1->maplevel);
    sc2->maxhp = sc1->maxhp;
    sc2->maxsp = sc1->maxsp;
    sc2->maxgrace = sc1->maxgrace;
}

/*
 * Writes the given score structure to a static buffer, and returns
 * a pointer to it.
 */

static char *put_score(const score *sc) {
    static char buf[MAX_BUF];

    snprintf(buf, MAX_BUF,
	 "%s:%s:%" FMT64 ":%s:%s:%d:%d:%d",sc->name,sc->title,sc->exp,sc->killer,sc->maplevel,
         sc->maxhp,sc->maxsp,sc->maxgrace);
    return buf;
}

/*
 * The oposite of put_score, get_score reads from the given buffer into
 * a static score structure, and returns a pointer to it.
 */

static score *get_score(char *bp) {
  static score sc;
  char *cp;

  if((cp=strchr(bp,'\n'))!=NULL)
    *cp='\0';

  if ((cp = spool(bp, "name")) == NULL)
    return NULL;
  strncpy(sc.name,cp,BIG_NAME);
  sc.name[BIG_NAME - 1] = '\0';

  if ((cp = spool(NULL, "title")) == NULL)
    return NULL;
  strncpy(sc.title,cp,BIG_NAME);
  sc.title[BIG_NAME - 1] = '\0';

  if ((cp = spool(NULL, "score")) == NULL)
    return NULL;

  sscanf(cp,"%" FMT64,&sc.exp);

  if ((cp = spool(NULL, "killer")) == NULL)
    return NULL;
  strncpy(sc.killer, cp, BIG_NAME);
  sc.killer[BIG_NAME - 1] = '\0';

  if ((cp = spool(NULL, "map")) == NULL)
    return NULL;
  strncpy(sc.maplevel, cp, BIG_NAME);
  sc.maplevel[BIG_NAME - 1] = '\0';

  if ((cp = spool(NULL, "maxhp")) == NULL)
    return NULL;
  sscanf(cp, "%d", &sc.maxhp);

  if ((cp = spool(NULL, "maxsp")) == NULL)
    return NULL;
  sscanf(cp, "%d", &sc.maxsp);

  if ((cp = spool(NULL, "maxgrace")) == NULL)
    return NULL;
  sscanf(cp, "%d", &sc.maxgrace);
  return &sc;
}

static char * draw_one_high_score(const score *sc) {
    static char retbuf[MAX_BUF];

    if(!strncmp(sc->killer,"quit",MAX_NAME))
	sprintf(retbuf,"%3d %10" FMT64 " %s the %s quit the game on map %s [%d][%d][%d].",
            sc->position,sc->exp,sc->name,sc->title,sc->maplevel,sc->maxhp,sc->maxsp,
		sc->maxgrace);
    else if(!strncmp(sc->killer,"left",MAX_NAME))
	sprintf(retbuf,"%3d %10" FMT64 " %s the %s left the game on map %s [%d][%d][%d].",
            sc->position,sc->exp,sc->name,sc->title,sc->maplevel,sc->maxhp,sc->maxsp,
		sc->maxgrace);
    else
	sprintf(retbuf,"%3d %10" FMT64 " %s the %s was killed by %s on map %s [%d][%d][%d].",
            sc->position,sc->exp,sc->name,sc->title,sc->killer,sc->maplevel,
            sc->maxhp,sc->maxsp,sc->maxgrace);
    return retbuf;
}
/*
 * add_score() adds the given score-structure to the high-score list, but
 * only if it was good enough to deserve a place.
 */

static score *add_score(score *new_score) {
  FILE *fp;
  static score old_score;
  score *tmp_score,pscore[HIGHSCORE_LENGTH];
  char buf[MAX_BUF], filename[MAX_BUF], *bp;
  int nrofscores=0,flag=0,i,comp;
 
  new_score->position=HIGHSCORE_LENGTH+1;
  old_score.position= -1;
  sprintf(filename,"%s/%s",settings.localdir,HIGHSCORE);
  if((fp=open_and_uncompress(filename,1,&comp))!=NULL) {
    while(fgets(buf,MAX_BUF,fp)!=NULL&&nrofscores<HIGHSCORE_LENGTH) {
      if((tmp_score=get_score(buf))==NULL) break;
      if(!flag&&new_score->exp>=tmp_score->exp) {
        copy_score(new_score,&pscore[nrofscores]);
        new_score->position=nrofscores;
        flag=1;
        if(++nrofscores>=HIGHSCORE_LENGTH)
          break;
      }
      if(!strcmp(new_score->name,tmp_score->name)) { /* Another entry */
        copy_score(tmp_score,&old_score);
        old_score.position=nrofscores;
        if(flag)
          continue;
      }
      copy_score(tmp_score,&pscore[nrofscores++]);
    }
    close_and_delete(fp, comp);
  }
  if(old_score.position!=-1&&old_score.exp>=new_score->exp)
    return &old_score; /* Did not beat old score */
  if(!flag&&nrofscores<HIGHSCORE_LENGTH)
    copy_score(new_score,&pscore[nrofscores++]);
  if((fp=fopen(filename,"w"))==NULL) {
    LOG(llevError, "Cannot write to highscore file %s: %s\n", filename, strerror_local(errno));
    return NULL;
  }
  for(i=0;i<nrofscores;i++) {
    bp=put_score(&pscore[i]);
    fprintf(fp,"%s\n",bp);
  }
  fclose(fp);
  if(flag) {
/* Eneq(@csd.uu.se): Patch to fix error in adding a new score to the
   hiscore-list */
    if(old_score.position==-1)
      return new_score;
    return &old_score;
  }
  new_score->position= -1;
  if(old_score.position!=-1)
    return &old_score;
  if(nrofscores) {
    copy_score(&pscore[nrofscores-1],&old_score);
    return &old_score;
  }
  LOG(llevError,"Highscore error.\n");
  return NULL;
}

void check_score(object *op) {
    score new_score;
    score *old_score;

    if(op->stats.exp==0)
	return;

    if(!op->contr->name_changed) {
	if(op->stats.exp>0) {
	    new_draw_info(NDI_UNIQUE, 0,op,"As you haven't changed your name, you won't");
	    new_draw_info(NDI_UNIQUE, 0,op,"get into the high-score list.");
	}
	return;
    }
    if(QUERY_FLAG(op,FLAG_WAS_WIZ)) {
	new_draw_info(NDI_UNIQUE, 0,op,"Since you have been in wizard mode,");
	new_draw_info(NDI_UNIQUE, 0,op,"you can't enter the high-score list.");
	return;
    }
    if (op->contr->explore) {
	new_draw_info(NDI_UNIQUE, 0,op,"Since you were in explore mode,");
	new_draw_info(NDI_UNIQUE, 0,op,"you can't enter the high-score list.");
	return;
    }
    strncpy(new_score.name,op->name,BIG_NAME);
    new_score.name[BIG_NAME-1] = '\0';
    strncpy(new_score.title,op->contr->own_title,BIG_NAME);
    if(new_score.title[0]=='\0')
	strncpy(new_score.title,op->contr->title,BIG_NAME);
    new_score.title[BIG_NAME-1] = '\0';
    strncpy(new_score.killer,op->contr->killer,BIG_NAME);
    if(new_score.killer[0]=='\0')
	strcpy(new_score.killer,"a dungeon collapse");
    new_score.killer[BIG_NAME-1] = '\0';
    new_score.exp=op->stats.exp;
    if(op->map == NULL)
	*new_score.maplevel = '\0';
    else { 
	strncpy(new_score.maplevel,
		op->map->name?op->map->name:op->map->path,
		BIG_NAME-1);
	new_score.maplevel[BIG_NAME-1] = '\0';
    }
    new_score.maxhp=(int) op->stats.maxhp;
    new_score.maxsp=(int) op->stats.maxsp;
    new_score.maxgrace=(int) op->stats.maxgrace;
    if((old_score=add_score(&new_score))==NULL) {
	new_draw_info(NDI_UNIQUE, 0,op,"Error in the highscore list.");
	return;
    }
    if(new_score.position == -1) {
	new_score.position = HIGHSCORE_LENGTH+1; /* Not strictly correct... */
	if(!strcmp(old_score->name,new_score.name))
	    new_draw_info(NDI_UNIQUE, 0,op,"You didn't beat your last highscore:");
	else
	    new_draw_info(NDI_UNIQUE, 0,op,"You didn't enter the highscore list:");
	new_draw_info(NDI_UNIQUE, 0,op, draw_one_high_score(old_score));
	new_draw_info(NDI_UNIQUE, 0,op, draw_one_high_score(&new_score));
	return;
    }
    if(old_score->exp>=new_score.exp)
	new_draw_info(NDI_UNIQUE, 0,op,"You didn't beat your last score:");
    else
	new_draw_info(NDI_UNIQUE, 0,op,"You beat your last score:");

    new_draw_info(NDI_UNIQUE, 0,op, draw_one_high_score(old_score));
    new_draw_info(NDI_UNIQUE, 0,op, draw_one_high_score(&new_score));
}



/* displays the high score file.  object is the calling object
 * (null if being called via command line.)  max is the maximum
 * number of scores to display.  match, if set, is the name or class
 * to match to.
 */

void display_high_score(object *op,int max, const char *match) {
    const size_t maxchar = 80;
    FILE *fp;
    char buf[MAX_BUF],*scorebuf, *bp, *cp;
    int i=0,j=0,comp;
    score *sc;

    sprintf(buf,"%s/%s",settings.localdir,HIGHSCORE);
    if((fp=open_and_uncompress(buf,0,&comp))==NULL) {
	LOG(llevError, "Cannot open highscore file %s: %s\n", buf, strerror_local(errno));
	if(op!=NULL)
	    new_draw_info(NDI_UNIQUE, 0,op,"There is no highscore file.");
	return;
    }
    if(op != NULL)
	    clear_win_info(op);
    new_draw_info(NDI_UNIQUE, 0,op,"Nr    Score    Who [max hp][max sp][max grace]");

    while(fgets(buf,MAX_BUF,fp)!=NULL) {
	if(j>=HIGHSCORE_LENGTH||i>=(max-1))
	    break;
	if((sc=get_score(buf))==NULL)
	    break;
	sc->position=++j;
	if (match==NULL) {
	    scorebuf=draw_one_high_score(sc);
	    i++;
	} else {
	    if (!strcasecmp(sc->name, match) || !strcasecmp(sc->title, match)) {
		scorebuf=draw_one_high_score(sc);
		i++;
	    }
	    else continue;
	}
	/* Replaced what seemed to an overly complicated word wrap method 
	 * still word wraps, but assumes at most 2 lines of data.
	 * mw - 2-12-97
	 */
	strncpy(buf,scorebuf,MAX_BUF);
	buf[MAX_BUF-1] = '\0';
	cp=buf;
	while (strlen(cp)> maxchar) {
	    bp = cp+maxchar-1;
	    while (*bp != ' ' && bp>cp) bp--;
	    *bp='\0';
	    if (op == NULL) {
		LOG(llevDebug, "%s\n", cp);
	    }
	    else {
		new_draw_info(NDI_UNIQUE, 0,op,cp);
	    }
	    sprintf(buf, "            %s", bp+1);
	    cp = buf;
	    i++;
	}
	if(op == NULL) 
		LOG(llevDebug, "%s\n", buf);
	else
		new_draw_info(NDI_UNIQUE, 0,op,buf);
    }
    close_and_delete(fp, comp);
}
