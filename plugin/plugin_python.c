/*****************************************************************************/
/* CFPython - A Python module for Crossfire RPG.                             */
/*****************************************************************************/
/* The goal of this module is to provide support for Python scripts into     */
/* Crossfire. Guile support existed before, but it was put directly in the   */
/* code, a thing that prevented easy building of Crossfire on platforms that */
/* didn't have a Guile port. And Guile was seen as difficult to learn and was*/
/* also less popular than Python in the Crossfire Community.                 */
/* So, I finally decided to replace Guile with Python and made it a separate */
/* module since it is not a "critical part" of the code. Of course, it also  */
/* means that it will never be as fast as it could be, but it allows more    */
/* flexibility (and although it is not as fast as compiled-in code, it should*/
/* be fast enough for nearly everything on most today computers).            */
/*****************************************************************************/
/* Please note that it is still very beta - some of the functions may not    */
/* work as expected and could even cause the server to crash.                */
/*****************************************************************************/
/* Version history:                                                          */
/* 0.1 "Ophiuchus"   - Initial Alpha release                                 */
/* 0.5 "Stalingrad"  - Message length overflow corrected.                    */
/* 0.6 "Kharkov"     - Message and Write correctly redefined.                */
/* 0.7 "Koursk"      - Setting informations implemented.                     */
/*****************************************************************************/
/* Version: 0.6 Beta (also known as "Kharkov")                               */
/* Contact: yann.chachkoff@mailandnews.com                                   */
/*****************************************************************************/
/* That code is placed under the GNU General Public Licence (GPL)            */
/* (C)2001 by Chachkoff Yann (Feel free to deliver your complaints)          */
/*****************************************************************************/

/* First let's include the header file needed                                */

#include <plugin_python.h>
#include <stdarg.h>

#define PYTHON_DEBUG   /* give us some general infos out */

#undef MODULEAPI
#ifdef WIN32
#ifdef PYTHON_PLUGIN_EXPORTS
#define MODULEAPI __declspec(dllexport)
#else
#define MODULEAPI __declspec(dllimport)
#endif
#else
#define MODULEAPI
#endif

#define CHECK_MAP(mapptr) if((mapptr) == 0) { set_exception("NULL map given"); return NULL; }
#define CHECK_OBJ(objptr) if((objptr) == 0) { set_exception("NULL object given"); return NULL; }

/****************************************/
/* Utility functions                    */
/* Mostly added for Win32 compatibility */
/****************************************/

/* wrapper for free_object */
static void PyFreeObject( object* ob )
    {
    CFParm lCFR;
    lCFR.Value[ 0 ] = ob;
    PlugHooks[ HOOK_FREEOBJECT ]( &lCFR );
    }

/* wrapper for free */
static void PyFreeMemory( CFParm* CFR )
    {
    CFParm lCFR;
    lCFR.Value[ 0 ] = CFR;
    PlugHooks[ HOOK_FREEMEMORY ]( &lCFR );
    }

/* wrapper for fix_player */
static void PyFixPlayer( object* pl )
    {
    CFParm lCFR;
    lCFR.Value[ 0 ] = pl;
    PlugHooks[ HOOK_FIXPLAYER ]( &lCFR );
    }

/* Set up an Python exception object.
 */
static void set_exception(const char *fmt, ...)
{
    char buf[1024];
    va_list arg;

    va_start(arg, fmt);
    vsnprintf(buf, sizeof(buf), fmt, arg);
    va_end(arg);

    PyErr_SetString(PyExc_ValueError, buf);
}

/* Create an object. The parameter name may be an object name ("writing pen")
 * or an archetype name ("stylus"). An object name can have artifact suffixes
 * ("levitation boots of granite of mobility"). Returns NULL (and sets the
 * Python exception object) if the object does not exist.
 */
static object *create_object(char *name)
{
    CFParm lCFP;
    CFParm *CFR;
    object *ob;

    /* Try to create object by object name. */
    lCFP.Value[0] = name;
    CFR = PlugHooks[HOOK_GETARCHBYOBJNAME](&lCFP);
    ob = CFR->Value[0];
    PyFreeMemory(CFR);

    if(strncmp(query_name(ob), "singluarity", 11) == 0)
    {
        /* Object name failed, try archetype name. */

        PyFreeObject(ob);

        lCFP.Value[0] = name;
        CFR = PlugHooks[HOOK_GETARCHETYPE](&lCFP);
        ob = CFR->Value[0];
        PyFreeMemory(CFR);

        if(strncmp(query_name(ob), "singluarity", 11) == 0)
        {
            PyFreeObject(ob);
            set_exception("object '%s' does not exist", name);
            return NULL;
        }
    }
    else
    {
        char *obname;
        char *suffix;

        /* Object name found, try adding artifact suffixes. */

        obname = query_base_name(ob, 0);

        /* Sanity check: obname should be a prefix of name. */
        if(strncmp(name, obname, strlen(obname)) != 0)
        {
            PyFreeObject(ob);
            set_exception("object name '%s' is not a prefix of '%s'", obname, name);
            return NULL;
        }

        suffix = name+strlen(obname);
        while(*suffix != '\0')
        {
            char *tmpname;
            int i;

            tmpname = strdup(suffix);
            for(i = strlen(suffix); i > 0; i--)
            {
                int success;

                tmpname[i] = '\0';

                GCFP.Value[0] = ob;
                GCFP.Value[1] = tmpname;
                CFR = PlugHooks[HOOK_CREATEARTIFACT](&GCFP);
                success = *(int *)CFR->Value[0];
                PyFreeMemory(CFR);

                if(success)
                {
                    suffix += i;
                    break;
                }
            }
            free(tmpname);

            if(i <= 0)
            {
                PyFreeObject(ob);
                set_exception("artifact suffix '%s' for '%s' does not exist", suffix, obname);
                return NULL;
            }
        }
    }

    return ob;
}

/* Allocate and clear a stack entry. Returns 0 if no space left.
 */
static int allocate_stack(void)
{
    if (StackPosition >= MAX_RECURSIVE_CALL-1)
    {
        printf("PYTHON - Can't execute script - No space left of stack\n");
        return 0;
    }

    StackPosition++;
    StackActivator[StackPosition] = NULL;
    StackWho[StackPosition] = NULL;
    StackOther[StackPosition] = NULL;
    StackText[StackPosition] = NULL;
    StackParm1[StackPosition] = 0;
    StackParm2[StackPosition] = 0;
    StackParm3[StackPosition] = 0;
    StackParm4[StackPosition] = 0;
    StackReturn[StackPosition] = 0;

    return 1;
}

/*****************************************************************************/
/* And now the big part - The implementation of CFPython functions in C.     */
/* All comments for those functions have the following entries:              */
/* - The name of the function;                                               */
/* - How it is called from Python;                                           */
/* - The development state.                                                  */
/* The development state can be:                                             */
/* - Unknown  : Don't know if it has been tested already or not;             */
/* - Stable   : Has been tested and works under any common case;             */
/* - Untested : Not yet tested;                                              */
/* - Unstable : Has been tested, but caused some problems/bugged.            */
/* Such a system may seem quite silly and boring, but I already got some     */
/* success while using it, so I put it here too. Feel free to change the     */
/* status field of any function that you may have tested if needed.          */
/*****************************************************************************/
/* The functions that are simple wrappers to CF id numbers are not commented */
/* with that system since they don't need debugging because they're simple.  */
/*****************************************************************************/

