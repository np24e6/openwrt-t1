#ifndef GSM_MODEM_API
#define GSM_MODEM_API

/** @file modem_api.h */

// Define how many modems gsmd-ng will support
#define MAX_MODEM_OBJS 12

/**
 * Enumeration of modem method action values
 */
typedef enum {
	ACT_OK, /*!< Valid result received */
	ACT_ERROR, /*!< Unknown error occurred */
	ACT_ARG_INVALID, /*!< Invalid or missing arguments */
	ACT_MALLOC_FAIL, /*!< Failed memory allocation */
	ACT_EXCEPTION, /*<! Fatal exception occurred */
	ACT_RESPONSE_INVALID, /*!< Response is not valid */
	ACT_URC_INVALID, /*!< URC response is not valid */
	ACT_NOT_SUPPORTED, /*!< Functionality not supported */
	ACT_TIMEOUT, /*!< Request timeout expired */
	ACT_SMS_LIMIT, /*!< SMS limit reached */

	__ACT_MAX,
} func_t;

/**
 * Enumeration of event types
 */
typedef enum {
	EVT_UNKNOWN, /*!< Unknown event */
	EVT_PIN_STATE, /*!< Pin state event */
	EVT_NET_MODE, /*!< Network mode event */
	EVT_CLIP_VALUE, /*!< Incoming voice call event */
	EVT_RING, /*!< Incoming call event */
	EVT_NO_CARRIER, /*!< The connection has been terminated or the attempt to establish a connection failed*/
	EVT_SND_SMS, /*!< Event called on successfull sms sending */

	EVT_RSSI_SIGNAL, /*!< RSSI signal event */
	EVT_RSCP_SIGNAL, /*!< RSCP signal event */
	EVT_ECIO_SIGNAL, /*!< EC/IO signal event */
	EVT_RSRP_SIGNAL, /*!< RSRP signal event */
	EVT_SINR_SIGNAL, /*!< SINR signal event */
	EVT_RSRQ_SIGNAL, /*!< RSRQ signal event */
	EVT_BER_SIGNAL, /*!< BER signal event */

	EVT_CMTI_VALUE, /*!< CMTI value event */
	EVT_CUSD_VALUE, /*!< CUSD value event */
	EVT_QIND_VALUE, /*!< QIND value event */
	EVT_CREG_VALUE, /*!< CREG state event */
	EVT_CGREG_VALUE, /*!< CGREG state event */
	EVT_C5GREG_VALUE, /*!< C5GREG state event */

	EVT_NEW_MDM, /*!< New modem event */
	EVT_SIM_STAT, /*!< SIM status event */
	EVT_FILE_UPLOAD, /*!< File upload event */
	EVT_EMM, /*!< EMM reject cause event */
	EVT_ESM, /*!< ESM reject cause event */

	EVT_OPERATOR, /*!< Operator change event */
	EVT_SIM_CHANGE, /*!< SIM change event */

	__EVT_MAX,
} evt_type_t;

/**
 * Enumeration of modem pin states
 */
enum pin_state_id {
	PIN_STATE_UNKNOWN, /*!< Unknown pin state */
	PIN_STATE_OK, /*!< Simcard is ready, pin is set */
	PIN_STATE_NOT_READY, /*!< Simcard is not ready */
	PIN_STATE_REQUIRED_PIN = 4, /*!< PIN required to use simcard */
	PIN_STATE_REQUIRED_PUK = 5, /*!< PUK required to use simcard */
	PIN_STATE_REQUIRED_PH_NET_PIN, /*!< Network personalization password required to use simcard */
	PIN_STATE_REQUIRED_PH_NET_PUK, /*!< Network personalization unlocking password required to use simcard */
	PIN_STATE_REQUIRED_PH_NETSUB_PIN, /*!< Network subset personalization password required to use simcard */
	PIN_STATE_REQUIRED_PH_NETSUB_PUK, /*!< Network subset personalization unlocking password required to use simcard */
	//CME ERROR:
	PIN_STATE_NOT_INSERTED = 10, /*!< Simcard is not inserted */
	PIN_STATE_SIM_FAILURE  = 13, /*!< Simcard failure */
	PIN_STATE_SIM_BUSY     = 14, /*!< Simcard busy */
	PIN_STATE_PUK_BLOCKED  = 15, /*!< Simcard PUK is blocked */

	__PIN_STATE_MAX,
};

/**
 * Enumeration of modem SIM states
 */
enum sim_state_id {
	SIM_STATE_UNKNOWN, /*!< Unknown sim state */
	SIM_STATE_NOT_INSERTED, /*!< Simcard is not inserted */
	SIM_STATE_INSERTED, /*!< Simcard is inserted */

	__SIM_STATE_MAX,
};

/**
 * Enumeration of modem network modes
 */
enum net_mode_id {
	NET_MODE_UNKNOWN, /*!< Unknown network mode */
	NET_MODE_AUTO, /*!< Automatically select mode */
	NET_MODE_NO_SERVICE, /*!< No service mode */

	// 2G
	NET_MODE_2G, /*!< 2G mode */
	NET_MODE_GSM, /*!< GSM network mode */
	NET_MODE_GPRS, /*!< GPRS network mode */
	NET_MODE_EDGE, /*!< EDGE network mode */

	// 3G
	NET_MODE_3G, /*!< 3G mode */
	NET_MODE_WCDMA, /*!< 3G WCDMA mode */
	NET_MODE_TDSCDMA, /*!< 3G TDSCDMA mode */
	NET_MODE_CDMA, /*!< 3G CDMA mode */
	NET_MODE_EVDO, /*!< 3G EVDO mode */
	NET_MODE_CDMA_EVDO, /*!< 3G CDMA/EVDO mode */
	NET_MODE_HSDPA, /*!< 3G high speed downlink packet access mode */
	NET_MODE_HSUPA, /*!< 3G high speed uplink packet access mode */
	NET_MODE_HSPA_PLUS, /*!< 3G evolved high speed packet access mode */
	NET_MODE_EHRPD, /*!< Enhanced HRPD */
	NET_MODE_HDR, /*!< High Data Rate */
	NET_MODE_UMTS, /*!< 3G UMTS mode */
	NET_MODE_HSDPA_PLUS_HSUPA, /*!< 3G high speed downlink + uplink packet access mode */

	// 4G
	NET_MODE_4G, /*!< 4G mode */
	NET_MODE_LTE, /*!< 4G LTE mode */

	// 5G
	NET_MODE_5G, /*!< 5G mode */
	NET_MODE_NR5G, /*!< 5G NR mode */
	NET_MODE_5G_NSA, /*!< 5G NSA mode */
	NET_MODE_5G_SA, /*!< 5G SA mode */

	// NB
	NET_MODE_CAT_M1, /*!< CAT-M1 mode */
	NET_MODE_CAT_NB, /*!< CAT-NB mode */
	NET_MODE_EMTC, /*!< eMTC mode */
	NET_MODE_NBIOT, /*!< NBIoT mode */

	// combined network modes
	NET_MODE_2G_3G, /*!< 2G and 3G mode */
	NET_MODE_GSM_WCDMA, /*!< GSM and 3G WCDMA mode */
	NET_MODE_2G_4G, /*!< 2G and 4G mode */
	NET_MODE_GSM_LTE, /*!< GSM and 4G LTE mode */
	NET_MODE_3G_4G, /*!< 3G and 4G mode */
	NET_MODE_WCDMA_LTE, /*!< 3G WCDMA and 4G LTE mode */
	NET_MODE_3G_5G, /*!< 3G and 5G mode */
	NET_MODE_WCDMA_NR5G, /*!< 3G WCDMA and 5G NR mode */
	NET_MODE_4G_5G, /*!< 4G and 5G mode */
	NET_MODE_LTE_NR5G, /*!< 4G LTE and 5G NR mode */

	__NET_MODE_MAX,
};

/**
 * Enumeration of SMS message format types
 */
enum sms_format_id {
	SMS_MODE_UNKNOWN, /*!< Unknown message format */
	SMS_MODE_PDU, /*!< PDU message format */
	SMS_MODE_TEXT, /*!< Text message format */

	__SMS_MODE_MAX,
};

/**
 * Enumeration of AT capabilities
 */
enum cap_set_id {
	CAP_SET_UNKNOWN, /*!< Unknown capability set*/
	CAP_SET_GSM, /*!< GSM ETSI capability set*/
	CAP_SET_FAX, /*!< Fax capability set*/
	CAP_SET_DS, /*!< Data compression capability set*/
	CAP_SET_MS, /*!< Modulation control capability set*/
	CAP_SET_ES, /*!< Error control capability set*/
	CAP_SET_MV18S, /*!< V18 modulation capability set*/

	__CAP_SET_MAX,
};

/**
 * Enumeration of SMS state id
 */
enum sms_state_id {
	SMS_STATE_UNREAD, /*!< Received unread message */
	SMS_STATE_READ, /*!< Received read message */
	SMS_STATE_UNSENT, /*!< Stored unsent message */
	SMS_STATE_SENT, /*!< Stored sent message */
	SMS_STATE_ALL, /*!< All messages */
	SMS_STATE_UNKNOWN, /*!< Unknown message */

	__SMS_STATE_MAX,
};

/**
 * Enumeration of signal query configuration values
 */
enum signal_query_cfg_id {
	SIGNAL_QUERY_UNKNOWN, /*!< Unknown query value */
	SIGNAL_QUERY_URC_ON, /*!< URC query is enabled */
	SIGNAL_QUERY_URC_OFF, /*!< URC query is disabled */

	__SIGNAL_QUERY_MAX,
};

/**
 * Enumeration of phone functionality values
 */
enum phone_func_id {
	PHONE_FUNC_UNKNOWN, /*<! Unknown value */
	PHONE_FUNC_MINIMAL, /*!< Minimal functionality */
	PHONE_FUNC_FULL, /*<! Full functionality */
	PHONE_FUNC_RF_DISABLE, /*<! Disable RX/TX(RF) transmit/receive */
	PHONE_FUNC_CEFS_CORRUPTED, /*<! CEFS partition is corrupted */
	PHONE_FUNC_BASEBAND_CORRUPTED, /*<! Baseband segment is corrupted */

	__PHONE_FUNC_MAX,
};

/**
 * Enumeration of GPRS attach modes
 */
enum gprs_attach_mode_id {
	GPRS_ATTACH_UNKNOWN, /*<! Unknown mode */
	GPRS_ATTACH_AUTO, /*<! Auto mode */
	GPRS_ATTACH_MANUAL, /*<! Manual mode */

	__GPRS_ATTACH_MAX,
};

/**
 * Enumeration of Error message format values
 */
