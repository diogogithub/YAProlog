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
* File:		myddas_sqlite3.c *
* Last rev:	22/03/05						 *
* mods:									 *
* comments:	Predicates for comunicating with a sqlite3 database system *
*									 *
*************************************************************************/

#if MYDDAS_SQLITE3

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sqlite3.h>
#include "Yap.h"
#include "Yatom.h"
#include "YapText.h"
#include "cut_c.h"
#include "eval.h"
#include "myddas.h"
#ifdef MYDDAS_STATS
#include "myddas_structs.h"
#include "myddas_statistics.h"
#endif
//#include "myddas_wkb2prolog.h"

#define CALL_SQLITE(f)                                                         \
  {                                                                            \
    int i;                                                                     \
    i = sqlite3_##f;                                                           \
    if (i != SQLITE_OK) {                                                      \
      fprintf(stderr, "%s failed with status %d: %s\n", #f, i,                 \
              sqlite3_errmsg(db));                                             \
      exit(1);                                                                 \
    }                                                                          \
  }

#define CALL_SQLITE_EXPECT(f, x)                                               \
  {                                                                            \
    int i;                                                                     \
    i = sqlite3_##f;                                                           \
    if (i != SQLITE_##x) {                                                     \
      fprintf(stderr, "%s failed with status %d: %s\n", #f, i,                 \
              sqlite3_errmsg(db));                                             \
      exit(1);                                                                 \
    }                                                                          \
  }

static Int null_id = 0;

typedef struct result_set {
  sqlite3_stmt *stmt;
  sqlite3 *db;
  char **res_set;
  int nrows;
  int length;
} resultSet;

void Yap_InitMYDDAS_SQLITE3Preds(void);
void Yap_InitBackMYDDAS_SQLITE3Preds(void);

static Int c_sqlite3_connect(USES_REGS1);
static Int c_sqlite3_disconnect(USES_REGS1);
static Int c_sqlite3_number_of_fields(USES_REGS1);
static Int c_sqlite3_get_attributes_types(USES_REGS1);
static Int c_sqlite3_query(USES_REGS1);
static Int c_sqlite3_table_write(USES_REGS1);
static Int c_sqlite3_row(USES_REGS1);
static Int c_sqlite3_row_cut(USES_REGS1);
static Int c_sqlite3_get_fields_properties(USES_REGS1);
static Int c_sqlite3_get_next_result_set(USES_REGS1);
static Int c_sqlite3_get_database(USES_REGS1);
static Int c_sqlite3_change_database(USES_REGS1);

void Yap_InitMYDDAS_SQLITE3Preds(void) {
  /* db_dbect: Host x User x Passwd x Database x dbection x ERROR_CODE */
  Yap_InitCPred("c_sqlite3_connect", 4, c_sqlite3_connect, 0);

  /* db_number_of_fields: Relation x connection x NumberOfFields */
  Yap_InitCPred("c_sqlite3_number_of_fields", 3, c_sqlite3_number_of_fields, 0);

  /* db_get_attributes_types: Relation x TypesList */
  Yap_InitCPred("c_sqlite3_get_attributes_types", 3,
                c_sqlite3_get_attributes_types, 0);

  /* db_query: SQLQuery x ResultSet x conection */
  Yap_InitCPred("c_sqlite3_query", 5, c_sqlite3_query, 0);

  /* db_disconnect: connection */
  Yap_InitCPred("c_sqlite3_disconnect", 1, c_sqlite3_disconnect, 0);

  /* db_table_write: Result Set */
  Yap_InitCPred("c_sqlite3_table_write", 1, c_sqlite3_table_write, 0);

  /* db_get_fields_properties: PredName x connection x PropertiesList*/
  Yap_InitCPred("c_sqlite3_get_fields_properties", 3,
                c_sqlite3_get_fields_properties, 0);

  Yap_InitCPred("c_sqlite3_get_next_result_set", 2,
                c_sqlite3_get_next_result_set, 0);

  /* c_sqlite3_get_database: connection x DataBaseName */
  Yap_InitCPred("c_sqlite3_get_database", 2, c_sqlite3_get_database, 0);

  /* c_sqlite3_change_database: connection x DataBaseName */
  Yap_InitCPred("c_sqlite3_change_database", 2, c_sqlite3_change_database, 0);
}