/*****************************************************************************/
/* Wrappers for attack numbers.                                              */
/*****************************************************************************/
#ifndef FIRE_DIRECTIONAL
#define FIRE_DIRECTIONAL 1
#endif
static PyObject* CFAttackTypePhysical(PyObject* self, PyObject* args)
{
  int val = AT_PHYSICAL;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeMagic(PyObject* self, PyObject* args)
{
  int val = AT_MAGIC;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeFire(PyObject* self, PyObject* args)
{
  int val = AT_FIRE;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeElectricity(PyObject* self, PyObject* args)
{
  int val = AT_ELECTRICITY;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeCold(PyObject* self, PyObject* args)
{
  int val = AT_COLD;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeConfusion(PyObject* self, PyObject* args)
{
  int val = AT_CONFUSION;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeAcid(PyObject* self, PyObject* args)
{
  int val = AT_ACID;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeDrain(PyObject* self, PyObject* args)
{
  int val = AT_DRAIN;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeWeaponmagic(PyObject* self, PyObject* args)
{
  int val = AT_WEAPONMAGIC;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeGhosthit(PyObject* self, PyObject* args)
{
  int val = AT_GHOSTHIT;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypePoison(PyObject* self, PyObject* args)
{
  int val = AT_POISON;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeSlow(PyObject* self, PyObject* args)
{
  int val = AT_SLOW;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeParalyze(PyObject* self, PyObject* args)
{
  int val = AT_PARALYZE;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeTurnUndead(PyObject* self, PyObject* args)
{
  int val = AT_TURN_UNDEAD;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeFear(PyObject* self, PyObject* args)
{
  int val = AT_FEAR;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeCancellation(PyObject* self, PyObject* args)
{
  int val = AT_CANCELLATION;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeDepletion(PyObject* self, PyObject* args)
{
  int val = AT_DEPLETE;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeDeath(PyObject* self, PyObject* args)
{
  int val = AT_DEATH;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeChaos(PyObject* self, PyObject* args)
{
  int val = AT_CHAOS;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeCounterspell(PyObject* self, PyObject* args)
{
  int val = AT_COUNTERSPELL;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeGodpower(PyObject* self, PyObject* args)
{
  int val = AT_GODPOWER;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeHolyWord(PyObject* self, PyObject* args)
{
  int val = AT_HOLYWORD;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeBlind(PyObject* self, PyObject* args)
{
  int val = AT_BLIND;
  if (!PyArg_ParseTuple(args,"",NULL))
      return NULL;
  return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeLifeStealing(PyObject* self, PyObject* args)
{
    int val = AT_LIFE_STEALING;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};
static PyObject* CFAttackTypeDisease(PyObject* self, PyObject* args)
{
    int val = AT_DISEASE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
};

static PyObject* CFEventApply(PyObject* self, PyObject* args)
{
    int val = EVENT_APPLY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventAttack(PyObject* self, PyObject* args)
{
    int val = EVENT_ATTACK;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventDeath(PyObject* self, PyObject* args)
{
    int val = EVENT_DEATH;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventDrop(PyObject* self, PyObject* args)
{
    int val = EVENT_DROP;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventPickup(PyObject* self, PyObject* args)
{
    int val = EVENT_PICKUP;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventSay(PyObject* self, PyObject* args)
{
    int val = EVENT_SAY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventStop(PyObject* self, PyObject* args)
{
    int val = EVENT_STOP;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventTime(PyObject* self, PyObject* args)
{
    int val = EVENT_TIME;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventThrow(PyObject* self, PyObject* args)
{
    int val = EVENT_THROW;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventTrigger(PyObject* self, PyObject* args)
{
    int val = EVENT_TRIGGER;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventClose(PyObject* self, PyObject* args)
{
    int val = EVENT_CLOSE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

static PyObject* CFEventTimer(PyObject* self, PyObject* args)
{
    int val = EVENT_TIMER;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",val);
}

/*****************************************************************************/
/* Stalingrad: XML Support Subsection starts here                            */
/*****************************************************************************/

/*****************************************************************************/
/* Name   : CFLoadXMLObject                                                  */
/* Python : LoadXMLObject(filename)                                          */
/* Status : Untested                                                         */
/*****************************************************************************/
/* Loads a crossfire XML-file into an object, including subobjects (if any). */
/* Note that I may have broken some XML rules (I hope I didn't, but...).     */
/*****************************************************************************/

/*****************************************************************************/
/* Name   : CFSaveXMLObject                                                  */
/* Python : SaveXMLObject(filename, object)                                  */
/* Status : Untested                                                         */
/*****************************************************************************/
/* Saves a crossfire object (subobjects included) into a file, using an XML  */
/* format. (At least I think it is mostly XML-compliant :)                   */
/*****************************************************************************/

/*****************************************************************************/
/* Stalingrad: XML Support Subsection ends here                              */
/*****************************************************************************/

/*****************************************************************************/
/* Name   : CFGetMapWidth                                                    */
/* Python : CFPython.GetMapWidth(map)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetMapWidth(PyObject* self, PyObject* args)
{
    int val;
    long map;
    if (!PyArg_ParseTuple(args,"l",&map))
        return NULL;
    CHECK_MAP(map);
    val = ((mapstruct *)(map))->width;
    return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Name   : CFGetMapHeight                                                   */
/* Python : CFPython.GetMapHeight(map)                                       */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetMapHeight(PyObject* self, PyObject* args)
{
    int val;
    long map;
    if (!PyArg_ParseTuple(args,"l",&map))
        return NULL;
    CHECK_MAP(map);
    val = ((mapstruct *)(map))->height;
    return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Name   : CFGetObjectAt                                                    */
/* Python : CFPython.GetObjectAt(map,x,y)                                    */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetObjectAt(PyObject* self, PyObject* args)
{
    int x, y;
    long map;
    long whoptr;

    if (!PyArg_ParseTuple(args,"lii",&map,&x,&y))
        return NULL;

    CHECK_MAP(map);

    whoptr = (long)(get_map_ob((mapstruct *)(map),x,y));
    return Py_BuildValue("l",whoptr);
};

/*****************************************************************************/
/* Name   : CFSetValue                                                       */
/* Python : CFPython.SetValue(object,value)                                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetValue(PyObject* self, PyObject* args)
{
    long whoptr;
    int newvalue;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&newvalue))
        return NULL;

    CHECK_OBJ(whoptr);
    if (newvalue < 0) {
        set_exception("value must not be negative");
        return NULL;
    }

    WHO->value = newvalue;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetValue                                                       */
/* Python : CFPython.GetValue(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetValue(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->value);
};

/*****************************************************************************/
/* Name   : CFSetSkillExperience                                             */
/* Python : CFPython.SetSkillExperience(object,skill,value)                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetSkillExperience(PyObject* self, PyObject* args)
{
    object *tmp;

    long whoptr;
    char *skill;
    int value2;
    sint64 value;
    sint64 currentxp;

    if (!PyArg_ParseTuple(args,"lsL",&whoptr,&skill,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < 0) {
        set_exception("experience value must not be negative");
        return NULL;
    }

    /* Browse the inventory of object to find a matching skill. */
    for (tmp=WHO->inv;tmp;tmp=tmp->below)
    {
        if(tmp->type==SKILL && strcmp(tmp->skill, skill) == 0) {
            currentxp = tmp->stats.exp;
            GCFP.Value[0] = (void *)(WHO);
            value = value - currentxp;
            GCFP.Value[1] = (void *)(&value);
            GCFP.Value[2] = (void *)(skill);
            value2 = SK_EXP_ADD_SKILL;
            GCFP.Value[3] = (void *)(&value2);
            (PlugHooks[HOOK_ADDEXP])(&GCFP);

            Py_INCREF(Py_None);
            return Py_None;
        };
    };

    set_exception("%s does not know the skill %s", query_name(WHO), skill);
    return NULL;
};

/*****************************************************************************/
/* Name   : CFGetSkillExperience                                             */
/* Python : CFPython.GetSkillExperience(object, skill)                       */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetSkillExperience(PyObject* self, PyObject* args)
{
    object *tmp;
    char *skill;
    long whoptr;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&skill))
        return NULL;

    CHECK_OBJ(whoptr);

    /* Browse the inventory of object to find a matching skill. */
    for (tmp=WHO->inv;tmp;tmp=tmp->below)
    {
        if(tmp->type==SKILL && strcmp(tmp->skill, skill) == 0) {
            return Py_BuildValue("L",(sint64)(tmp->stats.exp));
        }
    }

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFMatchString                                                    */
/* Python : CFPython.MatchString(firststr,secondstr)                         */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFMatchString(PyObject* self, PyObject* args)
{
    char *premiere;
    char *seconde;
    char *result;

    if (!PyArg_ParseTuple(args,"ss",&premiere,&seconde))
        return NULL;

    result = re_cmp(premiere, seconde);
    if (result != NULL)
    {
        return Py_BuildValue("i",1);
    } else
    {
        return Py_BuildValue("i",0);
    };
};

/*****************************************************************************/
/* Name   : CFSetCursed                                                      */
/* Python : CFPython.SetCursed(object,value)                                 */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetCursed(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);

  if (value!=0)
  {
        SET_FLAG(WHO, FLAG_CURSED);
  }
  else
  {
        CLEAR_FLAG(WHO, FLAG_CURSED);
  };
  Py_INCREF(Py_None);
  return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetDamned                                                      */
/* Python : CFPython.SetDamned(object,value)                                 */
/*****************************************************************************/
static PyObject* CFSetDamned(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);

    if (value!=0)
    {
        SET_FLAG(WHO, FLAG_DAMNED);
    }
    else
    {
        CLEAR_FLAG(WHO, FLAG_DAMNED);
    }
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFActivateRune                                                   */
/* Python : CFPython.ActivateRune(object_who,object_what)                    */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFActivateRune(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;

    if (!PyArg_ParseTuple(args,"ll",&whoptr,&whatptr))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_OBJ(whatptr);

    GCFP.Value[0] = (void *)(WHAT);
    GCFP.Value[1] = (void *)(WHO);
    (PlugHooks[HOOK_SPRINGTRAP])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFCheckTrigger                                                   */
/* Python : CFPython.CheckTrigger(object,objectwhat)                         */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCheckTrigger(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;

    if (!PyArg_ParseTuple(args,"ll",&whoptr,&whatptr))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_OBJ(whatptr);

    check_trigger(WHAT,WHO);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetUnaggressive                                                */
/* Python : CFPython.SetUnaggressive(who,value)                              */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetUnaggressive(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);

    if (value!=0)
    {
        SET_FLAG(WHO, FLAG_UNAGGRESSIVE);
    }
    else
    {
        CLEAR_FLAG(WHO, FLAG_UNAGGRESSIVE);
    };
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFCastAbility                                                    */
/* Python : CFPython.CastAbility(object,spell,direction,option)              */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCastAbility(PyObject* self, PyObject* args)
{
    long whoptr,casterptr;
    char *spell;
    object *spell_ob;
    int dir;
    char* op;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"llsis",&whoptr,&casterptr,&spell,&dir,&op))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_OBJ(casterptr);

    GCFP.Value[0] = spell;
    CFR = (PlugHooks[HOOK_GETARCHETYPE])(&GCFP);
    spell_ob = CFR->Value[0];
    PyFreeMemory( CFR );

    if (strncmp(query_name(spell_ob), "singluarity", 11) == 0)
    {
        /* spell object does not exist */
        PyFreeObject(spell_ob);
        set_exception("illegal spell name %s (unknown object)", spell);
        return NULL;
    }
    if (spell_ob->type != SPELL)
    {
        /* not a spell */
        PyFreeObject(spell_ob);
        set_exception("illegal spell name %s (not a spell object)", spell);
        return NULL;
    }

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(casterptr);
    GCFP.Value[2] = (void *)(&dir);
    GCFP.Value[3] = (void *)(spell_ob);
    GCFP.Value[4] = (void *)(op);
    CFR = (PlugHooks[HOOK_CASTSPELL])(&GCFP);
    PyFreeMemory( CFR );

    PyFreeObject(spell_ob);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetMapPath                                                     */
/* Python : CFPython.GetMapPath(objectmap)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetMapPath(PyObject* self, PyObject* args)
{
    long where;
    if (!PyArg_ParseTuple(args,"l",&where))
        return NULL;

    CHECK_MAP(where);

    return Py_BuildValue("s",((mapstruct *)(where))->path);
};

/*****************************************************************************/
/* Name   : CFGetMapObject                                                   */
/* Python : CFPython.GetMapObject()                                          */
/* Status : KIA                                                              */
/*****************************************************************************/
/* Remark : This function is deprecated and should not be used anymore.      */
/*****************************************************************************/
static PyObject* CFGetMapObject(PyObject* self, PyObject* args)
{
    PyErr_SetString(PyExc_NotImplementedError, "GetMapObject is not implemented");
    return NULL; /* Deprecated */
};


/*****************************************************************************/
/* Name   : CFGetMessage                                                     */
/* Python : CFPython.GetMessage(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetMessage(PyObject* self, PyObject* args)
{
    const char *msg;
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    msg = WHO->msg;
    if (msg == NULL)
        msg = "";
    return Py_BuildValue("s", msg);
};

/*****************************************************************************/
/* Name   : CFSetMessage                                                     */
/* Python : CFPython.SetMessage(object,message)                              */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetMessage(PyObject* self, PyObject* args)
{
    char *txt;
    long whoptr;
    if (!PyArg_ParseTuple(args,"ls",&whoptr, &txt))
        return NULL;

    CHECK_OBJ(whoptr);

    if (WHO->msg != NULL)
        free_string(WHO->msg);
    WHO->msg = add_string(txt);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetGod                                                         */
/* Python : CFPython.GetGod(object)                                          */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetGod(PyObject* self, PyObject* args)
{
    long whoptr;
    CFParm* CFR;
    char* value;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    CFR = (PlugHooks[HOOK_DETERMINEGOD])(&GCFP);
    value = (char *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    if (strcmp(value, "none") == 0)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("s",value);
};

/*****************************************************************************/
/* Name   : CFSetGod                                                         */
/* Python : CFPython.SetGod(object,godstr)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetGod(PyObject* self, PyObject* args)
{
    long whoptr;
    char* txt;
    char* prayname;
    object* tmp;
    CFParm* CFR0;
    CFParm* CFR;
    int value;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&txt))
        return NULL;

    CHECK_OBJ(whoptr);

    prayname = add_string("praying");

    GCFP1.Value[0] = (void *)(WHO);
    GCFP1.Value[1] = (void *)(prayname);

    GCFP2.Value[0] = (void *)(WHO);

    GCFP0.Value[0] = (char *)(txt);
    CFR0 = (PlugHooks[HOOK_FINDGOD])(&GCFP0);
    tmp = (object *)(CFR0->Value[0]);
    PyFreeMemory( CFR0 );

    if (tmp == NULL)
    {
        set_exception("illegal god name %s", txt);
        return NULL;
    }

    GCFP2.Value[1] = (void *)(tmp);

    CFR = (PlugHooks[HOOK_CMDRSKILL])(&GCFP1);
    value = *(int *)(CFR->Value[0]);
    if (value)
        (PlugHooks[HOOK_BECOMEFOLLOWER])(&GCFP2);
    PyFreeMemory( CFR );

    free_string(prayname);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetWeight                                                      */
/* Python : CFPython.SetWeight(object,value)                                 */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetWeight(PyObject* self, PyObject* args)
{
    long whoptr;
    long value;

    if (!PyArg_ParseTuple(args,"ll",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    /* I used an arbitrary bound of 1000000000 here */
    if (value > 1000000000)
    {
        set_exception("weight must not exceed 1000000000");
        return NULL;
    }
    else if (value < 0)
    {
        set_exception("weight must not be negative");
        return NULL;
    };
    WHO->weight = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFReadyMap                                                       */
/* Python : CFPython.ReadyMap(name)                                          */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFReadyMap(PyObject* self, PyObject* args)
{
    char *mapname;
    mapstruct *mymap;
    int val;
    CFParm* CFR;
    if (!PyArg_ParseTuple(args,"s",&mapname))
        return NULL;

    val = 0;
    GCFP.Value[0] = (void *)(mapname);
    GCFP.Value[1] = (void *)(&val);
    CFR = (PlugHooks[HOOK_READYMAPNAME])(&GCFP);
    mymap = (mapstruct *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("l",(long)(mymap));
};

/*****************************************************************************/
/* Name   : CFTeleport                                                       */
/* Python : CFPython.Teleport(object,mapptr,x,y)                             */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFTeleport(PyObject* self, PyObject* args)
{
    long whoptr;
    long where;
    int x, y;
    int val;

    if (!PyArg_ParseTuple(args,"llii",&whoptr,&where,&x,&y))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_MAP(where);

    if ((out_of_map((mapstruct*)(where),x,y))==0)
    {
        int k;
        object *tmp;
        k = find_first_free_spot(WHO->arch,(mapstruct*)(where),x,y);
        if (k==-1)
        {
            set_exception("no free spot found");
            return NULL;
        }

        GCFP.Value[0] = (void *)(WHO);
        (PlugHooks[HOOK_REMOVEOBJECT])(&GCFP);

        for(tmp=WHO;tmp!=NULL;tmp=tmp->more)
            tmp->x=x+freearr_x[k]+(tmp->arch==NULL?0:tmp->arch->clone.x),
            tmp->y=y+freearr_y[k]+(tmp->arch==NULL?0:tmp->arch->clone.y);

        val = 0;
        GCFP.Value[0] = (void *)(WHO);
        GCFP.Value[1] = (void *)((mapstruct *)(where));
        GCFP.Value[2] = NULL;
        GCFP.Value[3] = (void *)(&val);
        PyFreeMemory( (PlugHooks[HOOK_INSERTOBJECTINMAP])(&GCFP) );
    };

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFOutOfMap                                                       */
/* Python : CFPython.IsOutOfMap(object,x,y)                                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsOutOfMap(PyObject* self, PyObject* args)
{
    long whoptr;
    int x, y;

    if (!PyArg_ParseTuple(args,"lii",&whoptr,&x,&y))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i", out_of_map(WHO->map,x,y));
};

/*****************************************************************************/
/* Name   : CFPickUp                                                         */
/* Python : CFPython.Pickup(object,whatob)                                   */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFPickUp(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;

    if (!PyArg_ParseTuple(args,"ll",&whoptr,&whatptr))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_OBJ(whatptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(WHAT);
    (PlugHooks[HOOK_PICKUP])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetWeight                                                      */
/* Python : CFPython.GetWeight(object)                                       */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetWeight(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("l",WHO->weight);
};


/*****************************************************************************/
/* Name   : CFIsCanBePicked                                                  */
/* Python : CFPython.CanBePicked(object)                                     */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsCanBePicked(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",!QUERY_FLAG(WHO,FLAG_NO_PICK));
};

/*****************************************************************************/
/* Name   : CFGetMap                                                         */
/* Python : CFPython.GetMap(object)                                          */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetMap(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("l",(long)(WHO->map));
};

/*****************************************************************************/
/* Name   : CFGetNextObject                                                  */
/* Python : CFPython.GetNextObject(object)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetNextObject(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("l",(long)(WHO->below));
};

/*****************************************************************************/
/* Name   : CFGetPreviousObject                                              */
/* Python : CFPython.GetPreviousObject(object)                               */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetPreviousObject(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("l",(long)(WHO->above));
};

/*****************************************************************************/
/* Name   : CFGetFirstObjectOnSquare                                         */
/* Python : CFPython.GetFirstObjectOnSquare(map,x,y)                         */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetFirstObjectOnSquare(PyObject* self, PyObject* args)
{
    long map;
    int x, y;
    object* val;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"lii",&map,&x,&y))
        return NULL;

    CHECK_MAP(map);

    GCFP.Value[0] = (mapstruct *)(map);
    GCFP.Value[1] = (void *)(&x);
    GCFP.Value[2] = (void *)(&y);
    CFR = (PlugHooks[HOOK_GETMAPOBJECT])(&GCFP);
    val = (object *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("l",(long)(val));
};

/*****************************************************************************/
/* Name   : CFSetQuantity                                                    */
/* Python : CFPython.SetQuantity(object,nrof)                                */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetQuantity(PyObject* self, PyObject* args)
{
    long whatptr;
    uint32 value;
    int val = UP_OBJ_CHANGE;

    if (!PyArg_ParseTuple(args,"li",&whatptr,&value))
        return NULL;

    CHECK_OBJ(whatptr);
    if (value < 0)
    {
        set_exception("value must not be negative");
        return NULL;
    };
    WHAT->nrof = value;

    GCFP.Value[0] = (void *)(WHAT);
    GCFP.Value[1] = (void *)(&val);
    (PlugHooks[HOOK_UPDATEOBJECT])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetQuantity                                                    */
/* Python : CFPython.GetQuantity(object)                                     */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetQuantity(PyObject* self, PyObject* args)
{
    long whatptr;

    if (!PyArg_ParseTuple(args,"l",&whatptr))
        return NULL;

    CHECK_OBJ(whatptr);

    return Py_BuildValue("l",WHAT->nrof);
};

/*****************************************************************************/
/* Name   : CFInsertObjectInside                                             */
/* Python : CFPython.InsertObjectInside(object,environment)                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFInsertObjectInside(PyObject* self, PyObject* args)
{
    long whatptr;
    long whereptr;
    object *myob;

    if (!PyArg_ParseTuple(args,"ll",&whatptr,&whereptr))
        return NULL;

    CHECK_OBJ(whatptr);
    CHECK_OBJ(whereptr);

    myob = WHAT;
    if (!QUERY_FLAG(myob,FLAG_REMOVED))
    {
        GCFP.Value[0] = (void *)(myob);
        (PlugHooks[HOOK_REMOVEOBJECT])(&GCFP);
    }
    myob = insert_ob_in_ob(myob, WHERE);
    if (WHERE->type == PLAYER)
    {
        GCFP.Value[0] = (void *)(WHERE);
        GCFP.Value[1] = (void *)(myob);
        (PlugHooks[HOOK_ESRVSENDITEM])(&GCFP);
    };
    Py_INCREF(Py_None);
    return Py_None;

};

/*****************************************************************************/
/* Name   : CFFindPlayer                                                     */
/* Python : CFPlayer.FindPlayer(name)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFFindPlayer(PyObject* self, PyObject* args)
{
    player *foundpl;
    object *foundob;
    CFParm *CFR;
    char* txt;

    if (!PyArg_ParseTuple(args,"s",&txt))
        return NULL;

    GCFP.Value[0] = (void *)(txt);
    CFR = (PlugHooks[HOOK_FINDPLAYER])(&GCFP);
    foundpl = (player *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    if (foundpl!=NULL)
        foundob = foundpl->ob;
    else
        foundob = NULL;
    return Py_BuildValue("l",(long)(foundob));
};

/*****************************************************************************/
/* Name   : CFApply                                                          */
/* Python : CFPython.Apply(object, whatobj, flags)                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFApply(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;
    int flags;
    CFParm* CFR;
    int retval;

    if (!PyArg_ParseTuple(args,"lli",&whoptr,&whatptr,&flags))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_OBJ(whatptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(WHAT);
    GCFP.Value[2] = (void *)(&flags);
    CFR = (PlugHooks[HOOK_MANUALAPPLY])(&GCFP);
    retval = *(int *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("i",retval);
};

/*****************************************************************************/
/* Name   : CFDrop                                                           */
/* Python : CFPython.Drop(object, name)                                      */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFDrop(PyObject* self, PyObject* args)
{
    long whoptr;
    char* name;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&name))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(name);
    CFR = (PlugHooks[HOOK_CMDDROP])(&GCFP);
    PyFreeMemory( CFR );

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFTake                                                           */
/* Python : CFPython.Take(object,name)                                       */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFTake(PyObject* self, PyObject* args)
{
    long whoptr;
    char* name;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&name))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(name);
    CFR = (PlugHooks[HOOK_CMDTAKE])(&GCFP);
    PyFreeMemory( CFR );

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFIsInvisible                                                    */
/* Python : CFPython.IsInvisible(object)                                     */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFIsInvisible(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->invisible);
};

/*****************************************************************************/
/* Name   : CFWhoAmI                                                         */
/* Python : CFPython.WhoAmI()                                                */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFWhoAmI(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("l",(long)(StackWho[StackPosition]));
};

/*****************************************************************************/
/* Name   : CFWhoIsActivator                                                 */
/* Python : CFPython.WhoIsActivator()                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFWhoIsActivator(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("l",(long)(StackActivator[StackPosition]));
};

/*****************************************************************************/
/* Name   : CFWhatIsMessage                                                  */
/* Python : CFPython.WhatIsMessage()                                         */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFWhatIsMessage(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;

    if (!StackText[StackPosition])
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("s",StackText[StackPosition]);
};

/*****************************************************************************/
/* Name   : CFSay                                                            */
/* Python : CFPython.Say(object,message)                                     */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSay(PyObject* self, PyObject* args)
{
    long whoptr;
    char *message;
    char *buf;
    int val;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&message))
        return NULL;

    CHECK_OBJ(whoptr);

    buf = (char *)(malloc(sizeof(char)*(strlen(message)+strlen(query_name(WHO))+20)));
    sprintf(buf, "%s says: %s", query_name(WHO),message);
    val = NDI_NAVY|NDI_UNIQUE;
    GCFP.Value[0] = (void *)(&val);
    GCFP.Value[1] = (void *)(WHO->map);
    GCFP.Value[2] = (void *)(buf);
    (PlugHooks[HOOK_NEWINFOMAP])(&GCFP);

    free(buf);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetInvisible                                                   */
/* Python : CFPython.SetInvisible(object,value)                              */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetInvisible(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);

    WHO->invisible = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetExperience                                                  */
/* Python : CFPython.GetExperience(object)                                   */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetExperience(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("l",WHO->stats.exp);
};

/*****************************************************************************/
/* Name   : CFGetSpeed                                                       */
/* Python : CFPython.GetSpeed(object)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetSpeed(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("d",WHO->speed);
};

/*****************************************************************************/
/* Name   : CFSetSpeed                                                       */
/* Python : CFPython.SetSpeed(object,value)                                  */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetSpeed(PyObject* self, PyObject* args)
{
    long whoptr;
    double value;

    if (!PyArg_ParseTuple(args,"ld",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value< -9.99 || value> 9.99)
    {
        set_exception("value must be between -9.99 and 9.99");
        return NULL;
    }

    WHO->speed = (float) value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetFood                                                        */
/* Python : CFPython.GetFood(object)                                         */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetFood(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.food);
};

/*****************************************************************************/
/* Name   : CFSetFood                                                        */
/* Python : CFPython.SetFood(object, value)                                  */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetFood(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < 0 || value > 999)
    {
        set_exception("value must be between 0 and 999");
        return NULL;
    }

    WHO->stats.food = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetGrace                                                       */
/* Python : CFPython.GetGrace(object)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetGrace(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.grace);
};

/*****************************************************************************/
/* Name   : CFSetGrace                                                       */
/* Python : CFPython.SetGrace(object, value)                                 */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetGrace(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < -32000 || value > 32000)
    {
        set_exception("value must be between -32000 and 32000");
        return NULL;
    }

    WHO->stats.grace = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetReturnValue                                                 */
/* Python : CFPython.GetReturnValue()                                        */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetReturnValue(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;

    return Py_BuildValue("i",StackReturn[StackPosition]);
};

/*****************************************************************************/
/* Name   : CFSetReturnValue                                                 */
/* Python : CFPython.SetReturnValue(value)                                   */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetReturnValue(PyObject* self, PyObject* args)
{
    int value;
    if (!PyArg_ParseTuple(args,"i",&value))
        return NULL;

    StackReturn[StackPosition] = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetDirection                                                   */
/* Python : CFPython.GetDirection(object)                                    */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetDirection(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->direction);
};

/*****************************************************************************/
/* Name   : CFSetDirection                                                   */
/* Python : CFPython.SetDirection(object, value)                             */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetDirection(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);

    WHO->direction = value;
    SET_ANIMATION(WHO, WHO->direction);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetLastSP                                                      */
/* Python : CFPython.GetLastSP(object)                                       */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetLastSP(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->last_sp);
};

/*****************************************************************************/
/* Name   : CFSetLastSP                                                      */
/* Python : CFPython.SetLastSP(object, value)                                */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetLastSP(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < 0 || value > 32000)
    {
        set_exception("value must be between 0 and 32000");
        return NULL;
    }

    WHO->last_sp = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetLastGrace                                                   */
/* Python : CFPython.GetLastGrace(object)                                    */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetLastGrace(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->last_grace);
};

/*****************************************************************************/
/* Name   : CFSetLastGrace                                                   */
/* Python : CFPython.SetLastGrace(object,value)                              */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetLastGrace(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < 0 || value > 32000)
    {
        set_exception("value must be between 0 and 32000");
        return NULL;
    }

    WHO->last_grace = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFFixObject                                                      */
/* Python : CFPython.FixObject(object)                                       */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFFixObject(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    PyFixPlayer(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetFace                                                        */
/* Python : CFPython.SetFace(object,string)                                  */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetFace(PyObject* self, PyObject* args)
{
    char* txt;
    long whoptr;
    CFParm* CFR;
    int val = UP_OBJ_FACE;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&txt))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(txt);
    CFR = (PlugHooks[HOOK_FINDANIMATION])(&GCFP);
    WHO->animation_id = *(int *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(&val);
    (PlugHooks[HOOK_UPDATEOBJECT])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetAttackType                                                  */
/* Python : CFPython.GetAttackType(object)                                   */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetAttackType(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->attacktype);
};

/*****************************************************************************/
/* Name   : CFSetAttackType                                                  */
/* Python : CFPython.SetAttackType(object,value)                             */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetAttackType(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);

    WHO->attacktype = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetDamage                                                      */
/* Python : CFPython.SetDamage(object,value)                                 */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetDamage(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < 0 || value > 120)
    {
        set_exception("value must be between 0 and 120");
        return NULL;
    }

    WHO->stats.dam = value;

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetDamage                                                      */
/* Python : CFPython.GetDamage(object)                                       */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetDamage(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.dam);
};

/*****************************************************************************/
/* Name   : CFSetBeenApplied                                                 */
/* Python : CFPython.SetBeenApplied(object,value)                            */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetBeenApplied(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);

    if (value!=0)
        SET_FLAG(WHO,FLAG_BEEN_APPLIED);
    else
        CLEAR_FLAG(WHO,FLAG_BEEN_APPLIED);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetIdentified                                                  */
/* Python : CFPython.SetIdentified(object,value)                             */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetIdentified(PyObject* self, PyObject* args)
{
    long whoptr;
    int value;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);

    if (value!=0)
        SET_FLAG(WHO,FLAG_IDENTIFIED);
    else
        CLEAR_FLAG(WHO,FLAG_IDENTIFIED);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFKillObject                                                     */
/* Python : CFPython.KillObject(object,what_object,killtype)                 */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFKillObject(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;
    int ktype;
    int k = 1;
    event *evt;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"lli",&whoptr,&whatptr,&ktype))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_OBJ(whatptr);

    WHAT->speed = 0;
    WHAT->speed_left = 0.0;
    update_ob_speed(WHAT);

    if(QUERY_FLAG(WHAT,FLAG_REMOVED))
    {
        set_exception("trying to remove removed object");
        return NULL;
    }

    WHAT->stats.hp = -1;
    GCFP.Value[0] = (void *)(WHAT);
    GCFP.Value[1] = (void *)(&k);
    GCFP.Value[2] = (void *)(WHO);
    GCFP.Value[3] = (void *)(&ktype);
    CFR = (PlugHooks[HOOK_KILLOBJECT])(&GCFP);
    PyFreeMemory( CFR );

    /* WHAT->script_str_death = NULL; */
    /* WHAT->script_death = NULL; */
/*
    WHAT->event_hook[EVENT_DEATH] = NULL;
    WHAT->event_plugin[EVENT_DEATH] = NULL;
    WHAT->event_options[EVENT_DEATH] = NULL;
*/
    if ((evt = find_event(WHAT, EVENT_DEATH))!=NULL)
    {
        evt->hook = NULL;
        evt->plugin = NULL;
        evt->options = NULL;
    }
   /* This is to avoid the attack routine to continue after we called
    * killObject, since the attacked object no longer exists.
    * By fixing guile_current_other to NULL, guile_use_weapon_script will
    * return -1, meaning the attack function must be immediately terminated.
    */
    if (WHAT==StackOther[StackPosition])
    {
        StackOther[StackPosition] = NULL;
    };
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFWhoIsOther                                                     */
/* Python : CFPython.WhoIsOther()                                            */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFWhoIsOther(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("l",(long)(StackOther[StackPosition]));
};

/*****************************************************************************/
/* Name   : CFDirectionN                                                     */
/* Python : CFPython.DirectionN()                                            */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFDirectionN(PyObject* self, PyObject* args)
{
    int i=1;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   : CFDirectionNE                                                    */
/* Python : CFPython.DirectionNE()                                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFDirectionNE(PyObject* self, PyObject* args)
{
    int i=2;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   : CFDirectionE                                                     */
/* Python : CFPython.DirectionE()                                            */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFDirectionE(PyObject* self, PyObject* args)
{
    int i=3;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   : CFDirectionSE                                                    */
/* Python : CFPython.DirectionSE()                                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFDirectionSE(PyObject* self, PyObject* args)
{
    int i=4;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   : CFDirectionS                                                     */
/* Python : CFPython.DirectionS()                                            */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFDirectionS(PyObject* self, PyObject* args)
{
    int i=5;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   : CFDirectionSW                                                    */
/* Python : CFPython.DirectionSW()                                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFDirectionSW(PyObject* self, PyObject* args)
{
    int i=6;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   : CFDirectionW                                                     */
/* Python : CFPython.DirectionW()                                            */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFDirectionW(PyObject* self, PyObject* args)
{
    int i=7;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   : CFDirectionNW                                                    */
/* Python : CFPython.DirectionNW()                                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFDirectionNW(PyObject* self, PyObject* args)
{
    int i=8;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",i);
};

/*****************************************************************************/
/* Name   : CFCastSpell                                                      */
/* Python : CFPython.CastSpell(object,spell_object,direction,string)         */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCastSpell(PyObject* self, PyObject* args)
{
    long whoptr, spellptr;
    int dir;
    char* op;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"llis",&whoptr,&spellptr,&dir,&op))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_OBJ(spellptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(WHO);
    GCFP.Value[2] = (void *)(&dir);
    GCFP.Value[3] = (void *)((object*)spellptr);
    GCFP.Value[4] = (void *)(op);
    CFR = (PlugHooks[HOOK_CASTSPELL])(&GCFP);
    PyFreeMemory( CFR );

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFForgetSpell                                                    */
/* Python : CFPython.ForgetSpell(object,spell)                               */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFForgetSpell(PyObject* self, PyObject* args)
{
    long whoptr;
    char *spell;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&spell))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(spell);
    (PlugHooks[HOOK_FORGETSPELL])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFAcquireSpell                                                   */
/* Python : CFPython.AcquireSpell(object,spell_object)                       */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFAcquireSpell(PyObject* self, PyObject* args)
{
    long whoptr, spellptr;
    int i = 0;

    if (!PyArg_ParseTuple(args,"ll",&whoptr,&spellptr))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_OBJ(spellptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(spellptr);
    GCFP.Value[2] = (void *)(&i);
    (PlugHooks[HOOK_LEARNSPELL])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFDoKnowSpell                                                    */
/* Python : CFPython.DoKnowSpell(object,string)                              */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFDoKnowSpell(PyObject* self, PyObject* args)
{
    char *spell;
    long whoptr;
    CFParm* CFR;
    object *ob;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&spell))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(spell);
    CFR = (PlugHooks[HOOK_CHECKFORSPELL])(&GCFP);
    ob = (object *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("l",(long)ob);
};

/*****************************************************************************/
/* Name   : CFCheckInvisibleObjectInside                                     */
/* Python : CFPython.CheckInvisibleObjectInside(object,string)               */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCheckInvisibleObjectInside(PyObject* self, PyObject* args)
{
    int whoptr;
    char *id;
    object* tmp2;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&id))
        return NULL;

    CHECK_OBJ(whoptr);

    for(tmp2=WHO->inv;tmp2 !=NULL; tmp2=tmp2->below)
    {
        if(tmp2->type == FORCE && tmp2->slaying && !strcmp(tmp2->slaying,id))
            break;
    };

    return Py_BuildValue("l",(long)(tmp2));
};

/*****************************************************************************/
/* Name   : CFCreateInvisibleObjectInside                                    */
/* Python : CFPython.CreateInvisibleObjectInside(object,string)              */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCreateInvisibleObjectInside(PyObject* self, PyObject* args)
{
    long whereptr;
    char* txt;
    char txt2[6];
    object *myob;
    object *where;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"ls",&whereptr,&txt))
        return NULL;

    CHECK_OBJ(whereptr);

    where = (object *)(whereptr);

    strcpy(txt2,"force");

    GCFP.Value[0] = (void *)(txt2);
    CFR = (PlugHooks[HOOK_GETARCHETYPE])(&GCFP);
    myob = (object *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    if(strncmp(query_name(myob), "singluarity", 11) == 0)
    {
        PyFreeObject(myob);
        set_exception("can't find archetype 'force'");
        return NULL;
    }
    myob->speed = 0.0;
    GCFP.Value[0] = (void *)(myob);
    (PlugHooks[HOOK_UPDATESPEED])(&GCFP);

    myob->slaying = add_string(txt);
    myob = insert_ob_in_ob(myob, where);

    GCFP.Value[0] = (void *)(where);
    GCFP.Value[1] = (void *)(myob);
    (PlugHooks[HOOK_ESRVSENDITEM])(&GCFP);

    return Py_BuildValue("l",(long)(myob));
};

/*****************************************************************************/
/* Name   : CFCreateObjectInside                                             */
/* Python : CFPython.CreateObjectInside(string,object)                       */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCreateObjectInside(PyObject* self, PyObject* args)
{
    object *myob;
    object *where;
    long whereptr;
    char *txt;

    if (!PyArg_ParseTuple(args,"sl",&txt, &whereptr))
        return NULL;

    CHECK_OBJ(whereptr);

    where = (object *)(whereptr);

    myob = create_object(txt);
    if (myob == NULL)
        return NULL;

    myob = insert_ob_in_ob(myob, where);
    if (where->type == PLAYER)
    {
        GCFP.Value[0] = (void *)(where);
        GCFP.Value[1] = (void *)(myob);
        (PlugHooks[HOOK_ESRVSENDITEM])(&GCFP);
    };
    return Py_BuildValue("l",(long)(myob));
};

/*****************************************************************************/
/* Name   : CFCheckMap                                                       */
/* Python : CFPython.CheckMap(what_string,map_string,pos)                    */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCheckMap(PyObject* self, PyObject* args)
{
    char *what;
    long mapptr;
    int x, y;
    object* foundob;
    mapstruct* map;
    sint16 nx, ny;
    int mflags;

    if (!PyArg_ParseTuple(args,"sl(ii)",&what,&mapptr,&x,&y))
        return NULL;

    CHECK_MAP(mapptr);

    map = (mapstruct *)mapptr;

    /* make sure the map is swapped in */
    if (map->in_memory != MAP_IN_MEMORY)
    {
        CFParm* CFR;
        int val;

        val = 0;
        GCFP.Value[0] = map->name;
        GCFP.Value[1] = &val;
        CFR = PlugHooks[HOOK_READYMAPNAME](&GCFP);
        map = CFR->Value[0];
        PyFreeMemory(CFR);

	if (map == NULL)
	    return Py_BuildValue("l",(long)0);
    }

    mflags = get_map_flags(map, &map, x, y, &nx, &ny);
    if (mflags & P_OUT_OF_MAP)
        return Py_BuildValue("l",(long)0);

    foundob = present_arch(
        find_archetype(what),
        map,
        nx, ny
    );
    return Py_BuildValue("l",(long)(foundob));
};

/*****************************************************************************/
/* Name   : CFCheckArchInventory                                             */
/* Python : CFPython.CheckArchInventory(who, 'arch_name')                    */
/* Status : Stable                                                           */
/* Info   : This routine search explicit for a arch_name.                    */
/*****************************************************************************/
static PyObject* CFCheckArchInventory(PyObject* self, PyObject* args)
{
    long whoptr;
    char* whatstr;
    object* tmp;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&whatstr))
        return NULL;

    CHECK_OBJ(whoptr);

    for (tmp = WHO->inv; tmp != NULL; tmp = tmp->below)
    {
        if (!strcmp(tmp->arch->name,whatstr))
            break;
    };

    return Py_BuildValue("l",(long)(tmp));
};

/*****************************************************************************/
/* Name   : CFCheckInventory                                                 */
/* Python : CFPython.CheckInventory(object,string)                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCheckInventory(PyObject* self, PyObject* args)
{
    long whoptr;
    char* whatstr;
    object* tmp;
    object* foundob;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&whatstr))
        return NULL;

    CHECK_OBJ(whoptr);

    foundob = present_arch_in_ob(find_archetype(whatstr),WHO);
    if (foundob != NULL)
        return Py_BuildValue("l",(long)(foundob));

    for (tmp = WHO->inv; tmp; tmp = tmp->below)
    {
        if (!strncmp(query_name(tmp),whatstr,strlen(whatstr)))
        {
            return Py_BuildValue("l",(long)(tmp));
        };
        if (!strncmp(tmp->name,whatstr,strlen(whatstr)))
        {
            return Py_BuildValue("l",(long)(tmp));
        };
    };

    return Py_BuildValue("l",(long)0);
};

/*****************************************************************************/
/* Name   : CFGetName                                                        */
/* Python : CFPython.GetName(object)                                         */
/* Status : Untested  - hm, query name should return CF name like "name      */
/*        : (unpaid) (cursed) and not the 'pure' name - we must test this    */
/*****************************************************************************/
static PyObject* CFGetName(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("s",WHO->name);
};

/*****************************************************************************/
/* Name   : CFSetName                                                        */
/* Python : CFPython.SetName(object, name[, name_pl])                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetName(PyObject* self, PyObject* args)
{
    long whoptr;
    char *txt;
    char *txt_pl = NULL;

    if (!PyArg_ParseTuple(args,"ls|s",&whoptr,&txt,&txt_pl))
        return NULL;

    CHECK_OBJ(whoptr);

    if (txt_pl == NULL)
        txt_pl = txt;
    if (WHO->name != NULL)
        DELETE_STRING(WHO->name);
    if (strcmp(txt, ""))
        WHO->name = add_string(txt);
    if (WHO->name_pl != NULL)
        DELETE_STRING(WHO->name_pl);
    if (strcmp(txt_pl, ""))
        WHO->name_pl = add_string(txt_pl);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetTitle                                                       */
/* Python : CFPython.GetTitle(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetTitle(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    if (!WHO->title)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("s",WHO->title);
};


/*****************************************************************************/
/* Name   : CFSetTitle                                                       */
/* Python : CFPython.SetTitle(object, name)                                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetTitle(PyObject* self, PyObject* args)
{
    long whoptr;
    char *txt;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&txt))
        return NULL;

    CHECK_OBJ(whoptr);

    if (WHO->title != NULL)
        DELETE_STRING(WHO->title);
    if(txt && strcmp(txt,""))
        WHO->title = add_string(txt);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetSlaying                                                     */
/* Python : CFPython.GetSlaying(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetSlaying(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("s",WHO->slaying != NULL ? WHO->slaying : "");
};

/*****************************************************************************/
/* Name   : CFSetSlaying                                                     */
/* Python : CFPython.SetSlaying(object, name)                                */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSetSlaying(PyObject* self, PyObject* args)
{
    long whoptr;
    char *txt;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&txt))
        return NULL;

    CHECK_OBJ(whoptr);

    if (WHO->slaying != NULL)
        DELETE_STRING(WHO->slaying);
    if(txt && strcmp(txt,""))
        WHO->slaying = add_string(txt);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFCreateObject                                                   */
/* Python : CFPython.CreateObject(arch_name,pos[,map])                       */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCreateObject(PyObject* self, PyObject* args)
{
    object *myob;
    char *txt;
    CFParm* CFR;
    int x,y;
    int val;
    long map = StackWho[StackPosition] != NULL ? (long)StackWho[StackPosition]->map : 0;

    if (!PyArg_ParseTuple(args,"s(ii)|l",&txt, &x,&y,&map))
        return NULL;

    CHECK_MAP(map);

    myob = create_object(txt);
    if (myob == NULL)
        return NULL;

    myob->x = x;
    myob->y = y;
    val = 0;
    GCFP.Value[0] = (void *)(myob);
    GCFP.Value[1] = (void *)(map);
    GCFP.Value[2] = NULL;
    GCFP.Value[3] = (void *)(&val);
    CFR = (PlugHooks[HOOK_INSERTOBJECTINMAP])(&GCFP);
    myob = (object *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("l",(long)(myob));
};

/*****************************************************************************/
/* Name   : CFRemoveObject                                                   */
/* Python : CFPython.RemoveObject(object)                                    */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFRemoveObject(PyObject* self, PyObject* args)
{
    void* whoptr;
    object* myob;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    myob = (object *)(whoptr);
    GCFP.Value[0] = (void *)(myob);
    (PlugHooks[HOOK_REMOVEOBJECT])(&GCFP);

    if (StackActivator[StackPosition] != NULL && StackActivator[StackPosition]->type == PLAYER)
    {
        GCFP.Value[0] = (void *)(StackActivator[StackPosition]);
        GCFP.Value[1] = (void *)(StackActivator[StackPosition]);
        (PlugHooks[HOOK_ESRVSENDINVENTORY])(&GCFP);
    };
    PyFreeObject( whoptr );
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFIsAlive                                                        */
/* Python : CFPython.IsAlive(object)                                         */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsAlive(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_ALIVE));
};

/*****************************************************************************/
/* Name   : CFIsWiz                                                          */
/* Python : CFPython.IsDungeonMaster(object)                                 */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsWiz(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_WIZ));
};

/*****************************************************************************/
/* Name   : CFWasWiz                                                         */
/* Python : CFPython.WasDungeonMaster(object)                                */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFWasWiz(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_WAS_WIZ));
};

/*****************************************************************************/
/* Name   : CFIsApplied                                                      */
/* Python : CFPython.IsApplied(object)                                       */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsApplied(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_APPLIED));
};

/*****************************************************************************/
/* Name   : CFIsUnpaid                                                       */
/* Python : CFPython.IsUnpaid(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsUnpaid(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_UNPAID));
};

/*****************************************************************************/
/* Name   : CFIsFlying                                                       */
/* Python : CFPython.IsFlying(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsFlying(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_FLYING));
};

/*****************************************************************************/
/* Name   : CFIsMonster                                                      */
/* Python : CFPython.IsMonster(object)                                       */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsMonster(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_MONSTER));
};

/*****************************************************************************/
/* Name   : CFIsFriendly                                                     */
/* Python : CFPython.IsFriendly(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsFriendly(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_FRIENDLY));
};

/*****************************************************************************/
/* Name   : CFIsGenerator                                                    */
/* Python : CFPython.IsGenerator(object)                                     */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsGenerator(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_GENERATOR));
};

/*****************************************************************************/
/* Name   : CFIsThrown                                                       */
/* Python : CFPython.IsThrown(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsThrown(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_IS_THROWN));
};

/*****************************************************************************/
/* Name   : CFCanSeeInvisible                                                */
/* Python : CFPython.CanSeeInvisible(object)                                 */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanSeeInvisible(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_SEE_INVISIBLE));
};

/*****************************************************************************/
/* Name   : CFCanRoll                                                        */
/* Python : CFPython.CanRoll(object)                                         */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanRoll(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CAN_ROLL));
};

/*****************************************************************************/
/* Name   : CFIsTurnable                                                     */
/* Python : CFPython.IsTurnable(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsTurnable(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_IS_TURNABLE));
};

/*****************************************************************************/
/* Name   : CFIsUsedUp                                                       */
/* Python : CFPython.IsUsedUp(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsUsedUp(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_IS_USED_UP));
};

/*****************************************************************************/
/* Name   : CFIsIdentified                                                   */
/* Python : CFPython.IsIdentified(object)                                    */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsIdentified(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_IDENTIFIED));
};

/*****************************************************************************/
/* Name   : CFIsSplitting                                                    */
/* Python : CFPython.IsSplitting(object)                                     */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsSplitting(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_SPLITTING));
};

/*****************************************************************************/
/* Name   : CFHitBack                                                        */
/* Python : CFPython.HitBack(object)                                         */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFHitBack(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_HITBACK));
};

/*****************************************************************************/
/* Name   : CFBlocksView                                                     */
/* Python : CFPython.BlocksView(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFBlocksView(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_BLOCKSVIEW));
};

/*****************************************************************************/
/* Name   : CFIsUndead                                                       */
/* Python : CFPython.IsUndead(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsUndead(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_UNDEAD));
};

/*****************************************************************************/
/* Name   : CFIsScared                                                       */
/* Python : CFPython.IsScared(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsScared(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_SCARED));
};

/*****************************************************************************/
/* Name   : CFIsUnaggressive                                                 */
/* Python : CFPython.IsUnaggressive(object)                                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsUnaggressive(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_UNAGGRESSIVE));
};

/*****************************************************************************/
/* Name   : CFReflectMissiles                                                */
/* Python : CFPython.ReflectMissiles(object)                                 */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFReflectMissiles(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_REFL_MISSILE));
};

/*****************************************************************************/
/* Name   : CFReflectSpells                                                  */
/* Python : CFPython.ReflectSpells(object)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFReflectSpells(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_REFL_SPELL));
};

/*****************************************************************************/
/* Name   : CFIsRunningAway                                                  */
/* Python : CFPython.IsRunningAway(object)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsRunningAway(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_RUN_AWAY));
};

/*****************************************************************************/
/* Name   : CFCanPassThru                                                    */
/* Python : CFPython.CanPassThru                                             */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanPassThru(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CAN_PASS_THRU));
};

/*****************************************************************************/
/* Name   : CFCanPickUp                                                      */
/* Python : CFPython.CanPickUp(object)                                       */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanPickUp(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_PICK_UP));
};

/*****************************************************************************/
/* Name   : CFIsUnique                                                       */
/* Python : CFPython.IsUnique(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsUnique(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_UNIQUE));
};

/*****************************************************************************/
/* Name   : CFCanCastSpell                                                   */
/* Python : CFPython.CanCastSpell(object)                                    */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanCastSpell(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CAST_SPELL));
};

/*****************************************************************************/
/* Name   : CFCanUseScroll                                                   */
/* Python : CFPython.CanUseScroll(object)                                    */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanUseScroll(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_SCROLL));
};

/*****************************************************************************/
/* Name   : CFCanUseWand                                                     */
/* Python : CFPython.CanUseWand(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanUseWand(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_RANGE));
};

/*****************************************************************************/
/* Name   : CFCanUseBow                                                      */
/* Python : CFPython.CanUseBow(object)                                       */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanUseBow(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_BOW));
};

/*****************************************************************************/
/* Name   : CFCanUseArmour                                                   */
/* Python : CFPython.CanUseArmour(object)                                    */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanUseArmour(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_ARMOUR));
};

/*****************************************************************************/
/* Name   : CFCanUseWeapon                                                   */
/* Python : CFPython.CanUseWeapon(object)                                    */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanUseWeapon(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_WEAPON));
};

