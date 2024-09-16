#pragma once
#include "lpac_typedefs.h"

ErrCode LPA__get_eid(const char* port_name, unsigned char** result, size_t* result_size);
ErrCode LPA__get_profiles(const char* port_name, unsigned char** result, size_t* result_size);
ErrCode LPA__enable_profile(const char* port_name, const char* iccid, unsigned char** result, size_t* result_size);
ErrCode LPA__disable_profile(const char* port_name, const char* iccid, unsigned char** result, size_t* result_size);
ErrCode LPA__delete_profile(const char* port_name, const char* iccid, unsigned char** result, size_t* result_size);
ErrCode LPA__start_rsp(const char* port_name, const char* activation_code, unsigned char** result, size_t* result_size);
ErrCode LPA__get_addresses(const char* port_name, unsigned char** result, size_t* result_size);
ErrCode LPA__set_smdp_address(const char* port_name, const char* smdp_address, unsigned char** result, size_t* result_size);
ErrCode LPA__set_profile_nickname(const char* port_name, const char* iccid, const char* nick_name, unsigned char** result, size_t* result_size);
ErrCode LPA__get_notifications(const char* port_name, unsigned char** result, size_t* result_size);
ErrCode LPA__process_notifications(const char* port_name, unsigned char** result, size_t* result_size);