enum err_msg_fmt_id {
	ERR_MSG_FMT_UNKNOWN, /*<! Unknown value */
	ERR_MSG_FMT_DISABLE, /*<! Disable result code */
	ERR_MSG_FMT_NUMERIC, /*<! Enable result code with numeric values */
	ERR_MSG_FMT_VERBOSE, /*<! Enable result code with verbose values */

	__ER_MSG_FMT_MAX,
};

/**
 * Enumeation of network registration mode values
 */
enum net_reg_mode_id {
	NET_REG_MODE_UNKNOWN, /*<! Unknown mode */
	NET_REG_MODE_DISABLE, /*<! Disable result code */
	NET_REG_MODE_ENABLE, /*<! Enable result code */
	NET_REG_MODE_ENABLE_LOC, /*<! Enable result code with information */

	__NET_REG_MODE_MAX,
};

/**
 * Enumeration of network registration stat values
 */
enum net_reg_stat_id {
	NET_REG_STAT_NOT_REGISTERED, /*<! Not registered */
	NET_REG_STAT_REGISTERED, /*<! Registered, home */
	NET_REG_STAT_SEARCHING, /*<! Searching */
	NET_REG_STAT_DENIED, /*<! Denied */
	NET_REG_STAT_UNKNOWN, /*<! Unknown stat */
	NET_REG_STAT_ROAMING, /*<! Roaming */
	NET_REG_STAT_SMS_ONLY_HOME, /*<! SMS only, home network */
	NET_REG_STAT_SMS_ONLY_ROAMING, /*<! SMS only, Roaming */
	NET_REG_STAT_EMERGENCY, /*<! Emergency services only */

	__NET_REG_STAT_MAX,
};

/**
 * Enumeration of network registration access technology values
 */
enum net_reg_act_id {
	NET_REG_ACT_UNKNOWN, /*<! Unknown value */
	NET_REG_ACT_GSM, /*<! GSM technology */
	NET_REG_ACT_GSM_COMPACT, /*<! GSM Compact technology */
	NET_REG_ACT_UTRAN, /*<! UTRAN technology */
	NET_REG_ACT_GSM_EGPRS, /*<! GSM, EGPRS technologies */
	NET_REG_ACT_UTRAN_HSDPA, /*<! UTRAN, HSDPA technologies */
	NET_REG_ACT_UTRAN_HSUPA, /*<! UTRAN, HSUPA technologies */
	NET_REG_ACT_UTRAN_HSDPA_HSUPA, /*<! UTRAN, HSDPA, HSUPA technologies */
	NET_REG_ACT_EUTRAN, /*<! E-UTRAN technology */
	NET_REG_ACT_CDMA, /*<! CDMA technology */
	NET_REG_ACT_EUTRAN_5GCN, /*<! E-UTRAN connected to a 5GCN */
	NET_REG_ACT_NR_5GCN, /*<! NR connected to 5GCN */
	NET_REG_ACT_NG_RAN, /*<! NG-RAN technology */
	NET_REG_ACT_EUTRAN_NR, /*<! E-UTRAN-NR dual connectivity */

	__NET_REG_ACT_MAX,
};

/**
 * Enumeration of MBN auto selection values
 */
enum mbn_auto_sel_id {
	MBN_AUTO_SEL_UNKNOWN, /*<! Unknown value */
	MBN_AUTO_SEL_DISABLE, /*<! Disable auto selection */
	MBN_AUTO_SEL_ENABLE, /*<! Enable auto selection */

	__MBN_AUTO_SEL_MAX,
};

/**
 * Enumeration of roaming service values
 */
enum roaming_svc_id {
	ROAMING_SVC_UNKNOWN, /*<! Unknown value */
	ROAMING_SVC_DISABLE, /*<! Disable service */
	ROAMING_SVC_ENABLE, /*<! Enable service */
	ROAMING_SVC_AUTO, /*<! Auto mode */

	__ROAMING_SVC_MAX,
};

/**
 * Enumeration of M2M service values
 */
enum m2m_state_id {
	M2M_STATE_UNKNOWN, /*<! Unknown value */
	M2M_STATE_DISABLED, /*<! Disable service */
	M2M_STATE_ENABLED, /*<! Enable service */

	__M2M_STATE_MAX,
};

/**
 * Enumeration of operator selection mode values
 */
enum op_slc_mode_id {
	OP_SLC_MODE_UNKNOWN, /*<! Unknown value */
	OP_SLC_MODE_AUTO, /*<! Auto mode */
	OP_SLC_MODE_MANUAL, /*<! Manual mode */
	OP_SLC_MODE_UNREGISTER, /*<! Unregister mode */
	OP_SLC_MODE_ONLY_FORMAT, /*<! Set only-format mode */
	OP_SLC_MODE_MANUAL_AUTO, /*<! Manual-auto mode */

	__OP_SLC_MODE_MAX,
};

/**
 * Enumeration of operator selection format values
 */
enum op_slc_fmt_id {
	OP_SLC_FMT_UNKNOWN, /*<! Unknown value */
	OP_SLC_FMT_LONG, /*<! Alphanumeric long format */
	OP_SLC_FMT_SHORT, /*<! Alphanumeric short format */
	OP_SLC_FMT_NUMERIC, /*<! Numeric format */

	__OP_SLC_FMT_MAX,
};

/**
 * Enumeration of operator selection state values
 */
enum op_slc_stat_id {
	OP_SLC_STAT_UNKNOWN, /*<! Unknown value */
	OP_SLC_STAT_AVAILABLE, /*<! Available state */
	OP_SLC_STAT_CURRENT, /*<! Current state */
	OP_SLC_STAT_FORBIDDEN, /*<! Forbidden state */

	__OP_SLC_STAT_MAX,
};

/**
 * Enumeration of calling line identification presentation mode values
 */
enum clip_mode_id {
	CLIP_MODE_UNKNOWN, /*<! Unknown value */
	CLIP_MODE_DISABLE, /*<! Disabled mode */
	CLIP_MODE_ENABLE, /*<! Enabled mode */

	__CLIP_MODE_MAX,
};

/**
 * Enumeration of calling line identification presentation state values
 */
enum clip_stat_id {
	CLIP_STAT_UNKNOWN, /*<! Unknown value */
	CLIP_STAT_NOT_PROVISIONED, /*<! Not provisioned state */
	CLIP_STAT_PROVISIONED, /*<! Provisioned state */

	__CLIP_STAT_MAX,
};

/**
 * Enumeration of cellular result code format values
 */
enum cell_rc_id {
	CELL_RC_UNKNOWN, /*<! Unknown value */
	CELL_RC_DISABLE, /*<! Disable format */
	CELL_RC_ENABLE, /*<! Enable format */

	__CEL_RC_MAX,
};

/**
 * Enumeration of message storage type values
 */
enum msg_storage_id {
	MSG_STORAGE_UNKNOWN, /*<! Unknown value */
	MSG_STORAGE_USIM, /*<! USIM storage */
	MSG_STORAGE_MOBILE_EQUIP, /*<! Mobile equipment storage */
	MSG_STORAGE_MOBILE_EQUIP_ANY, /*<! Anything related with mob equip */

	__MSG_STORAGE_MAX,
};

/**
 * Enumaration of UE network state values
 */
enum ue_state_id {
	UE_STATE_UNKNOWN, /*<! Unknown value */
	UE_STATE_SEARCH, /*<! UE seraching */
	UE_STATE_LIMSRV, /*<! UE limited service */
	UE_STATE_NOCONN, /*<! UE idle mode */
	UE_STATE_CONNECT, /*<! UE active mode */
	UE_STATE_NOT_SUP, /*<! Not supported */

	__UE_STATE_MAX,
};

/**
 * Enumeration of USSD mode values
 */
enum ussd_mode_id {
	USSD_MODE_UNKNOWN, /*<! Unknown value */
	USSD_MODE_DISABLE, /*<! Disable the result code presentation */
	USSD_MODE_ENABLE, /*<! Enable the result code presentation */
	USSD_MODE_CANCEL, /*<! Cancel session */

	__USSD_MODE_MAX,
};

/**
 * Enumeration of USSD response state values
 */
enum ussd_state_id {
	USSD_STATE_ACT_NO, /*<! No further action */
	USSD_STATE_ACT_REQ, /*<! Action required */
	USSD_STATE_TERM, /*<! Terminated by network */
	USSD_STATE_LOC_RESP, /*<! Local client responded */
	USSD_STATE_NOT_SUP, /*<! Operation not supported */
	USSD_STATE_TIMEOUT, /*<! Network timeout*/
	USSD_STATE_UNKNOWN, /*<! USSD state unknown */

	__USD_STATE_MAX,
};

/**
 * Enumeration of usbnet mode values
 */
enum usbnet_mode_id {
	USBNET_MODE_UNKNOWN, /*<! Unknown value */
	USBNET_MODE_DEFAULT, /*<! Default mode, depends on specific modem */
	USBNET_MODE_RMNET, /*<! RmNet mode */
	USBNET_MODE_ECM, /*<! ECM mode */
	USBNET_MODE_MBIM, /*<! MBIM mode */
	USBNET_MODE_RNDIS, /*<! RNDIS mode */
	USBNET_MODE_NCM, /*<! NCM mode */

	__USBNET_MODE_MAX,
};

/**
 * Enumeration of nat mode values
 */
enum nat_mode_id {
	NAT_MODE_UNKNOWN, /*<! Unknown value */
	NAT_MODE_ROUTER, /*<! Router mode */
	NAT_MODE_BRIDGE, /*<! Bridge mode */

	__NAT_MODE_MAX,
};

/**
 * Enumeration of ue usage setting values
 */
enum ue_usage_id {
	UE_USAGE_UNKNOWN, /*<! Unknown value */
	UE_USAGE_VOICE_CENTRIC, /*<! voice centric mode */
	UE_USAGE_DATA_CENTRIC, /*<! data centric mode */

	__UE_USAGE_MAX,
};

/**
 * Enumeration of secure boot setting values
 */
enum secboot_mode_id {
	SEC_BOOT_UNKNOWN, /*<! Unknown value */
	SEC_BOOT_DISABLED, /*<! Secure boot disabled mode */
	SEC_BOOT_ENABLED, /*<! Secure boot enabled mode */

	__SEC_BOOT_MAX,
};

/**
 * Enumeration of PDP types
 */
enum pdp_type_id {
	PDP_T_IP, /*<! IPV4 type */
	PDP_T_PPP, /*<! PPP type */
	PDP_T_IPV6, /*<! IPV6 type */
	PDP_T_IPV4V6, /*<! IPV4V6 type */
	PDP_T_UNKNOWN, /*<! Unknown type */

	__PDP_T_MAX,
};

/**
 * Enumeration of PDP data compression types
 */
enum pdp_data_comp_id {
	PDP_DT_COMP_OFF, /*<! Disabled compression type */
	PDP_DT_COMP_PREF, /*<! Manuf. prefeared compression type */
	PDP_DT_COMP_V42, /*<! V.42 compression type */
	PDP_DT_COMP_V44, /*<! V.44 compression type */
	PDP_DT_COMP_UNKNOWN, /*<! Unknown compression type */

