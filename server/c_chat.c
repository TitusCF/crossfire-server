/*
 * Object (handling) commands
 *	++Jam
 */

#include <global.h>
#include <loader.h>
#include <sproto.h>

int command_say (object *op, char *params)
{
  char buf[MAX_BUF];

  if (!params) return 0;
  sprintf(buf, "%s says: %s",op->name, params);
  new_info_map(NDI_WHITE,op->map, buf);
  communicate(op, params);
  
  return 0;
}

int command_last (object *op, char *params)
{
  new_draw_info(NDI_UNIQUE, 0,op, "Last not yet implemented.");
  return 0;
}
