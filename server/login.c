/*
 * static char *rcsid_login_c =
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
#include <spells.h>
#include <loader.h>
#include <define.h>

extern void sub_weight (object *, signed long);
extern void add_weight (object *, signed long);

static void copy_file(const char *filename, FILE *fpout);

/* If flag is non zero, it means that we want to try and save everyone, but
 * keep the game running.  Thus, we don't want to free any information.
 */
void emergency_save(int flag) {
  player *pl;
#ifndef NO_EMERGENCY_SAVE
  trying_emergency_save = 1;
  if(editor)
    return;
  LOG(llevError,"Emergency save:  ");
  for(pl=first_player;pl!=NULL;pl=pl->next) {
    if(!pl->ob) {
      LOG(llevError, "No name, ignoring this.\n");
      continue;
    }
    LOG(llevError,"%s ",pl->ob->name);
    draw_ext_info(NDI_UNIQUE, 0,pl->ob, MSG_TYPE_ADMIN,  MSG_TYPE_ADMIN_LOADSAVE,
		  "Emergency save...", NULL);

/* If we are not exiting the game (ie, this is sort of a backup save), then
 * don't change the location back to the village.  Note that there are other
 * options to have backup saves be done at the starting village
 */
    if (!flag) {
	strcpy(pl->maplevel, first_map_path);
	if(pl->ob->map!=NULL)
	    pl->ob->map = NULL;
	pl->ob->x = -1;
	pl->ob->y = -1;
    }
    if(!save_player(pl->ob,flag)) {
      LOG(llevError, "(failed) ");
      draw_ext_info(NDI_UNIQUE, 0,pl->ob,MSG_TYPE_ADMIN,  MSG_TYPE_ADMIN_LOADSAVE,
		    "Emergency save failed, checking score...", NULL);
    }
    check_score(pl->ob);
  }
  LOG(llevError,"\n");
#else
  LOG(llevInfo,"Emergency saves disabled, no save attempted\n");
#endif
  /* If the game is exiting, remove the player locks */
  if (!flag) {
    for(pl=first_player;pl!=NULL;pl=pl->next) {
      if(pl->ob) {
      }
    }
  }
}

/* Delete character with name.  if new is set, also delete the new
 * style directory, otherwise, just delete the old style playfile
 * (needed for transition)
 */
void delete_character(const char *name, int new) {
    char buf[MAX_BUF], err[MAX_BUF];

    sprintf(buf,"%s/%s/%s.pl",settings.localdir,settings.playerdir,name);
    if(unlink(buf)== -1)
	LOG(llevDebug, "Cannot delete character file %s: %s\n", buf, strerror_local(errno, err, sizeof(err)));
    if (new) {
	sprintf(buf,"%s/%s/%s",settings.localdir,settings.playerdir,name);
	/* this effectively does an rm -rf on the directory */
	remove_directory(buf);
    }
}

/* This verify that a character of name exits, and that it matches
 * password.  It return 0 if there is match, 1 if no such player,
 * 2 if incorrect password.
 */

int verify_player(const char *name, char *password)
{
    char buf[MAX_BUF];
    int comp;
    FILE *fp;

    if (strpbrk(name, "/.\\") != NULL) {
	LOG(llevError, "Username contains illegal characters: %s\n", name);
	return 1;
    }

    snprintf(buf, sizeof(buf), "%s/%s/%s/%s.pl", settings.localdir, settings.playerdir, name, name);
    if (strlen(buf) >= sizeof(buf)-1) {
	LOG(llevError, "Username too long: %s\n", name);
	return 1;
    }

    if ((fp=open_and_uncompress(buf,0,&comp))==NULL) return 1;

    /* Read in the file until we find the password line.  Our logic could
     * be a bit better on cleaning up the password from the file, but since
     * it is written by the program, I think it is fair to assume that the
     * syntax should be pretty standard.
     */
    while (fgets(buf, MAX_BUF-1, fp) != NULL) {
	if (!strncmp(buf,"password ",9)) {
	    buf[strlen(buf)-1]=0;	/* remove newline */
	    if (check_password(password, buf+9)) {
		close_and_delete(fp, comp);
		return 0;
	    }
	    else {
		close_and_delete(fp, comp);
		return 2;
	    }
	}
    }
    LOG(llevDebug,"Could not find a password line in player %s\n", name);
    close_and_delete(fp, comp);
    return 1;
}