void Yap_InitBackMYDDAS_SQLITE3Preds(void) {
  /* db_row: ResultSet x Arity x ListOfArgs */
  Yap_InitCPredBackCut("c_sqlite3_row", 3, sizeof(Int), c_sqlite3_row,
                       c_sqlite3_row, c_sqlite3_row_cut, 0);
}

static Int c_sqlite3_connect(USES_REGS1) {

  Term arg_file = Deref(ARG1);
  Term arg_db = ARG4;

  MYDDAS_UTIL_CONNECTION new = NULL;
  sqlite3 *db;

  const char *file = AtomName(AtomOfTerm(arg_file));

  CALL_SQLITE(open(file, &db));

  if (!Yap_unify(arg_db, MkAddressTerm(db)))
    return FALSE;
  else {
    /* Criar um novo no na lista de ligacoes*/
    new = myddas_util_add_connection(db, NULL, API_SQLITE3);

    if (new == NULL) {
#ifdef DEBUG
      fprintf(stderr, "ERROR: ** c_db_my_connect ** Error allocating memory\n");
#endif
      return FALSE;
    }
    return TRUE;
  }
}

#ifdef MYDDAS_STATS
static MYDDAS_STATS_TIME myddas_stat_init_query(sqlite3 *db) {
  MYDDAS_UTIL_connecTION node = myddas_util_search_connection(db);
  MyddasULInt count = 0;

  /* Count the number of querys made to the server */
  MyddasULInt number_querys;
  MYDDAS_STATS_CON_GET_NUMBER_QUERIES_MADE(node, number_querys);
  MYDDAS_STATS_CON_SET_NUMBER_QUERIES_MADE(node, ++number_querys);
  MYDDAS_STATS_CON_GET_NUMBER_QUERIES_MADE_COUNT(node, count);
  MYDDAS_STATS_CON_SET_NUMBER_QUERIES_MADE_COUNT(node, ++count);
  /* Measure time spent by the sqlite3 Server
     processing the SQL Query */
  return myddas_stats_walltime();
  return NULL;
}

static MYDDAS_STATS_TIME
myddas_stat_end_query(MYDDAS_STATS_TIME start) {
  MYDDAS_STATS_TIME diff = NULL;
  /* Measure time spent by the sqlite3 Server
     processing the SQL Query */
  end = myddas_stats_walltime();

  MYDDAS_STATS_INITIALIZE_TIME_STRUCT(diff, time_copy);
  myddas_stats_subtract_time(diff, end, start);
  diff = myddas_stats_time_copy_to_final(diff);

  MYDDAS_FREE(end, struct myddas_stats_time_struct);
  MYDDAS_FREE(start, struct myddas_stats_time_struct);

  MYDDAS_STATS_CON_GET_TOTAL_TIME_DBSERVER(node, total_time);
  /* Automacally updates the MYDDAS_STRUCTURE */
  myddas_stats_add_time(total_time, diff, total_time);
  MYDDAS_STATS_CON_GET_TOTAL_TIME_DBSERVER_COUNT(node, count);
  MYDDAS_STATS_CON_SET_TOTAL_TIME_DBSERVER_COUNT(node, ++count);

  MYDDAS_STATS_TIME time = NULL;
  MYDDAS_STATS_CON_GET_LAST_TIME_DBSERVER(node, time);
  myddas_stats_move_time(diff, time);
  MYDDAS_STATS_CON_GET_LAST_TIME_DBSERVER_COUNT(node, count);
  MYDDAS_STATS_CON_SET_LAST_TIME_DBSERVER_COUNT(node, ++count);
  return diff;
}

