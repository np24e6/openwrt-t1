#ifndef GSM_MODEM_BAND_H
#define GSM_MODEM_BAND_H
#include "limits.h"
/** @file modem_band.h
 * This file contains GSM/WCDMA/LTE/5G supported bands.
*/

/**
 * Modem GSM band enumeration
 */
enum modem_gsm_band_id {
	MB_GSM_NONE = 0ULL, //!< Reserved
	MB_GSM_ANY  = 1ULL, //!< Reserved

	MB_GSM_850  = (1ULL << 1), //!< GSM 850
	MB_GSM_900  = (1ULL << 2), //!< GSM 900
	MB_GSM_1800 = (1ULL << 3), //!< GSM 1800
	MB_GSM_1900 = (1ULL << 4), //!< GSM 1900

	__MB_GSM_MAX,
	__MB_GSM_ALIGN = ULLONG_MAX,
};

/**
 * Modem WCDMA band enumeration
 */
enum modem_wcdma_band_id {
	MB_WCDMA_NONE = 0ULL, //!< Reserved
	MB_WCDMA_ANY  = 1ULL, //!< Reserved

	MB_WCDMA_700  = (1ULL << 1), //!< WCDMA 700
	MB_WCDMA_800  = (1ULL << 2), //!< WCDMA 800
	MB_WCDMA_850  = (1ULL << 3), //!< WCDMA 850
	MB_WCDMA_J850 = (1ULL << 4), //!< WCDMA Japan 850
	MB_WCDMA_900  = (1ULL << 5), //!< WCDMA 900
	MB_WCDMA_1500 = (1ULL << 6), //!< WCDMA 1500
	MB_WCDMA_1700 = (1ULL << 7), //!< WCDMA 1700
	MB_WCDMA_1800 = (1ULL << 8), //!< WCDMA 1800
	MB_WCDMA_1900 = (1ULL << 9), //!< WCDMA 1900
	MB_WCDMA_2100 = (1ULL << 10), //!< WCDMA 2100
	MB_WCDMA_2600 = (1ULL << 11), //!< WCDMA 2600
	MB_WCDMA_3500 = (1ULL << 12), //!< WCDMA 3500

	__MB_WCDMA_MAX,
	__MB_WCDMA_ALIGN = ULLONG_MAX,
};

/**
 * Modem LTE band enumeration
 */
enum modem_lte_band_id {
	MB_LTE_NONE = 0ULL, //!< Reserved
	MB_LTE_ANY  = 1ULL, //!< Reserved

	MB_LTE_B1 = (1ULL << 1), //!< LTE B1
	MB_LTE_B2 = (1ULL << 2), //!< LTE B2
	MB_LTE_B3 = (1ULL << 3), //!< LTE B3
	MB_LTE_B4 = (1ULL << 4), //!< LTE B4
	MB_LTE_B5 = (1ULL << 5), //!< LTE B5
	/* */
	MB_LTE_B7 = (1ULL << 6), //!< LTE B7
	MB_LTE_B8 = (1ULL << 7), //!< LTE B8
	/* */
	MB_LTE_B12 = (1ULL << 8), //!< LTE B12
	MB_LTE_B13 = (1ULL << 9), //!< LTE B13
	MB_LTE_B14 = (1ULL << 10), //!< LTE B14
	/* */
	MB_LTE_B17 = (1ULL << 11), //!< LTE B17
	MB_LTE_B18 = (1ULL << 12), //!< LTE B18
	MB_LTE_B19 = (1ULL << 13), //!< LTE B19
	MB_LTE_B20 = (1ULL << 14), //!< LTE B20
	/* */
	MB_LTE_B25 = (1ULL << 15), //!< LTE B25
	MB_LTE_B26 = (1ULL << 16), //!< LTE B26
	/* */
	MB_LTE_B27 = (1ULL << 17), //!< LTE B27
	MB_LTE_B28 = (1ULL << 18), //!< LTE B28
	MB_LTE_B29 = (1ULL << 19), //!< LTE B29
	MB_LTE_B30 = (1ULL << 20), //!< LTE B30
	MB_LTE_B31 = (1ULL << 21), //!< LTE B31
	/* */
	MB_LTE_B32 = (1ULL << 22), //!< LTE B32
	/* */
	MB_LTE_B34 = (1ULL << 23), //!< LTE B34
	/* */
	MB_LTE_B38 = (1ULL << 24), //!< LTE B38
	MB_LTE_B39 = (1ULL << 25), //!< LTE B39
	/* */
	MB_LTE_B40 = (1ULL << 26), //!< LTE B40
	MB_LTE_B41 = (1ULL << 27), //!< LTE B41
	MB_LTE_B42 = (1ULL << 28), //!< LTE B42
	MB_LTE_B43 = (1ULL << 29), //!< LTE B43
	/* */
	MB_LTE_B48 = (1ULL << 30), //!< LTE B48
	/* */
	MB_LTE_B66 = (1ULL << 31), //!< LTE B66
	/* */
	MB_LTE_B71 = (1ULL << 32), //!< LTE B71
	MB_LTE_B72 = (1ULL << 33), //!< LTE B72
	MB_LTE_B73 = (1ULL << 34), //!< LTE B73
	/* */
	MB_LTE_B85 = (1ULL << 35), //!< LTE B85