/* Checks to see if anyone else by 'name' is currently playing. 
 * If we find that file or another character of some name is already in the
 * game, we don't let this person join (we should really let the new player
 * enter the password, and if correct, disconnect that socket and attach it to
 * the players current session.
 * If no one by that name is currently playing, we then make sure the name
 * doesn't include any bogus characters.
 * We return 0 if the name is in use/bad, 1 if it is OK to use this name.
 */

int check_name(player *me,const char *name) {

    if (*name=='\0') {
	draw_ext_info(NDI_UNIQUE, 0,me->ob,MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
		      "Your username cannot be blank.", NULL);
	return 0;
    }

    if(!playername_ok(name)) {
	draw_ext_info(NDI_UNIQUE, 0,me->ob,MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
		      "That name contains illegal characters. Use letters, hyphens and underscores only. Hyphens and underscores are not allowed as the first character.", NULL);
	return 0;
    }
    if (strlen(name) >= MAX_NAME) {
	draw_ext_info_format(NDI_UNIQUE, 0,me->ob,MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
		      "That name is too long. (Max length: %d characters)", NULL, MAX_NAME);
	return 0;
    }

    return 1;
}

void destroy_object (object *op)
{
    object *tmp;
    while ((tmp = op->inv))
	destroy_object (tmp);

    if (!QUERY_FLAG(op, FLAG_REMOVED))
	remove_ob(op);
    free_object(op);
}

/*
 * If flag is set, it's only backup, ie dont remove objects from inventory
 * If BACKUP_SAVE_AT_HOME is set, and the flag is set, then the player
 * will be saved at the emergency save location.
 * Returns non zero if successful.
 */

int save_player(object *op, int flag) {
  FILE *fp;
  char filename[MAX_BUF], *tmpfilename,backupfile[MAX_BUF];
  object *tmp, *container=NULL;
  player *pl = op->contr;
  int i,wiz=QUERY_FLAG(op,FLAG_WIZ);
  long checksum;
#ifdef BACKUP_SAVE_AT_HOME
  sint16 backup_x, backup_y;
#endif

  if (!op->stats.exp) return 0;	/* no experience, no save */

  flag&=1;

  if(!pl->name_changed||(!flag&&!op->stats.exp)) {
    if(!flag) {
      draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOADSAVE,
		    "Your game is not valid, game not saved.", NULL);
    }
    return 0;
  }

    /* Sanity check - some stuff changes this when player is exiting */
    if (op->type != PLAYER) return 0;

    /* Prevent accidental saves if connection is reset after player has
     * mostly exited.
     */
    if (pl->state != ST_PLAYING && pl->state != ST_GET_PARTY_PASSWORD)
	return 0;

  if (flag == 0)
    terminate_all_pets(op);

  /* Delete old style file */
  sprintf(filename,"%s/%s/%s.pl",settings.localdir,settings.playerdir,op->name);
  unlink(filename);

  sprintf(filename,"%s/%s/%s/%s.pl",settings.localdir,settings.playerdir,op->name,op->name);
  make_path_to_file(filename);
  tmpfilename = tempnam_local(settings.tmpdir,NULL);
  fp=fopen(tmpfilename, "w");
  if(!fp) {
    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOADSAVE,
		  "Can't open file for save.", NULL);
    LOG(llevDebug,"Can't open file for save (%s).\n",tmpfilename);
    free(tmpfilename);
    return 0;
  }