/* measure transfer time */
static void myddas_stat_transfer_query(MYDDAS_STATS_TIME diff) {
  /* Measure time spent by the sqlite3 Server
     transferring the result of the last query
     back to the client */
  start = myddas_stats_walltime();
  /* Measure time spent by the sqlite3 Server
     transferring the result of the last query
     back to the client */
  end = myddas_stats_walltime();

  MYDDAS_STATS_INITIALIZE_TIME_STRUCT(diff, time_copy);
  myddas_stats_subtract_time(diff, end, start);
  diff = MYDDAS_STATS_TIME_copy_to_final(diff);

  MYDDAS_FREE(end, struct myddas_stats_time_struct);
  MYDDAS_FREE(start, struct myddas_stats_time_struct);

  MYDDAS_STATS_CON_GET_TOTAL_TIME_TRANSFERING(node, total_time);
  /* Automacally updates the MYDDAS_STRUCTURE */
  myddas_stats_add_time(total_time, diff, total_time);
  MYDDAS_STATS_CON_GET_TOTAL_TIME_TRANSFERING_COUNT(node, count);
  MYDDAS_STATS_CON_SET_TOTAL_TIME_TRANSFERING_COUNT(node, ++count);

  time = NULL;
  MYDDAS_STATS_CON_GET_LAST_TIME_TRANSFERING(node, time);
  MYDDAS_STATS_CON_GET_LAST_TIME_TRANSFERING_COUNT(node, count);
  MYDDAS_STATS_CON_SET_LAST_TIME_TRANSFERING_COUNT(node, ++count);
  myddas_stats_move_time(diff, time);

  /* Measure the number of Rows returned from the server */
  if (res_set != NULL) {
    /* With an INSERT statement, sqlite3_(use or store)_result()
       returns a NULL pointer*/

    /* This is only works if we use sqlite3_store_result */
    MyddasUInt numberRows = sqlite3_num_rows(res_set);
    MyddasUInt rows;
    myddas_stat_transfer_query(diff);

    MYDDAS_STATS_CON_GET_TOTAL_ROWS(node, rows);
    numberRows = numberRows + rows;
    MYDDAS_STATS_CON_SET_TOTAL_ROWS(node, numberRows);
    MYDDAS_STATS_CON_GET_TOTAL_ROWS_COUNT(node, count);
    MYDDAS_STATS_CON_SET_TOTAL_ROWS_COUNT(node, ++count);

    /* Calculate the ammount of data sent by the server */
    MyddasUInt total, number_fields = sqlite3_num_fields(res_set);
    sqlite3_ROW row;
    MyddasULInt i;
    total = 0;
    while ((row = sqlite3_fetch_row(res_set)) != NULL) {
      sqlite3_field_seek(res_set, 0);

      for (i = 0; i < number_fields; i++) {
        if (row[i] != NULL)
          total = total + strlen(row[i]);
      }
    }
    MYDDAS_STATS_CON_SET_LAST_BYTES_TRANSFERING_FROM_DBSERVER(node, total);
    MYDDAS_STATS_CON_GET_LAST_BYTES_TRANSFERING_FROM_DBSERVER_COUNT(node,
                                                                    count);
    MYDDAS_STATS_CON_SET_LAST_BYTES_TRANSFERING_FROM_DBSERVER_COUNT(node,
                                                                    ++count);

    MyddasUInt bytes = 0;
    MYDDAS_STATS_CON_GET_TOTAL_BYTES_TRANSFERING_FROM_DBSERVER(node, bytes);
    total = total + bytes;
    MYDDAS_STATS_CON_SET_TOTAL_BYTES_TRANSFERING_FROM_DBSERVER(node, total);
    MYDDAS_STATS_CON_GET_TOTAL_BYTES_TRANSFERING_FROM_DBSERVER_COUNT(node,
                                                                     count);
    MYDDAS_STATS_CON_SET_TOTAL_BYTES_TRANSFERING_FROM_DBSERVER_COUNT(node,
                                                                     ++count);
  }
}
#endif