/*****************************************************************************/
/* Name   : CFCanUseRing                                                     */
/* Python : CFPython.CanUseRing(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanUseRing(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_RING));
};

/*****************************************************************************/
/* Name   : CFHasXRays                                                       */
/* Python : CFPython.HasXRays(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFHasXRays(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_XRAYS));
};

/*****************************************************************************/
/* Name   : CFIsFloor                                                        */
/* Python : CFPython.IsFloor(object)                                         */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsFloor(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_IS_FLOOR));
};

/*****************************************************************************/
/* Name   : CFIsLifeSaver                                                    */
/* Python : CFPython.IsLifesaver(object)                                     */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsLifeSaver(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_LIFESAVE));
};

/*****************************************************************************/
/* Name   : CFIsSleeping                                                     */
/* Python : CFPython.IsSleeping(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsSleeping(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_SLEEP));
};

/*****************************************************************************/
/* Name   : CFStandStill                                                     */
/* Python : CFPython.StandStill(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFStandStill(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_STAND_STILL));
};

/*****************************************************************************/
/* Name   : CFOnlyAttack                                                     */
/* Python : CFPython.OnlyAttack(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFOnlyAttack(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_ONLY_ATTACK));
};

/*****************************************************************************/
/* Name   : CFIsConfused                                                     */
/* Python : CFPython.IsConfused(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsConfused(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CONFUSED));
};

/*****************************************************************************/
/* Name   : CFHasStealth                                                     */
/* Python : CFPython.HasStealth(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFHasStealth(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_STEALTH));
};

/*****************************************************************************/
/* Name   : CFIsCursed                                                       */
/* Python : CFPython.IsCursed(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsCursed(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CURSED));
};

/*****************************************************************************/
/* Name   : CFIsDamned                                                       */
/* Python : CFPython.IsDamned(object)                                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsDamned(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_DAMNED));
};

/*****************************************************************************/
/* Name   : CFIsKnownMagical                                                 */
/* Python : CFPython.IsKnownMagical(object)                                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsKnownMagical(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_KNOWN_MAGICAL));
};

/*****************************************************************************/
/* Name   : CFIsKnownCursed                                                  */
/* Python : CFPython.IsKnownCursed(object)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsKnownCursed(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_KNOWN_CURSED));
};

/*****************************************************************************/
/* Name   : CFCanUseSkill                                                    */
/* Python : CFPython.CanUseSkill(object)                                     */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanUseSkill(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_CAN_USE_SKILL));
};

