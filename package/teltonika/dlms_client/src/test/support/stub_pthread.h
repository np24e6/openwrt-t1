#ifndef STUB_PTHREAD_H
#define STUB_PTHREAD_H

#define pthread_create	      pthread_create_orig
#define pthread_mutex_init    pthread_mutex_init_orig
#define pthread_mutex_destroy pthread_mutex_destroy_orig
// #define pthread_mutex_lock    pthread_mutex_lock_orig
// #define pthread_mutex_unlock  pthread_mutex_unlock_orig
#include <pthread.h>
#undef pthread_create
#undef pthread_mutex_init
#undef pthread_mutex_destroy
// #undef pthread_mutex_lock
// #undef pthread_mutex_unlock

int pthread_create(pthread_t *thread, const pthread_attr_t *attr, void *(*start_routine)(void *), void *arg);
int pthread_mutex_init(pthread_mutex_t *__mutex, const pthread_mutexattr_t *__mutexattr);
int pthread_mutex_destroy(pthread_mutex_t *__mutex);
// int pthread_mutex_lock(pthread_mutex_t *__mutex);
// int pthread_mutex_unlock(pthread_mutex_t *__mutex);

#endif