/* db_query: SQLQuery x ResultSet x connection */
static Int c_sqlite3_query(USES_REGS1) {
  Term arg_sql_query = Deref(ARG1);
  Term arg_result_set = Deref(ARG2);
  Term arg_db = Deref(ARG3);
  Term arg_mode = Deref(ARG4);
  Term arg_arity = Deref(ARG5);

  const char *sql = AtomName(AtomOfTerm(arg_sql_query));
  const char *mode = AtomName(AtomOfTerm(arg_mode));
  sqlite3 *db = AddressOfTerm(arg_db);
  sqlite3_stmt *stmt;

  #if MYDDAS_STATS
  MYDDAS_STATS_TIME start, end;
  #endif
  
  int length = strlen(sql);
  struct result_set *rs = malloc(sizeof(struct result_set));
  if (!rs)
    return FALSE;
  rs->db = db;

  #if MYDDAS_STATS
  start = myddas_stat_init_query(db);
  #endif

  /* Send query to server and process it */
  if (strcmp(mode, "store_result") != 0) {
    // Leave data for extraction
    printf(" SQL 0: %s\n", sql);
    CALL_SQLITE(prepare_v2(db, sql, -1, &stmt, NULL));
    rs->stmt = stmt;
    rs->res_set = NULL;
    rs->nrows = -1;
    rs->length = sqlite3_column_count(stmt);
    if (!Yap_unify(arg_arity, MkIntegerTerm(rs->length))) {
      free(rs);
      return FALSE;
    }
    if (!Yap_unify(arg_result_set, MkAddressTerm(rs))) {
      free(rs);
      return FALSE;
    }
    return true;
  } else {
    // construct an intermediate table, res_set
    char **res_set;
    char *msg;
    int nrows;

    CALL_SQLITE(get_table(db, sql, &res_set, &nrows, &length, &msg));

    // end = myddas_stat_end_query( start );
    if (res_set == NULL) {
#ifdef DEBUG
      printf("Empty Query!\n");
#endif
      return TRUE;
    }
    // INSERT statements don't return any res_set
    if (nrows == 0) {
      return TRUE;
    }
    if (!Yap_unify(arg_arity, MkIntegerTerm(nrows))) {
      free(rs);
      sqlite3_free_table(res_set);
      return FALSE;
    }
    rs->stmt = NULL;
    rs->res_set = res_set;
    rs->nrows = nrows;
    rs->length = length;
    if (!Yap_unify(arg_result_set, MkAddressTerm(rs))) {
      free(rs);
      sqlite3_free_table(res_set);
      return FALSE;
    }
  }
  return TRUE;
}

static Int c_sqlite3_number_of_fields(USES_REGS1) {
  Term arg_relation = Deref(ARG1);
  Term arg_db = Deref(ARG2);
  Term arg_fields = ARG3;

  const char *relation = AtomName(AtomOfTerm(arg_relation));
  sqlite3 *db = AddressOfTerm(arg_db);
  sqlite3_stmt *stmt;

  char sql[256];

  sprintf(sql, "SELECT * FROM `%s`", relation);

  /* executar a query SQL */
  printf(" SQL 1: %s\n", sql);
  CALL_SQLITE(prepare_v2(db, sql, -1, &stmt, NULL));

  int fields = sqlite3_column_count(stmt);

  CALL_SQLITE(finalize(stmt));

  return Yap_unify(arg_fields, MkIntegerTerm(fields));
}