/*****************************************************************************/
/* Name   : CFHasBeenApplied                                                 */
/* Python : CFPython.HasBeenApplied(object)                                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFHasBeenApplied(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_BEEN_APPLIED));
};

/*****************************************************************************/
/* Name   : CFCanUseRod                                                      */
/* Python : CFPython.CanUseRod(object)                                       */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanUseRod(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_ROD));
};

/*****************************************************************************/
/* Name   : CFCanUseHorn                                                     */
/* Python : CFPython.CanUseHorn(object)                                      */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanUseHorn(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_USE_HORN));
};

/*****************************************************************************/
/* Name   : CFMakeInvisible                                                  */
/* Python : CFPython.MakeInvisible(object)                                   */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFMakeInvisible(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_MAKE_INVIS));
};

/*****************************************************************************/
/* Name   : CFIsBlind                                                        */
/* Python : CFPython.IsBlind(object)                                         */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFIsBlind(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_BLIND));
};

/*****************************************************************************/
/* Name   : CFCanSeeInDark                                                   */
/* Python : CFPython.CanSeeInDark(object)                                    */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFCanSeeInDark(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",QUERY_FLAG(WHO,FLAG_SEE_IN_DARK));
};

/*****************************************************************************/
/* Name   : CFGetAC                                                          */
/* Python : CFPython.GetAC(object)                                           */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetAC(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.ac);
};

