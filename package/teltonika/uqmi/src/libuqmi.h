#ifndef __LIBUQMI_H
#define __LIBUQMI_H

#include "uqmi.h"
#include "commands.h"

void dump_packet(const char *prefix, void *ptr, int len);

void keep_client_id(struct qmi_dev *qmi, const char *optarg);
void release_client_id(struct qmi_dev *qmi, const char *optarg);
#endif
