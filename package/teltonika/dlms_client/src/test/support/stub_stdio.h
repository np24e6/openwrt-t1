#ifndef STUB_STDIO_H
#define STUB_STDIO_H

#define fopen	fopen_orig
#define fgets	fgets_orig
#define fprintf fprintf_orig
#define fflush	fflush_orig
#define fclose	fclose_orig
#define remove	remove_orig
#include <stdio.h>
#undef fopen
#undef fgets
#undef fprintf
#undef fflush
#undef fclose
#undef remove

FILE *fopen(const char *__restrict __filename, const char *__restrict __modes);
char *fgets(char *__restrict __s, int __n, FILE *__restrict __stream);
int fprintf(FILE *__restrict __stream, const char *__restrict __format, ...);
int fflush(FILE *__stream);
int fclose(FILE *__stream);
int remove(const char *__filename);

#endif // STUB_STDIO_H