/*****************************************************************************/
/* Name   : CFGetWC                                                          */
/* Python : CFPython.GetWC(object)                                           */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetWC(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.wc);
};

/*****************************************************************************/
/* Name   : CFGetCha                                                         */
/* Python : CFPython.GetCharisma(object)                                     */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetCha(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.Cha);
};

/*****************************************************************************/
/* Name   : CFGetCon                                                         */
/* Python : CFPython.GetConstitution(object)                                 */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetCon(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.Con);
};

/*****************************************************************************/
/* Name   : CFGetDex                                                         */
/* Python : CFPython.GetDexterity(object)                                    */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetDex(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.Dex);
};

/*****************************************************************************/
/* Name   : CFGetHP                                                          */
/* Python : CFPython.GetHP(object)                                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetHP(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.hp);

};

/*****************************************************************************/
/* Name   : CFGetInt                                                         */
/* Python : CFPython.GetIntelligence(object)                                 */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetInt(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.Int);
};

/*****************************************************************************/
/* Name   : CFGetPow                                                         */
/* Python : CFPython.GetPower(object)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetPow(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.Pow);
};

/*****************************************************************************/
/* Name   : CFGetSP                                                          */
/* Python : CFPython.GetSP(object)                                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetSP(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.sp);
};

/*****************************************************************************/
/* Name   : CFGetStr                                                         */
/* Python : CFPython.GetStrength(object)                                     */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetStr(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.Str);
};

