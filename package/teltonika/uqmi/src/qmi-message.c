/*
 * uqmi -- tiny QMI support implementation
 *
 * Copyright (C) 2014-2015 Felix Fietkau <nbd@openwrt.org>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "qmi-message.h"
#include "qmi-wds-error-types.c"

#define LOG(...) do { \
	fprintf(stdout, ##__VA_ARGS__); fflush(stdout); \
} while (0);

#define DD LOG("DD[***%s:%d]\n", __func__, __LINE__)

#define ERROR_VERBOSE_STR "QMI call error, reason type"

static uint8_t buf[QMI_BUFFER_LEN];
static unsigned int buf_ofs;

uint8_t *__qmi_get_buf(unsigned int *ofs)
{
	*ofs = buf_ofs;
	return buf;
}

void __qmi_alloc_reset(void)
{
	buf_ofs = 0;
}

void *__qmi_alloc_static(unsigned int len)
{
	void *ret;

	if (buf_ofs + len > sizeof(buf)) {
		fprintf(stderr, "ERROR: static buffer for message data too small\n");
		abort();
	}

	ret = &buf[buf_ofs];
	buf_ofs += len;
	memset(ret, 0, len);
	return ret;
}

char *__qmi_copy_string(void *data, unsigned int len)
{
	char *res;

	res = (char *) &buf[buf_ofs];
	buf_ofs += len + 1;
	memcpy(res, data, len);
	res[len] = 0;
	return res;
}

struct tlv *tlv_get_next(void **buf, unsigned int *buflen)
{
	struct tlv *tlv = NULL;
	unsigned int tlv_len;

	if (*buflen < sizeof(*tlv))
		return NULL;

	tlv = *buf;
	tlv_len = le16_to_cpu(tlv->len) + sizeof(*tlv);
	if (tlv_len > *buflen)
		return NULL;

	*buflen -= tlv_len;
	*buf += tlv_len;
	return tlv;
}

static struct tlv *qmi_msg_next_tlv(struct qmi_msg *qm, int add)
{
	int tlv_len;
	void *tlv;

	if (qm->qmux.service == QMI_SERVICE_CTL) {
		tlv = qm->ctl.tlv;
		tlv_len = le16_to_cpu(qm->ctl.tlv_len);
		qm->ctl.tlv_len = cpu_to_le16(tlv_len + add);
	} else {
		tlv = qm->svc.tlv;
		tlv_len = le16_to_cpu(qm->svc.tlv_len);
		qm->svc.tlv_len = cpu_to_le16(tlv_len + add);
	}

	tlv += tlv_len;

	return tlv;
}

void tlv_new(struct qmi_msg *qm, uint8_t type, uint16_t len, void *data)
{
	struct tlv *tlv;

	tlv = qmi_msg_next_tlv(qm, sizeof(*tlv) + len);
	tlv->type = type;
	tlv->len = cpu_to_le16(len);
	memcpy(tlv->data, data, len);
}

void qmi_init_request_message(struct qmi_msg *qm, QmiService service)
{
	memset(qm, 0, sizeof(*qm));
	qm->marker = 1;
	qm->qmux.service = service;
}

int qmi_complete_request_message(struct qmi_msg *qm)
{
	void *tlv_end = qmi_msg_next_tlv(qm, 0);
	void *msg_start = &qm->qmux;

	qm->qmux.len = cpu_to_le16(tlv_end - msg_start);
	return tlv_end - msg_start + 1;
}

int qmi_check_message_status(void *tlv_buf, unsigned int len)
{
	struct tlv *tlv;
	struct {
		uint16_t status;
		uint16_t code;
	} __packed *status;

	while ((tlv = tlv_get_next(&tlv_buf, &len)) != NULL) {
		if (tlv->type != 2)
			continue;

		if (tlv_data_len(tlv) != sizeof(*status))
			return QMI_ERROR_INVALID_DATA;

		status = (void *) tlv->data;
		if (!status->status)
			return 0;

		return le16_to_cpu(status->code);
	}

	return QMI_ERROR_NO_DATA;
}

void *qmi_msg_get_tlv_buf(struct qmi_msg *qm, int *tlv_len)
{
	void *ptr;
	int len;

	if (qm->qmux.service == QMI_SERVICE_CTL) {
		ptr = qm->ctl.tlv;
		len = qm->ctl.tlv_len;
	} else {
		ptr = qm->svc.tlv;
		len = qm->svc.tlv_len;
	}

	if (tlv_len)
		*tlv_len = len;

	return ptr;
}

char *qmi_wds_verbose_call_end_reason_get_string (QmiWdsVerboseCallEndReasonType type, uint16_t reason)
{
	switch (type) {
	case QMI_WDS_VERBOSE_CALL_END_REASON_TYPE_MIP:
		LOG("%s MIP: \"", ERROR_VERBOSE_STR);
	        return qmi_wds_verbose_call_end_reason_mip_get_string ((QmiWdsVerboseCallEndReasonMip)reason);
	case QMI_WDS_VERBOSE_CALL_END_REASON_TYPE_INTERNAL:
		LOG("%s INTERNAL: \"", ERROR_VERBOSE_STR);
	        return qmi_wds_verbose_call_end_reason_internal_get_string ((QmiWdsVerboseCallEndReasonInternal)reason);
	case QMI_WDS_VERBOSE_CALL_END_REASON_TYPE_CM:
		LOG("%s CM: \"", ERROR_VERBOSE_STR);
	        return qmi_wds_verbose_call_end_reason_cm_get_string ((QmiWdsVerboseCallEndReasonCm)reason);
	case QMI_WDS_VERBOSE_CALL_END_REASON_TYPE_3GPP:
		LOG("%s 3GPP: \"", ERROR_VERBOSE_STR);
	        return qmi_wds_verbose_call_end_reason_3gpp_get_string ((QmiWdsVerboseCallEndReason3gpp)reason);
	case QMI_WDS_VERBOSE_CALL_END_REASON_TYPE_PPP:
		LOG("%s PPP: \"", ERROR_VERBOSE_STR);
	        return qmi_wds_verbose_call_end_reason_ppp_get_string ((QmiWdsVerboseCallEndReasonPpp)reason);
	case QMI_WDS_VERBOSE_CALL_END_REASON_TYPE_EHRPD:
		LOG("%s EHRPD: \"", ERROR_VERBOSE_STR);
	        return qmi_wds_verbose_call_end_reason_ehrpd_get_string ((QmiWdsVerboseCallEndReasonEhrpd)reason);
	case QMI_WDS_VERBOSE_CALL_END_REASON_TYPE_IPV6:
		LOG("%s IPV6: \"", ERROR_VERBOSE_STR);
	        return qmi_wds_verbose_call_end_reason_ipv6_get_string ((QmiWdsVerboseCallEndReasonIpv6)reason);
	default:
        return "Reason type not found";
	}
	return NULL;
}