	__PDP_DT_COMP_MAX,
};

/**
 * Enumeration of PDP header compression types
 */
enum pdp_hdr_comp_id {
	PDP_HDR_COMP_OFF, /*<! Disabled compression type */
	PDP_HDR_COMP_PREF, /*<! Manuf. prefeared compression type */
	PDP_HDR_COMP_RFC1144, /*<! RFC1144 compression type */
	PDP_HDR_COMP_RFC2507, /*<! RFC2507 compression type */
	PDP_HDR_COMP_RFC3095, /*<! RFC3095 compression type */
	PDP_HDR_COMP_UNKNOWN, /*<! Unknown compression type */

	__PDP_HDR_COMP_MAX,
};

/**
 * Enumeration of PDP IPV4 address allocation type. Controls
 * how the MT/TA requests to get the IPv4 address information.
 */
enum pdp_ipv4_addr_alloc_id {
	PDP_IPV4_ADDR_ALLOC_NAS, /*<! allocation through NAS type */
	PDP_IPV4_ADDR_ALLOC_DHCP, /*<! allocation through DHCP type */
	PDP_IPV4_ADDR_ALLOC_UNKNOWN, /*<! Unknown allocation type */

	__PDP_IPV4_ADDR_ALLOC_MAX,
};

/**
 * Enumeration of PDP context activation request types
 */
enum pdp_req_id {
	PDP_REQ_GEN, /*<! For new PDP context establishment or for handover
from a non-3GPP access network */
	PDP_REQ_EMERG, /*<! For emergency (bearer) services */
	PDP_REQ_UNKNOWN, /*<! Unknown request type */

	__PDP_REQ_MAX,
};

/**
 * Enumeration of net registration operation types
 */
enum net_reg_op_t {
	NET_REG_OP_CREG, /*<! Simple net registration */
	NET_REG_OP_CGREG, /*<! GPRS net registration */
	NET_REG_OP_CEREG, /*<! EPS net registration */
	NET_REG_OP_C5GREG, /*<! 5G net registration */

	__NET_REG_OP_MAX,
};

/**
 * Enumeration of facility lock state types
 */
enum fac_lock_state_t {
	FAC_LOCK_ST_UNKNOWN, /*<! Unknown facility lock command*/
	FAC_LOCK_ST_UNLOCKED, /*<! Facility unlock command*/
	FAC_LOCK_ST_LOCKED, /*<! Facility lock command*/

	__FAC_LOCK_ST_MAX,
};

/**
 * Enumeration of facility types
 */
enum facility_t {
	FACILITY_UNKNOWN, /*<! Unknown facility */
	FACILITY_SC, /*<! SIM facility */
	FACILITY_AO, /*<! Disable all outgoing calls */
	FACILITY_OI, /*<! Disable all international outgoing calls */
	FACILITY_OX, /*<! Disable all international outgoing calls, except for the home country */
	FACILITY_AI, /*<! Disable all incoming calls */
	FACILITY_IR, /*<! Except for home country, during international roaming, Disable all
incoming calls */
	FACILITY_AC, /*<! Disable all incoming services, only valid in case of mode=0 */
	FACILITY_AG, /*<! Disable all outgoing services, only valid in case of mode=0 */
	FACILITY_AB, /*<! Disable all services, only valid in case of mode=0*/
	FACILITY_FD, /*<! SIM card fixed dialing characteristics facility*/
	FACILITY_PF, /*<! Lock the first SIM inserted in the mobile phone*/
	FACILITY_PN, /*<! Network personalization facility */
	FACILITY_PP, /*<! Service supplier personalization facility*/
	FACILITY_PU, /*<! Network sub-set personalization facility*/
	FACILITY_PC, /*<!  Company personalization facility*/
	FACILITY_PS, /*<! PH - SIM (lock SIM in phone)(when other SIM card is inserted in, ME will
prompt to input the password; set ME to make it recognize several used
SIM cards. In such way, after inserting these cards, ME will not prompt to
input the password */

	__FACILITY_MAX,
};

/**
 * Enumeration of IMS state types
 */
enum ims_state_t {
	IMS_ST_UNKNOWN, /*<! Unknown IMS state value*/
	IMS_ST_MBN_DEP, /*<! The NV about IMS can be set by the configuration of the MBN file */
	IMS_ST_ENABLED, /*<! Enabled IMS state value*/
	IMS_ST_DISABLED, /*<! Disabled IMS state value*/

	__IMS_ST_MAX,
};

/**
 * Enumeration of VoLTE state types
 */
enum volte_state_t {
	VOLTE_ST_UNKNOWN, /*<! Unknown VoLTE state value*/
	VOLTE_ST_ENABLED, /*<! Enabled VoLTE state value*/
	VOLTE_ST_DISABLED, /*<! Disabled VoLTE state value*/

	__VOLTE_ST_MAX,
};

/**
 * Enumeration of PDP types
 */
enum apn_auth_t {
	APN_AUTH_UNKNOWN, /*<! Unknown APN authentification type*/
	APN_AUTH_NONE, /*<! None APN authentification type*/
	APN_AUTH_PAP, /*<! PAP APN authentification type*/
	APN_AUTH_CHAP, /*<! CHAP APN authentification type*/
	APN_AUTH_PAP_CHAP, /*<! PAP or CHAP APN authentification type*/

	__APN_AUTH_MAX,
};

/**
 * Enumeration of module activity status values
 */
enum mod_act_stat_id {
	MOD_ACT_STAT_UNKNOWN, /*<! Unknown module activity*/
	MOD_ACT_STAT_UNAVAILABLE, /*<! Module unavaliable*/
	MOD_ACT_STAT_READY, /*<! Module ready*/
	MOD_ACT_STAT_RINGING, /*<! Ringing*/
	MOD_ACT_STAT_CALL, /*<! Call in progress or on hold*/

	__MOD_ACT_STAT_MAX,
};

/**
 * Enumeration of polarity values
 */
enum polarity_id {
	POLARITY_UNKNOWN, /*<! Unknown polarity value*/
	POLARITY_LOW, /*<! Low polarity value*/
	POLARITY_HIGH, /*<! High polarity value*/

	__POLARITY_MAX,
};

/**
 * Enumeration of PS attachment state types
 */
enum ps_att_state_t {
	PS_ATT_ST_UNKNOWN, /*<! Unknown PS attachment state>*/
	PS_ATT_ST_DETACHED, /*<! MT is detached*/
	PS_ATT_ST_ATTACHED, /*<! MT is attached*/

	PS_ATT_ST_MAX,
};

/**
 * Enumeration of call direction values
 */
enum call_direction_id {
	CALL_DIR_UNKNOWN, /*<! Unknown call direction value*/
	CALL_DIR_MO, /*<! Mobile originated call direction value*/
	CALL_DIR_MT, /*<! Mobile terminated call direction value*/

	__CALL_DIR_MAX,
};

/**
 * Enumeration of call state values
 */
enum call_state_id {
	CALL_STATE_UNKNOWN, /*<! Unknown call state value*/
	CALL_STATE_ACTIVE, /*<! Active call state value*/
	CALL_STATE_HELD, /*<! Held call state value*/
	CALL_STATE_DIALING, /*<! Dialing call state value*/
	CALL_STATE_ALERTING, /*<! Alerting call state value*/
	CALL_STATE_INCOMING, /*<! Incoming call state value*/
	CALL_STATE_WAITING, /*<! Waiting call state value*/

	__CALL_STATE_MAX,
};

/**
 * Enumeration of call mode values
 */
enum call_mode_id {
	CALL_MODE_UNKNOWN, /*<! Unknown call mode value*/
	CALL_MODE_VOICE, /*<! Voice call mode value*/
	CALL_MODE_DATA, /*<! Data call mode value*/
	CALL_MODE_FAX, /*<! FAX call mode value*/

	__CALL_MODE_MAX,
};

/**
 * Enumeration of call type values
 */
enum call_type_id {
	CALL_TYPE_UNKNOWN, /*<! Unknown call mode value (legit value)*/
	CALL_TYPE_INTERNATIONAL, /*<! International call type value*/
	CALL_TYPE_NATIONAL, /*<! National call type value*/

	__CALL_TYPE_MAX,
};

/**
 * Enumeration of ring type values
 */
enum ring_type_id {
	RING_TYPE_UNKNOWN, /*<! Unknown ring type value*/
	RING_TYPE_ASYNC, /*<! Asynchronous transparent*/
	RING_TYPE_SYNC, /*<! Synchronous transparent*/
	RING_TYPE_RELASYNC, /*<! Asynchronous non-transparent*/
	RING_TYPE_RELSYNC, /*<! Synchronous non-transparent*/
	RING_TYPE_FAX, /*<! Facsimile*/
	RING_TYPE_VOICE, /*<! Voice*/

	__RING_TYPE_MAX,
};

/**
 * Enumeration of CLI validity type values
 */
enum cli_valid_id {
	CLI_VALID_UNKNOWN, /*<! Unknown CLI validity type */
	CLI_VALID_OK, /*<! Valid cli*/
	CLI_VALID_CALLER_DISABLED, /*<! CLI disabled by caller */
	CLI_VALID_UNAVAILABLE, /*<! CLI unavailable */

	__CLI_VALID_MAX,
};

/**
 * Enumeration of network category types
 */
enum net_cat_t {
	NET_CAT_UNKNOWN, /*<! Unknown network category*/
	NET_CAT_M1, /*<! LTE Cat M1 */
	NET_CAT_NB1, /*<! LTE Cat NB1*/
	NET_CAT_M1_AND_NB1, /*<! LTE Cat M1 and Cat NB1*/

	__NET_CAT_MAX,
};

/**
 * Enumeration of SIM status values
 */
enum sim_stat_t {
	SIM_STAT_UNKNOWN, /*<! Unknown SIM status value*/
	SIM_STAT_INVALID, /*<! SIM card status is invalid */
	SIM_STAT_VALID, /*<! SIM card status is valid*/
	SIM_STAT_NOT_EXISTS, /*<! SIM card does not exist*/

	__SIM_STAT_MAX,
};

/**
 * Enumeration of FOTA state values
 */
