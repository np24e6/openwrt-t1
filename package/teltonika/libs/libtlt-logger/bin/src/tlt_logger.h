#ifndef LOGGER_H
#define LOGGER_H

#include <stdarg.h>

#define MAX_LOG_MSG 5
#define SPAM_WARNING "(Will stop printing error messages due to log spam. "	  \
		     "That does not mean that the problem is no longer present!)"

typedef enum {
	L_DEBUG,
	L_INFO,
	L_NOTICE,
	L_WARNING,
	L_ERROR,
	L_CRIT,
	L_ALERT,
	L_EMERG
} log_level_type;

enum {
    L_TYPE_SYSLOG =  1,
    L_TYPE_STDOUT = (1 << 1),
};

typedef enum {
	LOG_ID_OK,
	LOG_ID_ERR_1,
	LOG_ID_ERR_2,
	LOG_ID_ERR_3,
	LOG_ID_ERR_4,
	LOG_ID_ERR_5
} log_id;

typedef struct log_count {
	/* Count how many times the message has been printed */
	int count;
	/* Unique message id that helps to determine if
	   a new error message has been encountered */
	log_id id;
} log_counter;

int logger_init(log_level_type _min_level, int logger_type, const char *prog_name);

void _log(log_level_type level, const char *fmt, ...)
	__attribute__((format (printf, 2, 3)));

/**
 * Logs defined log message using counter that counts
 * how many times it should be printed to reduce log spam
 * @param counter counter structure
 * @param id unique message id
 * @param level log message level
 * @param fmt string format
 */
void _log_with_count(log_counter *counter, log_id id, log_level_type level, char *fmt, ...)
	__attribute__((format (printf, 4, 5)));

#define log(level, fmt, ...) _log(level, fmt"\n", ##__VA_ARGS__)

#define log_count(counter, id, level, fmt, ...) \
	_log_with_count(counter, id, level, fmt"\n", ##__VA_ARGS__)

#define DEBUG(fmt, ...) \
	log(L_DEBUG, fmt, ##__VA_ARGS__)

#define INFO(fmt, ...) \
	log(L_INFO, fmt, ##__VA_ARGS__)

#define NOTICE(fmt, ...) \
	log(L_NOTICE, fmt, ##__VA_ARGS__)

#define WARN(fmt, ...) \
	log(L_WARNING, fmt, ##__VA_ARGS__)

#define ERROR(fmt, ...) \
	log(L_ERROR, fmt, ##__VA_ARGS__)

#define CRIT(fmt, ...) \
	log(L_CRIT, fmt, ##__VA_ARGS__)

#define PANIC(fmt, ...) 				\
	do { 						\
		log(L_CRIT, fmt, ##__VA_ARGS__);	\
		exit(EXIT_FAILURE);			\
	} while (0)

#endif
