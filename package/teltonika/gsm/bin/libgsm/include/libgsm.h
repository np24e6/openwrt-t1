#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include <libubus.h>
#include <gsm/modem_band.h>
#include <gsm/modem_api.h>

#define DEFAULT_MOD_NUM 0
#define READ_ALL_SMS	-1

typedef enum {
	LGSM_SUCCESS,
	LGSM_ERROR_UBUS,
	LGSM_MODEM_ERROR,
	LGSM_NOT_SUPPORTED,
	LGSM_ERROR_PARSE,
	LGSM_ERROR,
} lgsm_err_t;

typedef enum {
	LGSM_MDM_STATS,
	LGSM_MDM_MAX,
} lgsm_mdm_t;

typedef enum {
	LGSM_MDM_ATTR_NUM,
	LGSM_MDM_ATTR_MAX,
} lgsm_mdm_attr_t;

typedef enum {
	LGSM_INFO_NAME,
	LGSM_INFO_MODEL,
	LGSM_INFO_MANUF,
	LGSM_INFO_PROD_NUM,
	LGSM_INFO_DRIVER,
	LGSM_INFO_USB_ID,
	LGSM_INFO_VIDPID,
	LGSM_INFO_TTY,
	LGSM_INFO_GPS_TTY,
	LGSM_INFO_DATA_TTY,
	LGSM_INFO_BAUDRATE,
	LGSM_INFO_AUX,
	LGSM_INFO_SIMCOUNT,
	LGSM_INFO_BAND,
	LGSM_INFO_SERVICE,
	LGSM_INFO_CACHE,
	LGSM_INFO_MFUNC,
	LGSM_INFO_FEATURE_OPER_SCAN,
	LGSM_INFO_FEATURE_IPV6,
	LGSM_INFO_FEATURE_VOLTE,
	LGSM_INFO_FEATURE_MUTLI_APN,
	LGSM_INFO_FEATURE_DPO,
	LGSM_INFO_FEATURE_RMNET,
	LGSM_INFO_FEATURE_ECM,
	LGSM_INFO_FEATURE_DYNAMIC_MTU,
	LGSM_INFO_FEATURE_AUTO_IMS,
	LGSM_INFO_FEATURE_EXTENDED_TIMEOUT,
	LGSM_INFO_FEATURE_DHCP_FILTER,
	LGSM_INFO_BUILTIN,
	LGSM_INFO_FEATURE_CSD,
	LGSM_INFO_MAX,
} lgsm_info_t;

typedef enum {
	LGSM_CACHE_FIRMWARE,
	LGSM_CACHE_IMEI,
	LGSM_CACHE_SER_NUM,
	LGSM_CACHE_IMSI,
	LGSM_CACHE_PIN_STATE,
	LGSM_CACHE_PIN_STR,
	LGSM_CACHE_ICCID,
	LGSM_CACHE_VOLTE_ST,
	LGSM_CACHE_NET_MODE,
	LGSM_CACHE_NET_MODE_SEL,
	LGSM_CACHE_BAND,
	LGSM_CACHE_OP_FORMAT,
	LGSM_CACHE_OP_NAME,
	LGSM_CACHE_REG_STAT,
	LGSM_CACHE_REG_LAC,
	LGSM_CACHE_REG_CI,
	LGSM_CACHE_REG_ACT,
	LGSM_CACHE_SIM_PIN1,
	LGSM_CACHE_SIM_PUK1,
	LGSM_CACHE_TEMPERATURE,
	LGSM_CACHE_RSSI,
	LGSM_CACHE_MAX,
} lgsm_cache_t;

typedef enum {
	LGSM_EXEC_REQUEST,
	LGSM_EXEC_RESPONSE,
	LGSM_EXEC_ERROR,
	LGSM_EXEC_MAX,
} lgsm_exec_t;

typedef enum {
	LGSM_FW_RESPONSE,
	LGSM_FW_MAX,
} lgsm_fw_t;

typedef enum {
	LGSM_FORMAT_ID,
	LGSM_FORMAT_STR,
	LGSM_FORMAT_MAX,
} lgsm_fmt_t;

typedef enum {
	LGSM_GET_FUNC_ID,
	LGSM_GET_FUNC_STR,
	LGSM_GET_FUNC_MAX,
} lgsm_get_func_t;

typedef enum {
	LGSM_SIGNAL_QRY_CFG_ID,
	LGSM_SIGNAL_QRY_CFG_STR,
	LGSM_SIGNAL_QRY_CFG_THRESHOLD,
	LGSM_SIGNAL_QRY_CFG_INTERVAL,
	LGSM_SIGNAL_QRY_CFG_MAX,
} lgsm_cfg_t;

typedef enum {
	LGSM_GET_CLIP_MODE_ID,
	LGSM_GET_CLIP_MODE_STR,
	LGSM_GET_CLIP_STATE_ID,
	LGSM_GET_CLIP_STATE_STR,
	LGSM_GET_CLIP_MAX,
} lgsm_get_clip_t;

typedef enum {
	LGSM_NET_MODE_ID,
	LGSM_NET_MODE_STR,
	LGSM_NET_MODE_RSSI,
	LGSM_NET_MODE_BER,
	LGSM_NET_MODE_RSRP,
	LGSM_NET_MODE_RSCP,
	LGSM_NET_MODE_SINR,
	LGSM_NET_MODE_ECIO,
	LGSM_NET_MODE_RSRQ,
	LGSM_NET_MODE_MAX,
} lgsm_net_mode_t;

typedef enum {
	LGSM_MODE_ID,
	LGSM_MODE_STR,
	LGSM_MODE_MAX,
} lgsm_mode_t;

typedef enum {
	LGSM_NET_REG_MODE_ID,
	LGSM_NET_REG_MODE_STR,
	LGSM_NET_REG_MAX_MODE_ID,
	LGSM_NET_REG_MAX_MODE_STR,
	LGSM_NET_REG_STAT_ID,
	LGSM_NET_REG_STAT_STR,
	LGSM_NET_REG_LAC,
	LGSM_NET_REG_CI,
	LGSM_NET_REG_ACT_ID,
	LGSM_NET_REG_ACT_STR,
	LGSM_NET_REG_MAX,
} lgsm_net_reg_t;

typedef enum {
	LGSM_ERRNO_NUM,
	LGSM_ERRNO_ACT_STR,
	LGSM_ERRNO_MAX,
} lgsm_errno_t;

typedef enum {
	LGSM_OPER_SCAN_NAME,
	LGSM_OPER_SCAN_SHORT_NAME,
	LGSM_OPER_SCAN_NUM,
	LGSM_OPER_SCAN_STATE,
	LGSM_OPER_SCAN_STATE_STR,
	LGSM_OPER_SCAN_ACT,
	LGSM_OPER_SCAN_ACT_STR,
	LGSM_OPER_SCAN_MAX,
} lgsm_oper_scan_t;

typedef enum {
	LGSM_LIST,
	LGSM_LIST_MAX,
} lgsm_list_t;

typedef enum {
	LGSM_MBN_INDEX,
	LGSM_MBN_SELECTED,
	LGSM_MBN_ACTIVE,
	LGSM_MBN_NAME,
	LGSM_MBN_VERSION,
	LGSM_MBN_RELEASE_DATE,
	LGSM_MBN_MAX,
} lgsm_mbn_t;

typedef enum {
	LGSM_INDEX,
	LGSM_INDEX_MAX,
} lgsm_index_t;

typedef enum {
	LGSM_PDP_CTX_CID,
	LGSM_PDP_CTX_TYPE,
	LGSM_PDP_CTX_APN,
	LGSM_PDP_CTX_ADDR,
	LGSM_PDP_CTX_DT_COMP,
	LGSM_PDP_CTX_HDR_COMP,
	LGSM_PDP_CTX_IP_ALLOC,
	LGSM_PDP_CTX_REQ_TP,
	LGSM_PDP_CTX_MAX,
} lgsm_pdp_ctx_t;

typedef enum {
	LGSM_PDP_ADDR_CID,
	LGSM_PDP_ADDR_ADDR,
	LGSM_PDP_ADDR_ADDRV6,
	LGSM_PDP_ADDR_MAX,
} lgsm_pdp_addr_t;

typedef enum {
	LGSM_INC_CALL_IDX,
	LGSM_INC_CALL_DIR,
	LGSM_INC_CALL_STATE,
	LGSM_INC_CALL_MODE,
	LGSM_INC_CALL_TYPE,
	LGSM_INC_CALL_MULTIPARTY,
	LGSM_INC_CALL_NUMBER,
	LGSM_INC_CALL_ALPHA,
	LGSM_INC_CALL_MAX,
} lgsm_inc_call_t;

typedef enum {
	LGSM_APN_AUTH_CID,
	LGSM_APN_AUTH_TYPE,
	LGSM_APN_AUTH_APN,
	LGSM_APN_AUTH_UNAME,
	LGSM_APN_AUTH_PASS,
	LGSM_APN_AUTH_AUTH_TP,
	LGSM_APN_AUTH_MAX,
} lgsm_apn_auth_t;

typedef enum {
	LGSM_STATE_ID,
	LGSM_STATE_STR,
	LGSM_STATE_MAX,
} lgsm_state_t;

typedef enum {
	LGSM_PCNT_SIM_PIN1,
	LGSM_PCNT_SIM_PUK1,
	LGSM_PCNT_SIM_PIN2,
	LGSM_PCNT_SIM_PUK2,
	LGSM_PCNT_MAX,
} lgsm_pin_cnt_t;

typedef enum {
	LGSM_SMS_LIST,
	LGSM_SMS_MAX,
} lgsm_sms_list_t;

typedef enum {
	LGSM_READ_SMS_INDEX,
	LGSM_READ_SMS_SENDER,
	LGSM_READ_SMS_UDHL,
	LGSM_READ_SMS_IEI_TP,
	LGSM_READ_SMS_IEI_LEN,
	LGSM_READ_SMS_REF_ID,
	LGSM_READ_SMS_N_PARTS,
	LGSM_READ_SMS_NS_PARTS,
	LGSM_READ_SMS_PARTS,
	LGSM_READ_SMS_DATE,
	LGSM_READ_SMS_TIMESTAMP,
	LGSM_READ_SMS_TEXT,
	LGSM_READ_SMS_STATE,
	LGSM_READ_SMS_STATE_STR,
	LGSM_READ_SMS_MISSING,
	LGSM_READ_SMS_PDUS,
	LGSM_READ_SMS_MAX,
} lgsm_read_sms_t;

typedef enum {
	LGSM_READ_SMS_LIST,
	LGSM_READ_SMS_LIST_MAX,
} lgsm_read_sms_list_t;

typedef enum {
	LGSM_TYPE_LIST,
	LGSM_TYPE_LIST_MAX,
} lgsm_type_list_t;

typedef enum {
	LGSM_SEND_SMS_USED,
	LGSM_SEND_SMS_MAX,
} lgsm_send_sms_t;

typedef enum {
	LGSM_SMS_EVT_REP_CFG_MODE,
	LGSM_SMS_EVT_REP_CFG_MT,
	LGSM_SMS_EVT_REP_CFG_BM,
	LGSM_SMS_EVT_REP_CFG_DS,
	LGSM_SMS_EVT_REP_CFG_BFR,
	LGSM_SMS_EVT_REP_CFG_MAX,
} lgsm_sms_evt_rep_t;

typedef enum {
	LGSM_MSG_STORAGE_ZONE1,
	LGSM_MSG_STORAGE_ZONE2,
	LGSM_MSG_STORAGE_ZONE3,
	LGSM_MSG_STORAGE_ALT_ZONE1,
	LGSM_MSG_STORAGE_ALT_ZONE2,
	LGSM_MSG_STORAGE_ALT_ZONE3,
	LGSM_MSG_STORAGE_ZONE_MAX,
} lgsm_msg_storage_zone_t;

typedef enum {
	LGSM_MSG_STORAGE_OPTS_STR,
	LGSM_MSG_STORAGE_OPTS_STR_ID,
	LGSM_MSG_STORAGE_OPTS_USED,
	LGSM_MSG_STORAGE_OPTS_TOTAL,
	LGSM_MSG_STORAGE_OPTS_MAX,
} lgsm_msg_storage_opts_t;

typedef enum {
	LGSM_BAND_LIST,
	LGSM_BAND_LIST_MAX,
} lgsm_band_list_t;

typedef enum {
	LGSM_OP_SLCT_MOD_ID,
	LGSM_OP_SLCT_MOD_ID_STR,
	LGSM_OP_SLCT_FORM_ID,
	LGSM_OP_SLCT_FORM_ID_STR,
	LGSM_OP_SLCT_OP_NAME,
	LGSM_OP_SLCT_ACT_ID,
	LGSM_OP_SLCT_ACT_ID_STR,
	LGSM_OP_SLCT_MAX,
} lgsm_op_slct_t;

typedef enum {
	LGSM_STATUS_ID,
	LGSM_STATUS_ID_STR,
	LGSM_STATUS_MAX,
} lgsm_resp_t;

typedef enum {
	LGSM_TEMP,
	LGSM_TEMP_MAX,
} lgsm_rsp_temp_t;

typedef enum {
	LGSM_INT_VAL,
	LGSM_INT_VAL_MAX,
} lgsm_rsp_int_val_t;

typedef enum {
	LGSM_DISABLED,
	LGSM_DISABLED_MAX,
} lgsm_diabled_t;

typedef enum {
	LGSM_ENABLED,
	LGSM_ENABLED_MAX,
} lgsm_enabled_t;