	__MB_LTE_MAX,
	__MB_LTE_ALIGN = ULLONG_MAX,
};

/**
 * Modem LTE Narrow Band enumeration
 */
enum modem_lte_nb_band_id {
	MB_LTE_NB_NONE = 0ULL, //!< Reserved
	MB_LTE_NB_ANY  = 1ULL, //!< Reserved

	MB_LTE_NB1 = (1ULL << 1), //!< LTE B1
	MB_LTE_NB2 = (1ULL << 2), //!< LTE B2
	MB_LTE_NB3 = (1ULL << 3), //!< LTE B3
	MB_LTE_NB4 = (1ULL << 4), //!< LTE B4
	MB_LTE_NB5 = (1ULL << 5), //!< LTE B5
	/* */
	MB_LTE_NB7 = (1ULL << 6), //!< LTE B7
	MB_LTE_NB8 = (1ULL << 7), //!< LTE B8
	/* */
	MB_LTE_NB12 = (1ULL << 8), //!< LTE B12
	MB_LTE_NB13 = (1ULL << 9), //!< LTE B13
	MB_LTE_NB14 = (1ULL << 10), //!< LTE B14
	/* */
	MB_LTE_NB17 = (1ULL << 11), //!< LTE B17
	MB_LTE_NB18 = (1ULL << 12), //!< LTE B18
	MB_LTE_NB19 = (1ULL << 13), //!< LTE B19
	MB_LTE_NB20 = (1ULL << 14), //!< LTE B20
	/* */
	MB_LTE_NB25 = (1ULL << 15), //!< LTE B25
	MB_LTE_NB26 = (1ULL << 16), //!< LTE B26
	/* */
	MB_LTE_NB27 = (1ULL << 17), //!< LTE B27
	MB_LTE_NB28 = (1ULL << 18), //!< LTE B28
	MB_LTE_NB29 = (1ULL << 19), //!< LTE B29
	MB_LTE_NB30 = (1ULL << 20), //!< LTE B30
	MB_LTE_NB31 = (1ULL << 21), //!< LTE B31
	/* */
	MB_LTE_NB32 = (1ULL << 22), //!< LTE B32
	/* */
	MB_LTE_NB34 = (1ULL << 23), //!< LTE B34
	/* */
	MB_LTE_NB38 = (1ULL << 24), //!< LTE B38
	MB_LTE_NB39 = (1ULL << 25), //!< LTE B39
	/* */
	MB_LTE_NB40 = (1ULL << 26), //!< LTE B40
	MB_LTE_NB41 = (1ULL << 27), //!< LTE B41
	MB_LTE_NB42 = (1ULL << 28), //!< LTE B42
	MB_LTE_NB43 = (1ULL << 29), //!< LTE B43
	/* */
	MB_LTE_NB48 = (1ULL << 30), //!< LTE B48
	/* */
	MB_LTE_NB66 = (1ULL << 31), //!< LTE B66
	/* */
	MB_LTE_NB71 = (1ULL << 32), //!< LTE B71
	MB_LTE_NB72 = (1ULL << 33), //!< LTE B72
	MB_LTE_NB73 = (1ULL << 34), //!< LTE B73
	/* */
	MB_LTE_NB85 = (1ULL << 35), //!< LTE B85