/*****************************************************************************/
/* Name   : CFGetWis                                                         */
/* Python : CFPython.GetWisdom(object)                                       */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetWis(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.Wis);
};

/*****************************************************************************/
/* Name   : CFGetMaxHP                                                       */
/* Python : CFPython.GetMaxHP(object)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetMaxHP(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.maxhp);
};

/*****************************************************************************/
/* Name   : CFGetMaxSP                                                       */
/* Python : CFPython.GetMaxSP(object)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetMaxSP(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->stats.maxsp);
};

/*****************************************************************************/
/* Name   : CFGetXPos                                                        */
/* Python : CFPython.GetXPosition(object)                                    */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetXPos(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->x);
};

/*****************************************************************************/
/* Name   : CFGetYPos                                                        */
/* Python : CFPython.GetYPosition                                            */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetYPos(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->y);
};

/*****************************************************************************/
/* Name   : CFSetPosition                                                    */
/* Python : CFPython.SetPosition(object,pos)                                 */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetPosition(PyObject* self, PyObject* args)
{
    int x, y, k;
    long whoptr;

    k = 0;

    if (!PyArg_ParseTuple(args,"l(ii)",&whoptr,&x,&y))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(&x);
    GCFP.Value[2] = (void *)(&y);
    GCFP.Value[3] = (void *)(&k);
    GCFP.Value[4] = (void *)(NULL);
    PyFreeMemory((PlugHooks[HOOK_TRANSFEROBJECT])(&GCFP));

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetNickname                                                    */
/* Python : CFPython.SetNickname(object,name)                                */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetNickname(PyObject* self, PyObject* args)
{
    long whoptr;
    char *newnick;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&newnick))
        return NULL;

    CHECK_OBJ(whoptr);

    if (WHO->type==PLAYER)
    {
        GCFP.Value[0] = (void *)(WHO);
        GCFP.Value[1] = (void *)(newnick);
        CFR = (PlugHooks[HOOK_CMDTITLE])(&GCFP);
        PyFreeMemory( CFR );
    }
    else
    {
        WHO->title = add_string(newnick);
        if (WHO->env != NULL)
        {
            if (WHO->env->type == PLAYER)
            {
                GCFP.Value[0] = (void *)(WHO->env);
                GCFP.Value[1] = (void *)(WHO);
                (PlugHooks[HOOK_ESRVSENDITEM])(&GCFP);
            }
        };
    };

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetAC                                                          */
/* Python : CFPython.SetAC(object, value)                                    */
/* Status : Untested, AC may not survive fix_player()                        */
/*****************************************************************************/
static PyObject* CFSetAC(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < -120 || value > 120)
    {
        set_exception("value must be between -120 and 120");
        return NULL;
    }

    WHO->stats.ac = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetWC                                                          */
/* Python : CFPython.SetWC(object, value)                                    */
/* Status : Untested, WC may not survive fix_player()                        */
/*****************************************************************************/
static PyObject* CFSetWC(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < -120 || value > 120)
    {
        set_exception("value must be between -120 and 120");
        return NULL;
    }

    WHO->stats.wc = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetCha                                                         */
/* Python : CFPython.SetCharisma(object,value)                               */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetCha(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < -30 || value > 30)
    {
        set_exception("value must be between -30 and 30");
        return NULL;
    }

    WHO->stats.Cha = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Cha = value;
    };
    PyFixPlayer(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetCon                                                         */
/* Python : CFPython.SetConstitution(object,value)                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetCon(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < -30 || value > 30)
    {
        set_exception("value must be between -30 and 30");
        return NULL;
    }

    WHO->stats.Con = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Con = value;
    };
    PyFixPlayer(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetDex                                                         */
/* Python : CFPython.SetDexterity(object,value)                              */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetDex(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < -30 || value > 30)
    {
        set_exception("value must be between -30 and 30");
        return NULL;
    }

    WHO->stats.Dex = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Dex = value;
    };
    PyFixPlayer(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetHP                                                          */
/* Python : CFPython.SetHP(object,value)                                     */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetHP(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < 0 || value > 32000)
    {
        set_exception("value must be between 0 and 32000");
        return NULL;
    }

    WHO->stats.hp = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetInt                                                         */
/* Python : CFPython.SetIntelligence(object,value)                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetInt(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < -30 || value > 30)
    {
        set_exception("value must be between -30 and 30");
        return NULL;
    }

    WHO->stats.Int = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Int = value;
    };
    PyFixPlayer(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetMaxHP                                                       */
/* Python : CFPython.SetMaxHP(object,value)                                  */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetMaxHP(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < 0 || value > 32000)
    {
        set_exception("value must be between 0 and 32000");
        return NULL;
    }

    WHO->stats.maxhp = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetMaxSP                                                       */
/* Python : CFPython.SetMaxSP(object,value)                                  */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetMaxSP(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < 0 || value > 32000)
    {
        set_exception("value must be between 0 and 32000");
        return NULL;
    }

    WHO->stats.maxsp = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetPow                                                         */
/* Python : CFPython.SetPower(object,value)                                  */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetPow(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < -30 || value > 30)
    {
        set_exception("value must be between -30 and 30");
        return NULL;
    }

    WHO->stats.Pow = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Pow = value;
    };
    PyFixPlayer(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetSP                                                          */
/* Python : CFPython.SetSP(object,value)                                     */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetSP(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < 0 || value > 32000)
    {
        set_exception("value must be between 0 and 32000");
        return NULL;
    }

    WHO->stats.sp = value;
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetStr                                                         */
/* Python : CFPython.SetStrength(object,value)                               */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetStr(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < -30 || value > 30)
    {
        set_exception("value must be between -30 and 30");
        return NULL;
    }

    WHO->stats.Str = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Str = value;
    };
    PyFixPlayer(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFSetWis                                                         */
/* Python : CFPython.SetWisdom(object,value)                                 */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetWis(PyObject* self, PyObject* args)
{
    int value;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&value))
        return NULL;

    CHECK_OBJ(whoptr);
    if (value < -30 || value > 30)
    {
        set_exception("value must be between -30 and 30");
        return NULL;
    }

    WHO->stats.Wis = value;
    if (WHO->type == PLAYER)
    {
        WHO->contr->orig_stats.Wis = value;
    };
    PyFixPlayer(WHO);
    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Kharkov update : Write and Message changed.                               */
/* Write(message,dest[,color])                                               */
/* -> Writes a message to a specific player.                                 */
/* Message(message,dest[,color])                                             */
/* -> Writes a message to a specific map (given by an object in this map).   */
/*****************************************************************************/
/*****************************************************************************/
/* Name   : CFMessage                                                        */
/* Python : CFPython.Message(message,object[,color])                         */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFMessage(PyObject* self, PyObject* args)
{
    int   color = NDI_BLUE|NDI_UNIQUE;
    char *message;
    long  whoptr;

    if (!PyArg_ParseTuple(args,"sl|i",&message,&whoptr,&color))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(&color);
    GCFP.Value[1] = (void *)(WHO->map);
    GCFP.Value[2] = (void *)(message);
    (PlugHooks[HOOK_NEWINFOMAP])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFWrite                                                          */
/* Python : CFPython.Write(message,object[,color])                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFWrite(PyObject* self, PyObject* args)
{
    int   zero   = 0;
    char* message;
    long  whoptr;
    int   color  = NDI_UNIQUE | NDI_ORANGE;

    if (!PyArg_ParseTuple(args,"sl|i",&message,&whoptr,&color))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(&color);
    GCFP.Value[1] = (void *)(&zero);
    GCFP.Value[2] = (void *)(WHO);
    GCFP.Value[3] = (void *)(message);
    (PlugHooks[HOOK_NEWDRAWINFO])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFIsOfType                                                       */
/* Python : CFPython.IsOfType(object,type)                                   */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFIsOfType(PyObject* self, PyObject* args)
{
    int type;
    long whoptr;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&type))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->type == type);
};

/*****************************************************************************/
/* Name   : CFGetType                                                        */
/* Python : CFPython.GetType(object)                                         */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetType(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("i",WHO->type);
};

/* Those replace the old get-script... and set-script... system */
/*****************************************************************************/
/* Name   : CFGetEventHandler                                                */
/* Python : CFPython.GetEventHandler(object,event_number)                    */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetEventHandler(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;
    event *evt;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&eventnr))
        return NULL;

    CHECK_OBJ(whoptr);

    evt = find_event(WHO, eventnr);
    if (evt == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("s",evt->hook);
};