typedef enum {
	LGSM_IMSI_RESPONSE,
	LGSM_IMSI_MAX,
} lgsm_imsi_t;

typedef enum {
	LGSM_PORT_STR,
	LGSM_PORT_DEF_STR,
	LGSM_PORT_MAX,
} lgsm_port_t;

typedef enum {
	LGSM_ICCID_RESPONSE,
	LGSM_ICCID_MAX,
} lgsm_iccid_t;

typedef enum {
	LGSM_NET_INFO_NET_MODE,
	LGSM_NET_INFO_NET_MODE_ID,
	LGSM_NET_INFO_NET_MODE_SEL,
	LGSM_NET_INFO_BAND,
	LGSM_NET_INFO_BAND_ID,
	LGSM_NET_INFO_OPERNUM,
	LGSM_NET_INFO_MAX,
} lgsm_net_info_attrs_t;

typedef enum {
	LGSM_ENDC_CELL_SUP,
	LGSM_ENDC_PLMN_SUP,
	LGSM_ENDC_UNRESTR,
	LGSM_ENDC_5G_SUP,
	LGSM_ENDC_MAX,
} lgsm_endc_attrs_t;

typedef enum {
	LGSM_SERV_UE_STATE,
	LGSM_SERV_UE_STATE_STR,
	LGSM_SERV_ACC_TECH,
	LGSM_SERV_TDD_MODE,
	LGSM_SERV_OPER_MCC,
	LGSM_SERV_OPER_MNC,
	LGSM_SERV_UNPARSED,
	LGSM_SERV_CELL_MAX,
} lgsm_serv_cell_attrs_t;

typedef enum {
	LGSM_NEIGH_RFCN,
	LGSM_NEIGH_ACC_TECH,
	LGSM_NEIGH_UNPARSED,
	LGSM_NEIGH_CELL_MAX,
} lgsm_neigh_cell_attrs_t;

typedef enum {
	LGSM_IMS_MODE_ID,
	LGSM_IMS_MODE,
	LGSM_IMS_APPLY,
	LGSM_IMS_MAX,
} lgsm_ims_attrs_t;

typedef enum {
	LGSM_M2M_STATE,
	LGSM_M2M_MAX,
} lgsm_m2m_attrs_t;

typedef enum {
	LGSM_SET_IMS_RESTART,
	LGSM_SET_IMS_MAX,
} lgsm_set_ims_attrs_t;

typedef enum {
	LGSM_SET_ROAM_RELOAD,
	LGSM_SET_ROAM_MAX,
} lgsm_set_roam_attrs_t;

typedef enum {
	LGSM_VOLTE_MODE_ID,
	LGSM_VOLTE_MAN_MODE,
	LGSM_VOLTE_MAN_MAX,
} lgsm_volte_attrs_t;

typedef enum {
	LGSM_VOLTE_READY,
	LGSM_VOLTE_READY_MAX,
} lgsm_volte_ready_attrs_t;

typedef enum {
	LGSM_SET_VOLTE_RESTART,
	LGSM_SET_VOLTE_MAX,
} lgsm_set_volte_attrs_t;

typedef enum {
	LGSM_SIM_HOTSWAP_ENABLED,
	LGSM_SIM_HOTSWAP_POLARITY_ID,
	LGSM_SIM_HOTSWAP_POLARITY,
	LGSM_SIM_HOTSWAP_MAX,
} lgsm_sim_hotswap_attrs_t;

typedef enum {
	LGSM_PS_ATT_STATE_ID,
	LGSM_PS_ATT_STATE,
	LGSM_PS_ATT_MAX,
} lgsm_ps_att_attrs_t;

typedef enum {
	LGSM_TIME,
	LGSM_TIME_ZONE,
	LGSM_TIME_DST,
	LGSM_TIME_MAX,
} lgsm_time_attrs_t;

typedef enum {
	LGSM_ATTACHED_PDP_LIST_CID,
	LGSM_ATTACHED_PDP_LIST_STATE_ID,
	LGSM_ATTACHED_PDP_LIST_STATE,
	LGSM_ATTACHED_PDP_LIST_TYPE_ID,
	LGSM_ATTACHED_PDP_LIST_TYPE,
	LGSM_ATTACHED_PDP_LIST_IP_ADDR,
	LGSM_ATTACHED_PDP_LIST_MAX,
} lgsm_attached_pdp_ctx_attrs_t;

typedef enum {
	LGSM_ACT_PDP_LIST_CID,
	LGSM_ACT_PDP_LIST_STATE_ID,
	LGSM_ACT_PDP_LIST_STATE,
	LGSM_ACT_PDP_LIST_MAX,
} lgsm_act_pdp_ctx_attrs_t;

typedef enum {
	LGSM_DNS_ADDR_CID,
	LGSM_DNS_ADDR_PRI,
	LGSM_DNS_ADDR_SEC,
	LGSM_DNS_ADDR_MAX
} lgsm_dns_addr_attrs_t;

typedef enum {
	LGSM_PDP_CALL_MODE_ID,
	LGSM_PDP_CALL_MODE,
	LGSM_PDP_CALL_CID,
	LGSM_PDP_CALL_URC_EN,
	LGSM_PDP_CALL_STATE_ID,
	LGSM_PDP_CALL_STATE,
	LGSM_PDP_CALL_MAX,
} lgsm_pdp_call_attrs_t;

typedef enum {
	LGSM_CA_INFO_PRIMARY,
	LGSM_CA_INFO_FREQUENCY,
	LGSM_CA_INFO_BANDWIDTH,
	LGSM_CA_INFO_NET_MODE,
	LGSM_CA_INFO_BAND,
	LGSM_CA_INFO_CELL_ST,
	LGSM_CA_INFO_PCID,
	LGSM_CA_INFO_RSRP,
	LGSM_CA_INFO_RSRQ,
	LGSM_CA_INFO_RSSI,
	LGSM_CA_INFO_RSSNR,
	LGSM_CA_INFO_MAX,
} lgsm_ca_info_attrs_t;

typedef enum {
	LGSM_FILE_NAME,
	LGSM_FILE_SIZE,
	LGSM_FILE_MAX,
} lgsm_file_list_attrs_t;

typedef enum {
	LGSM_STORAGE_SPACE_FREE,
	LGSM_STORAGE_SPACE_TOTAL,
	LGSM_STORAGE_SPACE_MAX,
} lgsm_storage_space_attrs_t;

typedef enum {
	LGSM_TYPE_ID,
	LGSM_TYPE_STR,
	LGSM_TYPE_MAX,
} lgsm_type_attrs_t;

typedef enum {
	LGSM_PROVIDER_PLMN,
	LGSM_PROVIDER_FULL_NAME,
	LGSM_PROVIDER_SHORT_NAME,
	LGSM_PROVIDER_NAME,
	LGSM_PROVIDER_MAX
} lgsm_srvc_provider_attr_t;

typedef enum {
	LGSM_USBNET_MODE_ID,
	LGSM_USBNET_MODE_STR,
	LGSM_USBNET_MODE_IS_DEFAULT,
	LGSM_USBNET_MODE_MAX,
} lgsm_usbnet_mode_t;

typedef enum {
	LGSM_URC_IND_CFG_URC,
	LGSM_URC_IND_CFG_ENABLED,
	LGSM_URC_IND_CFG_MAX,
} lgsm_urc_ind_cfg_attrs_t;

typedef enum {
	LGSM_LTE_NAS_BACK_TIMER_VALUE,
	LGSM_LTE_NAS_BACK_TIMER_IS_DEFAULT,
	LGSM_LTE_NAS_BACK_TIMER_MAX,
} lgsm_lte_nas_back_timer_attrs_t;

typedef enum {
	LGSM_MTU_INFO_VALUE,
	LGSM_MTU_INFO_IS_DEFAULT,
	LGSM_MTU_INFO_MAX,
} lgsm_mtu_info_attrs_t;

typedef enum {
	LGSM_MODEM_FUNC_ID,
	LGSM_MODEM_FUNC_TYPE,
	LGSM_MODEM_FUNC_MAX,
} lgsm_modem_func_attrs_t;

typedef enum {
	LGSM_FLASH_STATE_VALUE,
	LGSM_FLASH_STATE_ID,
	LGSM_FLASH_STATE_ID_STR,
	LGSM_FLASH_STATE_VALUE_MAX,
} lgsm_flash_state_attrs_t;

typedef enum {
	LGSM_PDP_REG_INFO_CID,
	LGSM_PDP_REG_INFO_CSCF,
	LGSM_PDP_REG_INFO_DHCP,
	LGSM_PDP_REG_INFO_CN,
	LGSM_PDP_REG_INFO_MAX,
} lgsm_pdp_reg_info_attrs_t;

typedef enum {
	LGSM_EFS_VERSION_RESPONSE,
	LGSM_EFS_VERSION_MAX,
} lgsm_efs_version_t;

typedef enum {
	LGSM_SIM_SOFT_HOTPLUG_RECOVERY_CNT,
	LGSM_SIM_SOFT_HOTPLUG_DETECT_PERIOD,
	LGSM_SIM_SOFT_HOTPLUG_DETECT_CNT,
	LGSM_SIM_SOFT_HOTPLUG_MAX,
} lgsm_sim_soft_hotplug_attrs_t;

typedef enum {
	LGSM_CPU_SN,
	LGSM_CPU_SN_MAX,
} lgsm_cpu_sn_attrs_t;

typedef enum {
	LGSM_SIM_UNKNOWN,
	LGSM_SIM_ONE,
	LGSM_SIM_TWO,
	LGSM_SIM_MAX,
} lgsm_sim_t;

typedef enum {
	LGSM_GEA_ALGO_STAT,
	LGSM_GEA_ALGO_MAX,
} lgsm_gea_algo_attrs_t;

typedef enum {
	LGSM_USBCFG_VID,
	LGSM_USBCFG_PID,
	LGSM_USBCFG_DIAG,
	LGSM_USBCFG_NMEA,
	LGSM_USBCFG_AT_PORT,
	LGSM_USBCFG_MODEM,
	LGSM_USBCFG_RMNET,
	LGSM_USBCFG_ADB,
	LGSM_USBCFG_UAC,
	LGSM_USBCFG_MAX,
} lgsm_usbcfg_t;