/* db_get_attributes_types: RelName x connection -> TypesList */
static Int c_sqlite3_get_attributes_types(USES_REGS1) {
  Term arg_relation = Deref(ARG1);
  Term arg_db = Deref(ARG2);
  Term arg_types_list = Deref(ARG3);
  Term list, head;

  const char *relation = AtomName(AtomOfTerm(arg_relation));
  sqlite3 *db = (sqlite3 *)IntegerOfTerm(arg_db);
  char sql[256];
  int row;

  sqlite3_stmt *stmt;
  Int rc = TRUE;

  sprintf(sql, "SELECT * FROM `%s`", relation);

  /* executar a query SQL */
  printf(" SQL 3: %s\n", sql);
  CALL_SQLITE(prepare_v2(db, sql, -1, &stmt, NULL));

  int fields = sqlite3_column_count(stmt);

  list = arg_types_list;

  for (row = 0; row < fields; row++) {
    const char *tm;

    head = HeadOfTerm(list);
    rc = (rc && Yap_unify(head, MkAtomTerm(Yap_LookupAtom(
                                    sqlite3_column_name(stmt, row)))));
    list = TailOfTerm(list);
    head = HeadOfTerm(list);
    list = TailOfTerm(list);

    int type = sqlite3_column_type(stmt, row);
    switch (type) {
    case SQLITE_INTEGER:
      tm = "integer";
      break;
    case SQLITE_FLOAT:
      tm = "real";
      break;
    case SQLITE_TEXT:
      tm = "string";
      break;
    case SQLITE_BLOB:
      tm = "blob";
      break;
    case SQLITE_NULL:
    default:
      tm = "";
      break;
    }
    if (!Yap_unify(head, MkAtomTerm(Yap_LookupAtom(tm))))
      rc = FALSE;
  }

  CALL_SQLITE(finalize(stmt));

  return rc;
}

/* db_disconnect */
static Int c_sqlite3_disconnect(USES_REGS1) {
  Term arg_db = Deref(ARG1);

  sqlite3 *db = (sqlite3 *)IntegerOfTerm(arg_db);

  if ((myddas_util_search_connection(db)) != NULL) {
    myddas_util_delete_connection(db);
    sqlite3_close(db);
    return TRUE;
  } else {
    return FALSE;
  }
}

/* db_table_write: Result Set */
static Int c_sqlite3_table_write(USES_REGS1) {
  /*
    Term arg_res_set = Deref(ARG1);
    sqlite3_RES *res_set = (sqlite3_RES *) IntegerOfTerm(arg_res_set);

    mydas_util_table_write(res_set);
    sqlite3_free_result(res_set);
  */
  return TRUE;
}

static Int c_sqlite3_get_fields_properties(USES_REGS1) {
  Term nome_relacao = Deref(ARG1);
  Term arg_db = Deref(ARG2);
  Term fields_properties_list = Deref(ARG3);
  Term head, list;

  const char *relation = AtomName(AtomOfTerm(nome_relacao));
  char sql[256];
  Int num_fields, i;
  sqlite3 *db = (sqlite3 *)(IntegerOfTerm(arg_db));

  sqlite3_stmt *stmt;

  sprintf(sql, "SELECT * FROM `%s`", relation);

  /* executar a query SQL */
  // printf(" SQL 4: %s\n", sql);
  CALL_SQLITE(prepare_v2(db, sql, -1, &stmt, NULL));

  Functor functor = Yap_MkFunctor(Yap_LookupAtom("property"), 4);

  Term properties[4];

  list = fields_properties_list;

  num_fields = sqlite3_column_count(stmt);

  for (i = 0; i < num_fields; i++) {
    int not_null, prim, auto_inc;

    head = HeadOfTerm(list);

    const char *col = sqlite3_column_name(stmt, i);
    properties[0] = MkAtomTerm(Yap_LookupAtom(col));

    CALL_SQLITE(table_column_metadata(db, NULL, relation, col, NULL, NULL,
                                      &not_null, &prim, &auto_inc));
    properties[1] = MkIntegerTerm(not_null); // Can't be NULL

    properties[2] = MkIntegerTerm(prim); // It''s a primary key

    properties[3] = MkIntegerTerm(auto_inc);

    list = TailOfTerm(list);
    if (!Yap_unify(head, Yap_MkApplTerm(functor, 4, properties))) {
      return FALSE;
    }
  }

  sqlite3_finalize(stmt);

  return TRUE;
}