/* Eneq(@csd.uu.se): If we have an open container hide it. */
   if (op->container)  {
     container=op->container;
       op->container=NULL;
   }

  fprintf(fp,"password %s\n",pl->password);
  if (settings.set_title == TRUE)
      if(pl->own_title[0]!='\0')
	  fprintf(fp,"title %s\n",pl->own_title);

  fprintf(fp,"explore %d\n",pl->explore);
  fprintf(fp,"gen_hp %d\n",pl->gen_hp);
  fprintf(fp,"gen_sp %d\n",pl->gen_sp);
  fprintf(fp,"gen_grace %d\n",pl->gen_grace);
  fprintf(fp,"listening %d\n",pl->listening);
  fprintf(fp,"shoottype %d\n",pl->shoottype);
  fprintf(fp,"bowtype %d\n",pl->bowtype);
  fprintf(fp,"petmode %d\n",pl->petmode);
  fprintf(fp,"peaceful %d\n",pl->peaceful);
  fprintf(fp,"no_shout %d\n",pl->no_shout);
  fprintf(fp,"digestion %d\n",pl->digestion);
  fprintf(fp,"pickup %d\n", pl->mode);
  fprintf(fp,"outputs_sync %d\n", pl->outputs_sync);
  fprintf(fp,"outputs_count %d\n", pl->outputs_count);
  /* Match the enumerations but in string form */
  fprintf(fp,"usekeys %s\n", pl->usekeys==key_inventory?"key_inventory":
	  (pl->usekeys==keyrings?"keyrings":"containers"));
  /* Match the enumerations but in string form */
  fprintf(fp,"unapply %s\n", pl->unapply==unapply_nochoice?"unapply_nochoice":
	  (pl->unapply==unapply_never?"unapply_never":"unapply_always"));



#ifdef BACKUP_SAVE_AT_HOME
  if (op->map!=NULL && flag==0)
#else
  if (op->map!=NULL)
#endif
    fprintf(fp,"map %s\n",op->map->path);
  else
    fprintf(fp,"map %s\n",settings.emergency_mapname);
  
  fprintf(fp,"savebed_map %s\n", pl->savebed_map);
  fprintf(fp,"bed_x %d\nbed_y %d\n", pl->bed_x, pl->bed_y);
  fprintf(fp,"weapon_sp %f\n",pl->weapon_sp);
  fprintf(fp,"Str %d\n",pl->orig_stats.Str);
  fprintf(fp,"Dex %d\n",pl->orig_stats.Dex);
  fprintf(fp,"Con %d\n",pl->orig_stats.Con);
  fprintf(fp,"Int %d\n",pl->orig_stats.Int);
  fprintf(fp,"Pow %d\n",pl->orig_stats.Pow);
  fprintf(fp,"Wis %d\n",pl->orig_stats.Wis);
  fprintf(fp,"Cha %d\n",pl->orig_stats.Cha);

  fprintf(fp,"lev_array %d\n",op->level>10?10:op->level);
  for(i=1;i<=pl->last_level&&i<=10;i++) {
    fprintf(fp,"%d\n",pl->levhp[i]);
    fprintf(fp,"%d\n",pl->levsp[i]);
	 fprintf(fp,"%d\n",pl->levgrace[i]);
  }
  fprintf(fp,"endplst\n");

  SET_FLAG(op, FLAG_NO_FIX_PLAYER);
  CLEAR_FLAG(op, FLAG_WIZ);
#ifdef BACKUP_SAVE_AT_HOME
  if (flag) {
    backup_x = op->x;
    backup_y = op->y;
    op->x = -1;
    op->y = -1;
  }
  /* Save objects, but not unpaid objects.  Don't remove objects from
   * inventory.
   */
  save_object(fp, op, 2);
  if (flag) {
    op->x = backup_x;
    op->y = backup_y;
  }
#else
  save_object(fp, op, 3); /* don't check and don't remove */