typedef enum {
	LGSM_UBUS_INFO,
	LGSM_UBUS_EXEC,
	LGSM_UBUS_FW,
	LGSM_UBUS_APP_FW,
	LGSM_UBUS_SET_ERR_MSG_FMT,
	LGSM_UBUS_GET_ERR_MSG_FMT,
	LGSM_UBUS_SET_FUNC,
	LGSM_UBUS_GET_FUNC,
	LGSM_UBUS_SET_CLIP,
	LGSM_UBUS_GET_CLIP,
	LGSM_UBUS_SET_CRC,
	LGSM_UBUS_GET_CRC,
	LGSM_UBUS_SET_SIGNAL_QRY_CFG,
	LGSM_UBUS_GET_SIGNAL_QRY_CFG,
	LGSM_UBUS_GET_SIGNAL_QRY,
	LGSM_UBUS_SET_GPRS_MODE,
	LGSM_UBUS_GET_GPRS_MODE,
	LGSM_UBUS_SET_NET_REQ_STAT,
	LGSM_UBUS_GET_NET_REQ_STAT,
	LGSM_UBUS_SET_NET_GREQ_STAT,
	LGSM_UBUS_GET_NET_GREQ_STAT,
	LGSM_UBUS_SET_NET_EREQ_STAT,
	LGSM_UBUS_GET_NET_EREQ_STAT,
	LGSM_UBUS_SET_NET_5GREQ_STAT,
	LGSM_UBUS_GET_NET_5GREQ_STAT,
	LGSM_UBUS_GET_NET_REQ_STAT_ALL,
	LGSM_UBUS_GET_MBN,
	LGSM_UBUS_MBN_DEACTIVATE,
	LGSM_UBUS_MBN_SELECT,
	LGSM_UBUS_SET_MBN_AUTO_SEL,
	LGSM_UBUS_GET_MBN_AUTO_SEL,
	LGSM_UBUS_SET_NET_MODE,
	LGSM_UBUS_GET_NET_MODE,
	LGSM_UBUS_GET_NET_MODE_TECH,
	LGSM_UBUS_GET_NET_MODE_TYPE,
	LGSM_UBUS_SET_ROAMING,
	LGSM_UBUS_GET_ROAMING,
	LGSM_UBUS_SET_OPERATOR_SEL,
	LGSM_UBUS_GET_OPERATOR_SEL,
	LGSM_UBUS_SET_GSM_BAND,
	LGSM_UBUS_SET_WCDMA_BAND,
	LGSM_UBUS_SET_LTE_BAND,
	LGSM_UBUS_SET_LTE_NB_BAND,
	LGSM_UBUS_SET_5G_NSA_BAND,
	LGSM_UBUS_SET_5G_SA_BAND,
	LGSM_UBUS_SET_DEFAULT_BANDS,
	LGSM_UBUS_GET_BANDS,
	LGSM_UBUS_SET_PIN,
	LGSM_UBUS_GET_PIN_STATE,
	LGSM_UBUS_GET_PIN_COUNT,
	LGSM_UBUS_SET_PUK,
	LGSM_UBUS_SET_SMS_MODE,
	LGSM_UBUS_GET_SMS_MODE,
	LGSM_UBUS_SEND_SMS,
	LGSM_UBUS_READ_SMS,
	LGSM_UBUS_DELETE_SMS,
	LGSM_UBUS_SEND_CALL,
	LGSM_UBUS_ACCEPT_CALL,
	LGSM_UBUS_REJECT_CALL,
	LGSM_UBUS_GET_INCOMMING_CALLS,
	LGSM_UBUS_SET_SMS_EVT_REP_CFG,
	LGSM_UBUS_GET_SMS_EVT_REP_CFG,
	LGSM_UBUS_SET_MSG_STORAGE,
	LGSM_UBUS_GET_MSG_STORAGE,
	LGSM_UBUS_GET_TEMPERATURE,
	LGSM_UBUS_GET_IMSI,
	LGSM_UBUS_GET_NETWORK_INFO,
	LGSM_UBUS_GET_SERVING_CELL,
	LGSM_UBUS_GET_NEIGH_CELL,
	LGSM_UBUS_GET_ENDC_SUPPORT,
	LGSM_UBUS_GET_ICCID,
	LGSM_UBUS_SCAN_OPER,
	LGSM_UBUS_SET_URC_PORT,
	LGSM_UBUS_GET_URC_PORT,
	LGSM_UBUS_SET_FAC_LOCK,
	LGSM_UBUS_GET_FAC_LOCK,
	LGSM_UBUS_SET_DHCP_PKT_FLTR,
	LGSM_UBUS_GET_DHCP_PKT_FLTR,
	LGSM_UBUS_SET_USBNET_CFG,
	LGSM_UBUS_GET_USBNET_CFG,
	LGSM_UBUS_SET_NAT_CFG,
	LGSM_UBUS_GET_NAT_CFG,
	LGSM_UBUS_SET_UE_USAGE_CFG,
	LGSM_UBUS_GET_UE_USAGE_CFG,
	LGSM_UBUS_SET_SIM_STATE_REP,
	LGSM_UBUS_GET_SIM_STAT,
	LGSM_UBUS_SET_PDP_CTX,
	LGSM_UBUS_GET_PDP_CTX,
	LGSM_UBUS_GET_PDP_ADDR,
	LGSM_UBUS_GET_PDP_ADDR_LIST,
	LGSM_UBUS_SET_APN_AUTH,
	LGSM_UBUS_GET_APN_AUTH,
	LGSM_UBUS_SET_IMS_STATE,
	LGSM_UBUS_GET_IMS_STATE,
	LGSM_UBUS_SET_VOLTE_MAN_STATE,
	LGSM_UBUS_GET_VOLTE_MAN_STATE,
	LGSM_UBUS_GET_M2M_STATE,
	LGSM_UBUS_SET_M2M_STATE,
	LGSM_UBUS_GET_VOLTE_READY,
	LGSM_UBUS_SET_LTE_NAS_BACK_TIMER,
	LGSM_UBUS_GET_LTE_NAS_BACK_TIMER,
	LGSM_UBUS_SET_MTU_INFO,
	LGSM_UBUS_GET_MTU_INFO,
	LGSM_UBUS_GET_MOD_STATUS,
	LGSM_UBUS_SET_SIM_INITDELAY,
	LGSM_UBUS_GET_SIM_INITDELAY,
	LGSM_UBUS_SET_SIM_HOTSWAP,
	LGSM_UBUS_GET_SIM_HOTSWAP,
	LGSM_UBUS_GET_SIM_SLOT,
	LGSM_UBUS_CHANGE_SIM_SLOT,
	LGSM_UBUS_SET_SIM_SLOT,
	LGSM_UBUS_SET_WAN_PING,
	LGSM_UBUS_GET_WAN_PING,
	LGSM_UBUS_SET_PS_ATT_STATE,
	LGSM_UBUS_GET_PS_ATT_STATE,
	LGSM_UBUS_SET_NET_CAT,
	LGSM_UBUS_GET_NET_CAT,
	LGSM_UBUS_SET_USSD,
	LGSM_UBUS_GET_TIME,
	LGSM_UBUS_SET_PDP_CTX_STATE,
	LGSM_UBUS_GET_ATTACHED_PDP_CTX_LIST,
	LGSM_UBUS_GET_ACT_PDP_CTX_LIST,
	LGSM_UBUS_SET_DNS_ADDR,
	LGSM_UBUS_GET_DNS_ADDR,
	LGSM_UBUS_SET_PDP_CALL,
	LGSM_UBUS_GET_PDP_CALL,
	LGSM_UBUS_GET_CA_INFO,
	LGSM_UBUS_GET_FILE_LIST,
	LGSM_UBUS_DELETE_FILE,
	LGSM_UBUS_UPLOAD_FILE,
	LGSM_UBUS_GET_STORAGE_SPACE,
	LGSM_UBUS_DFOTA_UPGRADE,
	LGSM_UBUS_SET_NEW_PASS,
	LGSM_UBUS_SET_NMEA,
	LGSM_UBUS_GET_NMEA,
	LGSM_UBUS_SERVICE_PROVIDER,
	LGSM_UBUS_GNSS_INIT,
	LGSM_UBUS_GNSS_DEINIT,
	LGSM_UBUS_GNSS_START,
	LGSM_UBUS_SET_NMEA_OUTPORT,
	LGSM_UBUS_GNSS_STOP,
	LGSM_UBUS_SET_URC_IND_CFG,
	LGSM_UBUS_GET_URC_IND_CFG,
	LGSM_UBUS_GET_SEC_BOOT_CFG,
	LGSM_UBUS_GET_CPU_SN,
	LGSM_UBUS_UPDATE_MODEM_FUNC,
	LGSM_UBUS_GET_DEFAULT_MODEM_FUNC,
	LGSM_UBUS_GET_FLASH_STATE,
	LGSM_UBUS_SET_CEFS_RESTORE,
	LGSM_UBUS_SET_SIM_SOFT_HOTPLUG,
	LGSM_UBUS_GET_SIM_SOFT_HOTPLUG,
	LGSM_UBUS_GET_AUTH_CORR_BIT,
	LGSM_UBUS_SET_AUTH_CORR_BIT,
	LGSM_UBUS_GET_PDP_PROFILE_REGISTRY,
	LGSM_UBUS_GET_LTESMS_FORMAT,
	LGSM_UBUS_SET_LTESMS_FORMAT,
	LGSM_UBUS_GET_EFS_VERSION,
	LGSM_UBUS_GET_PROD_LINE_MODE,
	LGSM_UBUS_SET_PROD_LINE_MODE,
	LGSM_UBUS_GET_AUTO_TZ_UPDATE,
	LGSM_UBUS_SET_AUTO_TZ_UPDATE,
	LGSM_UBUS_GET_GNSS_OPER_MODE,
	LGSM_UBUS_SET_GNSS_OPER_MODE,
	LGSM_UBUS_GET_URC_CAUSE,
	LGSM_UBUS_SET_URC_CAUSE,
	LGSM_UBUS_GET_DPO_MODE,
	LGSM_UBUS_SET_DPO_MODE,
	LGSM_UBUS_GET_NR5G_DISABLE_MODE,
	LGSM_UBUS_SET_NR5G_DISABLE_MODE,
	LGSM_UBUS_GET_GEA_ALGORYTHM,
	LGSM_UBUS_DISABLE_GEA_1_2_ALGORYTHM,
	LGSM_UBUS_SET_USBCFG,
	LGSM_UBUS_GET_USBCFG,
	//------
	__LGSM_UBUS_MAX,
} lgsm_method_t;

typedef struct {
	enum clip_mode_id mode_id;
	enum clip_stat_id state_id;
} lgsm_clip_t;

typedef struct {
	enum net_reg_mode_id net_mode_id;
	enum net_reg_mode_id net_max_mode_id; // Highest verbosity level the modem allows
	enum net_reg_stat_id net_stat_id;
	enum net_reg_act_id net_act_id;
	char net_lac[7]; // tac (three bytes) for 5greg
	char net_ci[21]; // cell id (u64 in decimal) for 5greg
} lgsm_net_reg_info_t;

typedef struct {
	enum signal_query_cfg_id cfg_id;
	int32_t change_threshold;
	int32_t report_interval;
} lgsm_signal_query_cfg_t;

enum lgsm_sig_val {
	LGSM_SIG_VAL_RSSI = 1 << 0, ///< redeived signal strength
	LGSM_SIG_VAL_BER  = 1 << 1, ///< channel bit error rate (in percent)
	LGSM_SIG_VAL_RSRP = 1 << 2, ///< reference signal received power
	LGSM_SIG_VAL_RSCP = 1 << 3, ///< received signal code power
	LGSM_SIG_VAL_SINR = 1 << 4, ///< signal to interference plus noise ratio
	LGSM_SIG_VAL_RSRQ = 1 << 5, ///< reference signal received quality
	LGSM_SIG_VAL_ECIO =
		1
		<< 6, ///< ratio of the received energy per PN chip to the total received power spectral density
};
typedef struct {
	enum net_mode_id net_id;
	uint32_t sig_vals; ///< flags of lgsm_sig_val
	int32_t rssi;
	int32_t ber;
	int32_t rsrp;
	int32_t rscp;
	int32_t sinr;
	int32_t rsrq;
	int32_t ecio;
} lgsm_signal_t;

typedef struct {
	uint32_t index;
	uint32_t selected;
	uint32_t active;
	char name[64];
} lgsm_mbn_info_t;

typedef struct {
	enum usbnet_mode_id usbnet_mode;
	bool is_default;
} lgsm_usbnet_info_t;

typedef struct {
	lgsm_mbn_info_t *mbn_info;
	uint32_t cnt;
} lgsm_mbn_info_arr_t;

typedef struct {
	uint32_t id;
	enum pdp_type_id type;
	char *apn;
	char *username;
	char *pass;
	enum apn_auth_t auth_tp;
} lgsm_apn_auth_info_t;

typedef struct {
	enum facility_t fac;
	enum fac_lock_state_t st;
	char *pass;
} lgsm_fac_lock_info_t;

typedef struct {
	uint32_t id;
	enum pdp_type_id type;
	char *apn;
	char *addr;
	enum pdp_data_comp_id dt_comp;
	enum pdp_hdr_comp_id hdr_comp;
	enum pdp_ipv4_addr_alloc_id ip_alloc;
	enum pdp_req_id req;
} lgsm_pdp_ctx_info_t;

typedef struct {
	lgsm_pdp_ctx_info_t *pdp_ctx_info;
	uint32_t cnt;
} lgsm_pdp_ctx_info_arr_t;

typedef struct {
	uint32_t id;
	char *addr;
	char *addr_v6;
} lgsm_pdp_addr_info_t;

typedef struct {
	lgsm_pdp_addr_info_t *pdp_addr_info;
	uint32_t cnt;
} lgsm_pdp_addr_info_arr_t;

typedef struct {
	uint32_t idx;
	enum call_direction_id dir;
	enum call_state_id state;
	enum call_mode_id mode;
	enum call_type_id type;
	bool is_multiparty;
	char *number;
	char *alpha;
} lgsm_voice_call_info_t;

typedef struct {
	lgsm_voice_call_info_t *voice_call_info;
	uint32_t cnt;
} lgsm_voice_call_info_arr_t;

typedef struct {
	char op_name[256];
	enum op_slc_mode_id op_mode;
	enum op_slc_fmt_id op_fmt;
	enum net_reg_act_id op_act;
} lgsm_op_slc_mode_t;

typedef struct {
	char port[32];
	char def_port[32];
} lgsm_urc_port_t;

typedef struct {
	uint8_t sim_pin1;
	uint8_t sim_puk1;
	uint8_t sim_pin2;
	uint8_t sim_puk2;
} lgsm_pin_count_t;

typedef struct {
	char **band_arr;
	uint32_t band_cnt;
} lgsm_band_t;

typedef struct {
	char **service_mode_arr;
	uint32_t service_mode_cnt;
} lgsm_service_mode_t;

typedef struct {
	enum msg_storage_id storage_id;
	uint32_t used;
	uint32_t total;
} lgsm_msg_storage_t;

typedef struct {
	uint32_t index;
	char *msg_text; // We can get a really long message
	char number[32];
	char date[32];
	uint64_t timestamp;
	enum sms_state_id stat_id;
	uint8_t udhl; /*!< User data header length */
	uint8_t iei_tp; /*!< (Part of user data header) */
	uint8_t iei_len; /*!< (Part of user data header) */
	uint8_t ref_id; /*!< Reference ID if message (Part of user data header) */
	uint8_t n_parts; // sms pdu count (part of user data header) */
	uint8_t ns_parts; // number of PDU numbers stored in 'parts' */
	uint8_t *parts; /*!< 'n_parts' length array of  multipart SMS part numbers (Part of user data header) */
	bool missing; // Indicated if any part is missing
	size_t npdus; // Number of pdu indices
	unsigned char *pdus; // Alloced array of contained PDU indices
} lgsm_sms_t;

typedef struct {
	lgsm_sms_t *sms;
	uint32_t sms_cnt;
} lgsm_sms_list_arr_t;

typedef struct {
	uint32_t mode;
	uint32_t mt;
	uint32_t bm;
	uint32_t ds;
	uint32_t bfr;
} lgsm_sms_evt_rep_cfg_t;

typedef struct {
	enum net_mode_id net_mode;
	enum net_mode_id nm_sel; //determines union field
	union {
		enum modem_gsm_band_id gsm;
		enum modem_wcdma_band_id wcdma;
		enum modem_lte_band_id lte;
		enum modem_5g_band_id nr5g; //SA bands
		enum modem_nsa5g_band_id nsa5g; //NSA bands
	} band;
	uint32_t opernum;
} lgsm_net_info_t;

typedef struct {
	int id;
	char type[32];
} lgsm_modem_func_t;