/*****************************************************************************/
/* Name   : CFSetEventHandler                                                */
/* Python : CFPython.SetEventHandler(object,event_number,script_name)        */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetEventHandler(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;
    char* scriptname;
    event *evt;

    if (!PyArg_ParseTuple(args,"lis",&whoptr, &eventnr, &scriptname))
        return NULL;

    CHECK_OBJ(whoptr);

    evt = find_event(WHO, eventnr);
    if (evt == NULL)
    {
        set_exception("illegal event number %d", eventnr);
        return NULL;
    }

    evt->hook = add_string(scriptname);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetEventPlugin                                                 */
/* Python : CFPython.GetEventPlugin(object,event_number)                     */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetEventPlugin(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;
    event *evt;

    if (!PyArg_ParseTuple(args,"li",&whoptr, &eventnr))
        return NULL;

    CHECK_OBJ(whoptr);

    evt = find_event(WHO, eventnr);
    if (evt == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("s",evt->plugin);
};

/*****************************************************************************/
/* Name   : CFSetEventPlugin                                                 */
/* Python : CFPython.SetEventPlugin(object,event_number,script_name)         */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetEventPlugin(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;
    char* scriptname;

    event *evt;

    if (!PyArg_ParseTuple(args,"lis",&whoptr, &eventnr, &scriptname))
        return NULL;

    CHECK_OBJ(whoptr);

    evt = find_event(WHO, eventnr);
    if (evt == NULL)
    {
        set_exception("illegal event number %d", eventnr);
        return NULL;
    }

    evt->plugin = add_string(scriptname);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetEventOptions                                                */
/* Python : CFPython.GetEventOptions(object,event_number)                    */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetEventOptions(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;
    event *evt;

    if (!PyArg_ParseTuple(args,"li",&whoptr, &eventnr))
        return NULL;

    CHECK_OBJ(whoptr);

    evt = find_event(WHO, eventnr);
    if (evt == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("s",evt->options);
};

/*****************************************************************************/
/* Name   : CFSetEventOptions                                                */
/* Python : CFPython.SetEventOptions(object,event_number,script_name)        */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSetEventOptions(PyObject* self, PyObject* args)
{
    long whoptr;
    int eventnr;
    char* scriptname;
    event *evt;

    if (!PyArg_ParseTuple(args,"lis",&whoptr, &eventnr, &scriptname))
        return NULL;

    CHECK_OBJ(whoptr);

    evt = find_event(WHO, eventnr);
    if (evt == NULL)
    {
        set_exception("illegal event number %d", eventnr);
        return NULL;
    }

    evt->options = add_string(scriptname);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFLoadObject                                                     */
/* Python : CFPython.LoadObject(string)                                      */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFLoadObject(PyObject* self, PyObject* args)
{
    object *whoptr;
    char *dumpob;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args, "s",&dumpob))
        return NULL;

    /* First step: We create the object */
    GCFP.Value[0] = (void *)(dumpob);
    CFR = (PlugHooks[HOOK_LOADOBJECT])(&GCFP);
    whoptr = (object *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("l",(long)(whoptr));
};

/*****************************************************************************/
/* Name   : CFSaveObject                                                     */
/* Python : CFPython.SaveObject(what)                                        */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFSaveObject(PyObject* self, PyObject* args)
{
    long whoptr;
    char *result;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args, "l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    CFR = (PlugHooks[HOOK_DUMPOBJECT])(&GCFP);
    result = (char *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("s",result);
};

/*****************************************************************************/
/* Name   : CFGetIP                                                          */
/* Python : CFPython.GetIP(object)                                           */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetIP(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args, "l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    if (WHO->contr == NULL)
    {
        Py_INCREF(Py_None);
        return Py_None;
    }

    return Py_BuildValue("s",WHO->contr->socket.host);
};

/*****************************************************************************/
/* Name   : CFGetInventory                                                   */
/* Python : CFPython.GetInventory(object)                                    */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetInventory(PyObject* self, PyObject* args)
{
    long whoptr;

    if (!PyArg_ParseTuple(args, "l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("l", (long)(WHO->inv));
};

/*****************************************************************************/
/* Name   : CFGetInternalName                                                */
/* Python : CFPython.GetInternalName(object)                                 */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetInternalName(PyObject* self, PyObject* args)
{
    long whoptr;
    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    return Py_BuildValue("s",WHO->name != NULL ? WHO->name : "");
};

/*****************************************************************************/
/* Name   : CFRegisterCommand                                                */
/* Python : CFPython.RegisterCommand(cmdname,scriptname,speed)               */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFRegisterCommand(PyObject* self, PyObject* args)
{
    char *cmdname;
    char *scriptname;
    double cmdspeed;
    int i;

    if (!PyArg_ParseTuple(args, "ssd",&cmdname,&scriptname,&cmdspeed))
        return NULL;

    if (cmdspeed < 0)
    {
        set_exception("speed must not be negative");
        return NULL;
    }

    for (i=0;i<NR_CUSTOM_CMD;i++)
    {
        if (CustomCommand[i].name != NULL)
        {
            if (!strcmp(CustomCommand[i].name,cmdname))
            {
                set_exception("command '%s' is already registered", cmdname);
                return NULL;
            }
        }
    }
    for (i=0;i<NR_CUSTOM_CMD;i++)
    {
        if (CustomCommand[i].name == NULL)
        {
            CustomCommand[i].name = strdup_local(cmdname);
            CustomCommand[i].script = strdup_local(scriptname);
            CustomCommand[i].speed = cmdspeed;
            i = NR_CUSTOM_CMD;
        }
    };

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFCostFlagFTrue                                                  */
/* Python : CFPython.CostFlagFTrue()                                         */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCostFlagFTrue(PyObject* self, PyObject* args)
{
    int flag=F_TRUE;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",flag);
};

/*****************************************************************************/
/* Name   : CFCostFlagFBuy                                                   */
/* Python : CFPython.CostFlagFBuy()                                          */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCostFlagFBuy(PyObject* self, PyObject* args)
{
    int flag=F_BUY;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",flag);
};

/*****************************************************************************/
/* Name   : CFCostFlagFSell                                                  */
/* Python : CFPython.CostFlagFSell()                                         */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFCostFlagFSell(PyObject* self, PyObject* args)
{
    int flag=F_SELL;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",flag);
};

/*****************************************************************************/
/* Name   : CFCostFlagFNoBargain                                             */
/* Python : CFPython.CostFlagFNoBargain()                                    */
/*****************************************************************************/
static PyObject* CFCostFlagFNoBargain(PyObject* self, PyObject* args)
{
    int flag=F_NO_BARGAIN;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",flag);
};

/*****************************************************************************/
/* Name   : CFCostFlagFIdentified                                            */
/* Python : CFPython.CostFlagFIdentified()                                   */
/*****************************************************************************/
static PyObject* CFCostFlagFIdentified(PyObject* self, PyObject* args)
{
    int flag=F_IDENTIFIED;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",flag);
};

/*****************************************************************************/
/* Name   : CFCostFlagFNotCursed                                             */
/* Python : CFPython.CostFlagFNotCursed()                                    */
/*****************************************************************************/
static PyObject* CFCostFlagFNotCursed(PyObject* self, PyObject* args)
{
    int flag=F_NOT_CURSED;
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("i",flag);
};

/*****************************************************************************/
/* Name   : CFGetObjectCost                                                  */
/* Python : CFPython.GetObjectCost(buyer,object,type)                        */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFGetObjectCost(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;
    int flag;
    uint64 cost;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"lli",&whoptr,&whatptr,&flag))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_OBJ(whatptr);

    GCFP.Value[0] = (void *)(WHAT);
    GCFP.Value[1] = (void *)(WHO);
    GCFP.Value[2] = (void *)(&flag);
    CFR = (PlugHooks[HOOK_QUERYCOST])(&GCFP);
    cost=*(uint64*)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("L",cost);
};

/*****************************************************************************/
/* Name   : CFGetObjectMoney                                                 */
/* Python : CFPython.GetObjectMoney(buyer)                                   */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFGetObjectMoney(PyObject* self, PyObject* args)
{
    long whoptr;
    uint64 amount;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"l",&whoptr))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    CFR = (PlugHooks[HOOK_QUERYMONEY])(&GCFP);
    amount=*(uint64*)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("L",amount);
};

/*****************************************************************************/
/* Name   : CFPayForItem                                                     */
/* Python : CFPython.PayForItem(buyer,object)                                */
/* Status : Untested                                                         */
/*****************************************************************************/
static PyObject* CFPayForItem(PyObject* self, PyObject* args)
{
    long whoptr;
    long whatptr;
    int val;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"ll",&whoptr,&whatptr))
        return NULL;

    CHECK_OBJ(whoptr);
    CHECK_OBJ(whatptr);

    GCFP.Value[0] = (void *)(WHAT);
    GCFP.Value[1] = (void *)(WHO);
    CFR = (PlugHooks[HOOK_PAYFORITEM])(&GCFP);
    val=*(int*)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Name   : CFPayAmount                                                      */
/* Python : CFPython.PayAmount(buyer,value)                                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFPayAmount(PyObject* self, PyObject* args)
{
    long whoptr;
    uint64 to_pay;
    int val;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"lL",&whoptr,&to_pay))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(&to_pay);
    GCFP.Value[1] = (void *)(WHO);
    CFR = (PlugHooks[HOOK_PAYFORAMOUNT])(&GCFP);
    val=*(int*)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("i",val);
};

/*****************************************************************************/
/* Name   : CFSendCustomCommand                                              */
/* Python : CFPython.SendCustomCommandwho, 'customcommand')                  */
/* Status : Stable                                                           */
/*****************************************************************************/
static PyObject* CFSendCustomCommand(PyObject* self, PyObject* args)
{
    long whoptr;
    char *customcmd;

    if (!PyArg_ParseTuple(args,"ls",&whoptr,&customcmd))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(customcmd);
    (PlugHooks[HOOK_SENDCUSTOMCOMMAND])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
};

/*****************************************************************************/
/* Name   : CFGetHumidity                                                    */
/* Python : CFPython.GetHumidity(x,y,map)                                    */
/* Status : Place Holder.                                                    */
/*****************************************************************************/
/* Returns the humidity level of a given square of a map.                    */
/*****************************************************************************/
static PyObject* CFGetHumidity(PyObject* self, PyObject* args)
{
    int val=0;
    int x, y;
    long map; /* mapstruct pointer */
    if (!PyArg_ParseTuple(args,"iil",&x,&y,&map))
        return NULL;

    CHECK_MAP(map);

    return Py_BuildValue("i",val);
}

/*****************************************************************************/
/* Name   : CFGetTemperature                                                 */
/* Python : CFPython.GetTemperature(x,y,map)                                 */
/* Status : Place Holder.                                                    */
/*****************************************************************************/
/* Returns the temperature of a given square of a map.                       */
/*****************************************************************************/
static PyObject* CFGetTemperature(PyObject* self, PyObject* args)
{
    int val=0;
    int x, y;
    long map; /* mapstruct pointer */
    if (!PyArg_ParseTuple(args,"iil",&x,&y,&map))
        return NULL;

    CHECK_MAP(map);

    return Py_BuildValue("i",val);
}

/*****************************************************************************/
/* Name   : CFGetPressure                                                    */
/* Python : CFPython.GetPressure(x,y,map)                                    */
/* Status : Place Holder.                                                    */
/*****************************************************************************/
/* Returns the pressure level of a given square of a map.                    */
/*****************************************************************************/
static PyObject* CFGetPressure(PyObject* self, PyObject* args)
{
    int val=0;
    int x, y;
    long map; /* mapstruct pointer */
    if (!PyArg_ParseTuple(args,"iil",&x,&y,&map))
        return NULL;

    CHECK_MAP(map);

    return Py_BuildValue("i",val);
}

/*****************************************************************************/
/* Name    : CFSetVariable                                                   */
/* Python  : CFPython.SetVariable(object,args)                               */
/* Status  : Unknown                                                         */
/*****************************************************************************/
/* Changes an object according to an argument string. Equivalent of the DM   */
/* patch command.                                                            */
/*****************************************************************************/
static PyObject* CFSetVariable(PyObject* self, PyObject* args)
{
    char *txt;
    long whoptr;

    if (!PyArg_ParseTuple(args, "ls", &whoptr, &txt))
      return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)txt;
    (PlugHooks[HOOK_SETVARIABLE])(&GCFP);

    Py_INCREF(Py_None);
    return Py_None;
}

/*****************************************************************************/
/* Name    : CFDecreaseObjectNr                                              */
/* Python  : CFPython.DecreaseObjectNr(object,value)                         */
/* Status  : Unknown                                                         */
/*****************************************************************************/
static PyObject* CFDecreaseObjectNr(PyObject* self, PyObject* args)
{
    long whoptr;
    int val;
    object* ob;
    CFParm* CFR;

    if (!PyArg_ParseTuple(args,"li",&whoptr,&val))
        return NULL;

    CHECK_OBJ(whoptr);

    GCFP.Value[0] = (void *)(WHO);
    GCFP.Value[1] = (void *)(&val);
    CFR = (PlugHooks[HOOK_DECREASEOBJECTNR])(&GCFP);
    ob = (object *)(CFR->Value[0]);
    PyFreeMemory( CFR );

    return Py_BuildValue("l", (long)ob);
}

/*****************************************************************************/
/* Name    : CFGetMapDir                                                     */
/* Python  : CFPython.GetMapDirectory()                                      */
/* Status  : Unknown                                                         */
/*****************************************************************************/
static PyObject* CFGetMapDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.mapdir);
}

/*****************************************************************************/
/* Name    : CFGetUniqueDir                                                  */
/* Python  : CFPython.GetUniqueDirectory()                                   */
/* Status  : Unknown                                                         */
/*****************************************************************************/
static PyObject* CFGetUniqueDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.uniquedir);
}

/*****************************************************************************/
/* Name    : CFGetTempDir                                                    */
/* Python  : CFPython.GetTempDirectory()                                     */
/* Status  : Unknown                                                         */
/*****************************************************************************/
static PyObject* CFGetTempDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.tmpdir);
}

/*****************************************************************************/
/* Name    : CFGetConfigurationDir                                           */
/* Python  : CFPython.GetConfigurationDirectory()                            */
/* Status  : Unknown                                                         */
/*****************************************************************************/
static PyObject* CFGetConfigurationDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.confdir);
}

/*****************************************************************************/
/* Name    : CFGetDataDir                                                    */
/* Python  : CFPython.GetDataDirectory()                                     */
/* Status  : Unknown                                                         */
/*****************************************************************************/
static PyObject* CFGetDataDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.datadir);
}

/*****************************************************************************/
/* Name    : CFGetLocalDir                                                   */
/* Python  : CFPython.GetLocalDirectory()                                    */
/* Status  : Unknown                                                         */
/*****************************************************************************/
static PyObject* CFGetLocalDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.localdir);
}

/*****************************************************************************/
/* Name    : CFGetPlayerDir                                                  */
/* Python  : CFPython.GetPlayerDirectory()                                   */
/* Status  : Unknown                                                         */
/*****************************************************************************/
static PyObject* CFGetPlayerDir(PyObject* self, PyObject* args)
{
    if (!PyArg_ParseTuple(args,"",NULL))
        return NULL;
    return Py_BuildValue("s",settings.playerdir);
}

/*****************************************************************************/
/* The Plugin Management Part.                                               */
/* Most of the functions below should exist in any CF plugin. They are used  */
/* to glue the plugin to the server core. All functions follow the same      */
/* declaration scheme (taking a CFParm* arg, returning a CFParm) to make the */
/* plugin interface as general as possible. And since the loading of modules */
/* isn't time-critical, it is never a problem. It could also make using      */
/* programming languages other than C to write plugins a little easier, but  */
/* this has yet to be proven.                                                */
/*****************************************************************************/

/*****************************************************************************/
/* Called whenever a Hook Function needs to be connected to the plugin.      */
/*****************************************************************************/
MODULEAPI CFParm* registerHook(CFParm* PParm)
{
    int Pos;
    f_plugin Hook;
    Pos = *(int*)(PParm->Value[0]);
    Hook=(f_plugin)(PParm->Value[1]);
    PlugHooks[Pos]=Hook;
    return NULL;
};

