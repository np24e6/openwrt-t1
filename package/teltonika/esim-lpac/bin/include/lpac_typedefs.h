/**
 * Copyright (c) Giesecke+Devrient Mobile Security GmbH 2023
 */
#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include "tlv_lengths.h"

#define FQDN_MAX_LEN 255
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
	eNoMem = 9,
	eSimBusy = 10
} ErrCode;

typedef struct {
	byte* data;
    size_t data_length;
	unsigned short status_word;
} APDU_response;

// pure plain TLV
typedef struct {
	unsigned short tag;
	byte nTag; // how many TAG bytes
	size_t length;
	byte nLength; // how many LENGTH bytes
} _BerTlv;

typedef struct asn1_list_iterator_s {
    unsigned short elem_tag;
    const byte* buffer;
    uint32_t buffer_size;
    uint32_t buffer_offset;
} asn1_list_iterator_t;

typedef enum octet_format_e {
	PLAIN_OCTET,
	BASE64_OCTET
} octet_format_t;

typedef struct octet_array_s {
	const unsigned char* data;
	uint32_t size;
	octet_format_t format;
} octet_array_t;

typedef struct fqdn_s {
	char fqdn[FQDN_MAX_LEN + 1];
} fqdn_t;

typedef struct subject_key_identifier_s {
	byte subject_key_identifier[SUBJECT_KEY_IDENTIFIER_SIZE];
} subject_key_identifier_t;

typedef struct transaction_id_s {
	byte transaction_id[TRANSACION_ID_MAX_SIZE];
	uint8_t transaction_id_size;
} transaction_id_t;

typedef struct isdp_aid_s {
	byte value[ISDP_AID_SIZE];
} isdp_aid_t;

typedef struct iccid_s {
	byte value[ICCID_SIZE];
} iccid_t;