typedef struct {
	lgsm_band_t band_list;
	lgsm_service_mode_t service_mode_list;

	char name[32];
	char model[32];
	char manuf[32];
	char product_num[32];
	char firmware[64];
	char usb_id[10];
	char vid_pid[10];
	char tty_port[32];
	char gps_port[32];
	char data_port[32];
	char aux_port[32];
	char serial_num[32];
	char imei[16];
	char imsi[16];
	uint32_t rssi_value;
	uint32_t baudrate;
	uint32_t simcount;
	uint32_t ubus_obj_id;
	enum pin_state_id pin_t;
	char iccid[32];
	bool volte_ready;
	bool oper_scan_support;
	bool ipv6_support;
	bool volte_support;
	bool multi_apn_support;
	bool dpo_support;
	bool rmnet_support;
	bool ecm_support;
	bool dynamic_mtu_support;
	bool auto_ims_support;
	bool extended_timeout;
	bool dhcp_filter_support;
	bool builtin;
	bool csd_support;
	lgsm_net_info_t net_info;
	lgsm_op_slc_mode_t op_slc;
	lgsm_net_reg_info_t net_reg_info;
	lgsm_pin_count_t pin_cnt;
	lgsm_modem_func_t mfunc;

} lgsm_t;

typedef struct {
	lgsm_t *gsm_arr;
	uint32_t modem_cnt;
} lgsm_arr_t;

typedef struct {
	uint32_t ue_state;
	char access_tech[16];
	char tdd_mode[8];
	char mcc[8];
	char mnc[8];
	char unparsed[64];
} lgsm_serv_cell_t;

typedef struct {
	lgsm_serv_cell_t *serv_arr;
	uint32_t serv_cnt;
} lgsm_serv_arr_t;

typedef struct {
	char access_tech[16];
	char unparsed[255];
	uint32_t rfcn;
} lgsm_neigh_cell_t;

typedef struct {
	lgsm_neigh_cell_t *neigh_arr;
	uint32_t neigh_cnt;
} lgsm_neigh_arr_t;

typedef struct {
	bool endc_cell_sup;
	bool endc_plmn_sup;
	bool endc_net_unrestr;
	bool endc_5g_sup;
} lgsm_endc_t;

typedef struct {
	char oper_name[128];
	char short_name[64];
	uint32_t oper_num;
	enum net_reg_act_id act;
	enum op_slc_stat_id stat;
} lgsm_oper_t;

typedef struct {
	lgsm_oper_t *oper_arr;
	uint32_t oper_cnt;
} lgsm_oper_scan_arr_t;

typedef struct {
	enum ims_state_t state;
} lgsm_ims_t;

typedef struct {
	enum volte_state_t state;
} lgsm_volte_t;

typedef struct {
	bool volte_ready;
} lgsm_volte_rdy_t;

typedef struct {
	enum polarity_id polarity;
	bool enabled;
} lgsm_sim_hotswap_t;

typedef struct {
	enum ps_att_state_t state;
} lgsm_ps_att_t;

typedef struct {
	char ip_addr[64];
	uint32_t id;
	enum pdp_type_id type;
	enum pdp_ctx_state_t state;
} lgsm_attached_pdp_ctx_t;

typedef struct {
	lgsm_attached_pdp_ctx_t *attached_pdp_ctx_arr;
	uint32_t cnt;
} lgsm_attached_pdp_ctx_arr_t;

typedef struct {
	uint32_t id;
	enum pdp_ctx_state_t state;
} lgsm_act_pdp_ctx_t;

typedef struct {
	lgsm_act_pdp_ctx_t *act_pdp_ctx_arr;
	uint32_t cnt;
} lgsm_act_pdp_ctx_arr_t;

typedef struct {
	char pri[64];
	char sec[64];
	uint32_t cid;
} lgsm_dns_t;

typedef struct {
	enum pdp_call_mode_id mode_id;
	enum pdp_call_state_id state_id;
	uint32_t cid;
	bool urc_en;
} lgsm_pdp_call_t;

typedef struct {
	char time[32];
	int time_zone;
	int dst;
} lgsm_time_t;

typedef struct {
	bool primary;
	uint32_t frequency;
	enum bandwidth_id bandwidth;
	enum net_mode_id net_mode;
	union {
		enum modem_gsm_band_id gsm;
		enum modem_wcdma_band_id wcdma;
		enum modem_lte_band_id lte;
		enum modem_5g_band_id nr5g; //SA bands
		enum modem_nsa5g_band_id nsa5g;
	} band;
	union {
		enum pcell_state_t pcell_st;
		enum scell_state_t scell_st;
	};
	uint32_t pcid;
	int rsrp;
	int rsrq;
	int rssi;
	int rssnr;
} lgsm_ca_info_t;

typedef struct {
	lgsm_ca_info_t *ca_info;
	uint32_t cnt;
} lgsm_ca_info_arr_t;

typedef struct {
	char path[128];
	int size;
} lgsm_file_t;

typedef struct {
	lgsm_file_t *file_list;
	uint32_t cnt;
} lgsm_file_list_arr_t;

typedef struct {
	int free;
	int total;
} lgsm_storage_space_t;

typedef struct {
	enum nmea_type_id *nmea_type_list;
	uint32_t cnt;
} lgsm_nmea_type_list_arr_t;

typedef struct {
	int plmn;
	char full_name[128];
	char short_name[128];
	char provider_name[128];
} lgsm_srvc_provider_t;

typedef struct {
	char urc[32];
	bool enabled;
} lgsm_urc_ind_cfg_t;

typedef struct {
	int value;
	bool is_default;
} lgsm_lte_nas_back_timer_t;

typedef struct {
	char value[32];
	bool is_default;
} lgsm_mtu_info_t;

typedef struct {
	int restore_cnt;
	enum cefs_restore_state_id restore_state;
} lgsm_flash_state_t;

typedef struct {
	int cid;
	int cscf;
	int dhcp;
	int cn;
} lgsm_pdp_registry_info_t;

typedef struct {
	lgsm_pdp_registry_info_t *pdp_registry_info;
	uint32_t cnt;
} lgsm_pdp_registry_info_arr_t;

typedef struct {
	uint32_t id;
	char str[16];
} lgsm_ltesms_formar_arr_t;

typedef struct {
	int recovery_count;
	int detect_period;
	int detect_count;
} lgsm_sim_soft_hotplug_t;

typedef struct {
	int stat_id;
} lgsm_gea_algo_t;

typedef struct {
	char *vid;
	char *pid;
	bool diag;
	bool nmea;
	bool at_port;
	bool modem;
	bool rmnet;
	bool adb;
	bool uac;
} lgsm_usbcfg_info_t;

typedef enum {
	LGSM_LABEL_STRING,
	LGSM_LABEL_INT,
	LGSM_LABEL_INT64,
	LGSM_LABEL_ERR_MSG_FMT,
	LGSM_LABEL_FUNC_ID,
	LGSM_LABEL_CLIP_T,
	LGSM_LABEL_CELL_RC_ID,
	LGSM_LABEL_SIGNAL_QRY_CFG,
	LGSM_LABEL_SIGNAL_T,
	LGSM_LABEL_ATTACH_MODE_ID,
	LGSM_LABEL_MBN_AUTO_SEL_ID,
	LGSM_LABEL_NET_REG_INFO_T,
	LGSM_LABEL_ROAM_SVC_ID,
	LGSM_LABEL_NET_MODE_ID,
	LGSM_LABEL_MBN_INFO_T,
	LGSM_LABEL_PDP_CTX_INFO_T,
	LGSM_LABEL_PDP_ADDR_T,
	LGSM_LABEL_PDP_ADDR_LIST_T,
	LGSM_LABEL_INC_CALL_INFO_T,
	LGSM_LABEL_APN_AUTH_T,
	LGSM_LABEL_OP_SLC_T,
	LGSM_LABEL_BAND_T,
	LGSM_LABEL_LOCK_ST_T,
	LGSM_LABEL_PIN_STATE_ID,
	LGSM_LABEL_SMS_FORMAT_ID,
	LGSM_LABEL_SMS_LIST_T,
	LGSM_LABEL_SMS_EVT_REP_CFG_T,
	LGSM_LABEL_MSG_STORAGE_T,
	LGSM_LABEL_INFO_T,
	LGSM_LABEL_TEMP_T,
	LGSM_LABEL_PIN_CNT_T,
	LGSM_LABEL_NET_INFO_T,
	LGSM_LABEL_SERV_CELL_INFO_T,
	LGSM_LABEL_ENDC_INFO_T,
	LGSM_LABEL_NEIGH_CELL_INFO_T,
	LGSM_LABEL_STATUS_T,
	LGSM_LABEL_OPER_SCAN_T,
	LGSM_LABEL_USBNET_MODE_T,
	LGSM_LABEL_NAT_MODE_T,
	LGSM_LABEL_UE_USAGE_MODE_T,
	LGSM_LABEL_IMS_STATE_T,
	LGSM_LABEL_SET_IMS_STATE_T,
	LGSM_LABEL_VOLTE_STATE_T,
	LGSM_LABEL_VOLTE_READY_T,
	LGSM_LABEL_SET_VOLTE_STATE_T,
	LGSM_LABEL_M2M_STATE_T,
	LGSM_LABEL_SIM_HOTSWAP_T,
	LGSM_LABEL_PS_ATT_STATE_T,
	LGSM_LABEL_NET_CAT_T,
	LGSM_LABEL_TIME_T,
	LGSM_LABEL_ATTACHED_PDP_LIST_T,
	LGSM_LABEL_ACT_PDP_LIST_T,
	LGSM_LABEL_DNS_ADDR_T,
	LGSM_LABEL_PDP_CALL_T,
	LGSM_LABEL_CA_INFO_T,
	LGSM_LABEL_FILE_LIST_T,
	LGSM_LABEL_DELETE_FILE_T,
	LGSM_LABEL_UPLOAD_FILE_T,
	LGSM_LABEL_GET_STORAGE_SPACE_T,
	LGSM_LABEL_GET_NMEA_TYPE_T,
	LGSM_LABEL_SRVC_PROVIDER_T,
	LGSM_LABEL_GET_URC_IND_CFG_T,
	LGSM_LABEL_GET_SEC_BOOT_CFG_T,
	LGSM_LABEL_LTE_NAS_BACK_TIMER,
	LGSM_LABEL_MODEM_DEFAULT_FUNC_T,
	LGSM_LABEL_UPDATE_MODEM_FUNC_T,
	LGSM_LABEL_GET_FLASH_STATE_T,
	LGSM_LABEL_GET_MTU_INFO_T,
	LGSM_LABEL_GET_PDP_PROFILE_REGISTRY,
	LGSM_LABEL_LTESMS_FORMAT,
	LGSM_LABEL_SIM_SOFT_HOTPLUG_T,
	LGSM_LABEL_URC_PORT_T,
	LGSM_LABEL_GNSS_OPER_MODE_T,
	LGSM_LABEL_DPO_MODE_T,
	LGSM_LABEL_DISABLE_5G_MODE_T,
	LGSM_LABEL_GEA_ALGO_T,
	LGSM_LABEL_ERROR,
	LGSM_LABEL_USBCFG_T,
} lgsm_resp_label_t;

typedef union {
	char *s;
	uint32_t u;
	uint64_t u64;
	func_t status;
	enum err_msg_fmt_id err_fmt;
	enum phone_func_id func_mode;
	lgsm_clip_t clip_info;
	enum cell_rc_id cell_id;
	lgsm_signal_query_cfg_t signal_qry_cfg;
	lgsm_signal_t signal_info;
	enum gprs_attach_mode_id gprs_mode;
	enum mbn_auto_sel_id mbn_auto_sel;
	lgsm_net_reg_info_t net_reg;
	enum roaming_svc_id roam_mode;
	enum net_mode_id net_mode;
	lgsm_usbnet_info_t usbnet;
	enum nat_mode_id nat_mode;
	enum ue_usage_id ue_usage_mode;
	enum fac_lock_state_t lock_state;
	lgsm_mbn_info_arr_t mbn_arr;
	lgsm_pdp_ctx_info_arr_t pdp_ctx_arr;
	lgsm_pdp_addr_info_t pdp_addr;
	lgsm_pdp_addr_info_arr_t pdp_addr_arr;
	lgsm_voice_call_info_arr_t voice_call_list;
	lgsm_apn_auth_info_t apn_auth;
	lgsm_op_slc_mode_t op_slc;
	lgsm_band_t band_list;
	enum pin_state_id pin_state;
	enum sms_format_id sms_format;
	lgsm_sms_list_arr_t sms_list;
	lgsm_sms_evt_rep_cfg_t sms_evt_rep_cfg;
	lgsm_msg_storage_t msg_storage[LGSM_MSG_STORAGE_ZONE_MAX];
	lgsm_t cache_data;
	int temperature;
	lgsm_pin_count_t pin_cnt;
	lgsm_net_info_t net_info;
	lgsm_serv_arr_t serv_cells;
	lgsm_neigh_arr_t neigh_cells;
	lgsm_oper_scan_arr_t oper_scans;
	lgsm_endc_t endc;
	lgsm_sim_hotswap_t sim_hotswap;
	lgsm_ims_t ims;
	lgsm_volte_t volte;
	lgsm_volte_rdy_t volte_rdy;
	bool restart;
	bool reload;
	enum mod_act_stat_id mod_status;
	lgsm_ps_att_t ps_att;
	enum net_cat_t net_cat;
	enum secboot_mode_id sec_boot;
	enum sim_stat_t sim_stat;
	lgsm_time_t time;
	lgsm_attached_pdp_ctx_arr_t attached_pdp_list;
	lgsm_act_pdp_ctx_arr_t act_pdp_list;
	lgsm_dns_t dns_addr;
	lgsm_pdp_call_t pdp_call;
	lgsm_ca_info_arr_t ca_info_arr;
	lgsm_file_list_arr_t file_list_arr;
	lgsm_storage_space_t storage_space;
	lgsm_nmea_type_list_arr_t nmea_type_arr;
	lgsm_srvc_provider_t srvc_provider;
	lgsm_urc_ind_cfg_t urc_ind_cfg;
	lgsm_lte_nas_back_timer_t lte_nas_back_timer;
	lgsm_mtu_info_t mtu_info;
	lgsm_modem_func_t modem_func;
	lgsm_flash_state_t flash_state;
	lgsm_pdp_registry_info_arr_t pdp_reg_info_arr;
	lgsm_ltesms_formar_arr_t ltesms_format;
	lgsm_sim_soft_hotplug_t sim_soft_hotplug;
	lgsm_urc_port_t urc_port;
	lgsm_gea_algo_t gea_algo;
	enum gnss_operation_mode_id gnss_oper_mode;
	enum dpo_mode_id dpo_mode;
	enum disable_nr5g_mode_id disable_5g_mode;
	lgsm_usbcfg_info_t usbcfg;
} lgsm_resp_data;

