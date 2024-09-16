
#ifndef MNFINFO_H
#define MNFINFO_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>

/*
 * mnfinfo_get_*() functions return ptr to static memory, which is not to be
 * free()d! the returned memory ptr is safely accessible throughout the using
 * program's runtime bad: free(mnfinfo_get_mac());
 *
 * returns NULL on /dev/mtdX reading failure, "N/A" if the particular device
 *  doesn't support the field, or a dummy string if the mtdblock space contains
 * garbage
 */
char *lmnfinfo_get_mac(void);
char *lmnfinfo_get_name(void);
char *lmnfinfo_get_maceth(void);
char *lmnfinfo_get_sn(void);
char *lmnfinfo_get_blver(void);
char *lmnfinfo_get_hwver(void);
char *lmnfinfo_get_branch(void);
char *lmnfinfo_get_full_hwver(void);
char *lmnfinfo_get_batch(void);
char *lmnfinfo_get_wps(void);
char *lmnfinfo_get_wifi_pw(void);
char *lmnfinfo_get_passw(void);
char *lmnfinfo_get_sim_pin(uint8_t sim_id);
uint32_t lmnfinfo_get_sim_pin_count();

// returns true on success
bool lmnfinfo_set_sim_pin(uint8_t sim_id, const char *pin);
#ifdef __cplusplus
}
#endif

#endif