#endif

  CLEAR_FLAG(op, FLAG_NO_FIX_PLAYER);

  if(!flag)
      while ((tmp = op->inv))
	  destroy_object (tmp);

  if (fclose(fp) == EOF) {	/* make sure the write succeeded */
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOADSAVE,
		      "Can't save character.", NULL);
	unlink(tmpfilename);
	free(tmpfilename);
	return 0;
  }
  checksum = 0;
  sprintf(backupfile, "%s.tmp", filename);
  rename(filename, backupfile);
  fp = fopen(filename,"w");
  if(!fp) {
    draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOADSAVE,
		  "Can't open file for save.", NULL);
    unlink(tmpfilename);
    free(tmpfilename);
    return 0;
  }
  fprintf(fp,"checksum %lx\n",checksum);
  copy_file(tmpfilename, fp);
  unlink(tmpfilename);
  free(tmpfilename);
  if (fclose(fp) == EOF) {	/* got write error */
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOADSAVE,
		      "Can't close file for save.", NULL);
	rename(backupfile, filename); /* Restore the original */
	return 0;
  }
  else
	unlink(backupfile);

  /* Eneq(@csd.uu.se): Reveal the container if we have one. */
  if (flag&&container!=NULL) 
    op->container = container;

  if (wiz) SET_FLAG(op,FLAG_WIZ);
  if(!flag)
	esrv_send_inventory(op, op);

  chmod(filename,SAVE_MODE);
  return 1;
}

static void copy_file(const char *filename, FILE *fpout) {
  FILE *fp;
  char buf[MAX_BUF];
  if((fp = fopen(filename,"r")) == NULL)
    return;
  while(fgets(buf,MAX_BUF,fp)!=NULL)
    fputs(buf,fpout);
  fclose(fp);
}

/* Simple function to print errors when password is
 * not correct
 */
static void wrong_password(object *op)
{
    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
	  "\nA character with this name already exists. "
	  "Please choose another name, or make sure you entered your "
	  "password correctly.\n",
	  NULL);

    FREE_AND_COPY(op->name, "noname");
    FREE_AND_COPY(op->name_pl, "noname");

    op->contr->socket.password_fails++;
    if (op->contr->socket.password_fails >= MAX_PASSWORD_FAILURES) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
	      "You gave an incorrect password too many times, "
	      "you will now be dropped from the server.", 
	      NULL);

	LOG(llevInfo, "A player connecting from %s has been dropped for password failure\n", 
	    op->contr->socket.host);

	op->contr->socket.status = Ns_Dead; /* the socket loop should handle the rest for us */
    }
    else get_name(op);
}

