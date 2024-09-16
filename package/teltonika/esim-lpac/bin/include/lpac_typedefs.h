#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#define nullptr 	NULL

typedef unsigned char byte;

typedef enum  {
	eOk = 0,
	eFatal = 1,
	eNotSupported = 2,
	eNotImpl = 3,
	eBadArg = 4,
	eJsonParseError = 5,
	eSessionCancelled = 6,
	eNotEnoughBuffer = 7,
	eNoData = 8,
	eNoMem = 9
} ErrCode;



