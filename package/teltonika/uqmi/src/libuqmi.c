#include <stdio.h>
#include <stdlib.h>

#include "libuqmi.h"

#ifdef DEBUG_PACKET
void dump_packet(const char *prefix, void *ptr, int len)
{
	unsigned char *data = ptr;
	int i;

	fprintf(stderr, "%s:", prefix);
	for (i = 0; i < len; i++)
		fprintf(stderr, " %02x", data[i]);
	fprintf(stderr, "\n");
}
#endif

void keep_client_id(struct qmi_dev *qmi, const char *optarg)
{
	QmiService svc = qmi_service_get_by_name(optarg);
	if (svc < 0) {
		fprintf(stderr, "Invalid service %s\n", optarg);
		exit(1);
	}
	qmi_service_get_client_id(qmi, svc);
}

void release_client_id(struct qmi_dev *qmi, const char *optarg)
{
	QmiService svc = qmi_service_get_by_name(optarg);
	if (svc < 0) {
		fprintf(stderr, "Invalid service %s\n", optarg);
		exit(1);
	}
	qmi_service_release_client_id(qmi, svc);
}
