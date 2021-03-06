/*************************************************************************
*									 *
*	 YAP Prolog 							 *
*									 *
*	Yap Prolog was developed at NCCUP - Universidade do Porto	 *
*									 *
* Copyright L.Damas, V.S.Costa and Universidade do Porto 1985-1997	 *
*									 *
**************************************************************************
*									 *
* File:		iopreds.c						 *
* Last rev:	5/2/88							 *
* mods:									 *
* comments:	Input/Output C implemented predicates			 *
*									 *
*************************************************************************/
#ifdef SCCS
static char SccsId[] = "%W% %G%";
#endif

/*
 * This file includes the definition of a miscellania of standard predicates
 * for yap refering to: Files and GLOBAL_Streams, Simple Input/Output, 
 *
 */

#include "Yap.h"
#include "Yatom.h"
#include "YapHeap.h"
#include "yapio.h"
#include "eval.h"
#include "YapText.h"
#include <stdlib.h>
#if HAVE_STDARG_H
#include <stdarg.h>
#endif
#if HAVE_CTYPE_H
#include <ctype.h>
#endif
#if HAVE_WCTYPE_H
#include <wctype.h>
#endif
#if HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if HAVE_SYS_TYPES_H
#include <sys/types.h>
#endif
#ifdef HAVE_SYS_STAT_H
#include <sys/stat.h>
#endif
#if HAVE_SYS_SELECT_H && !_MSC_VER && !defined(__MINGW32__) 
#include <sys/select.h>
#endif
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif
#if HAVE_STRING_H
#include <string.h>
#endif
#if HAVE_SIGNAL_H
#include <signal.h>
#endif
#if HAVE_FCNTL_H
/* for O_BINARY and O_TEXT in WIN32 */
#include <fcntl.h>
#endif
#ifdef _WIN32
#if HAVE_IO_H
/* Windows */
#include <io.h>
#endif
#endif
#if !HAVE_STRNCAT
#define strncat(X,Y,Z) strcat(X,Y)
#endif
#if !HAVE_STRNCPY
#define strncpy(X,Y,Z) strcpy(X,Y)
#endif
#if _MSC_VER || defined(__MINGW32__) 
#if HAVE_SOCKET
#include <winsock2.h>
#endif
#include <windows.h>
#ifndef S_ISDIR
#define S_ISDIR(x) (((x)&_S_IFDIR)==_S_IFDIR)
#endif
#endif
#include "iopreds.h"

#if _MSC_VER || defined(__MINGW32__) 
#define SYSTEM_STAT _stat
#else
#define SYSTEM_STAT stat
#endif


#undef PAR

#define    WRITE_DEFS() \
    PAR( "module", isatom, WRITE_MODULE ), \
    PAR( "attributes", isatom, WRITE_ATTRIBUTES ), \
    PAR( "cycles", booleanFlag, WRITE_CYCLES ), \
    PAR( "quoted", booleanFlag, WRITE_QUOTED ), \
    PAR( "ignore_ops", booleanFlag, WRITE_IGNORE_OPS ), \
    PAR( "max_depth",nat, WRITE_MAX_DEPTH ), \
    PAR( "numbervars", booleanFlag, WRITE_NUMBERVARS ), \
    PAR( "portrayed", booleanFlag, WRITE_PORTRAYED ), \
    PAR( "portray", booleanFlag, WRITE_PORTRAY ), \
    PAR( "priority", nat, WRITE_PRIORITY ), \
    PAR( "character_escapes", booleanFlag, WRITE_CHARACTER_ESCAPES ), \
    PAR( "backquotes", booleanFlag, WRITE_BACKQUOTES ), \
    PAR( "brace_terms", booleanFlag, WRITE_BRACE_TERMS ), \
    PAR( "fullstop", booleanFlag, WRITE_FULLSTOP ), \
    PAR( "nl", booleanFlag, WRITE_NL ), \
    PAR( "variable_names",ok, WRITE_VARIABLE_NAMES ),			\
    PAR( NULL, ok, WRITE_END )

#define PAR(x,y,z) z
 
 typedef enum open_enum_choices 
  {
    WRITE_DEFS()
  } open_choices_t;

#undef PAR

#define PAR(x,y,z) { x , y, z }
 
 
 static const param_t write_defs[] =
  {
    WRITE_DEFS()
  };
#undef PAR

  




