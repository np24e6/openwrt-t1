#include <libubus.h>

typedef enum {
	LRUT_FOTA_OK,
	LRUT_FOTA_UBUS_ERR,
	LRUT_FOTA_ARGUMENT_ERR,
} lrut_fota_t;

struct lrut_fota_process_st {
	int percents;
	char *process;
};

struct lrut_fota_info_st {
	char *fw;
	int fw_size;
#ifdef MOBILE_SUPPORT
	char *modem;
	int modem_size;
#endif
	char *conn_state;
};

lrut_fota_t lrut_fota_get_process(struct ubus_context *ubus, struct lrut_fota_process_st *process);
lrut_fota_t lrut_fota_set_process(struct ubus_context *ubus, int percents, char *process);
lrut_fota_t lrut_fota_get_info(struct ubus_context *ubus, struct lrut_fota_info_st *info);
lrut_fota_t lrut_fota_set_conn_state(struct ubus_context *ubus, char *conn_state);
lrut_fota_t lrut_fota_set_fw_info(struct ubus_context *ubus, char *fw_size, char *fw, char *stable_fw_size, char *stable_fw);
#ifdef MOBILE_SUPPORT
lrut_fota_t lrut_fota_set_modem_info(struct ubus_context *ubus, char *fw_size, char *fw, char *fw_ver, char *id);
#endif
lrut_fota_t lrut_fota_set_tap_info(struct ubus_context *ubus, char *mac, char *fw, char *fw_stable);
lrut_fota_t lrut_fota_reset_info(struct ubus_context *ubus);

/**
 * @brief Free info structure
 * 
 * @param info	pointer to info structure
 */
void lrut_free_info(struct lrut_fota_info_st *info);
