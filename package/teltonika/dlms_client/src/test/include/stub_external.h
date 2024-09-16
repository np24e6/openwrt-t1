#ifndef __STUB_EXTERNAL_H__  // pragma once generates warnings for this one for some reason
#define __STUB_EXTERNAL_H__  // so using traditional include guard instead

#define PRIVATE __attribute__((weak))
#define PUBLIC __attribute__((weak))
#define MAIN mymain
#define TEST_BREAK break;

#include "stub_uci.h"
#include "stub_blob.h"
#include "stub_blobmsg.h"
#include "stub_uloop.h"
#include "stub_sqlite3.h"
#include "stub_libubus.h"

#include <libubus.h>
#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#define pthread_mutex_lock mypthread_mutex_lock
#define pthread_mutex_unlock mypthread_mutex_unlock

int pthread_mutex_lock(pthread_mutex_t *__mutex);
int pthread_mutex_unlock(pthread_mutex_t *__mutex);

#define ubus_add_uloop myubus_add_uloop

#define strdup mystrdup
#define strcmp mystrcmp
#define strncmp mystrncmp
#define strlcpy mystrlcpy
#define strncat mystrncat
#define strtol mystrtol
#define strlen mystrlen
#define strtok_r mystrtok_r
#define strstr mystrstr

#define malloc mymalloc
#define calloc mycalloc
#define realloc myrealloc
#define free myfree
#define exit myexit

#define snprintf mysnprintf
#define vfprintf myvfprintf
#define snprintf mysnprintf
#define fprintf myfprintf

#define signal mysignal

#ifdef va_start
#undef va_start
#endif
#ifdef va_end
#undef va_end
#endif
#define va_start myva_start
#define va_end myva_end

#define clock_gettime myclock_gettime

void myubus_add_uloop(struct ubus_context *ctx);

void myva_start(va_list ap, ...);
void myva_end(va_list ap);

// apparently this is too hard for ceedling to swallow
//void (*)(int) mysignal(int signum, void (*handler)(int));
typedef void (*signal_handler_fp)(int);
signal_handler_fp mysignal(int signum, signal_handler_fp);

char *mystrdup(const char *s);
int mystrcmp(const char *s1, const char *s2);
int strncmp(const char *s1, const char *s2, size_t n);
size_t mystrlcpy(char *dst, const char *src, size_t size);
char *mystrncat(char *dest, const char *src, size_t n);
long int mystrtol(const char *nptr, char **endptr, int base);

void *mymalloc(size_t size);
void *mycalloc(size_t nmemb, size_t size);
void *myrealloc(void *ptr, size_t size);
void myfree(void *ptr);
void myexit(int status);

char *strtok_r(char *str, const char *delim, char **saveptr);

int mysnprintf(char *str, size_t size, const char *format, ...);
int myvfprintf(void *stream, const char *format, va_list ap);
int mysnprintf(char *str, size_t size, const char *format, ...);
int myfprintf(void *stream, const char *format, ...);

char *strstr(const char *haystack, const char *needle);

int mystrlen(const char *s);

int myclock_gettime(clockid_t clk_id, struct timespec *tp);

#endif