/* c_sqlite3_get_next_result_set: connection * NextResSet */
static Int c_sqlite3_get_next_result_set(USES_REGS1) {
  Term arg_db = Deref(ARG1);
  Term arg_next_res_set = Deref(ARG2);

  sqlite3 *db = (sqlite3 *)(IntegerOfTerm(arg_db));
  sqlite3_stmt *stmt;

  if ((stmt = sqlite3_next_stmt(db, NULL)) != NULL) {
    struct result_set *rs = malloc(sizeof(struct result_set));
    if (!rs)
      return FALSE;
    rs->stmt = stmt;
    rs->res_set = NULL;
    rs->nrows = -1;
    rs->length = sqlite3_column_count(stmt);
    rs->db = db;
    Yap_unify(arg_next_res_set, MkAddressTerm(rs));
  }
  return TRUE;
}

static Int c_sqlite3_get_database(USES_REGS1) {
  Term arg_con = Deref(ARG1);
  Term arg_database = Deref(ARG2);

  if (!Yap_unify(arg_database, arg_con))
    return FALSE;

  return TRUE;
}

static Int c_sqlite3_change_database(USES_REGS1) {
  /* no-op for now */
  return TRUE;
}

static Int c_sqlite3_row_cut(USES_REGS1) {
  struct result_set *res_set = NULL;

  res_set = AddressOfTerm(CBACK_CUT_ARG(1));
  sqlite3 *db = res_set->db;
  CALL_SQLITE(finalize(res_set->stmt));
  free(res_set);
  return TRUE;
}

#define cvt(s) cvt__(s PASS_REGS)

static Term cvt__(const char *s USES_REGS) {
  return Yap_CharsToTDQ(s, CurrentModule, LOCAL_encoding PASS_REGS);
}