#ifdef BEAM
int beam_write ( USES_REGS1 )
{
  Yap_StartSlots();
  Yap_plwrite (ARG1, GLOBAL_Stream+LOCAL_c_output_stream, 0, 0, GLOBAL_MaxPriority);
  Yap_CloseSlots();
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = 0L;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}
#endif

static bool
bind_variable_names(Term  t USES_REGS)
{
  while(!IsVarTerm(t) && IsPairTerm(t)) {
    Term tl = HeadOfTerm(t);
    Functor f;
    Term tv, t2, t1;

    if (!IsApplTerm(tl)) return FALSE;
    if ((f = FunctorOfTerm(tl)) != FunctorEq) {
      return FALSE;
    }
    t1 = ArgOfTerm(1, tl);
    if (IsVarTerm(t1)) {
       Yap_Error(INSTANTIATION_ERROR, t1, "variable_names");
       return false;
    }
    t2 = ArgOfTerm(2, tl);
    tv = Yap_MkApplTerm(FunctorDollarVar, 1, &t1);
    if (IsVarTerm(t2)) {
      Bind(VarOfTerm(t2), tv);
    }
    t = TailOfTerm(t);
  }
  return true;
}


static int
unbind_variable_names(Term  t USES_REGS)
{
  while(!IsVarTerm(t) && IsPairTerm(t)) {
    Term tl = HeadOfTerm(t);
    Functor f;
    Term  *tp2, t1;

    if (!IsApplTerm(tl)) return FALSE;
    if ((f = FunctorOfTerm(tl)) != FunctorEq) {
      return FALSE;
    }
    t1 = ArgOfTerm(1, tl);
    tp2 = RepAppl(tl)+2;
    while (*tp2 != t1) {
      tp2 = (CELL*)*tp2;
    }
    RESET_VARIABLE( tp2 );
    t = TailOfTerm(t);
  }
  return TRUE;
}


static bool
write_term ( int output_stream, Term t, xarg *args USES_REGS )
{
  bool rc;
  Term cm = CurrentModule;
  int depth, prio, flags =0;
  
  if (args[WRITE_MODULE].used) {
    CurrentModule = args[WRITE_MODULE].tvalue;
  }
  if (args[WRITE_VARIABLE_NAMES].used) {
    bind_variable_names(args[WRITE_VARIABLE_NAMES].tvalue PASS_REGS);
    flags |= Handle_vars_f;
  }
  if (args[WRITE_NUMBERVARS].used) {
    if (args[WRITE_NUMBERVARS].tvalue == TermTrue)
      flags |= Handle_vars_f;
  }
  if (args[WRITE_ATTRIBUTES].used) {
    Term ctl = args[WRITE_ATTRIBUTES].tvalue;
    if (ctl == TermWrite) {
      flags |= AttVar_None_f;
    } else if (ctl == TermPortray) {
      flags |= AttVar_None_f|AttVar_Portray_f;
    } else if (ctl == TermDots) {
      flags |= AttVar_Dots_f;
    } else if (ctl != TermIgnore) {
      Yap_Error(DOMAIN_ERROR_OUT_OF_RANGE, ctl, "write attributes should be one of {dots,ignore,portray,write}");
      rc = false;
      goto end;
    }
  }
  if (args[WRITE_CYCLES].used &&
      args[WRITE_CYCLES].tvalue == TermFalse) {
    flags |= Unfold_cyclics_f;
  }
  if (args[WRITE_QUOTED].used &&
      args[WRITE_QUOTED].tvalue == TermTrue) {
    flags |= Quote_illegal_f;
  }
  if (args[WRITE_IGNORE_OPS].used &&
      args[WRITE_IGNORE_OPS].tvalue == TermTrue) {
    flags |= Ignore_ops_f;
  }
  if (args[WRITE_PORTRAY].used &&
   args[WRITE_IGNORE_OPS].tvalue == TermTrue) {
    flags |= Ignore_ops_f;
  }
  if (args[WRITE_PORTRAYED].used &&
    args[WRITE_PORTRAYED].tvalue == TermTrue) {
    flags |= Use_portray_f;
  }
  if (args[WRITE_CHARACTER_ESCAPES].used &&
    args[WRITE_CHARACTER_ESCAPES].tvalue == TermFalse) {
    flags |= No_Escapes_f;
  }
  if (args[WRITE_BACKQUOTES].used &&
    args[WRITE_BACKQUOTES].tvalue == TermTrue) {
    flags |= BackQuote_String_f;
  }
  if (args[WRITE_BRACE_TERMS].used &&
      args[WRITE_BRACE_TERMS].tvalue == TermFalse) {
    flags |= No_Brace_Terms_f;
  }
  if (args[WRITE_FULLSTOP].used &&
    args[WRITE_FULLSTOP].tvalue == TermTrue) {
    flags |= Fullstop_f;
  }
  if (args[WRITE_NL].used &&
    args[WRITE_NL].tvalue == TermTrue) {
    flags |= New_Line_f;
  }

  if (args[WRITE_MAX_DEPTH].used) {
    depth = IntegerOfTerm(args[WRITE_MAX_DEPTH].tvalue);
  } else
      depth = LOCAL_max_depth;

  if (args[WRITE_PRIORITY].used) {
    prio = IntegerOfTerm(args[WRITE_PRIORITY].tvalue);
  } else {
    prio = GLOBAL_MaxPriority;
  }
  Yap_plwrite( t, GLOBAL_Stream+output_stream, depth, flags, prio);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  rc = true;

  end:
  if (args[WRITE_VARIABLE_NAMES].used) {
    unbind_variable_names(args[WRITE_VARIABLE_NAMES].tvalue PASS_REGS);
  }
  CurrentModule = cm;
  return rc;
}

