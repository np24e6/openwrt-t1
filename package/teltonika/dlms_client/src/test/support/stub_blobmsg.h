#ifndef STUB_BLOBMSG_H
#define STUB_BLOBMSG_H

#define blobmsg_add_field   blobmsg_add_field_orig
#define blobmsg_add_string  blobmsg_add_string_orig
#define blobmsg_add_u32	    blobmsg_add_u32_orig
#define blobmsg_get_string  blobmsg_get_string_orig
#define blobmsg_get_bool    blobmsg_get_bool_orig
#define blobmsg_get_u32	    blobmsg_get_u32_orig
#define blobmsg_parse	    blobmsg_parse_orig
#define blobmsg_data	    blobmsg_data_orig
#define blobmsg_len	    blobmsg_len_orig
#define blobmsg_data_len    blobmsg_data_len_orig
#define blobmsg_check_array blobmsg_check_array_orig
#include <libubox/blobmsg.h>
#undef blobmsg_add_field
#undef blobmsg_add_string
#undef blobmsg_add_u32
#undef blobmsg_get_string
#undef blobmsg_get_bool
#undef blobmsg_get_u32
#undef blobmsg_parse
#undef blobmsg_data
#undef blobmsg_len
#undef blobmsg_data_len
#undef blobmsg_check_array

int blobmsg_add_field(struct blob_buf *buf, int type, const char *name, const void *data, unsigned int len);
int blobmsg_add_string(struct blob_buf *buf, const char *name, const char *string);
int blobmsg_add_u32(struct blob_buf *buf, const char *name, uint32_t val);
char *blobmsg_get_string(struct blob_attr *attr);
bool blobmsg_get_bool(struct blob_attr *attr);
uint32_t blobmsg_get_u32(struct blob_attr *attr);
int blobmsg_parse(const struct blobmsg_policy *policy, int policy_len, struct blob_attr **tb, void *data,
		  unsigned int len);
void *blobmsg_data(const struct blob_attr *attr);
size_t blobmsg_len(const struct blob_attr *attr);
size_t blobmsg_data_len(const struct blob_attr *attr);
int blobmsg_check_array(const struct blob_attr *attr, int type);

#endif // STUB_BLOBMSG_H