typedef struct {
	lgsm_resp_label_t label;
	lgsm_resp_data data;
} lgsm_structed_info_t;

extern const struct blobmsg_policy g_oper_scan_attrs[];
extern const struct blobmsg_policy g_oper_attrs[];

/****************
*  PREP FUNCS  *
****************/

/**
 * Convert given arguments to prepared blob for gsmd ubus exec method.
 * @param[char]   cmd      AT command to be executed on modem.
 * @param[in]   timeout    Given timeout to track and wait for response from modem.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_exec_cmd(const char *cmd, uint32_t timeout);

/**
 * Convert given arguments to prepared blob for gsmd ubus set err fmt method.
 * @param[in]   fmt    Format in which error messages are returned.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_err_fmt(enum err_msg_fmt_id fmt);

/**
 * Convert given arguments to prepared blob for gsmd ubus set clip mode method.
 * @param[in]   fmt    Format in which calls are handled.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_clip_mode(enum clip_mode_id fmt);

/**
 * Convert given arguments to prepared blob for gsmd ubus set modem func method.
 * @param[in]   func     Functionality mode in which modem will work.
 * @param[in]   reset    Whether to reset the module.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_func(enum phone_func_id func, uint8_t reset);

/**
 * Convert given arguments to prepared blob for gsmd ubus send call method.
 * @param[in]   number  Phone number
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_phone_number(const char *number);

/**
 * Convert given arguments to prepared blob for gsmd ubus set cell rc format method.
 * @param[in]   id      ID for format of cell mode.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_crc_fmt(enum cell_rc_id id);

/**
 * Convert given arguments to prepared blob for gsmd ubus set signal query cfg method.
 * @param[in]   id       Signal query configuration ID.
 * @param[ptr]  chn_thr  Signal change threshold.
 * @param[ptr]  rep_int  Signal query report interval.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_signal_qry_cfg(enum signal_query_cfg_id id, int *chn_thr, int *rep_int);

/**
 * Convert given arguments to prepared blob for gsmd ubus set gprs mode method.
 * @param[in]   id    GPRS attach mode ID.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_gprs_mode(enum gprs_attach_mode_id id);

/**
 * Convert given arguments to prepared blob for gsmd ubus set net reg state method.
 * @param[in]   id    Net registration state ID.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_net_reg(enum net_reg_stat_id id);

/**
 * Convert given arguments to prepared blob for gsmd ubus mbn auto select method.
 * @param[in]   id    MBN auto select flag ID.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_mbn_auto_sel(enum mbn_auto_sel_id id);

/**
 * Convert given arguments to prepared blob for gsmd ubus set net mode method.
 * @param[in]   id   Network mode ID.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_net_mode(enum net_mode_id id);

/**
 * Convert given arguments to prepared blob for gsmd ubus set roaming service method.
 * @param[in]   id        Roaming service mode ID.
 * @param[in]   effect    Whether to take effect instantly.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_roaming_svc(enum roaming_svc_id id, uint8_t effect);

/**
 * Convert given arguments to prepared blob for gsmd ubus set operator select options method.
 * @param[in]   id        Operator select mode ID.
 * @param[in]   fmt       Operator select format.
 * @param[char] oper_num  Operator name or number.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_op_slct(enum op_slc_mode_id id, enum op_slc_fmt_id fmt, const char *oper_num);

/**
 * Convert given arguments to prepared blob for gsmd ubus set GSM bands method.
 * @param[in]   bands     GSM bands
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_gsm_band(uint64_t bands);

/**
 * Convert given arguments to prepared blob for gsmd ubus set WCDMA bands method.
 * @param[in]   bands     WCDMA bands
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_wcdma_band(uint64_t bands);

/**
 * Convert given arguments to prepared blob for gsmd ubus set LTE bands method.
 * @param[in]   bands     LTE bands
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_lte_band(uint64_t bands);

/**
 * Convert given arguments to prepared blob for gsmd ubus set LTE_NB bands method.
 * @param[in]   bands     LTE_NB bands
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_lte_nb_band(uint64_t bands);

/**
 * Convert given arguments to prepared blob for gsmd ubus set 5G SA bands method.
 * @param[in]   bands     5G_SA bands
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_5g_sa_band(uint64_t bands);

/**
 * Convert given arguments to prepared blob for gsmd ubus set 5G NSA bands method.
 * @param[in]   bands     5G_NSA bands
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_5g_nsa_band(uint64_t bands);

/**
 * Convert given arguments to prepared blob for gsmd ubus set pin code method.
 * @param[char]   pin        PIN code.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_pin_code(const char *pin);

/**
 * Convert given arguments to prepared blob for gsmd ubus set puk code method.
 * @param[char]   puk        PUK code.
 * @param[char]   pin        PIN code.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_puk_code(const char *puk, const char *pin);

/**
 * Convert given arguments to prepared blob for gsmd ubus set sms mode method.
 * @param[in]   id        SMS format ID.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_sms_mode(enum sms_format_id id);

/**
 * Convert given arguments to prepared blob for gsmd ubus send sms method.
 * @param[char]   number     Phone number to which we need to send SMS.
 * @param[char]   text       Text that is sent via SMS.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_sms(const char *number, const char *text);

/**
 * Convert given arguments to prepared blob for gsmd ubus method that needs index as single parameter.
 * @param[in]   index    Index.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_index(int32_t index);

/**
 * Convert given arguments to prepared blob for gsmd ubus read/delete sms method.
 * @param[in]   sms_index    SMS Index.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
inline struct blob_buf prepare_read_del_sms(int32_t sms_index)
{
	return prepare_index(sms_index);
}

/**
 * Convert given arguments to prepared blob for gsmd ubus read sms method.
 * @param[in]   sms_index 	SMS Index.
 * @param[in]   single 		Read only single PDU.
 * @param[in]   strip 		Do not print " <missing part> ".
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_read_sms(int32_t index, bool single, bool strip);

/**
 * Convert given arguments to prepared blob for gsmd ubus set sms storage method.
 * @param[in]   mem1    Message storage zone 1.
 * @param[in]   mem2    Message storage zone 2.
 * @param[in]   mem3    Message storage zone 3.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_sms_strg(enum msg_storage_id mem1, enum msg_storage_id mem2,
				 enum msg_storage_id mem3);

/**
 * Convert given arguments to prepared blob for gsmd ubus get sms storage method.
 * @param[in]   bool    Do you want to query, both sms storage locations
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_get_msg_strg(bool full);

/**
 * Convert given arguments to prepared blob for gsmd ubus set sms event reporting configuration method.
 * @param[in]   mode      Configuration mode
 * @param[in]   mt        The rules for storing received SMS depend on its data coding scheme
 * @param[in]   bm        The rules for storing received CBMs depend on its data coding scheme
 * @param[in]   ds        Where status is reported
 * @param[in]   bfr       Buffer related
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_sms_evt_rep_cfg(int mode, int mt, int bm, int ds, int bfr);

/**
 * Convert given arguments to prepared blob for gsmd ubus set urc port method.
 * @param[in]   port      URC port name
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_urc_port(const char *port);

/**
 * Convert given arguments to prepared blob for gsmd ubus get facility lock method.
 * @param[in]   facility 	Facility type enumeration value.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_fac(enum facility_t facility);

/**
 * Convert given arguments to prepared blob for gsmd ubus set facility lock method.
 * @param[in]   *fac_lock      Structure representing facility lock
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_fac_loc(lgsm_fac_lock_info_t *fac_lock);

/**
 * Convert given arguments to prepared blob for gsmd ubus set DHCP packet filtering configuration method.
 * @param[in]   disabled 	Flag if feature is disabled
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_dhcp_pkt_fltr(bool disabled);

/**
 * Convert given arguments to prepared blob for gsmd ubus set usbnet mode configuration method.
 * @param[in]   mode 	Mode of usbnet
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_usbnet_mode(enum usbnet_mode_id mode);

/**
 * Convert given arguments to prepared blob for gsmd ubus set nat mode configuration method.
 * @param[in]   mode 	Mode of nat
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_nat_mode(enum nat_mode_id mode);

/**
 * Convert given arguments to prepared blob for gsmd ubus set UE usage mode configuration method.
 * @param[in]   mode 	Mode of UE usage
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_ue_usage_mode(enum ue_usage_id mode);

/**
 * Convert given arguments to prepared blob for gsmd ubus set SIM state change reporting method.
 * @param[in]   enabled 	Is state change reporting is enabled
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_sim_state_rep(bool enabled);

/**
 * Convert given arguments to prepared blob for gsmd ubus set APN auth method.
 * @param[in]   *apn_auth 	Pointer to structure that holds new APN auth information
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_apn_auth(lgsm_apn_auth_info_t *apn_auth);

/**
 * Convert given arguments to prepared blob for gsmd ubus set PDP context method.
 * @param[in]   *pdp_ctx 	Pointer to structure that holds new PDP context information
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_pdp_ctx(lgsm_pdp_ctx_info_t *pdp_ctx);

/**
 * Convert given arguments to prepared blob for gsmd ubus set IMS state method.
 * @param[in]   state 	IMS state to be set
 * @param[in]   apply 	If true modem will be restarted if needed to apply this command
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_ims_state(enum ims_state_t state, bool apply);

/**
 * Convert given arguments to prepared blob for gsmd ubus set SIM initdelay method.
 * @param[in]   delay 	delay in seconds to be set
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_sim_initdelay(int delay);

/**
 * Convert given arguments to prepared blob for gsmd ubus set SIM hotswap method.
 * @param[in]   enabled 	Is hotswap enabled
 * @param[in]   polarity 	Polarity value enum to be set
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_sim_hotswap(bool enabled, enum polarity_id polarity);

/**
 * Convert given arguments to prepared blob for gsmd ubus set wan ping state method.
 * @param[in]   enabled 	Is WAN ping enabled
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_wan_ping(bool enabled);

/**
 * Convert given arguments to prepared blob for gsmd ubus set LTE NAT backoff timer method.
 * @param[in]   value 	LTE NAS backoff timer to be set
 *
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_lte_nas_backoff_timer(unsigned value);

/**
 * Convert given arguments to prepared blob for gsmd ubus set PS attachment state method.
 * @param[in]   state 	PS attachment state to be set
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_ps_att_state(enum ps_att_state_t state);

/**
 * Convert given arguments to prepared blob for gsmd ubus set network category method.
 * @param[in]   category  Network category to be set.
 * @param[ptr]  effect    Whether to take effect instantly.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_net_cat(enum net_cat_t category, uint8_t *effect);

/**
 * Convert given arguments to prepared blob for gsmd ubus set DNS address method.
 * @param[in]   cid       PDP context id.
 * @param[char] pri_dns   Primary DNS address.
 * @param[char] sec_dns   Secondary DNS address.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_dns_addr(int cid, const char *pri_dns, const char *sec_dns);

/**
 * Convert given arguments to prepared blob for gsmd ubus set PDP context state method.
 * @param[in]  cid       PDP context id.
 * @param[in]  pri_dns   PDP context state.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_pdp_ctx_state(int cid, enum pdp_ctx_state_t state);

/**
 * Convert given arguments to prepared blob for gsmd ubus get DNS address method.
 * @param[in]  cid       PDP context id.
 * @param[in]  pri_dns   PDP context state.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_cid(int32_t cid);

/**
 * Convert given arguments to prepared blob for gsmd ubus set PDP call method.
 * @param[in]  mode_id      PDP call mode id.
 * @param[in]  cid   		PDP context id.
 * @param[ptr] urc_en   	PDP call URC event enable flag.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_pdp_call(enum pdp_call_mode_id mode_id, uint32_t cid, uint8_t *urc_en);

/**
 * Convert given arguments to prepared blob for gsmd ubus set USSD method.
 * @param[in]   mode 		Mode.
 * @param[in]   request 	Request string.
 * @param[ptr]   dcs 		Data coding scheme.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_ussd(enum ussd_mode_id mode, const char *request, uint32_t *dcs);

/**
 * Convert given arguments to prepared blob for gsmd ubus upload file method.
 * @param[ptr]   path 		File path.
 * @param[ptr]   size 		File size.
 * @param[ptr]   timeout 	File upload timeout.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_upload_file(const char *path, uint32_t *size, uint32_t *timeout);

/**
 * Convert given arguments to prepared blob for gsmd ubus delete and list files methods.
 * @param[ptr]   path 		File path.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_path(const char *path);

/**
 * Convert given arguments to prepared blob for gsmd ubus set new password method.
 * @param[in]   *fac   	    Facility.
 * @param[in]   old_pass    Old password.
 * @param[in]   new_pass    New password.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_new_pass(enum facility_t fac, const char *old_pass, const char *new_pass);

/**
 * Convert given arguments to prepared blob for gsmd ubus get storage space method.
 * @param[ptr]   path	Storage type id.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_storage(enum storage_type_id type);

/**
 * Convert given arguments to prepared blob for gsmd ubus get nmea sentence type method.
 * @param[in]    sata_id  Satellite constellation type id.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_nmea_sata(enum nmea_sata_type_id sata_id);

/**
 * Convert given arguments to prepared blob for gsmd ubus set nmea sentence type method.
 * @param[in]	sata_id  	Satellite constellation type id.
 * @param[ptr]  types    	NMEA type array.
 * @param[in]   type_count 	NMEA type array count.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_nmea_type(enum nmea_sata_type_id sata_id, enum nmea_type_id types[], int type_count);

/**
 * Convert given arguments to prepared blob for gsmd ubus start GNSS method.
 * @param[in]   mode_id       GNSS mode id.
 * @param[ptr]  fix_max_time  Maximum positioning time
 * @param[ptr]  fix_max_dist  Requested positioning accuracy level
 * @param[ptr]  fix_count     Number of positioning attempts
 * @param[ptr]  fix_interval  Interval of data report
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_gnss_mode(enum gnss_mode_id mode_id, int *fix_max_time,
				  enum gnss_accuracy_id accuracy, int *fix_count, int *fix_interval);
/**
 * Convert given arguments to prepared blob for gsmd ubus set NMEA ouptput port method.
 * @param[in]    port_id  NMEA sentence output port.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_nmea_outport(enum nmea_output_port_id port_id);

/**
 * Convert given arguments to prepared blob for gsmd ubus operator scan.
 * @param[in]  block_ifup  Flag of whether to block ifup procedure or not
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_block_ifup(uint8_t block_ifup);

/**
 * Convert given arguments to prepared blob for gsmd ubus get URC indication configuration method.
 * @param[in]   urc    	    URC type.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_urc(enum urc_ind_type_id urc);

/**
 * Convert given arguments to prepared blob for gsmd ubus set URC indication configuration method.
 * @param[in]   urc    	    URC type.
 * @param[in]   enambed     Is URC enabled.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_urc_enabled(enum urc_ind_type_id urc, bool enabled);

/**
 * Convert given arguments to prepared blob for gsmd ubus set SIM software hotplug method.
 * @param[in] recovery_count  Number of times to resend an APDU.
 * @param[in] detect_period   Automatic detection cycle.
 * @param[in] detect_count    Number of times of automatic detection.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_sim_soft_hotplug(int recovery_count, int detect_period, int detect_count);

/**
 * Convert given arguments to prepared blob for gsmd ubus set authentication corresponding bit method.
 * @param[in]   enabled     Is authentication corresponding bit enabled.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_auth_corr_bit(bool enabled);

/**
 * Convert given arguments to prepared blob for gsmd ubus set production line mode method.
 * @param[in]   enabled     Is production line mode enabled.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_prod_line_mode(bool enabled);

/**
 * Convert given arguments to prepared blob for gsmd ubus set automatic timezone update method.
 * @param[in]   urc    	    URC type.
 * @param[in]   enabled     Is automatic timezone update enabled.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_auto_tz_update(bool enabled);

/**
 * Convert given arguments to prepared blob for gsmd ubus set GNSS operation mode method.
 * @param[in]   mode    GNSS operation mode id.
 * @return blob_buf. Allocated blob that's used for communication. (Need to be freed via blob_buf_free).
 */
