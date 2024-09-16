#ifndef _LOG_H_
#define _LOG_H_

#define LOG(fmt, ...)                                                                                        \
	do {                                                                                                 \
		fprintf(stdout, fmt "\n", ##__VA_ARGS__);                                                    \
		fflush(stdout);                                                                              \
	} while (0);

#define ERR(fmt, ...)                                                                                        \
	do {                                                                                                 \
		fprintf(stderr, "[%30s:%4d] error: " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);           \
		fflush(stderr);                                                                              \
	} while (0);

#define DBG(fmt, ...)                                                                                        \
	if (g_debug) {                                                                                       \
		do {                                                                                         \
			fprintf(stdout, "[%30s:%4d] " fmt "\n", __func__, __LINE__, ##__VA_ARGS__);          \
			fflush(stdout);                                                                      \
		} while (0);                                                                                 \
	}

#endif // _LOG_H_