	__MB_LTE_NB_MAX,
	__MB_LTE_NB_ALIGN = ULLONG_MAX,
};

/**
 * Modem NSA 5G band enumeration
 */
enum modem_nsa5g_band_id {
	MB_NSA_5G_NONE = 0ULL, //!< Reserved
	MB_NSA_5G_ANY  = 1ULL, //!< Reserved

	MB_NSA_5G_N1 = (1ULL << 1), //!< NSA 5G N1
	MB_NSA_5G_N2 = (1ULL << 2), //!< NSA 5G N2
	MB_NSA_5G_N3 = (1ULL << 3), //!< NSA 5G N3
	/* */
	MB_NSA_5G_N5 = (1ULL << 4), //!< NSA 5G N5
	/* */
	MB_NSA_5G_N7 = (1ULL << 5), //!< NSA 5G N7
	MB_NSA_5G_N8 = (1ULL << 6), //!< NSA 5G N8
	/* */
	MB_NSA_5G_N12 = (1ULL << 7), //!< NSA 5G N12
	MB_NSA_5G_N13 = (1ULL << 8), //!< NSA 5G N13
	MB_NSA_5G_N14 = (1ULL << 9), //!< NSA 5G N14
	/* */
	MB_NSA_5G_N20 = (1ULL << 10), //!< NSA 5G N20
	/* */
	MB_NSA_5G_N25 = (1ULL << 11), //!< NSA 5G N25
	MB_NSA_5G_N26 = (1ULL << 12), //!< NSA 5G N26
	/* */
	MB_NSA_5G_N28 = (1ULL << 13), //!< NSA 5G N28
	MB_NSA_5G_N29 = (1ULL << 14), //!< NSA 5G N29
	MB_NSA_5G_N30 = (1ULL << 15), //!< NSA 5G N30
	/* */
	MB_NSA_5G_N38 = (1ULL << 16), //!< NSA 5G N38
	/* */
	MB_NSA_5G_N40 = (1ULL << 17), //!< NSA 5G N40
	MB_NSA_5G_N41 = (1ULL << 18), //!< NSA 5G N41
	/* */
	MB_NSA_5G_N48 = (1ULL << 19), //!< NSA 5G N48
	/* */
	MB_NSA_5G_N66 = (1ULL << 20), //!< NSA 5G N66
	/* */
	MB_NSA_5G_N70 = (1ULL << 21), //!< NSA 5G N70
	MB_NSA_5G_N71 = (1ULL << 22), //!< NSA 5G N71
	/* */
	MB_NSA_5G_N75 = (1ULL << 23), //!< NSA 5G N75
	MB_NSA_5G_N76 = (1ULL << 24), //!< NSA 5G N76
	MB_NSA_5G_N77 = (1ULL << 25), //!< NSA 5G N77
	MB_NSA_5G_N78 = (1ULL << 26), //!< NSA 5G N78
	MB_NSA_5G_N79 = (1ULL << 27), //!< NSA 5G N79
	/* */
	MB_NSA_5G_N257 = (1ULL << 28), //!< NSA 5G N257
	MB_NSA_5G_N258 = (1ULL << 29), //!< NSA 5G N258
	/* */
	MB_NSA_5G_N260 = (1ULL << 30), //!< NSA 5G N260
	MB_NSA_5G_N261 = (1ULL << 31), //!< NSA 5G N261
	MB_NSA_5G_N262 = (1ULL << 32), //!< NSA 5G N262

	__MB_NSA_5G_MAX,
	__MB_NSA_5G_ALIGN = ULLONG_MAX,
};

/**
 * Modem 5G band enumeration
 */
enum modem_5g_band_id {
	MB_5G_NONE = 0ULL, //!< Reserved
	MB_5G_ANY  = 1ULL, //!< Reserved