struct blob_buf prepare_gnss_oper_mode(enum gnss_operation_mode_id mode);

/******************
*  SET HANDLERS  *
******************/

/**
 * Execute AT command
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] cmd   	    Command to be executed on modem.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_exec(struct ubus_context *ctx, const char *cmd, char **resp, uint32_t modem_num,
		     uint32_t timeout);
/**
 * Set modem err format
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   fmt   	    Error message format id.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_err_fmt(struct ubus_context *ctx, enum err_msg_fmt_id fmt, func_t *resp,
			    uint32_t modem_num);
/**
 * Set modem clip mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   fmt   	    Clip mode id.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_clip_mode(struct ubus_context *ctx, enum clip_mode_id fmt, func_t *resp,
			      uint32_t modem_num);
/**
 * Set modem functionality mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   func   	    CFUN mode id.
 * @param[in]   reset  	    Whether to reset the modem after setting functionality mode.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_func(struct ubus_context *ctx, enum phone_func_id func, uint8_t reset, func_t *resp,
			 uint32_t modem_num);
/**
 * Set modem crc format
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   id   	    Cell RC mode id.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_crc_fmt(struct ubus_context *ctx, enum cell_rc_id id, func_t *resp, uint32_t modem_num);
/**
 * Set modem signal query configuration
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   id   	    Signal query configuration id.
 * @param[ptr]  chn_thr     Signal change threshold value.
 * @param[ptr]  rep_int     Signal query report interval.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_signal_query_cfg(struct ubus_context *ctx, enum signal_query_cfg_id id, int *chn_thr,
				     int *rep_int, func_t *resp, uint32_t modem_num);
/**
 * Set modem gprs mode mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   id   	    GPRS attach mode id.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_gprs_mode(struct ubus_context *ctx, enum gprs_attach_mode_id id, func_t *resp,
			      uint32_t modem_num);
/**
 * Set modem network registration state
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   id   	    Network registration state id.
 * @param[in]   op   	    Network registration operation id. I.e. for "AT+CREG", "AT+CGREG", "AT+CEREG".
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_net_reg(struct ubus_context *ctx, enum net_reg_mode_id id, enum net_reg_op_t op,
			    func_t *resp, uint32_t modem_num);
/**
 * Set modem MBN auto select flag
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   id   	    MBN auto select mode id.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_mbn_auto_sel(struct ubus_context *ctx, enum mbn_auto_sel_id id, func_t *resp,
				 uint32_t modem_num);
/**
 * Set modem network mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   id   	    Network mode id.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_net_mode(struct ubus_context *ctx, enum net_mode_id id, func_t *resp, uint32_t modem_num);
/**
 * Set modem roaming service mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   id   	    Roaming service mode id.
 * @param[in]   data   	    Structed restart information.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_roam_svc(struct ubus_context *ctx, enum roaming_svc_id id, uint8_t effect,
			     lgsm_structed_info_t *data, uint32_t modem_num);
/**
 * Set modem operator selection mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   id   	    Operator selection mode id.
 * @param[in]   fmt   	    Operator selection format.
 * @param[char] oper_num    Operator number.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_op_slct(struct ubus_context *ctx, enum op_slc_mode_id id, enum op_slc_fmt_id fmt,
			    const char *oper_num, func_t *resp, uint32_t modem_num);
/**
 * Set GSM bands
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   bands  	    GSM bands
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_gsm_band(struct ubus_context *ctx, uint64_t bands, func_t *resp, uint32_t modem_num);

/**
 * Set WCDMA bands
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   bands  	    WCDMA bands
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_wcdma_band(struct ubus_context *ctx, uint64_t bands, func_t *resp, uint32_t modem_num);

/**
 * Set LTE bands
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   bands  	    LTE bands
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_lte_band(struct ubus_context *ctx, uint64_t bands, func_t *resp, uint32_t modem_num);

/**
 * Set LTE_NB bands
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   bands  	    LTE_NB bands
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_lte_nb_band(struct ubus_context *ctx, uint64_t bands, func_t *resp, uint32_t modem_num);

/**
 * Set 5G SA bands
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   bands  	    5G SA bands
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_5g_sa_band(struct ubus_context *ctx, uint64_t bands, func_t *resp, uint32_t modem_num);

/**
 * Set 5G NSA bands
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   bands  	    5G NSA bands
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_5g_nsa_band(struct ubus_context *ctx, uint64_t bands, func_t *resp, uint32_t modem_num);

/**
 * Set modem pin code
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] pin   	    PIN code.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_pin_code(struct ubus_context *ctx, const char *pin, func_t *resp, uint32_t modem_num);
/**
 * Set modem pin code
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] puk   	    PUK code.
 * @param[char] pin   	    PIN code.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_puk_code(struct ubus_context *ctx, const char *puk, const char *pin, func_t *resp,
			     uint32_t modem_num);
/**
 * Set modem sms mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   id   	    SMS formt id.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_sms_mode(struct ubus_context *ctx, enum sms_format_id id, func_t *resp,
			     uint32_t modem_num);

/**
 * Send voice call
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] number 	    Nubmer to which the voice call has to be made.
 * @param[ptr]  resp	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_send_voice_call(struct ubus_context *ctx, const char *number, func_t *resp,
				uint32_t modem_num);
/**
 * Send sms
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] number 	    Nubmer to which the SMS has to be sent.
 * @param[char] text   	    SMS text.
 * @param[ptr]  data	    Structed amount of SMS sent.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_send_sms(struct ubus_context *ctx, const char *number, const char *text,
			 lgsm_structed_info_t *data, uint32_t modem_num);

/**
 * Read SMS
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   sms_index   SMS index number.
 * @param[in]   single 		Request only single PDU.
 * @param[in]   strip 		Do not print " <missing part> " on missing PDU.
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed read SMS information
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_read_sms_ex(struct ubus_context *ctx, int32_t sms_index, bool single, bool strip,
			    uint32_t modem_num, lgsm_structed_info_t *data);

/**
 * Read SMS
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   sms_index   SMS index number.
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed read SMS information
 * @return lgsm_err_t. Return function status code.
 */
static inline lgsm_err_t lgsm_read_sms(struct ubus_context *ctx, int32_t sms_index, uint32_t modem_num,
				       lgsm_structed_info_t *data)
{
	return lgsm_read_sms_ex(ctx, sms_index, false, false, modem_num, data);
}

/**
 * Delete single SMS
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   sms_index   SMS index number.
 * @param[in]   single 		Delete only single PDU.
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed read SMS information
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_delete_sms_ex(struct ubus_context *ctx, int32_t sms_index, bool single, uint32_t modem_num,
			      lgsm_structed_info_t *data);

/**
 * Delete single SMS
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   sms_index   SMS index number.
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed read SMS information
 * @return lgsm_err_t. Return function status code.
 */
static inline lgsm_err_t lgsm_delete_sms(struct ubus_context *ctx, int32_t sms_index, uint32_t modem_num,
					 lgsm_structed_info_t *data)
{
	return lgsm_delete_sms_ex(ctx, sms_index, false, modem_num, data);
}

