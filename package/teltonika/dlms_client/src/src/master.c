#include "master.h"

#define SQL_TABLE	  "dlms_data"
#define DB_PATH		  "/tmp/dlms.db"
#define DB_TIMEOUT	  30000
#define DB_MAX_PAGE_COUNT 1024 //!< Set maximum number of pages in database (each page is 4096B)

#define DB_DELETE_COUNT   2000
#define DB_DELETE_QUERY   "DELETE FROM " SQL_TABLE " WHERE id IN (SELECT id FROM dlms_data ORDER BY id ASC LIMIT " UTL_STR(DB_DELETE_COUNT) ")"
#define DB_INSERT_QUERY	  "INSERT INTO " SQL_TABLE " VALUES (?, ?, ?, ?, ?)"
#define DB_QUERY_MAX_SIZE -1 //!< up to the first zero terminator

PRIVATE void *mstr_thread_routine(void *p);

PUBLIC int mstr_create_db(master *m)
{
	if (!sqlite3_threadsafe()) {
		log(L_ERROR, "SQLite3 is not thread-safe!\n");
		goto err0;
	}

	if (sqlite3_open(DB_PATH, &m->db)) {
		log(L_ERROR, "Failed to open database ('%s')\n", DB_PATH);
		goto err0;
	}

	if (sqlite3_busy_timeout(m->db, DB_TIMEOUT)) {
		log(L_ERROR, "Failed to set DB timeout: %s\n", sqlite3_errmsg(m->db));
		goto err1;
	}

	const char *query = "CREATE TABLE IF NOT EXISTS dlms_data ("
			    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
			    "time TIMESTAMP,"
			    "name TEXT,"
			    "size INT,"
			    "data TEXT"
			    ");";

	if (sqlite3_exec(m->db, query, NULL, NULL, NULL)) {
		log(L_ERROR, "Failed to CREATE TABLE dlms_data: %s\n", sqlite3_errmsg(m->db));
		goto err1;
	}

	query = "CREATE TABLE IF NOT EXISTS sent_id_table ("
		"id INTEGER PRIMARY KEY AUTOINCREMENT,"
		"sender_id TEXT,"
		"last_sent_id INT"
		");";

	if (sqlite3_exec(m->db, query, NULL, NULL, NULL)) {
		log(L_ERROR, "Failed to CREATE TABLE sent_id_table: %s\n", sqlite3_errmsg(m->db));
		goto err1;
	}

	if (sqlite3_exec(m->db, "PRAGMA max_page_count = " UTL_STR(DB_MAX_PAGE_COUNT), NULL, NULL, NULL)) {
		log(L_ERROR, "Failed to set max page count: %s\n", sqlite3_errmsg(m->db));
		goto err1;
	}

	if (sqlite3_prepare_v2(m->db, DB_INSERT_QUERY, DB_QUERY_MAX_SIZE, &m->stmt_insert, NULL) != SQLITE_OK) {
		log(L_ERROR, "Failed to prepare SQL statement: %s\n", sqlite3_errmsg(m->db));
		goto err1;
	}

	return 0;
err1:
	sqlite3_close_v2(m->db);
err0:
	return 1;
}

PUBLIC int mstr_initialize_cosem_groups(master *m)
{
	static pthread_t threads[MAX_COSEM_GROUPS_COUNT] = { 0 };
	for (size_t i = 0; i < m->cosem_group_count; i++) {
		if (pthread_create(&threads[i], NULL, mstr_thread_routine, m->cosem_groups[i])) {
			log(L_ERROR, "Failed to initialize COSEM group thread: %s\n",
			    UTL_SAFE_STR(m->cosem_groups[i]->name));
			return 1;
		}
	}

	return 0;
}

PRIVATE void *mstr_thread_routine(void *p)
{
	cosem_group *group = (cosem_group *)p;
	time_t t0	   = time(NULL);
	unsigned long tn   = 0;

	while (1) {
		int rc	   = 0;
		char *data = cg_read_group_codes(group, &rc);
		if (rc || !data) {
			log(L_ERROR, "('%d', '%s') Not writing to the database: %s", group->id,
			    UTL_SAFE_STR(group->name), UTL_SAFE_STR(data));
			goto end;
		}

		mstr_write_group_data_to_db(group, data);
	end:
		free(data);
		utl_smart_sleep(&t0, &tn, group->interval);
		TEST_BREAK
	}

	return NULL; // should never happen
}

PUBLIC void mstr_write_group_data_to_db(cosem_group *group, char *data)
{
	log(L_DEBUG, "('%d', '%s') Writing to database: %s", group->id, UTL_SAFE_STR(group->name), data);

	uint8_t i = 1;
	if (sqlite3_bind_null(g_master->stmt_insert, i++) != SQLITE_OK ||
	    sqlite3_bind_int(g_master->stmt_insert, i++, time(NULL)) != SQLITE_OK ||
	    sqlite3_bind_text(g_master->stmt_insert, i++, group->name, group->name ? strlen(group->name) : 0,
			      SQLITE_STATIC) != SQLITE_OK ||
	    sqlite3_bind_int(g_master->stmt_insert, i++, strlen(data)) != SQLITE_OK ||
	    sqlite3_bind_text(g_master->stmt_insert, i++, data, strlen(data), SQLITE_STATIC) !=
		    SQLITE_OK) {
		log(L_ERROR, "Failed to bind DB parameter: %s\n", sqlite3_errmsg(g_master->db));
		goto err;
	}

	int rc = sqlite3_step(g_master->stmt_insert);
	if (rc == SQLITE_FULL) {
		if (sqlite3_exec(g_master->db, DB_DELETE_QUERY, NULL, NULL, NULL) != SQLITE_OK) {
			log(L_ERROR, "DELETE failed: %s\n", sqlite3_errmsg(g_master->db));
			goto err;
		}

		log(L_DEBUG, "Repeat INSERT after DELETE");

		if (sqlite3_step(g_master->stmt_insert) != SQLITE_DONE) {
			log(L_ERROR, "INSERT failed after DELETE: %s", sqlite3_errmsg(g_master->db));
			goto err;
		}
	} else if (rc != SQLITE_DONE) {
		log(L_ERROR, "INSERT failed: %s\n", sqlite3_errmsg(g_master->db));
		goto err;
	}

err:
	sqlite3_reset(g_master->stmt_insert);
	sqlite3_clear_bindings(g_master->stmt_insert);
}

PUBLIC void mstr_db_free(master *m)
{
	if (!m) {
		return;
	}

	sqlite3_finalize(m->stmt_insert);
	sqlite3_close_v2(m->db);
}