void check_login(object *op) {
    FILE *fp;
    char filename[MAX_BUF];
    char buf[MAX_BUF],bufall[MAX_BUF];
    int i,value,comp;
    long checksum = 0;
    player *pl = op->contr, *pltmp;
    int correct = 0;
    time_t    elapsed_save_time=0;
    struct stat	statbuf;

    strcpy (pl->maplevel,first_map_path);

    /* First, lets check for newest form of save */
    sprintf(filename,"%s/%s/%s/%s.pl",settings.localdir,settings.playerdir,op->name,op->name);
    if (access(filename, F_OK)==-1) {
	/* not there,  Try the old style */

	sprintf(filename,"%s/%s/%s.pl",settings.localdir,settings.playerdir,op->name);
	/* Ok - old style exists.  Lets make the new style directory */
	if (access(filename, F_OK)==0) {
	    sprintf(buf,"%s/%s/%s",settings.localdir,settings.playerdir,op->name);
	    make_path_to_file(buf);
	}
    }

    for(pltmp=first_player; pltmp!=NULL; pltmp=pltmp->next) {
	if(pltmp!=pl && pltmp->ob->name != NULL && !strcmp(pltmp->ob->name,op->name)) {
	    if (check_password(pl->write_buf+1, pltmp->password)) {

		/* We could try and be more clever and re-assign the existing
		 * object to the new player, etc.  However, I'm concerned that
		 * there may be a lot of other state that still needs to be sent
		 * in that case (we can't make any assumptions on what the
		 * client knows, as maybe the client crashed), so treating it
		 * as just a normal login is the safest and easiest thing to do.
		 */

		pltmp->socket.status=Ns_Dead;

		save_player(pltmp->ob, 0);
		if(!QUERY_FLAG(pltmp->ob,FLAG_REMOVED)) {
		    /* Need to terminate the pets, since the new object
		     * will be different
		     */
		    terminate_all_pets(pltmp->ob);
		    remove_ob(pltmp->ob);
		}
		leave(pltmp,1);
		final_free_player(pltmp);
		break;
	    } else {
		wrong_password(op);
		return;
	    }
	}
    }

    /* If no file, must be a new player, so lets get confirmation of
     * the password.  Return control to the higher level dispatch,
     * since the rest of this just deals with loading of the file.
     */
    if ((fp=open_and_uncompress(filename,1,&comp)) == NULL) {
	confirm_password(op);
	return;
    }
    if (fstat(fileno(fp), &statbuf)) {
	LOG(llevError,"Unable to stat %s?\n", filename);
	elapsed_save_time=0;
    } else {
	elapsed_save_time = time(NULL) - statbuf.st_mtime;
	if (elapsed_save_time<0) {
	    LOG(llevError,"Player file %s was saved in the future? (%d time)\n", filename, elapsed_save_time);
	    elapsed_save_time=0;
	}
    }

    if(fgets(bufall,MAX_BUF,fp) != NULL) {
	if(!strncmp(bufall,"checksum ",9)) {
	    checksum = strtol_local(bufall+9,(char **) NULL, 16);
	    (void) fgets(bufall,MAX_BUF,fp);
	}
	if(sscanf(bufall,"password %s\n",buf)) {
	    /* New password scheme: */
	    correct=check_password(pl->write_buf+1,buf);
	}
	/* Old password mode removed - I have no idea what it 
	 * was, and the current password mechanism has been used
	 * for at least several years.
	 */
    }
    if (!correct) {
	wrong_password(op);
	return;
    }

#ifdef SAVE_INTERVAL
    pl->last_save_time=time(NULL);
#endif /* SAVE_INTERVAL */
    pl->party = NULL;
    if (settings.search_items == TRUE)
	pl->search_str[0]='\0';
    pl->name_changed=1;
    pl->orig_stats.Str=0;
    pl->orig_stats.Dex=0;
    pl->orig_stats.Con=0;
    pl->orig_stats.Int=0;
    pl->orig_stats.Pow=0;
    pl->orig_stats.Wis=0;
    pl->orig_stats.Cha=0;
    strcpy(pl->savebed_map, first_map_path);
    pl->bed_x=0, pl->bed_y=0;
    pl->spellparam[0] = '\0';
    
    /* Loop through the file, loading the rest of the values */
    while (fgets(bufall,MAX_BUF,fp)!=NULL) {
	sscanf(bufall,"%s %d\n",buf,&value);
        if (!strcmp(buf,"endplst"))
          break;
	else if (!strcmp(buf,"title") && settings.set_title == TRUE)
	    sscanf(bufall,"title %[^\n]",pl->own_title);
	else if (!strcmp(buf,"explore"))
	    pl->explore = value;
	else if (!strcmp(buf,"gen_hp"))
	    pl->gen_hp=value;
        else if (!strcmp(buf,"shoottype"))
	    pl->shoottype=(rangetype)value;
	else if (!strcmp(buf,"bowtype"))
	    pl->bowtype=(bowtype_t)value;
	else if (!strcmp(buf,"petmode"))
	    pl->petmode=(petmode_t)value;
        else if (!strcmp(buf,"gen_sp"))
	    pl->gen_sp=value;
        else if (!strcmp(buf,"gen_grace"))
	    pl->gen_grace=value;
        else if (!strcmp(buf,"listening"))
	    pl->listening=value;
        else if (!strcmp(buf,"peaceful"))
	    pl->peaceful=value;
		else if (!strcmp(buf,"no_shout"))
		pl->no_shout=value;
        else if (!strcmp(buf,"digestion"))
	    pl->digestion=value;
	else if (!strcmp(buf,"pickup"))
	    pl->mode=value;
	else if (!strcmp(buf,"outputs_sync"))
	    pl->outputs_sync = value;
	else if (!strcmp(buf,"outputs_count"))
	    pl->outputs_count = value;
        else if (!strcmp(buf,"map"))
	    sscanf(bufall,"map %s", pl->maplevel);
        else if (!strcmp(buf,"savebed_map"))
	    sscanf(bufall,"savebed_map %s", pl->savebed_map);
	else if (!strcmp(buf,"bed_x"))
	    pl->bed_x=value;
	else if (!strcmp(buf,"bed_y"))
	    pl->bed_y=value;
	else if (!strcmp(buf,"weapon_sp"))
	    sscanf(buf,"weapon_sp %f",&pl->weapon_sp);
        else if (!strcmp(buf,"Str"))
	    pl->orig_stats.Str=value;
        else if (!strcmp(buf,"Dex"))
	    pl->orig_stats.Dex=value;
        else if (!strcmp(buf,"Con"))
	    pl->orig_stats.Con=value;
        else if (!strcmp(buf,"Int"))
	    pl->orig_stats.Int=value;
        else if (!strcmp(buf,"Pow"))
	    pl->orig_stats.Pow=value;
        else if (!strcmp(buf,"Wis"))
	    pl->orig_stats.Wis=value;
        else if (!strcmp(buf,"Cha"))
	    pl->orig_stats.Cha=value;
	else if (!strcmp(buf,"usekeys")) {
	    if (!strcmp(bufall+8,"key_inventory\n"))
		pl->usekeys=key_inventory;
	    else if (!strcmp(bufall+8,"keyrings\n"))
		pl->usekeys=keyrings;
	    else if (!strcmp(bufall+8,"containers\n"))
		pl->usekeys=containers;
	    else LOG(llevDebug,"load_player: got unknown usekeys type: %s\n", bufall+8);
	}
	else if (!strcmp(buf,"unapply")) {
	    if (!strcmp(bufall+8,"unapply_nochoice\n"))
		pl->unapply=unapply_nochoice;
	    else if (!strcmp(bufall+8,"unapply_never\n"))
		pl->unapply=unapply_never;
	    else if (!strcmp(bufall+8,"unapply_always\n"))
		pl->unapply=unapply_always;
	    else LOG(llevDebug,"load_player: got unknown unapply type: %s\n", bufall+8);
	}
        else if (!strcmp(buf,"lev_array")){
	    for(i=1;i<=value;i++) {
		int j;
		fscanf(fp,"%d\n",&j);
		pl->levhp[i]=j;
		fscanf(fp,"%d\n",&j);
		pl->levsp[i]=j;
		fscanf(fp,"%d\n",&j);
		pl->levgrace[i]=j;
	    }
	/* spell_array code removed - don't know when that was last used.
	 * Even the load code below will someday be replaced by spells being
	 * objects.
	 */
     /*TODO remove block inside #if 0*/
	} else if (!strcmp(buf,"known_spell")) {
#if 0
	    /* Logic is left here in case someone wants to try
	     * and write code to update to spell objects.
	     */
	    char *cp=strchr(bufall,'\n');
	    *cp='\0';
	    cp=strchr(bufall,' ');
	    cp++;
	    for(i=0;i<NROFREALSPELLS;i++)
		if(!strcmp(spells[i].name,cp)) {
		    pl->known_spells[pl->nrofknownspells++]=i;
		    break;
		}
	    if(i==NROFREALSPELLS)
		LOG(llevDebug, "Error: unknown spell (%s)\n",cp);
#endif
	}
    } /* End of loop loading the character file */
    leave_map(op);
    op->speed=0;
    update_ob_speed(op);
    /*FIXME dangerous call, reset_object should be used to init freshly allocated obj struct!*/
    reset_object(op);
    op->contr = pl;
    pl->ob = op;
    /* this loads the standard objects values. */
    load_object(fp, op, LO_NEWFILE,0);
    close_and_delete(fp, comp);

    CLEAR_FLAG(op, FLAG_NO_FIX_PLAYER);

    strncpy(pl->title, op->arch->clone.name, sizeof(pl->title)-1);
    pl->title[sizeof(pl->title)-1] = '\0';
    
    /* If the map where the person was last saved does not exist,
     * restart them on their home-savebed. This is good for when
     * maps change between versions
     * First, we check for partial path, then check to see if the full
     * path (for unique player maps)
     */
    if (check_path(pl->maplevel,1)==-1) {
      if (check_path(pl->maplevel,0)==-1) {
	strcpy(pl->maplevel, pl->savebed_map);
	op->x = pl->bed_x, op->y = pl->bed_y;
      }
    }

    /* If player saved beyond some time ago, and the feature is
     * enabled, put the player back on his savebed map.
     */
    if ((settings.reset_loc_time >0) && (elapsed_save_time > settings.reset_loc_time)) {
	strcpy(pl->maplevel, pl->savebed_map);
	op->x = pl->bed_x, op->y = pl->bed_y;
    }

    /* make sure he's a player--needed because of class change. */
    op->type = PLAYER;

    enter_exit(op,NULL);

    pl->name_changed=1;
    pl->state = ST_PLAYING;
#ifdef AUTOSAVE
    pl->last_save_tick = pticks;
#endif
    op->carrying = sum_weight (op);
    /* Need to call fix_object now - program modified so that it is not
     * called during the load process (FLAG_NO_FIX_PLAYER set when
     * saved)
     * Moved ahead of the esrv functions, so proper weights will be
     * sent to the client.
     */
    link_player_skills(op);

    if ( ! legal_range (op, op->contr->shoottype))
        op->contr->shoottype = range_none;
    
    fix_object(op);
    
    /* if it's a dragon player, set the correct title here */
    if (is_dragon_pl(op) && op->inv != NULL) {
        object *tmp, *abil=NULL, *skin=NULL;
        for (tmp=op->inv; tmp!=NULL; tmp=tmp->below) {
	    if (tmp->type == FORCE) {
	        if (strcmp(tmp->arch->name, "dragon_ability_force")==0)
		    abil = tmp;
		else if (strcmp(tmp->arch->name, "dragon_skin_force")==0)
		    skin = tmp;
	    }
	}
	set_dragon_name(op, abil, skin);
    }
    
    draw_ext_info(NDI_UNIQUE, 0,op,MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
		  "Welcome Back!", NULL);
    draw_ext_info_format(NDI_UNIQUE | NDI_ALL | NDI_DK_ORANGE, 5, NULL,
		 MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_PLAYER,
		 "%s has entered the game.",
		 "%s has entered the game.",
		 pl->ob->name);

    /* Lauwenmark : Here we handle the LOGIN global event */
    execute_global_event(EVENT_LOGIN, pl, pl->socket.host);
    op->contr->socket.update_look=1;
    /* If the player should be dead, call kill_player for them
     * Only check for hp - if player lacks food, let the normal
     * logic for that to take place.  If player is permanently
     * dead, and not using permadeath mode, the kill_player will
     * set the play_again flag, so return.
     */
    if (op->stats.hp<0) {
	draw_ext_info(NDI_UNIQUE, 0,op, MSG_TYPE_ADMIN, MSG_TYPE_ADMIN_LOGIN,
		      "Your character was dead last your played.",
		      NULL);
	kill_player(op);
	if (pl->state != ST_PLAYING) return;
    }
    LOG(llevInfo,"LOGIN: Player named %s from ip %s\n", op->name, 
	op->contr->socket.host);

    /* Do this after checking for death - no reason sucking up bandwidth if
     * the data isn't needed.
     */
    esrv_new_player(op->contr,op->weight+op->carrying);
    esrv_send_inventory(op, op);
    esrv_add_spells(op->contr, NULL);
    esrv_send_pickup(pl);

    CLEAR_FLAG(op, FLAG_FRIENDLY);

    /* can_use_shield is a new flag.  However, the can_use.. seems to largely come
     * from the class, and not race.  I don't see any way to get the class information
     * to then update this.  I don't think this will actually break anything - anyone
     * that can use armour should be able to use a shield.  What this may 'break'
     * are features new characters get, eg, if someone starts up with a Q, they
     * should be able to use a shield.  However, old Q's won't get that advantage.
     */
    if (QUERY_FLAG(op, FLAG_USE_ARMOUR)) SET_FLAG(op, FLAG_USE_SHIELD);
    return;
}