/**
 * Set modem message storage cfg
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   mem   	    MSG storage mode id array for each zone.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @param[in]   timeout     Time to wait for the response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_msg_strg(struct ubus_context *ctx, enum msg_storage_id mem[3], func_t *resp,
			     uint32_t modem_num);

/**
 * Set URC port
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   port 		URC port string
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_urc_port(struct ubus_context *ctx, const char *port, func_t *resp, uint32_t modem_num);

/**
 * Set facility lock
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   *fac_lock   Structure representing facility lock
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_fac_lock(struct ubus_context *ctx, lgsm_fac_lock_info_t *fac_lock, func_t *resp,
			     uint32_t modem_num);

/**
 * Get facility lock
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   facility 	Facility type enumeration value.
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Struct to read facility lock information
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_get_fac_lock(struct ubus_context *ctx, enum facility_t facility, uint32_t modem_num,
			     lgsm_structed_info_t *data);

/**
 * Configure newly arrived SMS messages
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] evt_cfg	    Parameters.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_sms_evt_rep_cfg(struct ubus_context *ctx, lgsm_sms_evt_rep_cfg_t *evt_cfg, func_t *resp,
				    uint32_t modem_num);

/**
 * Configure DHCP packet filter
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in] 	bool	    Is feature disabled.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_dhcp_pkt_fltr(struct ubus_context *ctx, bool disabled, func_t *resp, uint32_t modem_num);

/**
 * Configure USB net mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in] 	mode	    USB net mode to be set.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_usbnet_mode(struct ubus_context *ctx, enum usbnet_mode_id mode, func_t *resp,
				uint32_t modem_num);

/**
 * Configure NAT mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in] 	mode	    NAT mode to be set.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_nat_mode(struct ubus_context *ctx, enum nat_mode_id mode, func_t *resp,
			     uint32_t modem_num);

/**
 * Configure UE usage mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in] 	mode	    UE usage mode to be set.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_ue_usage_mode(struct ubus_context *ctx, enum ue_usage_id mode, func_t *resp,
				  uint32_t modem_num);

/**
 * Set default bands described 'in modem_info.c:g_mid_data'
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_default_bands(struct ubus_context *ctx, func_t *resp, uint32_t modem_num);

/**
 * Set SIM state change reporting configuration
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   enabled 	Is state change reporting is enabled
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_sim_state_rep(struct ubus_context *ctx, func_t *resp, bool enabled, uint32_t modem_num);

/**
 * Set APN authentification
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   *apn_auth 	Pointer to structure that holds new APN auth information
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_apn_auth(struct ubus_context *ctx, func_t *resp, lgsm_apn_auth_info_t *apn_auth,
			     uint32_t modem_num);

/**
 * Read SMS
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   index    	APN index number.
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed read SMS information
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_get_apn_auth(struct ubus_context *ctx, int32_t index, uint32_t modem_num,
			     lgsm_structed_info_t *data);

/**
 * Set MTU info
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   value    	MTU value in little endian.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_mtu_info(struct ubus_context *ctx, char *value, func_t *resp, uint32_t modem_num);

/**
 * Set PDP context
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   *pdp_ctx 	Pointer to structure that holds new PDP context information
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_pdp_ctx(struct ubus_context *ctx, func_t *resp, lgsm_pdp_ctx_info_t *pdp_ctx,
			    uint32_t modem_num);

/**
 * Get PDP address
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   *index 		Index of PDP context
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed PDP address information
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_get_pdp_addr(struct ubus_context *ctx, int index, uint32_t modem_num,
			     lgsm_structed_info_t *data);

/**
 * Get DNS address
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   *cid 	    Index of PDP context
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed DNS information
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_get_dns_addr(struct ubus_context *ctx, int cid, uint32_t modem_num,
			     lgsm_structed_info_t *data);

/**
 * Get PDP call status
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   *cid 	    Index of PDP context
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed PDP call information
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_get_pdp_call(struct ubus_context *ctx, int32_t cid, uint32_t modem_num,
			     lgsm_structed_info_t *data);

/**
 * Set IMS state for Quectel modems
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   state 	    IMS state to be set.
 * @param[in]   modem_num   Modem identification number.
 * @param[bool] apply	    If true modem will be restarted if needed to apply this command
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_ims_state(struct ubus_context *ctx, enum ims_state_t state, lgsm_structed_info_t *data,
			      uint32_t modem_num, bool apply);

/**
 * Set VoLTE state
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   state 	    VoLTE state to be set.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_volte_state(struct ubus_context *ctx, enum ims_state_t state, lgsm_structed_info_t *data,
				uint32_t modem_num);

/**
 * Set M2M state
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   id 	    	M2M state to be set.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_m2m_state(struct ubus_context *ctx, enum m2m_state_id id, func_t *resp,
			      uint32_t modem_num);

/**
 * Set SIM initdelay
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   delay 		Delay in seconds to be set.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_sim_initdelay(struct ubus_context *ctx, func_t *resp, int delay, uint32_t modem_num);

/**
 * Send voice call
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   number 		Phone number to call
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_send_call(struct ubus_context *ctx, func_t *resp, const char *number, uint32_t modem_num);

/**
 * Accept voice call
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_accept_call(struct ubus_context *ctx, func_t *resp, uint32_t modem_num);

/**
 * Reject voice call
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_reject_call(struct ubus_context *ctx, func_t *resp, uint32_t modem_num);

/**
 * Set SIM hotswap
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   enabled 	Is hotswap enabled
 * @param[in]   polarity 	Polarity value enum to be set
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_sim_hotswap(struct ubus_context *ctx, func_t *resp, bool enabled,
				enum polarity_id polarity, uint32_t modem_num);

/**
 * Set SIM slot
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   index       SIM slot to be set.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_sim_slot(struct ubus_context *ctx, func_t *resp, int index, uint32_t modem_num);

/**
 * Change SIM slot
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_change_sim_slot(struct ubus_context *ctx, func_t *resp, uint32_t modem_num);

/**
 * Set WAN ping state
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   enabled 	Is WAN ping enabled
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_wan_ping(struct ubus_context *ctx, func_t *resp, bool enabled, uint32_t modem_num);

/**
 * Set LTE NAS backoff timer value
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   value 		LTE NAS backoff timer value be set.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_lte_nas_backoff_timer(struct ubus_context *ctx, func_t *resp, unsigned value,
					  uint32_t modem_num);

/**
 * Set PS attachment state
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   state 		PS attachment state to be set.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_ps_att_state(struct ubus_context *ctx, func_t *resp, enum ps_att_state_t state,
				 uint32_t modem_num);

/**
 * Set network category to be searched under LTE RAT
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   category 	Network category to be set.
 * @param[ptr]  effect 		Whether to take effect instantly.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_net_cat(struct ubus_context *ctx, func_t *resp, enum net_cat_t category, uint8_t *effect,
			    uint32_t modem_num);

/**
 * Set DNS address for a given PDP context
 * @param[ptr]  ctx   	   Ubus ctx.
 * @param[char] resp   	   Response from modem for the executed AT command.
 * @param[in]   cid        PDP context id
 * @param[prt]  pri_dns    Primary DNS address
 * @param[ptr]  sec_dns    Secondary DNS address
 * @param[in]   modem_num  Modem identification number.
 * @return lgsm_err_t.     Return function status code.
 */
lgsm_err_t lgsm_set_dns_addr(struct ubus_context *ctx, func_t *resp, int cid, const char *pri_dns,
			     const char *sec_dns, uint32_t modem_num);

/**
 * Set PDP context state
 * @param[ptr]  ctx   	   Ubus ctx.
 * @param[char] resp   	   Response from modem for the executed AT command.
 * @param[in]   cid        PDP context id
 * @param[in]   state      PDP context state
 * @param[in]   modem_num  Modem identification number.
 * @return lgsm_err_t.     Return function status code.
 */
lgsm_err_t lgsm_set_pdp_ctx_state(struct ubus_context *ctx, func_t *resp, int cid, enum pdp_ctx_state_t state,
				  uint32_t modem_num);

/**
 * Set PDP context state
 * @param[ptr]  ctx   	   Ubus ctx.
 * @param[char] resp   	   Response from modem for the executed AT command.
 * @param[in]   mode_id    PDP call mode id.
 * @param[in]   cid	   PDP context id
 * @param[ptr]	urc_en	   PDP call URC event enable flag.
 * @param[in]   modem_num  Modem identification number.
 * @return lgsm_err_t.	   Return function status code.
 */
lgsm_err_t lgsm_set_pdp_call(struct ubus_context *ctx, func_t *resp, enum pdp_call_mode_id mode_id,
			     uint32_t cid, uint8_t *urc_en, uint32_t modem_num);

/**
 * Set SIM software hotplug
 * @param[ptr]  ctx   	      Ubus ctx.
 * @param[char] resp   	      Response from modem for the executed AT command.
 * @param[in] recovery_count  Number of times to resend an APDU.
 * @param[in] detect_period   Automatic detection cycle.
 * @param[in] detect_count    Number of times of automatic detection.
 * @param[in] modem_num       Modem identification number.
 * @return lgsm_err_t.	      Return function status code.
 */
lgsm_err_t lgsm_set_sim_soft_hotplug(struct ubus_context *ctx, func_t *resp, int recovery_count,
				     int detect_period, int detect_count, uint32_t modem_num);

/**
 * Set USSD
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   mode 		Mode.
 * @param[in]   request 	Request string.
 * @param[ptr]  dcs 		Data coding scheme.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_ussd(struct ubus_context *ctx, func_t *resp, enum ussd_mode_id mode, const char *request,
			 uint32_t *dcs, uint32_t modem_num);

/**
 * Get time
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   mode    	Time mode.
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed time information
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_get_time(struct ubus_context *ctx, enum time_mode_id mode, uint32_t modem_num,
			 lgsm_structed_info_t *data);

/**
 * Delete a file
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[ptr]  path        File path
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t.	    Return function status code.
 */
lgsm_err_t lgsm_delete_file(struct ubus_context *ctx, func_t *resp, const char *path, uint32_t modem_num);

/**
 * Upload a file
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[ptr]  path        File path
 * @param[ptr]  size        File size
 * @param[ptr]  timeout     File upload timeout
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t.	    Return function status code.
 */
lgsm_err_t lgsm_upload_file(struct ubus_context *ctx, func_t *resp, const char *path, uint32_t *size,
			    uint32_t *timeout, uint32_t modem_num);

/**
 * Get file list
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[ptr]  path        File path
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed file list information
 * @return lgsm_err_t.	    Return function status code.
 */
lgsm_err_t lgsm_get_file_list(struct ubus_context *ctx, const char *path, uint32_t modem_num,
			      lgsm_structed_info_t *data);

/**
 * Get storage space information
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   type        Storage type.
 * @param[in]   modem_num   Modem identification number.
 * @param[ptr]  data	    Structed storage space information.
 * @return lgsm_err_t.	    Return function status code.
 */
lgsm_err_t lgsm_get_storage_space(struct ubus_context *ctx, enum storage_type_id type, uint32_t modem_num,
				  lgsm_structed_info_t *data);

/**
 * Start DFOTA upgrade processs
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[ptr]  path        File or URL path
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t.	    Return function status code.
 */
lgsm_err_t lgsm_dfota_upgrade(struct ubus_context *ctx, func_t *resp, const char *path, uint32_t modem_num);

/**
 * Set new password
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   *fac   	    Facility.
 * @param[in]   old_pass    Old password.
 * @param[in]   new_pass    New password.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_new_pass(struct ubus_context *ctx, func_t *resp, enum facility_t fac,
			     const char *old_pass, const char *new_pass, uint32_t modem_num);

/**
 * Set nmea sentence type
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]  	sata	    satellite constellation.
 * @param[in]   types	    Nmea sentence type array.
 * @param[in]   type_count  Number of elements in types array
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_nmea(struct ubus_context *ctx, func_t *resp, enum nmea_sata_type_id sata_id,
			 enum nmea_type_id types[], int type_count, uint32_t modem_num);

/**
 * Get nmea sentence type
 * @param[ptr]  ctx         Ubus ctx.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   sata        satellite constellation.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_get_nmea(struct ubus_context *ctx, enum nmea_sata_type_id sata_id, uint32_t modem_num,
			 lgsm_structed_info_t *data);

/**
 * Initialize GNSS
 * @param[ptr]  ctx         Ubus ctx.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_gnss_init(struct ubus_context *ctx, func_t *resp, uint32_t modem_num);

/**
 * Denitialize GNSS
 * @param[ptr]  ctx         Ubus ctx.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_gnss_deinit(struct ubus_context *ctx, func_t *resp, uint32_t modem_num);

/**
 * Start GNSS
 * @param[ptr]  ctx           Ubus ctx.
 * @param[char] resp          Response from modem for the executed AT command.
 * @param[in]   mode_id       GNSS mode id.
 * @param[ptr]  fix_max_time  Maximum positioning time
 * @param[ptr]  accuracy      Requested positioning accuracy level
 * @param[ptr]  fix_count     Number of positioning attempts
 * @param[ptr]  fix_interval  Interval of data report
 * @param[in]   modem_num     Modem identification number.
 */
lgsm_err_t lgsm_gnss_start(struct ubus_context *ctx, func_t *resp, enum gnss_mode_id mode_id,
			   int *fix_max_time, enum gnss_accuracy_id accuracy, int *fix_count,
			   int *fix_interval, uint32_t modem_num);

/**
 * Set NMEA output port
 * @param[ptr]  ctx         Ubus ctx.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   port_id     Ouptut port id for NMEA sentences.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_set_nmea_outport(struct ubus_context *ctx, func_t *resp, enum nmea_output_port_id port_id,
				 uint32_t modem_num);

/**
 * Stop GNSS
 * @param[ptr]  ctx         Ubus ctx.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_gnss_stop(struct ubus_context *ctx, func_t *resp, uint32_t modem_num);

/**
 * Set CEFS restore
 * @param[ptr]  ctx         Ubus ctx.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_set_cefs_restore(struct ubus_context *ctx, func_t *resp, uint32_t modem_num);

/**
 * Scan available operators
 * @param[ptr]  ctx         Ubus ctx
 * @param[in]   block_ifup  Flag of whether to block ifup procedure or not
 * @param[in]   modem_num   Modem identification number
 * @param[ptr]  data	    Structed information about operators
 */
lgsm_err_t lgsm_scan_operators(struct ubus_context *ctx, uint8_t block_ifup, uint32_t modem_num,
			       lgsm_structed_info_t *data);

/**
 * Set URC indication configuration
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   urc 	    URC type.
 * @param[in]   enabled	    Is URC enabled.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_urc_ind_cfg(struct ubus_context *ctx, func_t *resp, enum urc_ind_type_id urc,
				bool enabled, uint32_t modem_num);

/**
 * Get URC indication configuration
 * @param[ptr]  ctx         Ubus ctx.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   urc         URC type.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_get_urc_ind_cfg(struct ubus_context *ctx, enum urc_ind_type_id urc, uint32_t modem_num,
				lgsm_structed_info_t *data);

/**
 * Update modem current functionality mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   fmt 	    modem mode enum type.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_update_modem_func(struct ubus_context *ctx, enum modem_func_t fmt, func_t *resp,
				  uint32_t modem_num);

/**
 * Deactivate MBN list
 * @param[ptr]  ctx         Ubus ctx.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_mbn_deactivate(struct ubus_context *ctx, func_t *resp, uint32_t modem_num);

/**
 * select MBN from the list
 * @param[ptr]  ctx         Ubus ctx.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   mbn_name    MBN name to be selected.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_mbn_select(struct ubus_context *ctx, func_t *resp, const char *mbn_name, uint32_t modem_num);

/**
 * Get URC indication configuration
 * @param[ptr]  ctx         Ubus ctx.
 * @param[in]   enabled	    Is authentication corresponding bit setting is enabled.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_set_auth_corr_bit(struct ubus_context *ctx, bool enabled, func_t *resp, uint32_t modem_num);

/**
 * @param[ptr]  ctx         Ubus ctx.
 * @param[in]   enabled	    Is production line mode enabled.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_set_prod_line_mode(struct ubus_context *ctx, bool enabled, func_t *resp, uint32_t modem_num);

/**
 * Set automatic timezone update configuration
 * @param[ptr]  ctx         Ubus ctx.
 * @param[in]   enabled	    Is authentication corresponding bit setting is enabled.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_set_auto_tz_update(struct ubus_context *ctx, bool enabled, func_t *resp, uint32_t modem_num);

/**
 * Set GNSS operation mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   mode 	    GNSS operation mode enum type.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_gnss_oper_mode(struct ubus_context *ctx, func_t *resp, enum gnss_operation_mode_id mode,
				   uint32_t modem_num);

/**
 * Set urc cause support configuration
 * @param[ptr]  ctx         Ubus ctx.
 * @param[in]   enabled	    Is urc cause support enabled.
 * @param[char] resp        Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 */