	MB_5G_N1 = (1ULL << 1), //!< 5G N1
	MB_5G_N2 = (1ULL << 2), //!< 5G N2
	MB_5G_N3 = (1ULL << 3), //!< 5G N3
	/* */
	MB_5G_N5 = (1ULL << 4), //!< 5G N5
	/* */
	MB_5G_N7 = (1ULL << 5), //!< 5G N7
	MB_5G_N8 = (1ULL << 6), //!< 5G N8
	/* */
	MB_5G_N12 = (1ULL << 7), //!< 5G N12
	MB_5G_N13 = (1ULL << 8), //!< 5G N13
	MB_5G_N14 = (1ULL << 9), //!< 5G N14
	/* */
	MB_5G_N20 = (1ULL << 10), //!< 5G N20
	/* */
	MB_5G_N25 = (1ULL << 11), //!< 5G N25
	MB_5G_N26 = (1ULL << 12), //!< 5G N26
	/* */
	MB_5G_N28 = (1ULL << 13), //!< 5G N28
	MB_5G_N29 = (1ULL << 14), //!< 5G N29
	MB_5G_N30 = (1ULL << 15), //!< 5G N30
	/* */
	MB_5G_N38 = (1ULL << 16), //!< 5G N38
	/* */
	MB_5G_N40 = (1ULL << 17), //!< 5G N40
	MB_5G_N41 = (1ULL << 18), //!< 5G N41
	/* */
	MB_5G_N48 = (1ULL << 19), //!< 5G N48
	/* */
	MB_5G_N66 = (1ULL << 20), //!< 5G N66
	/* */
	MB_5G_N70 = (1ULL << 21), //!< 5G N70
	MB_5G_N71 = (1ULL << 22), //!< 5G N71
	/* */
	MB_5G_N75 = (1ULL << 23), //!< 5G N75
	MB_5G_N76 = (1ULL << 24), //!< 5G N76
	MB_5G_N77 = (1ULL << 25), //!< 5G N77
	MB_5G_N78 = (1ULL << 26), //!< 5G N78
	MB_5G_N79 = (1ULL << 27), //!< 5G N79

	__MB_5G_MAX,
	__MB_5G_ALIGN = ULLONG_MAX,
};

/**
 * Convert GSM band enumeration value to string value.
 * @param[in]	band	Enumeration value of GSM band.
 * @return const char *. String of GSM band value.
 */
const char *modem_gsm_band_str(enum modem_gsm_band_id band);

/**
 * Convert WCDMA band enumeration value to string value.
 * @param[in]	band	Enumeration value of WCDMA band.
 * @return const char *. String of WCDMA band value.
 */
const char *modem_wcdma_band_str(enum modem_wcdma_band_id band);

/**
 * Convert LTE band enumeration value to string value.
 * @param[in]	band	Enumeration value of LTE band.
 * @return const char *. String of LTE band value.
 */
const char *modem_lte_band_str(enum modem_lte_band_id band);

/**
 * Convert LTE band enumeration value to LTE CAT-NB string value.
 * @param[in]	band	Enumeration value of LTE band.
 * @return const char *. String of LTE NB band value.
 */
const char *modem_lte_nb_band_str(enum modem_lte_nb_band_id band);

/**
 * Convert NSA 5G band enumeration value to string value.
 * @param[in]	band	Enumeration value of NSA 5G band.
 * @return const char *. String of NSA 5G band value.
 */
const char *modem_nsa_5g_band_str(enum modem_nsa5g_band_id band);

/**
 * Convert 5G band enumeration value to string value.
 * @param[in]	band	Enumeration value of 5G band.
 * @return const char *. String of 5G band value.
 */
const char *modem_5g_band_str(enum modem_5g_band_id band);

/**
 * Convert GSM band enumeration value to return string value.
 * @param[in]	band	  Enumeration value of GSM band.
 * @param[in]	band_str  Band string value buffer.
 * @return const char *. String of GSM band return value.
 */
char *modem_gsm_band_ret_str(enum modem_gsm_band_id band, char *band_str);

