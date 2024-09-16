#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include <libubus.h>
#include <gsm/modem_api.h>

typedef enum {
	URC_VALUE,
	URC_MODEM_ID,
	URC_VALUE_T_MAX,
} urc_value_t;

struct urc_double_data {
	double value;
	char *modem_id;
};

struct urc_int_data {
	int32_t value;
	char *modem_id;
};

struct urc_string_data {
	char *value;
	char *modem_id;
};

typedef enum {
	EMPTY_MODEM_ID_VALUE,
	EMPTY_T_MAX,
} urc_empty_t;

struct urc_empty_data {
	const char *modem_id;
};

typedef enum {
	PIN_STATE_URC_VALUE,
	PIN_STATE_URC_MODEM_ID,
	__PIN_STATE_URC_MAX,
} urc_pin_state_t;

typedef enum {
	NET_STATE_VALUE,
	NET_STATE_MODEM_ID,
	__NET_STATE_MAX,
} urc_net_state_t;

typedef enum {
	QIND_PERCENT_VALUE,
	QIND_ERROR_VALUE,
	QIND_STATE_VALUE,
	QIND_MODEM_ID_VALUE,
	QIND_T_MAX,
} urc_qind_t;

typedef enum {
	CLIP_SUBADDRESS_VALUE,
	CLIP_SA_TYPE_VALUE,
	CLIP_ALPHA_VALUE,
	CLIP_NUMBER_VALUE,
	CLIP_TYPE_VALUE,
	CLIP_VALIDITY_VALUE,
	CLIP_MODEM_ID_VALUE,
	CLIP_T_MAX,
} urc_clip_t;

typedef enum {
	RING_TYPE_VALUE,
	RING_MODEM_ID_VALUE,
	RING_T_MAX,
} urc_ring_t;

typedef enum {
	SND_SMS_CNT_VALUE,
	SND_SMS_NUM_VALUE,
	SND_SMS_MODEM_ID_VALUE,
	SND_SMS_T_MAX,
} urc_snd_sms_t;

typedef enum {
	CUSD_RESP_VALUE,
	CUSD_STATE_VALUE,
	CUSD_SCHEME_VALUE,
	CUSD_T_MAX,
} urc_cusd_t;

typedef enum {
	NEW_MDM_NAME,
	NEW_MDM_ID,
	NEW_MDM_MAX,
} new_mdm_t;

typedef enum {
	SIM_STAT_VALUE,
	SIM_STAT_T_MAX,
} urc_sim_status_t;

typedef enum {
	EMM_ERR_VAL,
	EMM_ERR_T_MAX,
} urc_emm_err_t;

typedef enum {
	ESM_ERR_VAL,
	ESM_ERR_T_MAX,
} urc_esm_err_t;

typedef enum {
	LURC_SUCCESS,
	LURC_NOT_SUPPORTED,
	LURC_ERROR_PARSE,
	LURC_ERROR,
} lurc_err_t;

struct pin_state_t {
	enum pin_state_id state;
	const char *modem_id;
};

struct net_state_t {
	enum net_reg_stat_id state;
	const char *modem_id;
};

struct cusd_t {
	const char *response;
	uint32_t state;
	uint32_t coding_scheme;
};

struct snd_sms_t {
	uint32_t used;
	const char *number;
	const char *modem_id;
};

struct clip_t {
	const char *subaddress;
	uint32_t sa_type;
	const char *alpha;
	const char *number;
	enum cli_valid_id validity;
	enum call_type_id type;
	const char *modem_id;
};

struct ring_t {
	enum ring_type_id type;
	const char *modem_id;
};

struct qind_t {
	uint32_t percent;
	uint32_t error;
	enum fota_state_t state;
	const char *modem_id;
};

struct modem_info {
	const char *ubus_path;
	uint32_t ubus_id;
	uint32_t modem_num;
};

struct sim_status_t {
	uint32_t sim_status;
};

struct emm_err_t {
	enum emm_cause_id cause_id;
	const char *modem_id;
};

struct esm_err_t {
	enum esm_cause_id cause_id;
	const char *modem_id;
};

struct evt_parser {
	lurc_err_t (*cache_type_cb)(struct blob_attr *msg, const struct blobmsg_policy *msg_policy,
				    void *value);
	const struct blobmsg_policy *msg_policy;
	uint32_t n_policy;
	uint16_t size;
};

evt_type_t parse_event_type(const char *urc);
struct evt_parser *get_evt_parser(evt_type_t evt_type);
extern const char *g_event_types[];

#ifdef __cplusplus
}
#endif
