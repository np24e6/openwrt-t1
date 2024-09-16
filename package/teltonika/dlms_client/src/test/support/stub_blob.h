#ifndef STUB_BLOB_H
#define STUB_BLOB_H

#define blob_data     blob_data_orig
#define blob_len      blob_len_orig
#define blob_buf_init blob_buf_init_orig
#define blob_buf_free blob_buf_free_orig
#define blob_pad_len  blob_pad_len_orig
#define blob_next     blob_next_orig
#include <blob.h>
#undef blob_data
#undef blob_len
#undef blob_buf_init
#undef blob_buf_free
#undef blob_pad_len
#undef blob_next

void *blob_data(const struct blob_attr *attr);
size_t blob_len(const struct blob_attr *attr);
int blob_buf_init(struct blob_buf *buf, int id);
void blob_buf_free(struct blob_buf *buf);
size_t blob_pad_len(const struct blob_attr *attr);
struct blob_attr *blob_next(const struct blob_attr *attr);

#endif // STUB_BLOB_H
