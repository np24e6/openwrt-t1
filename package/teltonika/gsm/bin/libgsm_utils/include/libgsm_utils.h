
#ifdef __cplusplus
extern "C" {
#endif

#pragma once

#include <libubus.h>
#include <gsm/modem_api.h>

#define LGSMU_MODEM_ID_LEN 32
#define LGSMU_UBUS_MODEM_PATH "gsm.modem*"

typedef enum { LGSMU_IPV4 = 4, LGSMU_IPV6 = 6 } lgsmu_ip_v;

typedef enum { LGSMU_SUCCESS, LGSMU_ERROR } lgsmu_stat;

typedef enum { LGSMU_STATE_UNKNOWN, LGSMU_STATE_CONNECTED, LGSMU_STATE_DISCONNECTED } lgsmu_conn_state;

typedef struct {
	char modem_id[LGSMU_MODEM_ID_LEN];
	uint32_t modem_num;
} lgsmu_modem_t;

typedef struct {
	lgsmu_modem_t *arr;
	uint16_t modem_cnt;
} lgsmu_modem_arr_t;

typedef struct {
	char *ip;
	size_t mask;
	lgsmu_ip_v type;
} lgsmu_addr_t;

typedef struct {
	char *modem_id;
	uint8_t sim;
	char *bridge_ipaddr;
	char *method;
} lgsmu_data_t;

typedef struct {
	char *name;
	uint8_t up;
	lgsmu_addr_t *addr_arr;
	size_t n_addr;
	lgsmu_data_t data;
} lgsmu_iface_t;

typedef struct {
	lgsmu_iface_t **iface_arr;
	size_t n_ifaces;
} lgsmu_iface_arr_t;

/**
 * @brief Returns mobile connection state my modem_id
 * 
 * @param ubus UBUS context
 * @param modem_id modemid
 * @return lgsmu_conn_state 
 */
lgsmu_conn_state lgsmu_get_connstate(struct ubus_context *ubus, const char *modem_id);

/**
 * @brief Returns connection state string
 * 
 * @param state connection state id
 * @return connection state string
 */
const char *lgsmu_conn_state_str(lgsmu_conn_state state);

/**
 * @brief get mobile interface by modem ID
 * 
 * @param ubus_ctx UBUS context 
 * @param dst pointer to the destination structure where the interface data is to be copied
 * @param modem_id modem ID
 * @return size_t number of interfaces found
 */
size_t lgsmu_get_interfaces_by_modem(struct ubus_context *ubus_ctx, lgsmu_iface_arr_t *dst,
				     const char *modem_id);

/**
 * @brief free interface structure
 * 
 * @param iface poiter to interface structure
 */
void lgsmu_free_iface(lgsmu_iface_t *iface);

/**
 * @brief free interface array structure
 * 
 * @param arr poiter to interface array structure
 */
void lgsmu_free_iface_arr(lgsmu_iface_arr_t *arr);

/**
 * @brief get modem structure lgsmu_stat by modem ID
 * 
 * @param ubus UBUS context
 * @param dst pointer to the destination structure where the content is to be copied
 * @param modem_id modem ID
 * @return lgsmu_stat 
 * @retval LGSMU_SUCCESS on success
 * @retval LGSMU_ERROR on fail
 */
lgsmu_stat lgsmu_get_modem_by_id(struct ubus_context *ubus, lgsmu_modem_t *dst,
		const char *modem_id);

/**
 * @brief get modem list
 *
 * @param ubus UBUS context
 * @param dst pointer to the destination structure where the list is to be allocated
 * @return lgsmu_stat 
 * @retval LGSMU_SUCCESS on success
 * @retval LGSMU_ERROR on fail
 */
lgsmu_stat lgsmu_get_modems(struct ubus_context *ubus, lgsmu_modem_arr_t *dst);

/**
 * @brief free modems structure
 * 
 * @param modems poiter to modems structure
 */
void lgsmu_modems_free(lgsmu_modem_arr_t *modems);

/**
 * @brief Get the default modem
 * 
 * @param ubus UBUS context 
 * @param dst pointer to the destination structure where the content is to be copied
 * @return lgsmu_stat 
 * @retval LGSMU_SUCCESS on success
 * @retval LGSMU_ERROR on fail
 */
lgsmu_stat lgsmu_get_default_modem(struct ubus_context *ubus, lgsmu_modem_t *dst);

/**
 * @brief Get the default modem ID
 * 
 * @param ubus UBUS context 
 * @return char* default modem ID
 */
char *lgsmu_get_default_modem_id(struct ubus_context *ubus);

/**
 * @brief Get the default modem number
 * 
 * @param ubus UBUS context 
 * @return uint32_t default modem number
 */
uint32_t lgsmu_get_default_modem_num(struct ubus_context *ubus);

/**
 * @brief convert modem USB id to number
 * 
 * @param ubus UBUS context
 * @param modem_id USB ID
 * @return int -1 on fail, unsigned value on success
 */
int lgsmu_modem_id_to_num(struct ubus_context *ubus_ctx, const char *modem_id);

/**
 * @brief Get operator name by specified format
 *
 * @param ubus UBUS context
 * @param modem_num Modem number
 * @param format Desired format enum
 * @return char* operator name in specified format
 */
char *lgsmu_get_operator_name(struct ubus_context *ubus, int modem_num, enum op_slc_fmt_id format);

#ifdef __cplusplus
}
#endif
