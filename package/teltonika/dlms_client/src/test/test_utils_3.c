
#ifndef TEST
#define TEST
#endif
#ifdef TEST

#include "unity.h"
#include "utils.h"
#include "mock_tlt_logger.h"

#include "mock_stub_external.h"
#include "mock_stub_utils_3.h"

#include "mock_converters.h"

#undef strlen

void test_utl_add_error_message(void)
{
	char *data = "";
	char str[256] = { 0 };

	char *expected_str = "\"device_one\":";
	utl_append_obj_name_Expect(&data, "device_one");
	utl_append_obj_name_ReturnMemThruPtr_data(&expected_str, sizeof(expected_str));

        expected_str = "{\"error\": 4, \"result\": \"Connection error\"}";
	mysnprintf_ExpectAndReturn(str, sizeof(str), "{\"error\": %d, \"result\": \"%s\"}", 5);
	mysnprintf_ReturnMemThruPtr_str(expected_str, strlen(expected_str));

	utl_append_to_str_Expect(&data, expected_str);
        expected_str = "\"device_one\": {\"error\": 4, \"result\": \"Connection error\"}";
	utl_append_to_str_ReturnThruPtr_destination(&expected_str);

	utl_add_error_message(&data, "device_one", "Connection error", 4);

        TEST_ASSERT_EQUAL_STRING(data, "\"device_one\": {\"error\": 4, \"result\": \"Connection error\"}");
}

#endif