enum fota_state_t {
	FOTA_ST_UNKNOWN, /*<! Unknown FOTA state value*/
	FOTA_ST_FTPSTART, /*<! Start downloading the package from FTP server*/
	FOTA_ST_HTTPSTART, /*<! Start downloading the package from HTTP(S) server*/
	FOTA_ST_FILESTART, /*<! Start downloading the package from file*/
	FOTA_ST_DOWNLOADING, /*<! Downloading the package from HTTP(S) server*/
	FOTA_ST_FTPEND, /*<! Finish downloading the package from FTP server*/
	FOTA_ST_HTTPEND, /*<! Finish downloading the package from HTTP(S) server*/
	FOTA_ST_FILEEND, /*<! Finish downloading the package from file*/
	FOTA_ST_START, /*<! Start upgrading the firmware via FOTA*/
	FOTA_ST_UPDATING, /*<! Upgrading the firmware via FOTA*/
	FOTA_ST_END, /*<! Finish upgrading the firmware via FOTA*/

	__FOTA_ST_MAX,
};

/**
 * Enumeration of PDP context states
 */
enum pdp_ctx_state_t {
	PDP_CTX_STATE_UNKNOWN, /*<! Unknown PDP context state value */
	PDP_CTX_STATE_ACTIVATED, /*<! Activated PDP context state */
	PDP_CTX_STATE_DEACTIVATED, /*<! Deactivated PDP context state */

	__PDP_CTX_STATE_MAX,
};

/**
 * Enumeration of PDP call modes
 */
enum pdp_call_mode_id {
	PDP_CALL_MODE_UNKNOWN, /*<! Unknown mode */
	PDP_CALL_MODE_DISCONNECT, /*<! Disconnect a call */
	PDP_CALL_MODE_ONCE, /*<! Make a call once */
	PDP_CALL_MODE_AUTO, /*<! Make a call and automatically remake after disconnection */
	PDP_CALL_MODE_AUTO_STARTUP, /*<! Make a call and automatically remake after disconnection,
					 and automatically call after startup */
	__PDP_CALL_MODE_MAX,
};

/**
 * Enumeration of PDP call state
 */
enum pdp_call_state_id {
	PDP_CALL_STATE_UNKNOWN, /*<! Unknown call state */
	PDP_CALL_STATE_FAILED, /*<! Call failed */
	PDP_CALL_STATE_SUCCESFUL, /*<! Call successful */

	__PDP_CALL_STATE_MAX,
};

/**
 * Enumeration of time mode values
 */
enum time_mode_id {
	TIME_MODE_UNKNOWN, /*<! Unknown time mode value*/
	TIME_MODE_LATEST, /*<! Latest time that has been synced*/
	TIME_MODE_GMT, /*<! Current GMT time calulated from latest time*/
	TIME_MODE_LOCAL, /*<! Current LOCAL time calclated from latest time*/

	__TIME_MODE_MAX,
};

/**
 * Enumeration of primary cell state values
 */
enum pcell_state_t {
	PCELL_ST_NO_SERVING, /*<! No serving */
	PCELL_ST_REGISTERED, /*<! Registered */
	PCELL_ST_UNKNOWN, /*<! Unknown primary cell state>*/

	__PCELL_ST_MAX,
};

/**
 * Enumeration of secondary cell state values
 */
enum scell_state_t {
	SCELL_ST_DECONFIGURED, /*<! Deconfigured */
	SCELL_ST_DEACTIVATED, /*<! Configuration deactivated */
	SCELL_ST_ACTIVATED, /*<! Configuration activated */
	SCELL_ST_UNKNOWN, /*<! Unknown secondary cell state>*/

	__SCELL_ST_MAX,
};

/**
 * Enumeration of bandwidth values
 */
enum bandwidth_id {
	BANDWIDTH_UNKNOWN,
	BANDWIDTH_1_4,
	BANDWIDTH_3,
	BANDWIDTH_5,
	BANDWIDTH_10,
	BANDWIDTH_15,
	BANDWIDTH_20,

	__BANDWIDTH_MAX,
};

/**
 * Enumeration of bandwidth values for 5G connection, used in cell_info
 */
enum bandwidth_5G_id {
	BANDWIDTH_5G_5,
	BANDWIDTH_5G_10,
	BANDWIDTH_5G_15,
	BANDWIDTH_5G_20,
	BANDWIDTH_5G_25,
	BANDWIDTH_5G_30,
	BANDWIDTH_5G_40,
	BANDWIDTH_5G_50,
	BANDWIDTH_5G_60,
	BANDWIDTH_5G_70,
	BANDWIDTH_5G_80,
	BANDWIDTH_5G_90,
	BANDWIDTH_5G_100,
	BANDWIDTH_5G_200,
	BANDWIDTH_5G_400,

	__BANDWIDTH_5G_MAX,
};

/**
 * Enumeration of storage types
 */
enum storage_type_id {
	STORAGE_UNKNOWN, /*<! Unknown storage type */
	STORAGE_UFS, /*<! UFS storage type */
	STORAGE_RAM, /*<! RAM storage type */
	STORAGE_SD, /*<! SD storage type */

	__STORAGE_MAX,
};

/**
 * Enumeration of NMEA sentence type
 */
enum nmea_type_id {
	NMEA_UNKNOWN, /*<! Unknown NMEA type */
	NMEA_DISABLE, /*<! Disabled NMEA sentences */
	NMEA_GSV, /*<! GSV NMEA type */
	NMEA_GSA, /*<! GSA NMEA type */
	NMEA_GNS, /*<! GNS NMEA type */
	NMEA_GGA, /*<! GGA NMEA type */
	NMEA_RMC, /*<! RMC NMEA type */
	NMEA_VTG, /*<! VTG NMEA type */
	NMEA_GLL, /*<! GLL NMEA type */

	__NMEA_MAX,
};

/**
 * Enumeration of NMEA Satellite Constellation type
 */
enum nmea_sata_type_id {
	NMEA_SATA_UNKNOWN, /*< Unknown NMEA satellite type */
	NMEA_SATA_GPS, /*<! GPS satellite type */
	NMEA_SATA_GALILEO, /*<! GALILEO satellite type */
	NMEA_SATA_GLONASS, /*<! GLONASS satellite type */
	NMEA_SATA_BEIDOU, /*<! BEIDOU satellite type */
	NMEA_SATA_GN, /*<! COMBINED satellite type */
	NMEA_SATA_QZSS, /*<! QZSS satellite type */

	__SATA_MAX,
};

/**
 * Enumeration of modem states
 */
enum modem_state_id {
	MODEM_STATE_UNKNOWN, /*< Unknown modem state */
	MODEM_STATE_IDLE, /*< Idle state */
	MODEM_STATE_SMS_SEND, /*< SMS sending state */
	MODEM_STATE_OPT_SELECT, /*< Operator selecting state */
	MODEM_STATE_OPT_SCAN, /*< Operator scanning state */
	MODEM_STATE_CMD_EXEC, /*< Regular command execution state */
	MODEM_STATE_FOTA, /*< Modem in FOTA mode state */

	__MODME_STATE_MAX,
};

/**
 * Enumeration of GNSS mode values
 */
enum gnss_mode_id {
	GNSS_MODE_UNKNOWN, /*< Unknown GNSS mode */
	GNSS_MODE_STAND_ALONE, /*< Stand-alone GNSS mode */
	GNSS_MODE_MS_BASED, /*< MS-based GNSS mode */
	GNSS_MODE_MS_ASSISTED, /*< MS-assisted GNSS mode */
	GNSS_MODE_SPEED_OPTIMAL, /*< Speed-optimal GNSS mode */

	__GNSS_MODE_MAX,
};

/**
 * Enumeration of GNSS accuracy values
 */
enum gnss_accuracy_id {
	GNSS_ACCURACY_UNKNOWN, /*< Unknown GNSS accuracy level */
	GNSS_ACCURACY_LOW, /*< Low GNSS accuracy level */
	GNSS_ACCURACY_MEDIUM, /*< Medium GNSS accuracy level*/
	GNSS_ACCURACY_HIGH, /*< High GNSS accuracy level*/

	__GNSS_ACCURACY_MAX,
};

/**
 * Enumeration of GNSS operation modes
 */
enum gnss_operation_mode_id {
	GNSS_OPER_UNKNOWN, /*<! Unknown GNSS operation mode */
	GNSS_OPER_GPS_SBAS_QZSS, /*<! GPS+SBAS+QZSS operation mode */
	GNSS_OPER_BDS, /*<! BDS operation mode */
	GNSS_OPER_GPS_GL_GA_SBAS_QZSS, /*<! GPS+GLONASS+GALILEO+SBAS+QZSS operation mode */
	GNSS_OPER_GPS_BDS_GL_SBAS_QZSS, /*<! GPS+BDS+GALILEO+SBAS+QZSS operation mode */
	GNSS_OPER_GPS_GL, /*<! GPS+GLONASS operation mode */
	GNSS_OPER_GPS_BDS, /*<! GPS+BeiDou operation mode */
	GNSS_OPER_GPS_GA, /*<! GPS+GALILEO operation mode */
	GNSS_OPER_GPS_QZSS, /*<! GPS+QZSS operation mode */
	GNSS_OPER_CAMPED, /*<! Selected on MCC operation mode */

	__GNSS_OPER_MAX,
};

/**
 * Enumeration of NMEA ouptut ports
 */
enum nmea_output_port_id {
	NMEA_PORT_UNKNOWN, /*< Unknown NMEA port */
	NMEA_PORT_NONE, /*< Disabled NMEA output */
	NMEA_PORT_USB_NMEA, /*< USB NMEA output port */
	NMEA_PORT_UART, /*< UART output port */

	__NMEA_PORT_MAX,
};

/**
 * Enumeration of URC indication types
 */
enum urc_ind_type_id {
	URC_IND_UNKNOWN, /*< Unknown URC type */
	URC_IND_ALL, /*<! Main switch for all URCs */
	URC_IND_CSQ, /*<! RSSI and BER change URC */
	URC_IND_SMS_FULL, /*<! SMS storage full URC */
	URC_IND_RING, /*<! RING URC */
	URC_IND_SMS_INCOMING, /*<! Incoming message URC */
	URC_IND_ACT, /*<! Network access technology change URC */
	URC_IND_CCINFO, /*<! Voice call state change URC */

	__URC_IND_MAX,
};

/**
 * Enumeration of Voice domain preference
 */
enum urc_voice_domain_pref {
	URC_VOICE_DOMAIN_CS, /* <! CS Voice only */
	URC_VOICE_DOMAIN_ISM_PS, /* <! IMS PS Voice only */
	URC_VOICE_DOMAIN_CS_IMS_PS, /* <! CS voice preferred, IMS PS Voice as secondary */
	URC_VOICE_DOMAIN_IMS_PS_CS, /* <! IMS PS voice preferred, CS Voice as secondary. */

	__URC_VOICE_DOMAIN_MAX,
};

/**
 * Enumeration of LTE SMS format
 */
enum urc_lte_sms_format {
	URC_UNKNOWN_FORMAT, /* <! unknown FORMAT */
	URC_CDMA_FORMAT, /* <! CDMA FORMAT */
	URC_GSM_FORMAT, /* <! GSM FORMAT */
	URC_LTE_FORMAT, /* <! LTE FORMAT */