/**
 * Convert WCDMA band enumeration value to return string value.
 * @param[in]	band	  Enumeration value of WCDMA band.
 * @param[in]	band_str  Band string value buffer.
 * @return const char *. String of LTEWCDMA band return value.
 */
char *modem_wcdma_band_ret_str(enum modem_wcdma_band_id band, char *band_str);

/**
 * Convert LTE band enumeration value to return string value.
 * @param[in]	band	  Enumeration value of LTE band.
 * @param[in]	band_str  Band string value buffer.
 * @return const char *. String of LTE band return value.
 */
char *modem_lte_band_ret_str(enum modem_lte_band_id band, char *band_str);

/**
 * Convert LTE NB band enumeration value to return string value.
 * @param[in]	band	  Enumeration value of LTE NB band.
 * @param[in]	band_str  Band string value buffer.
 * @return const char *. String of LTE NB band return value.
 */
char *modem_lte_nb_band_ret_str(enum modem_lte_nb_band_id band, char *band_str);

/**
 * Convert 5G NSA band enumeration value to return string value.
 * @param[in]	band	  Enumeration value of 5G NSA band.
 * @param[in]	band_str  Band string value buffer.
 * @return const char *. String of 5G NSA band return value.
 */
char *modem_nsa_5g_band_ret_str(enum modem_nsa5g_band_id band, char *band_str);

/**
 * Convert 5G band enumeration value to return string value.
 * @param[in]	band	  Enumeration value of 5G band.
 * @param[in]	band_str  Band string value buffer.
 * @return const char *. String of 5G band return value.
 */
char *modem_5g_band_ret_str(enum modem_5g_band_id band, char *band_str);

/**
 * Retrieve gsm band value by string.
 * @param[in]	*name	String of gsm band.
 * @return enum modem_gsm_band_id. Enumeration value of gsm band value.
 * If band is not found enum `__MB_GSM_MAX` is returned.
 */
enum modem_gsm_band_id modem_info_gsm_band_val(const char *name);

/**
 * Retrieve wcdma band value by string.
 * @param[in]	*name	String of wcdma band.
 * @return enum modem_wcdma_band_id. Enumeration value of wcdma band value.
 * If band is not found enum `__MB_WCDMA_MAX` is returned.
 */
enum modem_wcdma_band_id modem_info_wcdma_band_val(const char *name);

/**
 * Retrieve lte band value by string.
 * @param[in]	*name	String of lte band.
 * @return enum modem_lte_band_id. Enumeration value of lte band value.
 * If band is not found enum `__MB_LTE_MAX` is returned.
 */
enum modem_lte_band_id modem_info_lte_band_val(const char *name);

/**
 * Retrieve lte band value by lte cat-nb string.
 * @param[in]	*name	String of lte band.
 * @return enum modem_lte_nb_band_id. Enumeration value of lte band value.
 * If band is not found enum `__MB_LTE_MAX` is returned.
 */
enum modem_lte_nb_band_id modem_info_lte_nb_band_val(const char *name);

/**
 * Retrieve nsa 5g band value by string.
 * @param[in]	*name	String of lte band.
 * @return enum modem_nsa5g_band_id. Enumeration value of nsa 5g band value.
 * If band is not found enum `__MB_NSA_5G_MAX` is returned.
 */
enum modem_nsa5g_band_id modem_info_nsa_5g_band_val(const char *name);

/**
 * Retrieve 5g band value by string.
 * @param[in]	*name	String of lte band.
 * @return enum modem_nsa5g_band_id. Enumeration value of 5g band value.
 * If band is not found enum `__MB_5G_MAX` is returned.
 */
enum modem_5g_band_id modem_info_5g_band_val(const char *name);

/**
 * Retrieve gsm band value by return format string.
 * @param[in]	*name	String of gsm band.
 * @return enum modem_gsm_band_id. Enumeration value of gsm band value.
 * If band is not found enum `__MB_GSM_MAX` is returned.
 */
enum modem_gsm_band_id modem_info_gsm_band_ret_val(const char *name);