lgsm_err_t lgsm_set_urc_cause_support(struct ubus_context *ctx, bool enabled, func_t *resp,
				      uint32_t modem_num);

/**
 * Set DPO operation mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   mode 	    DPO mode enum type.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_dpo_mode(struct ubus_context *ctx, func_t *resp, enum dpo_mode_id mode,
			     uint32_t modem_num);

/**
 * Set DISABLE 5G mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   mode 	    Disable 5G mode enum type.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_quectel_set_nr5g_disable_mode(struct ubus_context *ctx, func_t *resp,
					      enum disable_nr5g_mode_id mode, uint32_t modem_num);

/**
 * Disable GEA1 and GEA2 algorithms
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[char] resp   	    Response from modem for the executed AT command.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_disable_gea1_2_algo(struct ubus_context *ctx, func_t *resp, uint32_t modem_num);

/******************
*  GET HANDLERS  *
******************/

/**
 * Parse modem info method response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_info_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
   * Parse iccid method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_iccid_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
   * Parse operator scan method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_oper_scan_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse FW method response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_fw_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse modem model method response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
//void handle_model_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse IMSI method response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_imsi_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse Network info method response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_network_info_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse Serving cell info method response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_serving_cell_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse Neighbour cells info method response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_neigh_cell_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse error format response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_err_fmt_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse Cell RC format response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_crc_fmt_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse modem fucntionality response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_func_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse CLIP mode response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_clip_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse signal query configuration response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_signal_query_cfg_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse signal query repsonse
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_signal_query_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse FW method response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_gprs_mode_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse network registration status response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_net_reg_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse network registration status response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_net_reg_all_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse EN-DC support response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_endc_support_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse MBN list response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_mbn_list(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse MBN auto select response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_mbn_auto_sel_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse network mode response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_net_mode_tech_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse network mode type response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_net_mode_type_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse roaming state response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_roaming_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse service provider response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_srvc_provider_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse operator select response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_op_select_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse band list response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_band_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse pin state response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_pin_st_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse pin state response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_pin_cnt_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse sms mode response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_sms_mode_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse message storage response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_msg_storage_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse send SMS response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_sms_send_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse SMS response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_read_sms_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse SMS delete response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_delete_sms_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse SMS get sms event reporting configuration response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_sms_evt_rep_cfg_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse Temperature response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_temperature_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse DHCP packet filtering status response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_dhcp_pkt_fltr_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse usbnet mode cfg response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_usbnet_cfg_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse nat mode cfg response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_nat_cfg_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
 * Parse nat mode cfg response
 * @param[ptr]   info      Blob from gsmd.
 * @param[ptr]   parsed    Parsed union readable information.
 */
void handle_get_ue_usage_cfg_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
   * Parse urc port retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_urc_port_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
   * Parse facility lock retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_fac_lock_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);
/**
   * Parse APN auth retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_apn_auth_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse PDP context list retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_pdp_ctx_list(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse PDP address retreival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_pdp_addr_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse PDP address list retreival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_pdp_addr_list_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse IMS state retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_ims_state_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse IMS state set method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_set_ims_state_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse roaming state set method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_set_roaming_state_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse VoLTE state retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_volte_man_state_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse M2M state retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_m2m_state_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse VoLTE ready retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_volte_ready_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse VoLTE set state method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_set_volte_man_state_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse SIM initdelay retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_sim_initdelay_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse SIM hotswap retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_sim_hotswap_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse sim slot retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_sim_slot_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse wan ping retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_wan_ping_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse incoming call list retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_inc_voice_calls_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse LTE NAS backoff timer retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_lte_nas_backoff_timer_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse MTU INFO method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_mtu_info_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse module status retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_mod_status_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse PS attachment state retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_ps_att_state_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse network category retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_net_cat_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse SIM status retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_sim_stat_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse time retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_time_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse active PDP context list retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_act_pdp_ctx_list_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse attached PDP context list retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_attached_pdp_ctx_list_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse DNS address retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_dns_addr_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse PDP call retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_pdp_call_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse PDP context list retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_ca_info_list(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse file list retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_file_list_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse storage space information retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_storage_space_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse nmea type information retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_nmea_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse URC indication configuration information retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_urc_ind_cfg_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse get secure boot configuration method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_sec_boot_cfg_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse get cpu serial number method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_cpu_sn_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse MODEM FUNCTIONALITY indication information retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_def_modem_func(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse Flash state inofrmation method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_flash_state_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse PDP profile registry information method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_pdp_profile_registry(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse Authenctication corresponding bit information method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_ltesms_format(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse SIM softwre hotplug retrival method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_sim_soft_hotplug_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse Authenctication corresponding bit information method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_auth_corr_bit_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse efs version method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_efs_verion_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse production line mode information method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_prod_line_mode_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse Automatic timezone update method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_auto_tz_update_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse GNSS operation mode method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_gnss_oper_mode_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse URC cause support method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_urc_cause_support(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse DPO mode method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_dpo_mode_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse DISABLE 5G mode method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_disable_5g_mode_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse GEA algorythm method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_gea_algo(struct blob_attr *info, lgsm_structed_info_t *parsed);

/**
   * Parse dhcpv6 state method response
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    Parsed union readable information.
   */
void handle_get_usbcfg_rsp(struct blob_attr *info, lgsm_structed_info_t *parsed);

/*********************
*  STRUCT HANDLERS  *
*********************/

/**
 * Free gsm array
 * @param[ptr] gsm_arr_t  Pointer to gsm array table
 */
void handle_gsm_arr_free(lgsm_arr_t *gsm_arr_t);
/**
 * Free gsm table
 * @param[ptr] gsm_t Pointer to gsm table
 */
void handle_gsm_free(lgsm_t *gsm_t);
//-----------------------------
/**
 * Free structed info table
 * *param[ptr] gsm_structed_info_t  Pointer to structed gsm info table
 */
void handle_gsm_structed_info_free(lgsm_structed_info_t *gsm_structed_info);
//-----------------------------
/*
 * Parse status code into readable error message
 * @param[in]   status  Libgsm status error code.
 * @return char. Readable error message.
 */
const char *lgsm_strerror(lgsm_err_t status);
/*
 * Get modem count
 * @param[ptr]  ctx        Ubus context.
 * @param[in]   modem_num  modem count.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_get_modem_num(struct ubus_context *ctx, uint32_t *modem_num);
/*
 * Get all available modem information
 * @param[ptr]  ctx        Ubus context.
 * @param[ptr]  gsm_arr_t  libgsm modem info array table.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_get_modems_info(struct ubus_context *ctx, lgsm_arr_t *gsm_arr_t);
/*
 * Get specific modem information
 * @param[ptr] ctx 	       Ubus context
 * @param[ptr] gsm_t       libgsm modem info table
 * @param[in]  modem_num   Modem number
 * @return     lgsm_err_t. Return function status code
 */
lgsm_err_t lgsm_get_modem_info(struct ubus_context *ctx, lgsm_t *gsm_t, uint32_t modem_num);
/*
 * Subscribe to given modem ubus object
 * @param[ptr]  	  ctx        Ubus context.
 * @param[ptr]  	  gsm_sub    Ubus subscriber context.
 * @param[ubus_handler_t] cb	     Subscriber state change callback.
 * @param[ptr] 		  rem_cb     Object remove callback.
 * @param[in]	  	  modem_num  Modem number.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_subscribe(struct ubus_context *ctx, struct ubus_subscriber *gsm_sub, ubus_handler_t cb,
			  void *rem_cb, uint32_t modem_num);
/*
 * Subscribe to given modem using ubus object id
 * @param[ptr]			ctx          Ubus context.
 * @param[ptr]			gsm_sub      Ubus subscriber context.
 * @param[ubus_handler_t]	cb	     Subscriber state change callback.
 * @param[ptr]			rem_cb       Object remove callback.
 * @param[in]			ubus_obj_id  Modem ubus object id.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_subscribe_obj(struct ubus_context *ctx, struct ubus_subscriber *gsm_sub, ubus_handler_t cb,
			      void *rem_cb, uint32_t ubus_obj_id);
/*
 * Subscribe to given modem using ubus object path
 * @param[ptr]			ctx          Ubus context.
 * @param[ptr]			gsm_sub      Ubus subscriber context.
 * @param[ubus_handler_t]	cb	     Subscriber state change callback.
 * @param[ptr]			rem_cb       Object remove callback.
 * @param[str]			modem_path   Modem ubus object path.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_subscribe_name(struct ubus_context *ctx, struct ubus_subscriber *gsm_sub, ubus_handler_t cb,
			       void *rem_cb, char *modem_path);
/*
 * Execute given method and get blob info
 * @param[ptr]  ctx      Ubus context.
 * @param[in]   method_n method table method id.
 * @param[in]   obj      Modems ubus object id.
 * @param[ptr]  info     Blob where the response will be put.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_handle_get(struct ubus_context *ctx, lgsm_method_t method_n, uint32_t obj,
			   struct blob_attr **info);

/*
 * Set given method information
 * @param[ptr]  ctx      Ubus context.
 * @param[in]   method_n method table method id.
 * @param[in]   obj      Modems ubus object id.
 * @param[ptr]  info     Blob which will be sent to modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_handle_set(struct ubus_context *ctx, lgsm_method_t method_n, uint32_t obj,
			   struct blob_attr **info);
/*
 * Handle given GSMD ubus methods
 * @param[ptr]  ctx        Ubus context.
 * @param[in]   method_n   method table method id.
 * @param[in]   modem_num  modem number.
 * @param[ptr]  info       Blob which will be sent or taken.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_handle_methods(struct ubus_context *ctx, lgsm_method_t method_n, uint32_t modem_num,
			       struct blob_attr **info, ...);
/*
 * Handle given GSMD ubus methods
 * @param[ptr]  ctx        Ubus context.
 * @param[in]   method_n   method table method id.
 * @param[in]   modem_num  modem number.
 * @param[ptr]  data       Structurized response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_handle_methods_structed(struct ubus_context *ctx, lgsm_method_t method_n, uint32_t modem_num,
					lgsm_structed_info_t *data);

/*
 * Handle given GSMD ubus methods
 * @param[ptr]  ctx        Ubus context.
 * @param[in]   method_n   method table method id.
 * @param[in]   obj_id     modems ubus object id.
 * @param[ptr]  info       Blob which will be sent or taken.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_handle_obj_methods(struct ubus_context *ctx, lgsm_method_t method_n, uint32_t obj_id,
				   struct blob_attr **info, ...);

/*
 * Handle given GSMD ubus methods
 * @param[ptr]  ctx        Ubus context.
 * @param[in]   method_n   method table method id.
 * @param[in]   obj_id     modems ubus object id.
 * @param[ptr]  data       Structurized response from the modem.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_handle_obj_methods_structed(struct ubus_context *ctx, lgsm_method_t method_n, uint32_t obj_id,
					    lgsm_structed_info_t *data);

/**
 * Set LTE SMS format mode
 * @param[ptr]  ctx   	    Ubus ctx.
 * @param[in]   mode   	    format mode id.
 * @param[in]   resp   	    Status code for modem setting configuration.
 * @param[in]   modem_num   Modem identification number.
 * @return lgsm_err_t. Return function status code.
   */
lgsm_err_t lgsm_set_ltesms_format(struct ubus_context *ctx, enum urc_lte_sms_format mode, func_t *resp,
				  uint32_t modem_num);

/*
 * Set modem functionality
 * @param[ptr]  ctx        Ubus context.
 * @param[in]   modem_num  Modem number.
 * @param[in]   nmea       NMEA port config value
 * @param[in]   modem      Modem port config value
 * @param[in]   adb        ADB port config value
 * @param[in]   uac        UAC port config value
 * @param[in]   resp       Status code for modem setting configuration.
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_set_usbcfg(struct ubus_context *ctx, bool nmea, bool modem, bool adb, bool uac, func_t *resp,
			   uint32_t modem_num);

/**
   * Checks if 'info' blob contains error message
   * @param[ptr]   info      Blob from gsmd.
   * @param[ptr]   parsed    If not NULL, and error message is found then, sets 'label' and 'data.status' to found 'errno'
   *  of type 'func_t'
   * @return lgsm_err_t. Returns LGSM_ERROR if error message is found. LGSM_SUCCESS otherwise
   */
lgsm_err_t lgsm_is_err(struct blob_attr *info, lgsm_structed_info_t *parsed);

/*
 * Convert UBUS error code into lgsm_err_t
 * @param[in] ubus_err UBUS error code
 * @return lgsm_err_t. Return function status code.
 */
lgsm_err_t lgsm_parse_ubus_err(int ubus_err);
#ifdef __cplusplus
}
#endif