static Int
write_term2 ( USES_REGS1 )
{
  
    /* '$write'(+Flags,?Term) */
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  xarg * args = Yap_ArgListToVector ( ARG2, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE == DOMAIN_ERROR_OUT_OF_RANGE)
      LOCAL_Error_TYPE = DOMAIN_ERROR_WRITE_OPTION;
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  yhandle_t mySlots = Yap_StartSlots();
  int output_stream = LOCAL_c_output_stream;
  if (output_stream == -1) output_stream = 1;
  LOCK(GLOBAL_Stream[output_stream].streamlock);
  write_term( output_stream, ARG2, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}


static Int
write_term3 ( USES_REGS1 )
{
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  xarg *args = Yap_ArgListToVector ( ARG3, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE == DOMAIN_ERROR_OUT_OF_RANGE)
      LOCAL_Error_TYPE = DOMAIN_ERROR_WRITE_OPTION;
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  int output_stream = Yap_CheckTextStream (ARG1, Output_Stream_f, "write/2");
  if (output_stream < 0 )
    return false;
  yhandle_t mySlots = Yap_StartSlots();
  write_term( output_stream, ARG2, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}

static Int
write2 ( USES_REGS1 )
{
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */

 
  xarg *args;
  yhandle_t mySlots;
  int output_stream = Yap_CheckTextStream (ARG1, Output_Stream_f, "write/2");
  if (output_stream < 0 )
    return false;
  args  = Yap_ArgListToVector ( TermNil, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE == DOMAIN_ERROR_OUT_OF_RANGE)
      LOCAL_Error_TYPE = DOMAIN_ERROR_WRITE_OPTION;
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  mySlots = Yap_StartSlots();
  args[WRITE_NUMBERVARS].used = true;
  args[WRITE_NUMBERVARS].tvalue = TermTrue;
  write_term( output_stream, ARG2, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}

static Int
write1 ( USES_REGS1 )
{
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  int output_stream = LOCAL_c_output_stream;
  if (output_stream == -1) output_stream = 1;
  xarg * args = Yap_ArgListToVector ( TermNil, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  yhandle_t mySlots = Yap_StartSlots();
  args[WRITE_NUMBERVARS].used = true;
  args[WRITE_NUMBERVARS].tvalue = TermTrue;
  LOCK(GLOBAL_Stream[output_stream].streamlock);
  write_term( output_stream, ARG1, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}

static Int
write_canonical1 ( USES_REGS1 )
{
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  int output_stream = LOCAL_c_output_stream;
  if (output_stream == -1) output_stream = 1;
  xarg * args = Yap_ArgListToVector ( TermNil, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  yhandle_t mySlots = Yap_StartSlots();
  args[WRITE_IGNORE_OPS].used = true;
  args[WRITE_IGNORE_OPS].tvalue = TermTrue;
  args[WRITE_QUOTED].used = true;
  args[WRITE_QUOTED].tvalue = TermTrue;
  LOCK(GLOBAL_Stream[output_stream].streamlock);
  write_term( output_stream, ARG1, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}

static Int
write_canonical ( USES_REGS1 )
{
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  xarg * args = Yap_ArgListToVector ( TermNil, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  int output_stream = Yap_CheckTextStream (ARG1, Output_Stream_f, "write/2");
  if (output_stream < 0 )
    return false;
  yhandle_t mySlots = Yap_StartSlots();
  args[WRITE_IGNORE_OPS].used = true;
  args[WRITE_IGNORE_OPS].tvalue = TermTrue;
  args[WRITE_QUOTED].used = true;
  args[WRITE_QUOTED].tvalue = TermTrue;
  write_term( output_stream, ARG2, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}

static Int
writeq1 ( USES_REGS1 )
{
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  xarg *args = Yap_ArgListToVector ( TermNil, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  yhandle_t mySlots = Yap_StartSlots();
  int output_stream = LOCAL_c_output_stream;
  if (output_stream == -1) output_stream = 1;
  args[WRITE_NUMBERVARS].used = true;
  args[WRITE_NUMBERVARS].tvalue = TermTrue;
  args[WRITE_QUOTED].used = true;
  args[WRITE_QUOTED].tvalue = TermTrue;
  write_term( output_stream, ARG1, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);

  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}


static Int
writeq ( USES_REGS1 )
{
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  xarg *args = Yap_ArgListToVector ( TermNil, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  int output_stream = Yap_CheckTextStream (ARG1, Output_Stream_f, "write/2");
  if (output_stream < 0 )
    return false;
  yhandle_t mySlots = Yap_StartSlots();
   args[WRITE_NUMBERVARS].used = true;
   args[WRITE_NUMBERVARS].tvalue = TermTrue;
   args[WRITE_QUOTED].used = true;
   args[WRITE_QUOTED].tvalue = TermTrue;
  write_term( output_stream,  ARG2, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}


static Int
print1 ( USES_REGS1 )
{
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  xarg *args = Yap_ArgListToVector ( TermNil, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  yhandle_t mySlots = Yap_StartSlots();
  int output_stream = LOCAL_c_output_stream;
  if (output_stream == -1) output_stream = 1;
   args[WRITE_PORTRAY].used = true;
   args[WRITE_PORTRAY].tvalue = TermTrue;
   args[WRITE_NUMBERVARS].used = true;
   args[WRITE_NUMBERVARS].tvalue = TermTrue;
  LOCK(GLOBAL_Stream[output_stream].streamlock);
  write_term( output_stream,  ARG1, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}


static Int
print ( USES_REGS1 )
{  
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  xarg *args = Yap_ArgListToVector ( TermNil, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  int output_stream = Yap_CheckTextStream (ARG1, Output_Stream_f, "write/2");
  if (output_stream < 0 )
    return false;
  yhandle_t mySlots = Yap_StartSlots();
  args[WRITE_PORTRAY].used = true;
  args[WRITE_PORTRAY].tvalue = TermTrue;
  args[WRITE_NUMBERVARS].used = true;
  args[WRITE_NUMBERVARS].tvalue = TermTrue;
  write_term( output_stream, ARG2, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}

static Int
writeln1 ( USES_REGS1 )
{
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  int output_stream = LOCAL_c_output_stream;
  if (output_stream == -1) output_stream = 1;
  xarg *args = Yap_ArgListToVector ( TermNil, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  yhandle_t mySlots = Yap_StartSlots();
  args[WRITE_NL].used = true;
  args[WRITE_NL].tvalue = TermTrue;
  args[WRITE_NUMBERVARS].used = true;
  args[WRITE_NUMBERVARS].tvalue = TermTrue;
  LOCK(GLOBAL_Stream[output_stream].streamlock);
  write_term( output_stream, ARG1, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}


static Int
writeln ( USES_REGS1 )
{
  
  /* notice: we must have ASP well set when using portray, otherwise
     we cannot make recursive Prolog calls */
  xarg *args = Yap_ArgListToVector ( TermNil, write_defs, WRITE_END  );
  if (args == NULL) {
    if (LOCAL_Error_TYPE)
      Yap_Error(LOCAL_Error_TYPE, LOCAL_Error_Term, NULL);
    return false;
  }
  int output_stream = Yap_CheckTextStream (ARG1, Output_Stream_f, "writeln/2");
  if (output_stream < 0)
    return false;
  yhandle_t mySlots = Yap_StartSlots();
  args[WRITE_NL].used = true;
  args[WRITE_NL].tvalue = TermTrue;
  args[WRITE_NUMBERVARS].used = true;
  args[WRITE_NUMBERVARS].tvalue = TermTrue;
  write_term( output_stream, ARG1, args PASS_REGS);
  UNLOCK(GLOBAL_Stream[output_stream].streamlock);
  Yap_CloseSlots( mySlots );
  if (EX != 0L) {
    Term ball = Yap_PopTermFromDB(EX);
    EX = NULL;
    Yap_JumpToEnv(ball);
    return(FALSE);
  }
  return (TRUE);
}



static Int
p_write_depth ( USES_REGS1 )
{				/* write_depth(Old,New)          */
  Term t1 = Deref (ARG1);
  Term t2 = Deref (ARG2);
  Term t3 = Deref (ARG3);

  if (!IsVarTerm (t1) && !IsIntegerTerm (t1)) {
    Yap_Error(TYPE_ERROR_INTEGER,t1,"write_depth/3");
    return FALSE;
  }
  if (!IsVarTerm (t2) && !IsIntegerTerm (t2)) {
    Yap_Error(TYPE_ERROR_INTEGER,t2,"write_depth/3");
    return FALSE;
  }
  if (!IsVarTerm (t3) && !IsIntegerTerm (t3)) {
    Yap_Error(TYPE_ERROR_INTEGER,t3,"write_depth/3");
    return FALSE;
  }
  if (IsVarTerm (t1))
    {
      Term t = MkIntegerTerm (LOCAL_max_depth);
      if (!Yap_unify_constant(t1, t))
	return FALSE;
    }
  else
    LOCAL_max_depth = IntegerOfTerm (t1);
  if (IsVarTerm (t2))
    {
      Term t = MkIntegerTerm (LOCAL_max_list);
      if (!Yap_unify_constant (t2, t))
	return FALSE;
    }
  else
    LOCAL_max_list = IntegerOfTerm (t2);
  if (IsVarTerm (t3))
    {
      Term t = MkIntegerTerm (LOCAL_max_write_args);
      if (!Yap_unify_constant (t3, t))
	return FALSE;
    }
  else
    LOCAL_max_write_args = IntegerOfTerm (t3);
  return TRUE;
}

static Int
dollar_var( USES_REGS1 )
{
  Term in = Deref(ARG1);
  if (IsVarTerm(in)) {
    Term t2;
    if (!IsVarTerm(t2=Deref(ARG2))) {
      if (IsApplTerm(t2) &&
          FunctorOfTerm( t2 ) == FunctorDollarVar ) {
        return Yap_unify(ArgOfTerm(1, t2), ARG1);
      }
      Yap_Error( TYPE_ERROR_COMPOUND, ARG2 , "");
      return false;
    } else {
      Yap_Error( INSTANTIATION_ERROR, ARG2 , "");
    }
  }
  Term t2 = Yap_unify( MkVarTerm(), ARG1);
  Term tv = Yap_MkApplTerm(FunctorDollarVar, 1, &t2);
  return Yap_unify(tv, ARG2);
}


void
Yap_InitWriteTPreds(void)
{
   Yap_InitCPred ("write_term", 2, write_term2, SyncPredFlag);
   Yap_InitCPred ("write_term", 3, write_term3, SyncPredFlag);
  Yap_InitCPred ("write", 1, write1, SyncPredFlag);
  Yap_InitCPred ("write", 2, write2, SyncPredFlag);
  Yap_InitCPred ("writeq", 1, writeq1, SyncPredFlag);
  Yap_InitCPred ("writeq", 2, writeq, SyncPredFlag);
  Yap_InitCPred ("writeln", 1, writeln1, SyncPredFlag);
  Yap_InitCPred ("writeln", 2, writeln, SyncPredFlag);
  Yap_InitCPred ("write_canonical", 1,write_canonical1, SyncPredFlag);
  Yap_InitCPred ("write_canonical", 2, write_canonical, SyncPredFlag);
  Yap_InitCPred ("print", 1, print1, SyncPredFlag);
  Yap_InitCPred ("print", 2, print, SyncPredFlag);
  Yap_InitCPred ("write_depth", 3, p_write_depth, SafePredFlag|SyncPredFlag);
 ;
  Yap_InitCPred ("$VAR", 2, dollar_var, SafePredFlag);
 ;
}