/* db_row: ResultSet x Arity_ListOfArgs x ListOfArgs -> */
static Int c_sqlite3_row(USES_REGS1) {
#ifdef MYDDAS_STATS
  /*   Measure time used by the  */
  /*      c_sqlite3_row function */
  // MYDDAS_STATS_TIME start,end,total_time,diff;
  MyddasULInt count = 0;
  start = myddas_stats_walltime();
#endif
  Term arg_result_set = Deref(ARG1);
  Term arg_arity = Deref(ARG2);
  Term arg_list_args = Deref(ARG3);
  Int rc = TRUE;

  if (IsVarTerm(arg_result_set)) {
    if (!c_sqlite3_query(PASS_REGS1)) {
      cut_fail();
    }
    arg_result_set = Deref(ARG1);
    EXTRA_CBACK_ARG(3, 1) = arg_result_set;
    EXTRA_CBACK_ARG(3, 2) = MkIntegerTerm(0);
  }
  struct result_set *res_set = AddressOfTerm(arg_result_set);

  Term head, list, null_atom[1];
  Int i, arity;
  list = arg_list_args;
  arity = IntegerOfTerm(arg_arity);
  sqlite3 *db = res_set->db;
  if (res_set->stmt == NULL) {
    CACHE_REGS
    Int indx = IntegerOfTerm(EXTRA_CBACK_ARG(3, 2));
    Int rc = true;
// data needs to be copied to Prolog
// row by row
#ifdef MYDDAS_STATS
    MYDDAS_STATS_TIME diff;

    MYDDAS_STATS_INITIALIZE_TIME_STRUCT(diff, time_copy);
#endif
    while (indx / arity < res_set->nrows) {
      for (i = 0; i < arity; i++) {
        /* Ts -> List */
        const char *field = res_set->res_set[indx++];
        head = HeadOfTerm(list);
        list = TailOfTerm(list);
        rc = (rc && Yap_unify(head, cvt(field)));
      }
      if (rc)
        return rc;
    }
#ifdef MYDDAS_STATS
    myddas_stat_transfer_query(diff);
#endif
    cut_fail();
  }
  // busy-waiting
  int res;
  while ((res = sqlite3_step(res_set->stmt)) == SQLITE_BUSY)
    ;
  if (res == SQLITE_DONE) {
    // no more data
    CALL_SQLITE(finalize(res_set->stmt));
    free(res_set);
#ifdef MYDDAS_STATS
    end = myddas_stats_walltime();

    MYDDAS_STATS_INITIALIZE_TIME_STRUCT(diff, time_copy);
    myddas_stats_subtract_time(diff, end, start);
    diff = myddas_stats_time_copy_to_final(diff);

    MYDDAS_FREE(end, struct myddas_stats_time_struct);
    MYDDAS_FREE(start, struct myddas_stats_time_struct);

    MYDDAS_STATS_GET_DB_ROW_FUNCTION(total_time);
    myddas_stats_add_time(total_time, diff, total_time);
    MYDDAS_STATS_GET_DB_ROW_FUNCTION_COUNT(count);
    MYDDAS_STATS_SET_DB_ROW_FUNCTION_COUNT(++count);

    MYDDAS_FREE(diff, struct myddas_stats_time_struct);
#endif          /* MYDDAS_STATS */
    cut_fail(); /* This macro already does a return FALSE */

  } else if (res == SQLITE_ROW) {
    list = arg_list_args;
    Term tf;

    for (i = 0; i < arity; i++) {
      /* convert data types here */
      head = HeadOfTerm(list);
      list = TailOfTerm(list);

      int type = sqlite3_column_type(res_set->stmt, i);
      switch (type) {
      case SQLITE_INTEGER:
        tf = Yap_Mk64IntegerTerm(sqlite3_column_int64(res_set->stmt, i));
        break;
      case SQLITE_FLOAT:
        tf = MkFloatTerm(sqlite3_column_double(res_set->stmt, i));
        break;
      case SQLITE_TEXT:
        tf = MkAtomTerm(Yap_LookupAtom(
            (const char *)sqlite3_column_text(res_set->stmt, i)));
        break;
      case SQLITE_BLOB: {
        size_t bytes = sqlite3_column_bytes(res_set->stmt, i);
        tf = Yap_AllocExternalDataInStack(EXTERNAL_BLOB, bytes);
        memcpy(ExternalBlobFromTerm(tf), sqlite3_column_blob(res_set->stmt, i),
               bytes);
      } break;
      case SQLITE_NULL:
        null_atom[0] = MkIntegerTerm(null_id++);
        tf = Yap_MkApplTerm(Yap_MkFunctor(Yap_LookupAtom("null"), 1), 1,
                            null_atom);
        break;
      }
      if (!Yap_unify(head, tf))
        rc = FALSE;
    }
#ifdef MYDDAS_STATS
    end = myddas_stats_walltime();

    myddas_stats_subtract_time(diff, end, start);
    diff = myddas_stats_time_copy_to_final(diff);

    MYDDAS_FREE(end, struct myddas_stats_time_struct);
    MYDDAS_FREE(start, struct myddas_stats_time_struct);

    MYDDAS_STATS_GET_DB_ROW_FUNCTION(total_time);
    myddas_stats_add_time(total_time, diff, total_time);
    MYDDAS_STATS_GET_DB_ROW_FUNCTION_COUNT(count);
    MYDDAS_STATS_SET_DB_ROW_FUNCTION_COUNT(++count);

    MYDDAS_FREE(diff, struct myddas_stats_time_struct);
#endif /* MYDDAS_STATS */
  } else {
    Yap_Error(SYSTEM_ERROR_INTERNAL, TermNil, "sqlite3: %s",
              sqlite3_errmsg(db));
  }
  return rc;
}

#else

void Yap_InitMYDDAS_SQLITE3Preds(void);
void Yap_InitBackMYDDAS_SQLITE3Preds(void);

void Yap_InitMYDDAS_SQLITE3Preds(void) {}
void Yap_InitBackMYDDAS_SQLITE3Preds(void) {}

#endif