	__URC_LTESMS_FORMAT_MAX,
};

/**
 * Enumeration of Modem functionality
 */
enum modem_func_t {
	MODEM_FULL_MODE, /* <! Standard modem functionality */
	MODEM_DATA_ONLY, /* <! Modem without call functionality */
	MODEM_DYNAMIC_MODE, /* <! Modem has dynamic mode which depends by network */
	MODEM_LOW_POWER, /* <! Modem without operator scan functionality */
	MODEM_AUTO_VOLTE_MODE, /* <! Modem without VoLTE management (auto on)*/

	__MODEM_MODE_MAX,
};

/**
 * Enumeration of CEFS restore state
 */
enum cefs_restore_state_id {
	CEFS_RESTORE_NORMAL, /* <! CEFS restore normal range */
	CEFS_RESTORE_MINIMAL, /* <! CEFS restore minimal range */
	CEFS_RESTORE_MAXIMUM, /* <! CEFS restore maximum range */

	__CEFS_RESTORE_MAX,
};

/**
 * Enumeration of DPO modes
 */
enum dpo_mode_id {
	DPO_MODE_UNKNOWN, /*<! Unknown DPO mode */
	DPO_MODE_DISABLE, /*<! Disable DPO */
	DPO_MODE_ENABLE_DDC, /*<! Enable DPO with dynamic duty cycle */
	DPO_MODE_ENABLE_NO_EPS, /*<! Enable DPO only when the module is not connected to an external power supply */

	__DPO_MODE_MAX,
};

/**
 * Enumeration of NR5G disable mode
 */
enum disable_nr5g_mode_id {
	DISABLE_NR5G_MODE_NONE, /*<! None 5G network is disabled */
	DISABLE_NR5G_MODE_SA, /*<! Disabled SA */
	DISABLE_NR5G_MODE_NSA, /*<! Disabled NSA */
	DISABLE_NR5G_MODE_UNKNOWN, /*<! Unknown */

	__DISABLE_NR5G_MODE_MAX,
};

/**
 * Enumeration of EMM causes
 */
enum emm_cause_id {
	EMM_CAUSE_IMSI_UNKKNOWN_IN_HSS,
	EMM_CAUSE_ILLEGAL_UE,
	EMM_CAUSE_ILLEGAL_ME,
	EMM_CAUSE_EPS_SERVICES_NOT_ALLOWED,
	EMM_CAUSE_SERVICES_NOT_ALLOWED,
	EMM_CAUSE_UE_ID_CANT_BE_DERRIVED,
	EMM_CAUSE_IMPLICITLY_DETACHED,
	EMM_CAUSE_PLMN_NOT_ALLOWED,
	EMM_CAUSE_TRACKING_AREA_NOT_ALLOWED,
	EMM_CAUSE_ROAMING_NOT_ALLOWED,
	EMM_CAUSE_EPS_NOT_ALLOWED_IN_THIS_PLMN,
	EMM_CAUSE_NO_SUITABLE_CELLS,
	EMM_CAUSE_MSC_NOT_REACHABLE,
	EMM_CAUSE_NETWORK_FAILURE,
	EMM_CAUSE_CS_DOMAIN_NOT_AVAILABLE,
	EMM_CAUSE_ESM_FAIL,
	EMM_CAUSE_MAC_FAIL,
	EMM_CAUSE_SYNCH_FAIL,
	EMM_CAUSE_CONGESTION,
	EMM_CAUSE_UE_SECURITY_MISMATCH,
	EMM_CAUSE_SECURITY_MODE_REJECTED,
	EMM_CAUSE_NOT_AUTHORIZED_FOR_THIS_CGS,
	EMM_CAUSE_NON_EPS_AUTH_UNACCEPTABLE,
	EMM_CAUSE_5GCN_REDIRECT_REQUIRED,
	EMM_CAUSE_SERVICE_OPT_NOT_AUTHORIZED,
	EMM_CAUSE_CS_SERVICE_UNAVAILABLE,
	EMM_CAUSE_NO_ACTIVE_EPS_BEARER,
	EMM_CAUSE_SEVERE_NETWORK_FAIL,

	__EMM_CAUSE_MAX,
};

/**
 * Enumeration of ESM causes
 */
enum esm_cause_id {
	ESM_CAUSE_OPER_DETER_BARR,
	ESM_CAUSE_INSUFF_RESOURCES,
	ESM_CAUSE_MISSING_APN,
	ESM_CAUSE_UNKNOWN_PDN_TYPE,
	ESM_CAUSE_USR_AUTH_FAIL,
	ESM_CAUSE_REQ_REJECTED_BY_GW,
	ESM_CAUSE_REQ_REJECTED,
	ESM_CAUSE_SERVICE_OPT_UNSUPPORTED,
	ESM_CAUSE_SERVICE_OPT_NOT_SUBSCRIBED,
	ESM_CAUSE_SERVICE_OPT_OUT_OF_ORDER,
	ESM_CAUSE_PTI_ALREADY_IN_USE,
	ESM_CAUSE_REGULAR_REACTIVATION,
	ESM_CAUSE_EPS_QOS_NOT_ACCEPTED,
	ESM_CAUSE_NETWORK_FAIL,
	ESM_CAUSE_REACTIVATION_REQUESTED,
	ESM_CAUSE_SEMANTIC_ERR_IN_TFT,
	ESM_CAUSE_SYNTACT_ERR_IN_TFT,
	ESM_CAUSE_INVALID_EPS_BEARER_ID,
	ESM_CAUSE_SEMANTIC_ERR_IN_PCKT_FILTER,
	ESM_CAUSE_SYNTACT_ERR_IN_PCKT_FILTER,
	ESM_CAUSE_PTI_MISMATCH,
	ESM_CAUSE_LAST_PDN_DISCONN_NOT_ALLOWED,
	ESM_CAUSE_PDN_IPV4_ONLY_ALLOWED,
	ESM_CAUSE_PDN_IPV6_ONLY_ALLOWED,
	ESM_CAUSE_SINGLE_ADDR_BARR_ONLY_ALLOWED,
	ESM_CAUSE_ESM_INFO_NOT_RECEIVED,
	ESM_CAUSE_PDN_CONN_NOT_EXISTS,
	ESM_CAUSE_MULTI_PDN_NOT_ALLOWED,
	ESM_CAUSE_REQUEST_COLLISION,
	ESM_CAUSE_PDN_IPV4V6_ONLY_ALLOWED,
	ESM_CAUSE_PDN_NON_IP_ONLY_ALLOWED,
	ESM_CAUSE_UNSUPPORTED_QCI_VAL,
	ESM_CAUSE_BEARER_NOT_SUPPORTED,
	ESM_CAUSE_PDN_TYPE_ETH_ONLY_ALLOWED,
	ESM_CAUSE_MAX_EPS_BEARER_COUNT_REACHED,
	ESM_CAUSE_REQ_APN_NOT_SUPPORTED,
	ESM_CAUSE_INVALID_PTI,
	ESM_CAUSE_SEMANTICALLY_INCORRECT,
	ESM_CAUSE_INVALID_MAND_INFO,
	ESM_CAUSE_MSG_TYPE_INVALID,
	ESM_CAUSE_MSG_TYPE_NOT_COMP,
	ESM_CAUSE_INFO_EL_NON_EXISTANT,
	ESM_CAUSE_COND_IE_ERR,
	ESM_CAUSE_MSG_NOT_COMP,
	ESM_CAUSE_PROTOCOL_ERR,
	ESM_CAUSE_APN_RESTR_VAL_INCOMP,
	ESM_CAUSE_MULTI_ACC_TO_PDN_NOT_ALLOWED,

	__ESM_CAUSE_MAX,
};

/**
 * Convert modem action status value to string.
 * @param[in]	status	Action status value.
 * @return const char *. String of readable status value.
 */
const char *act_status_str(func_t status);

/**
 * Convert event type to name.
 * @param[in]	evt	Event type value.
 * @return const char *. String of readable event name.
 */
const char *evt_name_str(evt_type_t evt);

/**
 * Convert pin state value to string.
 * @param[in]	state	Pin state enumeration value.
 * @return const char *. String of readable pin state value.
 */
const char *pin_state_str(enum pin_state_id state);

/**
 * Convert network mode state value to string.
 * @param[in]	mode	Network mode enumeration value.
 * @return const char *. String of readable network mode state.
 */
const char *net_mode_str(enum net_mode_id mode);

/**
 * Convert network mode state value to passing value string.
 * @param[in]	mode	Network mode enumeration value.
 * @return const char *. String of network mode state.
 */
const char *net_mode_arg_str(enum net_mode_id id);

/**
 * Convert network mode argument string to enumeration value.
 * @param[in]	*arg	network mode string.
 * @return enum net_mode_id. Enumeration value of network mode.
 */
enum net_mode_id net_mode_arg_enum(const char *arg);

/**
 * Convert SMS message format type value to string.
 * @param[in]	format	SMS message type format enumeration value.
 * @return const char *. String of readable SMS message type format.
 */
const char *sms_mode_str(enum sms_format_id format);

/**
 * Convert SMS message format type value to string.
 * @param[in]	format	SMS message type format enumeration value.
 * @return const char *. String of SMS message type format.
 */
const char *sms_mode_arg_str(enum sms_format_id format);

/**
 * Convert SMS message type format argument string to enumeration value.
 * @param[in]	*arg	SMS message type format string.
 * @return enum sms_format_id. Enumeration value of SMS message type format.
 */
enum sms_format_id sms_mode_arg_enum(const char *arg);

/**
 * Convert signal query config value to string.
 * @param[in]	id	Signal query config enumeration value.
 * @return const char *. String of readable Signal query config value.
 */
const char *signal_query_cfg_str(enum signal_query_cfg_id id);

/**
 * Convert signal query config value to string.
 * @param[in]	id	Signal query config enumeration value.
 * @return const char *. String of Signal query config value.
 */
const char *signal_query_cfg_arg_str(enum signal_query_cfg_id id);

/**
 * Convert signal query config value string argument to enumeration value.
 * @param[in]	*arg	Signal query config value string argument.
 * @return enum signal_query_cfg_id. Enumeration value of signal query
 *  config value.
 */
enum signal_query_cfg_id signal_query_cfg_arg_enum(const char *arg);

/**
 * Convert phone functionality enumeration value to string.
 * @param[in]	func	Phone functionality enumeration value.
 * @return const char *. String of readable phone functionality value.
 */
const char *phone_func_str(enum phone_func_id func);

/**
 * Convert phone functionality enumeration value to string.
 * @param[in]	func	Phone functionality enumeration value.
 * @return const char *. String of phone functionality value.
 */
const char *phone_func_arg_str(enum phone_func_id func);

