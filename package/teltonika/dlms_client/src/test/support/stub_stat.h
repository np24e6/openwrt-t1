#ifndef STUB_STAT_H
#define STUB_STAT_H

struct stat;
typedef unsigned int __mode_t;

#define stat  stat_orig
#define mkdir mkdir_orig
#include <sys/stat.h>
#undef stat
#undef mkdir

int stat(const char *__restrict __file, struct stat *__restrict __buf);
int mkdir(const char *__path, __mode_t __mode);

#endif // STUB_STAT_H