/**
 * Retrieve wcdma band value by return format string.
 * @param[in]	*name	String of wcdma band.
 * @return enum modem_wcdma_band_id. Enumeration value of wcdma band value.
 * If band is not found enum `__MB_WCDMA_MAX` is returned.
 */
enum modem_wcdma_band_id modem_info_wcdma_band_ret_val(const char *name);

/**
 * Retrieve lte band value by return format string.
 * @param[in]	*name	String of lte band.
 * @return enum modem_lte_band_id. Enumeration value of lte band value.
 * If band is not found enum `__MB_LTE_MAX` is returned.
 */
enum modem_lte_band_id modem_info_lte_band_ret_val(const char *name);

/**
 * Retrieve lte band value by lte cat-nb return format string.
 * @param[in]	*name	String of lte band.
 * @return enum modem_lte_nb_band_id. Enumeration value of lte band value.
 * If band is not found enum `__MB_LTE_MAX` is returned.
 */
enum modem_lte_nb_band_id modem_info_lte_nb_band_ret_val(const char *name);

/**
 * Retrieve nsa 5g band value by return format string.
 * @param[in]	*name	String of lte band.
 * @return enum modem_nsa5g_band_id. Enumeration value of nsa 5g band value.
 * If band is not found enum `__MB_NSA_5G_MAX` is returned.
 */
enum modem_nsa5g_band_id modem_info_nsa_5g_band_ret_val(const char *name);

/**
 * Retrieve 5g band value by return format string.
 * @param[in]	*name	String of lte band.
 * @return enum modem_nsa5g_band_id. Enumeration value of 5g band value.
 * If band is not found enum `__MB_5G_MAX` is returned.
 */
enum modem_5g_band_id modem_info_5g_band_ret_val(const char *name);

/**
 * Convert WCDMA band enumeration value to band number string value.
 * @param[in]	band	Enumeration value of WCDMA band.
 * @return const char *. String of WCDMA band value.
 */
const char *wcdma_band_str(enum modem_wcdma_band_id band);

/**
 * Convert LTE band enumeration value to band number string value.
 * @param[in]	band	Enumeration value of LTE band.
 * @return const char *. String of LTE band value.
 */
const char *lte_band_str(enum modem_lte_band_id band);

/**
 * Convert NSA 5G band enumeration value to band number string value.
 * @param[in]	band	Enumeration value of NSA 5G band.
 * @return const char *. String of NSA 5G band value.
 */
const char *nsa_5g_band_str(enum modem_nsa5g_band_id band);

/**
 * Convert 5G band enumeration value to band number string value.
 * @param[in]	band	Enumeration value of 5G band.
 * @return const char *. String of 5G band value.
 */
const char *sa_5g_band_str(enum modem_5g_band_id band);

/**
 * Retrieve wcdma band value by band number string.
 * @param[in]	*name	String of wcdma band number.
 * @return enum modem_wcdma_band_id. Enumeration value of wcdma band value.
 * If band is not found enum `__MB_WCDMA_MAX` is returned.
 */
enum modem_wcdma_band_id wcdma_band_val(const char *name);

/**
 * Retrieve lte band value by band number string.
 * @param[in]	*name	String of lte band number.
 * @return enum modem_lte_band_id. Enumeration value of lte band value.
 * If band is not found enum `__MB_LTE_MAX` is returned.
 */
enum modem_lte_band_id lte_band_val(const char *name);

/**
 * Retrieve nsa 5g band value by band number string.
 * @param[in]	*name	String of nsa 5g band number.
 * @return enum modem_nsa5g_band_id. Enumeration value of nsa 5g band value.
 * If band is not found enum `__MB_NSA_5G_MAX` is returned.
 */
enum modem_nsa5g_band_id nsa_5g_band_val(const char *name);

/**
 * Retrieve 5g band value by band number string.
 * @param[in]	*name	String of 5g band number.
 * @return enum modem_nsa5g_band_id. Enumeration value of 5g band value.
 * If band is not found enum `__MB_5G_MAX` is returned.
 */
enum modem_5g_band_id sa_5g_band_val(const char *name);

#endif // GSM_MODEM_BAND_H