/**
 * Convert phone functionality value string argument to enumeration value.
 * @param[in]	*arg	Phone functionality string argument value.
 * @return enum phone_func_id. Enumeration value of phone functionality
 *  string argument value.
 */
enum phone_func_id phone_func_arg_enum(const char *arg);

/**
 * Convert GPRS attach mode enumeration value to string.
 * @param[in]	mode	GPRS attach mode enumeration value.
 * @return const char *. String of readable GPRS attach mode value.
 */
const char *gprs_attach_str(enum gprs_attach_mode_id mode);

/**
 * Convert GPRS attach mode enumeration value to string.
 * @param[in]	mode	GPRS attach mode enumeration value.
 * @return const char *. String of GPRS attach mode value.
 */
const char *gprs_attach_arg_str(enum gprs_attach_mode_id mode);

/**
 * Convert GPRS attach mode value string argument to enumeration value.
 * @param[in]	*arg	GPRS attach mode string argument value.
 * @return enum gprs_attach_mode_id. Enumeration value of GPRS attach mode
 *  string argument value.
 */
enum gprs_attach_mode_id gprs_attach_arg_enum(const char *arg);

/**
 * Convert Error message format enumeration value to string.
 * @param[in]	fmt	Error message format enumeration value.
 * @return const char *. String of readable Error message format value.
 */
const char *err_msg_fmt_str(enum err_msg_fmt_id fmt);

/**
 * Convert M2M state value string argument to enumeration value.
 * @param[in]	*arg	M2M status string argument value.
 * @return enum m2m_state_id. Enumeration value of M2M service string
 *  argument value.
 */
enum m2m_state_id m2m_arg_enum(const char *arg);

/**
 * Convert M2M state enumeration value to string.
 * @param[in]	id	M2M enumeration value.
 * @return const char *. String of readable M2M state value.
 */
const char *m2m_state_arg_str(enum m2m_state_id id);

/**
 * Convert Error message format enumeration value to string.
 * @param[in]	fmt	Error message format enumeration value.
 * @return const char *. String of Error message format value.
 */
const char *err_msg_fmt_arg_str(enum err_msg_fmt_id fmt);

/**
 * Convert Error message format value string argument to enumeration value.
 * @param[in]	*arg	Error message format string argument value.
 * @return enum err_msg_fmt_id. Enumeration value of Error message format string
 *  argument value.
 */
enum err_msg_fmt_id err_msg_fmt_arg_enum(const char *arg);

/**
 * Convert Network registration mode enumeration value to string.
 * @param[in]	fmt	Network registration mode enumeration value.
 * @return const char *. String of readable Network registration mode value.
 */
const char *net_reg_mode_str(enum net_reg_mode_id mode);

/**
 * Convert Network registration mode enumeration value to string.
 * @param[in]	fmt	Network registration mode enumeration value.
 * @return const char *. String of Network registration mode value.
 */
const char *net_reg_mode_arg_str(enum net_reg_mode_id mode);

/**
 * Convert Network registration mode value string argument to enumeration value.
 * @param[in]	*arg	Network registration mode string argument value.
 * @return enum net_reg_mode_id. Enumeration value of Network registration mode
 *  string argument value.
 */
enum net_reg_mode_id net_reg_mode_arg_enum(const char *arg);

/**
 * Convert Network registration status enumeration value to string.
 * @param[in]	stat	Network registration status enumeration value.
 * @return const char *. String of Network registration status value.
 */
const char *net_reg_stat_str(enum net_reg_stat_id stat);

/**
 * Convert Network registration access technology enumeration value to string.
 * @param[in]	act	Network registration access technology enumeration
 *  value.
 * @return const char *. String of Network registration access technology value.
 */
const char *net_reg_act_str(enum net_reg_act_id act);

/**
 * Convert MBN auto selection enumeration value to string.
 * @param[in]	mode	MBN auto selection enumeration value.
 * @return const char *. String of readable MBN auto selection value.
 */
const char *mbn_auto_sel_str(enum mbn_auto_sel_id mode);

/**
 * Convert MBN auto selection enumeration value to string.
 * @param[in]	mode	MBN auto selection enumeration value.
 * @return const char *. String of MBN auto selection value.
 */
const char *mbn_auto_sel_arg_str(enum mbn_auto_sel_id mode);

/**
 * Convert MBN auto selection value string argument to enumeration value.
 * @param[in]	*arg	MBN auto selection string argument value.
 * @return enum mbn_auto_sel_id. Enumeration value of MBN auto selection
 *  string argument value.
 */
enum mbn_auto_sel_id mbn_auto_sel_arg_enum(const char *arg);

/**
 * Convert Roaming service enumeration value to string.
 * @param[in]	id	Roaming service enumeration value.
 * @return const char *. String of Roaming service value.
 */
const char *roaming_svc_str(enum roaming_svc_id id);

/**
 * Convert Roaming service enumeration value to string.
 * @param[in]	id	Roaming service enumeration value.
 * @return const char *. String of readable Roaming service value.
 */
const char *roaming_svc_arg_str(enum roaming_svc_id id);

/**
 * Convert Roaming service value string argument to enumeration value.
 * @param[in]	*arg	Roaming service string argument value.
 * @return enum roaming_svc_id. Enumeration value of Roaming service string
 *  argument value.
 */
enum roaming_svc_id roaming_svc_arg_enum(const char *arg);

/**
 * Convert Operator selection mode enumeration value to string.
 * @param[in]	id	Operator selection mode enumeration value.
 * @return const char *. String of readable Operator selection mode value.
 */
const char *op_slc_mode_str(enum op_slc_mode_id id);

/**
 * Convert facility lock state enumeration value to string.
 * @param[in]	*arg 	Facility lock state string.
 * @return enum fac_lock_state_t. Enumeration value of facility lock state.
 */
enum fac_lock_state_t fac_lock_state_id(const char *arg);

/**
 * Convert facility lock state string to  enumeration value.
 * @param[in]	id 		Facility lock state enumeration value.
 * @return const char *. String of readable facility lock state value.
 */
const char *fac_lock_state_str(enum fac_lock_state_t id);

/**
 * Convert facility enumeration value to string.
 * @param[in]	*arg 	Facility type string.
 * @return facility_id. Facility enumeration value.
 */
enum facility_t facility_id(const char *arg);

/**
 * Convert facility type string to enumeration value.
 * @param[in]	id 		Facility tupe enumeration value.
 * @return const char *. String of facility type value.
 */
const char *facility_str(enum facility_t id);

/**
 * Convert Operator selection mode enumeration value to string.
 * @param[in]	id	Operator selection mode enumeration value.
 * @return const char *. String of Operator selection mode value.
 */
const char *op_slc_mode_arg_str(enum op_slc_mode_id id);

/**
 * Convert Operator selection mode value string argument to enumeration value.
 * @param[in]	*arg	Operator selection mode string argument value.
 * @return enum op_slc_mode_id. Enumeration value of Operator selection mode
 *  string argument value.
 */
enum op_slc_mode_id op_slc_mode_arg_enum(const char *arg);

/**
 * Convert Operator selection format enumeration value to string.
 * @param[in]	id	Operator selection format enumeration value.
 * @return const char *. String of readable Operator selection format value.
 */
const char *op_slc_fmt_str(enum op_slc_fmt_id id);

/**
 * Convert Operator selection format enumeration value to string.
 * @param[in]	id	Operator selection format enumeration value.
 * @return const char *. String of Operator selection format value.
 */
const char *op_slc_fmt_arg_str(enum op_slc_fmt_id id);

/**
 * Convert Operator selection format value string argument to enumeration value.
 * @param[in]	*arg	Operator selection format string argument value.
 * @return enum op_slc_fmt_id. Enumeration value of Operator selection format
 *  string argument value.
 */
enum op_slc_fmt_id op_slc_fmt_arg_enum(const char *arg);

/**
 * Convert Operator selection state enumeration value to string.
 * @param[in]	id	Operator selection state enumeration value.
 * @return const char *. String of Operator selection state value.
 */
const char *op_slc_stat_str(enum op_slc_stat_id id);

/**
 * Convert Calling line id presentation mode enumeration value to string.
 * @param[in]	id	Calling line id presentation mode enumeration value.
 * @return const char *. String of readable Calling line id presentation mode value.
 */
const char *clip_mode_str(enum clip_mode_id id);

/**
 * Convert Calling line id presentation mode enumeration value to string.
 * @param[in]	id	Calling line id presentation mode enumeration value.
 * @return const char *. String of Calling line id presentation mode value.
 */
const char *clip_mode_arg_str(enum clip_mode_id id);

/**
 * Convert Calling line id presentation mode value string argument to enumeration
 *  value.
 * @param[in]	*arg	Calling line id presentation mode string argument value.
 * @return enum clip_mode_id. Enumeration value of Calling line id presentation
 *  mode string argument value.
 */
enum clip_mode_id clip_mode_arg_enum(const char *arg);

/**
 * Convert Calling line id presentation state enumeration value to string.
 * @param[in]	id	Calling line id presentation state enumeration value.
 * @return const char *. String of Calling line id presentation state value.
 */
const char *clip_stat_str(enum clip_stat_id id);

/**
 * Convert Cellular result code format enumeration value to string.
 * @param[in]	id	Cellular result code format enumeration value.
 * @return const char *. String of readable Cellular result code format value.
 */
const char *cell_rc_str(enum cell_rc_id id);

/**
 * Convert Cellular result code format enumeration value to string.
 * @param[in]	id	Cellular result code format enumeration value.
 * @return const char *. String of Cellular result code format value.
 */
const char *cell_rc_arg_str(enum cell_rc_id id);

/**
 * Convert Cellular result code format value string argument to enumeration
 *  value.
 * @param[in]	*arg	Cellular result code format string argument value.
 * @return enum cell_rc_id. Enumeration value of Cellular result code format
 *  string argument value.
 */
enum cell_rc_id cell_rc_arg_enum(const char *arg);

/**
 * Convert Message storage type enumeration value to string.
 * @param[in]	id	Message storage type enumeration value.
 * @return const char *. String of readable Message storage type value.
 */
const char *msg_storage_str(enum msg_storage_id id);

/**
 * Convert Message storage type enumeration value to string.
 * @param[in]	id	Message storage type enumeration value.
 * @return const char *. String of Message storage type value.
 */
const char *msg_storage_arg_str(enum msg_storage_id id);

/**
 * Convert Message storage type value string argument to enumeration
 *  value.
 * @param[in]	*arg	Message storage type string argument value.
 * @return enum msg_storage_id. Enumeration value of Message storage type
 *  string argument value.
 */
enum msg_storage_id msg_storage_arg_enum(const char *arg);

/**
 * Convert SMS state type id to string.
 * @param[in]	id	SMS state id enumeration value.
 * @return const char *. String of SMS state type value.
 */
const char *sms_state_str(enum sms_state_id id);

