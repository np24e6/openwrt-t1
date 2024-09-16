#ifndef STUB_SQLITE3_H
#define STUB_SQLITE3_H

#define sqlite3_bind_blob sqlite3_bind_blob_orig
#define sqlite3_bind_int sqlite3_bind_int_orig
#define sqlite3_bind_null sqlite3_bind_null_orig
#define sqlite3_bind_text sqlite3_bind_text_orig
#define sqlite3_busy_timeout sqlite3_busy_timeout_orig
#define sqlite3_close sqlite3_close_orig
#define sqlite3_errmsg sqlite3_errmsg_orig
#define sqlite3_exec sqlite3_exec_orig
#define sqlite3_finalize sqlite3_finalize_orig
#define sqlite3_open sqlite3_open_orig
#define sqlite3_prepare_v2 sqlite3_prepare_v2_orig
#define sqlite3_reset sqlite3_reset_orig
#define sqlite3_step sqlite3_step_orig
#define sqlite3_stmt sqlite3_stmt_orig
#define sqlite3_column_int sqlite3_column_int_orig
#define sqlite3_clear_bindings sqlite3_clear_bindings_orig
#define sqlite3_threadsafe sqlite3_threadsafe_orig
#define sqlite3_close_v2 sqlite3_close_v2_orig
#define sqlite3 sqlite3_orig
#include <sqlite3.h>
#undef sqlite3_bind_blob
#undef sqlite3_bind_int
#undef sqlite3_bind_null
#undef sqlite3_bind_text
#undef sqlite3_busy_timeout
#undef sqlite3_close
#undef sqlite3_errmsg
#undef sqlite3_exec
#undef sqlite3_finalize
#undef sqlite3_open
#undef sqlite3_prepare_v2
#undef sqlite3_reset
#undef sqlite3_step
#undef sqlite3_stmt
#undef sqlite3_column_int
#undef sqlite3_clear_bindings
#undef sqlite3_threadsafe
#undef sqlite3_close_v2
#undef sqlite3

typedef struct {
	int x;
} sqlite3; // make cmock happy
typedef struct {
	int x;
} sqlite3_stmt; // make cmock happy

int sqlite3_bind_blob(sqlite3_stmt *stmt, int index, const void *value, int valueBytes, void(*destructor)(void *));
int sqlite3_bind_int(sqlite3_stmt *stmt, int index, int value);
int sqlite3_bind_null(sqlite3_stmt *stmt, int index);
int sqlite3_bind_text(sqlite3_stmt *stmt, int index, const char *value, int valueBytes, void(*destructor)(void *));
int sqlite3_busy_timeout(sqlite3 *db, int timeout);
int sqlite3_close(sqlite3 *db);
const char *sqlite3_errmsg(sqlite3 *db);
int sqlite3_exec(sqlite3 *db, const char *sql, int (*callback)(void*,int,char**,char**), void *p, char **errmsg);
int sqlite3_finalize(sqlite3_stmt *pStmt);
int sqlite3_open(const char *path, sqlite3 **db);
int sqlite3_prepare_v2(sqlite3 *db, const char *zSql, int nByte, sqlite3_stmt **ppStmt, const char **pzTail);
int sqlite3_reset(sqlite3_stmt *pStmt);
int sqlite3_step(sqlite3_stmt *pStmt);
int sqlite3_column_int(sqlite3_stmt*, int iCol);
int sqlite3_clear_bindings(sqlite3_stmt*);
int sqlite3_threadsafe(void);
int sqlite3_close_v2(sqlite3*);

#endif