/*****************************************************************************/
/* Called whenever an event is triggered, both Local and Global ones.        */
/*****************************************************************************/
/* Two types of events exist in CF:                                          */
/* - Local events: They are triggered by a single action on a single object. */
/*                 Nearly any object can trigger a local event               */
/*                 To warn the plugin of a local event, the map-maker needs  */
/*                 to use the event... tags in the objects of their maps.    */
/* - Global events: Those are triggered by actions concerning CF as a whole. */
/*                 Those events may or may not be triggered by a particular  */
/*                 object; they can't be specified by event... tags in maps. */
/*                 The plugin should register itself for all global events it*/
/*                 wants to be aware of.                                     */
/* Why those two types ? Local Events are made to manage interactions between*/
/* objects, for example to create complex scenarios. Global Events are made  */
/* to allow logging facilities and server management. Global Events tends to */
/* require more CPU time than Local Events, and are sometimes difficult to   */
/* bind to any specific object.                                              */
/*****************************************************************************/
MODULEAPI CFParm* triggerEvent(CFParm* PParm)
{
    int eventcode;
    static int result;


    eventcode = *(int *)(PParm->Value[0]);
    printf( "PYTHON - triggerEvent:: eventcode %d\n",eventcode);
    result = 0;
    switch(eventcode)
    {
        case EVENT_NONE:
            printf( "PYTHON - Warning - EVENT_NONE requested\n");
            break;
        case EVENT_ATTACK:
        case EVENT_APPLY:
        case EVENT_DEATH:
        case EVENT_DROP:
        case EVENT_PICKUP:
        case EVENT_SAY:
        case EVENT_STOP:
        case EVENT_TIME:
        case EVENT_THROW:
        case EVENT_TRIGGER:
        case EVENT_CLOSE:
        case EVENT_TIMER:
            result = HandleEvent(PParm);
            break;
        case EVENT_BORN:
        case EVENT_CRASH:
        case EVENT_GDEATH:
        case EVENT_GKILL:
        case EVENT_LOGIN:
        case EVENT_LOGOUT:
        case EVENT_REMOVE:
        case EVENT_SHOUT:
        case EVENT_MAPENTER:
        case EVENT_MAPLEAVE:
        case EVENT_CLOCK:
        case EVENT_MAPRESET:
        case EVENT_TELL:
        case EVENT_MUZZLE:
        case EVENT_KICK:
            result = HandleGlobalEvent(PParm);
            break;
    };
    GCFP.Value[0] = (void *)(&result);
    return &GCFP;
};

/*****************************************************************************/
/* Handles standard global events.                                            */
/*****************************************************************************/
MODULEAPI int HandleGlobalEvent(CFParm* PParm)
{
    FILE* Scriptfile;
    char *scriptname;
    char *filename;

    if (!allocate_stack())
    {
        return 0;
    };

    scriptname = NULL;
    switch(*(int *)(PParm->Value[0]))
    {
        case EVENT_CRASH:
            printf( "Unimplemented for now\n");
            break;
        case EVENT_BORN:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            scriptname = "python/events/python_born.py";
            break;
        case EVENT_GDEATH:
            StackWho[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            scriptname = "python/events/python_gdeath.py";
            break;
        case EVENT_GKILL:
            StackWho[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            scriptname = "python/events/python_gkill.py";
            break;
        case EVENT_LOGIN:
            StackActivator[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackWho[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            scriptname = "python/events/python_login.py";
            break;
        case EVENT_LOGOUT:
            StackActivator[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackWho[StackPosition] = ((player *)(PParm->Value[1]))->ob;
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            scriptname = "python/events/python_logout.py";
            break;
        case EVENT_REMOVE:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            scriptname = "python/events/python_remove.py";
            break;
        case EVENT_SHOUT:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            scriptname = "python/events/python_shout.py";
            break;
        case EVENT_MUZZLE:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            scriptname = "python/events/python_muzzle.py";
            break;
        case EVENT_KICK:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            StackText[StackPosition] = (char *)(PParm->Value[2]);
            scriptname = "python/events/python_kick.py";
            break;
        case EVENT_MAPENTER:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            scriptname = "python/events/python_mapenter.py";
            break;
        case EVENT_MAPLEAVE:
            StackActivator[StackPosition] = (object *)(PParm->Value[1]);
            scriptname = "python/events/python_mapleave.py";
            break;
        case EVENT_CLOCK:
            scriptname = "python/events/python_clock.py";
            break;
        case EVENT_MAPRESET:
            StackText[StackPosition] = (char *)(PParm->Value[1]);/* Map name/path */
            scriptname = "python/events/python_mapreset.py";
            break;
        case EVENT_TELL:
            scriptname = "python/events/python_tell.py";
            break;
    };

    if (scriptname != NULL) {
        filename = create_pathname(scriptname);

        Scriptfile = fopen(filename,"r");
        if (Scriptfile != NULL) {
            PyRun_SimpleFile(Scriptfile, filename);
            fclose(Scriptfile);
        }
    }

    StackPosition--;
    return 0;
};

/*****************************************************************************/
/* Handles standard local events.                                            */
/*****************************************************************************/
MODULEAPI int HandleEvent(CFParm* PParm)
{
    FILE* Scriptfile;

#ifdef PYTHON_DEBUG
    printf( "PYTHON - HandleEvent:: got script file >%s<\n",(char *)(PParm->Value[9]));
#endif
    if (!allocate_stack())
    {
        return 0;
    };

    StackActivator[StackPosition]   = (object *)(PParm->Value[1]);
    StackWho[StackPosition]         = (object *)(PParm->Value[2]);
    StackOther[StackPosition]       = (object *)(PParm->Value[3]);
    StackText[StackPosition]        = (char *)(PParm->Value[4]);
    StackParm1[StackPosition]       = *(uint32 *)(PParm->Value[5]);
    StackParm2[StackPosition]       = *(int *)(PParm->Value[6]);
    StackParm3[StackPosition]       = *(int *)(PParm->Value[7]);
    StackParm4[StackPosition]       = *(int *)(PParm->Value[8]);
    StackReturn[StackPosition]      = 0;
    /* RunPythonScript(scriptname); */
    Scriptfile = fopen(create_pathname((char *)(PParm->Value[9])),"r");
    if (Scriptfile == NULL)
    {
        printf( "PYTHON - The Script file %s can't be opened\n",(char *)(PParm->Value[9]));
        StackPosition--;
        return 0;
    };
    PyRun_SimpleFile(Scriptfile, create_pathname((char *)(PParm->Value[9])));
    fclose(Scriptfile);

#ifdef PYTHON_DEBUG
    printf( "PYTHON - HandleEvent:: script loaded (%s)!\n",(char *)(PParm->Value[9]));
#endif
    if (StackParm4[StackPosition] == SCRIPT_FIX_ALL)
    {
        if (StackOther[StackPosition] != NULL)
            PyFixPlayer(StackOther[StackPosition]);
        if (StackWho[StackPosition] != NULL)
            PyFixPlayer(StackWho[StackPosition]);
        if (StackActivator[StackPosition] != NULL)
            PyFixPlayer(StackActivator[StackPosition]);
    }
    else if (StackParm4[StackPosition] == SCRIPT_FIX_ACTIVATOR)
    {
        PyFixPlayer(StackActivator[StackPosition]);
    };
    StackPosition--;
    return StackReturn[StackPosition+1];
};

/*****************************************************************************/
/* Plugin initialization.                                                    */
/*****************************************************************************/
/* It is required that:                                                      */
/* - The first returned value of the CFParm structure is the "internal" name */
/*   of the plugin, used by objects to identify it.                          */
/* - The second returned value is the name "in clear" of the plugin, used for*/
/*   information purposes.                                                   */
/*****************************************************************************/
MODULEAPI CFParm* initPlugin(CFParm* PParm)
{
    printf("    CFPython Plugin loading.....\n");
    Py_Initialize();
    initCFPython();
    printf( "[Done]\n");
    GCFP.Value[0] = (void *) add_string(PLUGIN_NAME);
    GCFP.Value[1] = (void *) add_string(PLUGIN_VERSION);
    return &GCFP;
};

/*****************************************************************************/
/* Used to do cleanup before killing the plugin.                             */
/*****************************************************************************/
MODULEAPI CFParm* removePlugin(CFParm* PParm)
{
        return NULL;
};

/*****************************************************************************/
/* This function is called to ask various informations to the plugin.        */
/*****************************************************************************/
MODULEAPI CFParm* getPluginProperty(CFParm* PParm)
{

    static double dblval = 0.0;
    int i;
    if (PParm!=NULL)
    {
        if(!strcmp((char *)(PParm->Value[0]),"command?"))
        {
            if(!strcmp((char *)(PParm->Value[1]),PLUGIN_NAME))
            {
                GCFP.Value[0] = PParm->Value[1];
                GCFP.Value[1] = &cmd_aboutPython;
                GCFP.Value[2] = &dblval;
                return &GCFP;
            }
            else
            {
                for (i=0;i<NR_CUSTOM_CMD;i++)
                {
                    if (CustomCommand[i].name != NULL)
                    {
                        if (!strcmp(CustomCommand[i].name,(char *)(PParm->Value[1])))
                        {
                            printf( "PYTHON - Running command %s\n",CustomCommand[i].name);
                            GCFP.Value[0] = PParm->Value[1];
                            GCFP.Value[1] = cmd_customPython;
                            GCFP.Value[2] = &(CustomCommand[i].speed);
                            NextCustomCommand = i;
                            return &GCFP;
                        }
                    }
                }
            }
        }
        else
        {
            printf( "PYTHON - Unknown property tag: %s\n",(char *)(PParm->Value[0]));
        };
    };
    return NULL;
};

MODULEAPI int cmd_customPython(object *op, char *params)
{
    FILE* Scriptfile;
#ifdef PYTHON_DEBUG
    printf( "PYTHON - cmd_customPython called:: script file: %s\n",CustomCommand[NextCustomCommand].script);
#endif
    if (!allocate_stack())
    {
        return 0;
    };

    StackActivator[StackPosition]   = op;
    StackWho[StackPosition]         = op;
    StackOther[StackPosition]       = op;
    StackText[StackPosition]        = params;
    StackReturn[StackPosition]      = 0;
    Scriptfile = fopen(create_pathname(CustomCommand[NextCustomCommand].script),"r");
    if (Scriptfile == NULL)
    {
        printf( "PYTHON - The Script file %s can't be opened\n",CustomCommand[NextCustomCommand].script);
        StackPosition--;
        return 0;
    };
    PyRun_SimpleFile(Scriptfile, create_pathname(CustomCommand[NextCustomCommand].script));
    fclose(Scriptfile);
    StackPosition--;
    return StackReturn[StackPosition+1];
};

MODULEAPI int cmd_aboutPython(object *op, char *params)
{
    int color = NDI_BLUE|NDI_UNIQUE;
    char message[1024];

    sprintf(message,"%s (Pegasus)\n(C)2001 by Gros. The Plugin code is under GPL.",PLUGIN_VERSION);
    GCFP.Value[0] = (void *)(&color);
    GCFP.Value[1] = (void *)(op->map);
    GCFP.Value[2] = (void *)(message);
    (PlugHooks[HOOK_NEWINFOMAP])(&GCFP);

    return 0;
};

/*****************************************************************************/
/* The postinitPlugin function is called by the server when the plugin load  */
/* is complete. It lets the opportunity to the plugin to register some events*/
/*****************************************************************************/
MODULEAPI CFParm* postinitPlugin(CFParm* PParm)
{
    int i;
    /* We can now register some global events if we want */
    /* We'll only register the global-only events :      */
    /* BORN, CRASH, LOGIN, LOGOUT, REMOVE, and SHOUT.    */
    /* The events APPLY, ATTACK, DEATH, DROP, PICKUP, SAY*/
    /* STOP, TELL, TIME, THROW and TRIGGER are already   */
    /* handled on a per-object basis and I simply don't  */
    /* see how useful they could be for the Python stuff.*/
    /* Registering them as local would be probably useful*/
    /* for extended logging facilities.                  */

    printf( "PYTHON - Start postinitPlugin.\n");

    GCFP.Value[1] = (void *)(add_string(PLUGIN_NAME));
    i = EVENT_BORN;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_CRASH;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_LOGIN;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_LOGOUT;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_REMOVE;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_SHOUT;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_MAPENTER;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_MAPLEAVE;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

/*    i = EVENT_CLOCK; */
/*    GCFP.Value[0] = (void *)(&i); */
/*    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP); */

    i = EVENT_MAPRESET;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_MUZZLE;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    i = EVENT_KICK;
    GCFP.Value[0] = (void *)(&i);
    (PlugHooks[HOOK_REGISTEREVENT])(&GCFP);

    return NULL;
};

/*****************************************************************************/
/* Initializes the Python Interpreter.                                       */
/*****************************************************************************/
static PyObject* CFPythonError;
MODULEAPI void initCFPython()
{
        PyObject *m, *d;
        int i;
        FILE *scriptfile;
        const char *filename;

        printf( "PYTHON - Start initCFPython.\n");

        m = Py_InitModule("CFPython", CFPythonMethods);
        d = PyModule_GetDict(m);
        CFPythonError = PyErr_NewException("CFPython.error",NULL,NULL);
        PyDict_SetItemString(d,"error",CFPythonError);
        for (i=0;i<NR_CUSTOM_CMD;i++)
        {
            CustomCommand[i].name   = NULL;
            CustomCommand[i].script = NULL;
            CustomCommand[i].speed  = 0.0;
        };

        if (allocate_stack())
        {
            filename = create_pathname("python/events/python_init.py");
            scriptfile = fopen(filename, "r");
            if (scriptfile != NULL)
            {
                PyRun_SimpleFile(scriptfile, filename);
                fclose(scriptfile);
            }
            StackPosition--;
        }
};