/**
 * Convert SMS state string to SMS state enum.
 * @param[in]	*arg	SMS state string argument value.
 * @return enum sms_state_id. Enumeration of SMS state id value.
 */
enum sms_state_id sms_state_enum(const char *arg);

/**
 * Convert capability set type id to string.
 * @param[in]	id	capability setid enumeration value.
 * @return const char *. String of capavility set type value.
 */
const char *cap_set_str(enum cap_set_id);

/**
 * Convert capability set string to capability set enum.
 * @param[in]	*arg	CApability set string argument value.
 * @return enum capability_id . Enumeration of capability set id value.
 */
enum cap_set_id cap_set_enum(const char *arg);

/**
 * Convert UE state type id to string.
 * @param[in]   id      UE state id enumeration value.
 * @return const char *. String of SMS state type value.
 */
const char *ue_state_str(enum ue_state_id id);

/**
 * Convert PIN state to sim state.
 * @param[in]   id      PIN state id enumeration value.
 * @return enum sim_state_id. Enumeration of sim state.
 */
enum sim_state_id sim_state_enum(enum pin_state_id id);

/**
 * Convert usbnet mode string to usbnet mode enum.
 * @param[in]	*arg	usbnet mode string argument value.
 * @return enum usbnet_mode_id. Enumeration of usbnet mode id value.
 */
enum usbnet_mode_id usbnet_mode_id_enum(const char *arg);

/**
 * Convert usbnet mode id to string.
 * @param[in]   id      usbnet mode id enumeration value.
 * @return const char *. String of usbnet node id value.
 */
const char *usbnet_mode_id_str(enum usbnet_mode_id id);

/**
 * Convert nat mode string to nat mode enum.
 * @param[in]	*arg	nat mode string argument value.
 * @return enum nat_mode_id. Enumeration of nat mode id value.
 */
enum nat_mode_id nat_mode_id_enum(const char *arg);

/**
 * Convert nat mode id to string.
 * @param[in]   id      nat mode id enumeration value.
 * @return const char *. String of nat node id value.
 */
const char *nat_mode_id_str(enum nat_mode_id id);

/**
 * Convert ue usage string to ue usage id enum.
 * @param[in]	*arg	ue usage id string argument value.
 * @return enum ue_usage_id. Enumeration of ue usage id value.
 */
enum ue_usage_id ue_usage_id_enum(const char *arg);

/**
 * Convert secure boot mode string to secure boot mode id enum.
 * @param[in]	*arg	secure boot mode id string argument value.
 * @return enum secboot_mode_id. Enumeration of secure boot mode id value.
 */
enum secboot_mode_id sec_boot_mode_id_enum(const char *arg);

/**
 * Convert secure boot mode id to string.
 * @param[in]	id	secure boot mode id enumeration value.
 * @return const char *. String of secure boot mode id value.
 */
const char *sec_boot_mode_id_str(enum secboot_mode_id id);

/**
 * Convert ue usage id to string.
 * @param[in]   id      ue usage id enumeration value.
 * @return const char *. String of ue usage id value.
 */
const char *ue_usage_id_str(enum ue_usage_id id);

/**
 * Convert PDP type string to PDP type enum.
 * @param[in]	*arg	PDP type id string argument value.
 * @return enum pdp_type_id_enum. Enumeration of PDP type id value.
 */
enum pdp_type_id pdp_type_id_enum(const char *arg);

/**
 * Convert PDP type id to string.
 * @param[in]   id      PDP type id enumeration value.
 * @return const char *. String of PDP type value.
 */
const char *pdp_type_id_str(enum pdp_type_id id);

/**
 * Convert PDP data compression string to PDP data compression id enum.
 * @param[in]	*arg	PDP data compression id string argument value.
 * @return enum pdp_data_comp_id. Enumeration of PDP data compression id value.
 */
enum pdp_data_comp_id pdp_data_comp_id_enum(const char *arg);

/**
 * Convert PDP data compression id to string.
 * @param[in]   id      PDP data compression id enumeration value.
 * @return const char *. String of PDP data compression id value.
 */
const char *pdp_data_comp_id_str(enum pdp_data_comp_id id);

/**
 * Convert PDP header compression string to PDP header compression id enum.
 * @param[in]	*arg	PDP header compression id string argument value.
 * @return enum pdp_hdr_comp_id. Enumeration of PDP header compression id value.
 */
enum pdp_hdr_comp_id pdp_hdr_comp_id_enum(const char *arg);

/**
 * Convert PDP header compression id to string.
 * @param[in]   id      PDP header compression id enumeration value.
 * @return const char *. String of PDP header compression id value.
 */
const char *pdp_hdr_comp_id_str(enum pdp_hdr_comp_id id);

/**
 * Convert PDP IPV4 address allocation string to PDP IPV4 address allocation id enum.
 * @param[in]	*arg	PDP IPV4 address allocation id string argument value.
 * @return enum pdp_ipv4_addr_alloc_id. Enumeration of PDP IPV4 address allocation id value.
 */
enum pdp_ipv4_addr_alloc_id pdp_ipv4_addr_alloc_id_enum(const char *arg);

/**
 * Convert PDP IPV4 address allocation id to string.
 * @param[in]   id      PDP IPV4 address allocation id enumeration value.
 * @return const char *. String of PDP IPV4 address allocation id value.
 */
const char *pdp_ipv4_addr_alloc_id_str(enum pdp_ipv4_addr_alloc_id id);

/**
 * Convert PDP request string to PDP request id enum.
 * @param[in]	*arg	PDP request id string argument value.
 * @return enum pdp_req_id. Enumeration of PDP request id value.
 */
enum pdp_req_id pdp_req_id_enum(const char *arg);

/**
 * Convert PDP request id to string.
 * @param[in]   id      PDP request id enumeration value.
 * @return const char *. String of PDP request id value.
 */
const char *pdp_req_id_str(enum pdp_req_id id);

/**
 * Convert IMS state string to IMS state enum.
 * @param[in]	*arg	IMS state string argument value.
 * @return enum ims_state_t. Enumeration of IMS state value.
 */
enum ims_state_t ims_state_enum(const char *arg);
//
/**
 * Convert IMS state enum to string.
 * @param[in]   id      EMS state enumeration value.
 * @return const char *. String of IMS state value.
 */
const char *ims_state_str(enum ims_state_t state);

/**
 * Convert VoLTE state string to VoLTE state enum.
 * @param[in]	*arg	VoLTE state string argument value.
 * @return enum volte_state_t. Enumeration of VoLTE state value.
 */
enum volte_state_t volte_state_enum(const char *arg);

/**
 * Convert VoLTE state enum to string.
 * @param[in]   id      EMS state enumeration value.
 * @return const char *. String of VoLTE state value.
 */
const char *volte_state_str(enum volte_state_t state);

/**
 * Convert APN auth type string to APN auth type enum.
 * @param[in]	*arg	APN auth type string argument value.
 * @return enum apn_auth_t. Enumeration of APN auth type value.
 */
enum apn_auth_t apn_auth_enum(const char *arg);

/**
 * Convert APN auth type enum to string.
 * @param[in]   id      APN auth type enumeration value.
 * @return const char *. String of APN auth type value.
 */
const char *apn_auth_str(enum apn_auth_t type);

/**
 * Convert module status enum to string.
 * @param[in]   id      Module status enumeration value.
 * @return const char *. String of module status value.
 */
const char *mod_act_stat_str(enum mod_act_stat_id id);

/**
 * Convert polarity value string to polarity value enum.
 * @param[in]	*arg	Polarity value string argument value.
 * @return enum polarity_id. Enumeration of polarity value.
 */
enum polarity_id polarity_id_enum(const char *arg);

/**
 * Convert polarity value enum to string.
 * @param[in]   value      Polarity value enumeration value.
 * @return const char *. String of polarity value.
 */
const char *polarity_id_str(enum polarity_id value);

/**
 * Convert PS attachment state string to PS attachment state enum.
 * @param[in]	*arg	PS attachment state string argument value.
 * @return enum ps_att_state_t. Enumeration of PS attachment state value.
 */
enum ps_att_state_t ps_att_state_enum(const char *arg);

/**
 * Convert PS attachment state enum to string.
 * @param[in]   state      PS attachment state enumeration value.
 * @return const char *. String of PS attachment state value.
 */
const char *ps_att_state_str(enum ps_att_state_t state);

/**
 * Convert call direction value string to call direction value enum.
 * @param[in]	*arg	Call direction value string argument value.
 * @return enum call_direction_id. Enumeration of call direction value.
 */
enum call_direction_id call_direction_id_enum(const char *arg);

/**
 * Convert call direction value enum to string.
 * @param[in]   value      Call direction value enumeration value.
 * @return const char *. String of call direction value.
 */
const char *call_direction_id_str(enum call_direction_id value);

/**
 * Convert call state value string to call state value enum.
 * @param[in]	*arg	Call state value string argument value.
 * @return enum call_state_id. Enumeration of call state value.
 */
enum call_state_id call_state_id_enum(const char *arg);

/**
 * Convert call state value enum to string.
 * @param[in]   value      Call state value enumeration value.
 * @return const char *. String of call state value.
 */
const char *call_state_id_str(enum call_state_id value);

/**
 * Convert call mode value string to call mode value enum.
 * @param[in]	*arg	Call mode value string argument value.
 * @return enum call_mode_id. Enumeration of call mode value.
 */
enum call_mode_id call_mode_id_enum(const char *arg);

/**
 * Convert call mode value enum to string.
 * @param[in]   value      Call mode value enumeration value.
 * @return const char *. String of call mode value.
 */
const char *call_mode_id_str(enum call_mode_id value);

/**
 * Convert call type value string to call type value enum.
 * @param[in]	*arg	Call type value string argument value.
 * @return enum call_type_id. Enumeration of call type value.
 */
enum call_type_id call_type_id_enum(const char *arg);

/**
 * Convert call type value enum to string.
 * @param[in]   value      Call type value enumeration value.
 * @return const char *. String of call type value.
 */
const char *call_type_id_str(enum call_type_id value);

/**
 * Convert ring type value string to ring type value enum.
 * @param[in]	*arg	Ring type value string argument value.
 * @return enum ring_type_id. Enumeration of call type value.
 */
enum ring_type_id ring_type_id_enum(const char *arg);

/**
 * Convert ring type value enum to string.
 * @param[in]   value      Ring type value enumeration value.
 * @return const char *. String of ring type value.
 */
const char *ring_type_id_str(enum ring_type_id value);

/**
 * Convert CLI validity value enum to string.
 * @param[in]   value      CLI validity value enumeration value.
 * @return const char *. String of CLI validity value.
 */
const char *cli_valid_id_str(enum cli_valid_id value);

/**
 * Convert network category string to network category enum.
 * @param[in]	*arg	network category string argument value.
 * @return enum net_cat_t. Enumeration of network category value.
 */
enum net_cat_t net_cat_enum(const char *arg);

/**
 * Convert network category enum to string.
 * @param[in]   category    Network category enumeration value.
 * @return const char *. String of network category value.
 */
const char *net_cat_str(enum net_cat_t category);

/**
 * Convert SIM status enum to string.
 * @param[in]   stat     SIM status enumeration value.
 * @return const char *. String of SIM status value.
 */
const char *sim_stat_str(enum sim_stat_t stat);

/**
 * Convert FOTA state string to FOTA state enum.
 * @param[in]	*arg	FOTA state string argument value.
 * @return enum fota_state_t. Enumeration of FOTA state value.
 */
enum fota_state_t fota_state_enum(const char *arg);

/**
 * Convert FOTA state enum to string.
 * @param[in]   state    FOTA state enumeration value.
 * @return const char *. String of FOTA state value.
 */
const char *fota_state_str(enum fota_state_t state);

/**
 * Convert PDP context status action string to PDP context
 * state enum.
 * @param[in]	*arg	PDP context status argument value.
 * @return enum pdp_ctx_state_t. Enumeration of PDP context
 * status value.
 */
enum pdp_ctx_state_t pdp_ctx_state_enum(const char *arg);

/**
 * Convert PDP context status enum to string
 * @param[in]   state    PDP context status enumeration value.
 * @return const char *. String of PDP context status.
 */
const char *pdp_ctx_state_str(enum pdp_ctx_state_t state);

/**
 * Convert PDP call mode string to PDP call mode enum.
 * @param[in]	*arg	PDP call mode string argument value.
 * @return enum pdp_call_mode_id. Enumeration of PDP call mode value.
 */
enum pdp_call_mode_id pdp_call_mode_enum(const char *arg);

/**
 * Convert PDP call mode enum to string.
 * @param[in]   mode    PDP call mode enumeration value.
 * @return const char *. String of PDP call mode value.
 */
const char *pdp_call_mode_str(enum pdp_call_mode_id mode);

/**
 * Convert PDP call state enum to string.
 * @param[in]   state    PDP call state enumeration value.
 * @return const char *. String of PDP call state mode value.
 */
const char *pdp_call_state_str(enum pdp_call_state_id state);

/**
 * Convert USSD mode string to USSD mode enum.
 * @param[in]	*arg	USSD mode string argument value.
 * @return enum ussd_mode_id. Enumeration of USSD mode value.
 */
enum ussd_mode_id ussd_mode_enum(const char *arg);

/**
 * Convert USSD mode enum to string.
 * @param[in]   mode    USSD mode enumeration value.
 * @return const char *. String of USSD mode value.
 */
const char *ussd_mode_str(enum ussd_mode_id mode);

/**
 * Convert time mode string to time mode enum.
 * @param[in]	*arg	time mode string argument value.
 * @return enum time_mode_id. Enumeration of time mode value.
 */
enum time_mode_id time_mode_enum(const char *arg);

/**
 * Convert time mode enum to string.
 * @param[in]   mode    time mode enumeration value.
 * @return const char *. String of time mode value.
 */
const char *time_mode_str(enum time_mode_id mode);

/**
 * Convert primary cell state enum to string.
 * @param[in]   state    Primary cell state enumeration value.
 * @return const char *. String of primary cell state value.
 */
const char *pcell_state_str(enum pcell_state_t state);

/**
 * Convert secondary cell state enum to string.
 * @param[in]   state    Secondary cell state enumeration value.
 * @return const char *. String of secondary cell state value.
 */
const char *scell_state_str(enum scell_state_t state);

/**
 * Convert bandwidth enum to string.
 * @param[in]     id     Bandwidth enumeration value.
 * @return const char *. String of bandwidth value.
 */
const char *bandwidth_str(enum bandwidth_id id);

/**
 * Convert bandwidth enum to string.
 * @param[in]     id     Bandwidth enumeration value.
 * @return const char *. String of bandwidth value.
 */
const char *bandwidth_5G_str(enum bandwidth_5G_id id);

/**
 * Convert storage type string to storage type enum.
 * @param[in]	*arg	Storage type string argument value.
 * @return enum storage_type_id. Enumeration of storage type value.
 */
enum storage_type_id storage_type_enum(const char *arg);

/**
 * Convert storage type enum to string.
 * @param[in]    type    Storage type enumeration value.
 * @return const char *. String of storage type value.
 */
const char *storage_type_str(enum storage_type_id type);

/**
 * Convert nmea type string to nmea type enum.
 * @param[in]	*arg	Nmea type string argument value.
 * @return enum nmea_type_id. Enumeration of nmea type value.
 */
enum nmea_type_id nmea_type_enum(const char *arg);

/**
 * Convert nmea type enum to string.
 * @param[in]    type    Nmea type enumeration value.
 * @return const char *. String of nmea type value.
 */
const char *nmea_type_str(enum nmea_type_id type);

/**
 * Convert nmea satellite constellation type enum to string.
 * @param[in]    type    Nmea type enumeration value.
 * @return const char *. String of nmea type value.
 */
const char *nmea_sata_arg_str(enum nmea_sata_type_id sata);

/**
 * Convert nmea satellite constellation type string to enum.
 * @param[char]   *arg    Nmea satellite string value.
 * @return enum nmea_sata_type_id. enumeration of nmea arg value.
 */
enum nmea_sata_type_id nmea_sata_arg_enum(const char *arg);

/**
 * Convert GNSS mode string to enum.
 * @param[char]   *arg    GNSS mode string value.
 * @return enum gnss_mode_id. Enumeration of GNSS mode value.
 */
enum gnss_mode_id gnss_mode_enum(const char *arg);

/**
 * Convert GNSS mode enum to string.
 * @param[in]    mode    GNSS mode enumeration value.
 * @return const char *. String of GNSS mode value.
 */
const char *gnss_mode_str(enum gnss_mode_id mode);

/**
 * Convert GNSS accuracy level string to enum.
 * @param[char]   *arg    GNSS accuracy level string value.
 * @return enum gnss_accuracy_id. Enumeration of GNSS accuracy level value.
 */
enum gnss_accuracy_id gnss_accuracy_enum(const char *arg);

/**
 * Convert GNSS accuracy level enum to string.
 * @param[in]    accuracy    GNSS accuracy level enumeration value.
 * @return const char *. String of GNSS accuracy level value.
 */
const char *gnss_accuracy_str(enum gnss_accuracy_id accuracy);

/**
 * Convert modem state enum to string.
 * @param[in]    state     Modem state enumeration value.
 * @return const char *. String of modem state value.
 */
const char *modem_state_str(enum modem_state_id state);

/**
 * Convert NMEA output port string to enum.
 * @param[char]   *arg    NMEA output port string value.
 * @return enum nmea_output_port_id. Enumeration of NMEA output port value.
 */
enum nmea_output_port_id nmea_output_port_enum(const char *arg);

/**
 * Convert NMEA output port enum to string.
 * @param[in]    port    NMEA output port enumeration value.
 * @return const char *. String of NMEA output port value.
 */
const char *nmea_output_port_str(enum nmea_output_port_id port);

/**
 * Convert URC indication type enum to string.
 * @param[in]    type    URC type enumeration value.
 * @return const char *. String of URC type value.
 */
const char *urc_ind_arg_str(enum urc_ind_type_id sata);

/**
 * Convert URC indication type string to enum.
 * @param[char]   *arg    URC type string value.
 * @return enum urc_ind_type_id. enumeration of URC arg value.
 */
enum urc_ind_type_id urc_ind_arg_enum(const char *arg);

/**
 * Convert URC indication type enum to string.
 * @param[in]    type    URC type enumeration value.
 * @return const char *. String of URC type value.
 */
const char *urc_voice_domain_str(enum urc_voice_domain_pref sata);

/**
 * Convert LTE SMS format mode string to enum.
 * @param[char]   *arg    LTE SMS format mode string value.
 * @return enum urc_lte_sms_format. Enumeration of LTE SMS format mode value.
 */
enum urc_lte_sms_format urc_lte_sms_format_enum(const char *arg);

/**
 * Convert URC indication type enum to string.
 * @param[in]    type    URC type enumeration value.
 * @return const char *. String of URC type value.
 */
const char *urc_lte_sms_format_str(enum urc_lte_sms_format urc);

/**
 * Convert modem functionality enum to string.
 * @param[in]    type    enumeration value.
 * @return const char *. String of type value.
 */
const char *modem_func_str(enum modem_func_t str);

/**
 * Convert CEFS restore state enum to string.
 * @param[in]    cefs_id    CEFS state enumeration value.
 * @return const char *. String of CEFS restore state value.
 */
const char *cefs_restore_state_str(enum cefs_restore_state_id cefs_id);

/**
 * Convert GNSS mode string to enum.
 * @param[char]   *arg    GNSS mode string value.
 * @return enum gnss_operation_mode_id. Enumeration of GNSS mode value.
 */
enum gnss_operation_mode_id gnss_operation_mode_enum(const char *arg);

/**
 * Convert GNSS mode enum to string.
 * @param[in]    mode    GNSS mode enumeration value.
 * @return const char *. String of GNSS mode value.
 */
const char *gnss_operation_mode_str(enum gnss_operation_mode_id mode);

/**
 * Convert EMM error cause enum to string.
 * @param[in]   emm_cause   EMM cause enumeration value.
 * @return const char *.    String of EMM error cause value.
 */
const char *emm_cause_str(enum emm_cause_id emm_cause);

/**
 * Convert ESM error cause enum to string.
 * @param[in]   esm_cause   ESM cause enumeration value.
 * @return const char *.    String of ESM error cause value.
 */
const char *esm_cause_str(enum esm_cause_id esm_cause);

/**
 * Convert DPO mode string to enum.
 * @param[char]   *arg    DPO mode string value.
 * @return enum dpo_mode_id. Enumeration of DPO mode value.
 */
enum dpo_mode_id dpo_mode_enum(const char *arg);

/**
 * Convert DPO mode enum to string.
 * @param[in]    mode    DPO mode enumeration value.
 * @return const char *. String of DPO mode value.
 */
const char *dpo_mode_str(enum dpo_mode_id mode);

/**
 * Convert Disable 5G mode string to enum.
 * @param[char]   *arg    disable mode string value.
 * @return enum disable_nr5g_mode_id. Enumeration of disable 5G mode value.
 */
enum disable_nr5g_mode_id disable_nr5g_mode_enum(const char *arg);

/**
 * Convert DPO mode enum to string.
 * @param[in]    mode    disable 5G mode enumeration value.
 * @return const char *. String of disable 5G mode value.
 */
const char *disable_nr5g_mode_str(enum disable_nr5g_mode_id mode);

#endif // GSM_MODEM_API
