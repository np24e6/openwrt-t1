#ifndef RALINK_H
#define RALINK_H

#include <stdint.h>

#define RT_PRIV_IOCTL	  (SIOCIWFIRSTPRIV + 0x01)
#define RTPRIV_IOCTL_SET  (SIOCIWFIRSTPRIV + 0x02)
#define RT_PRIV_IOCTL_EXT (SIOCIWFIRSTPRIV + 0x0E)

#define RTPRIV_IOCTL_BBP (SIOCIWFIRSTPRIV + 0x03)
#define RTPRIV_IOCTL_MAC (SIOCIWFIRSTPRIV + 0x05)

#define RTPRIV_IOCTL_E2P (SIOCIWFIRSTPRIV + 0x07)

#define RTPRIV_IOCTL_ATE (SIOCIWFIRSTPRIV + 0x08)

#define RTPRIV_IOCTL_STATISTICS		  (SIOCIWFIRSTPRIV + 0x09)
#define RTPRIV_IOCTL_ADD_PMKID_CACHE	  (SIOCIWFIRSTPRIV + 0x0A)
#define RTPRIV_IOCTL_RADIUS_DATA	  (SIOCIWFIRSTPRIV + 0x0C)
#define RTPRIV_IOCTL_GSITESURVEY	  (SIOCIWFIRSTPRIV + 0x0D)
#define RTPRIV_IOCTL_ADD_WPA_KEY	  (SIOCIWFIRSTPRIV + 0x0E)
#define RTPRIV_IOCTL_GET_MAC_TABLE	  (SIOCIWFIRSTPRIV + 0x0F)
#define RTPRIV_IOCTL_GET_MAC_TABLE_STRUCT (SIOCIWFIRSTPRIV + 0x1F)
#define RTPRIV_IOCTL_STATIC_WEP_COPY	  (SIOCIWFIRSTPRIV + 0x10)

#define RTPRIV_IOCTL_SHOW	   (SIOCIWFIRSTPRIV + 0x11)
#define RTPRIV_IOCTL_WSC_PROFILE   (SIOCIWFIRSTPRIV + 0x12)
#define RTPRIV_IOCTL_QUERY_BATABLE (SIOCIWFIRSTPRIV + 0x16)

#define RTPRIV_IOCTL_GET_AR9_SHOW (SIOCIWFIRSTPRIV + 0x17)

#define RTPRIV_IOCTL_SET_WSCOOB	  (SIOCIWFIRSTPRIV + 0x19)
#define RTPRIV_IOCTL_WSC_CALLBACK (SIOCIWFIRSTPRIV + 0x1A)

#define RTPRIV_IOCTL_IWINFO	     (SIOCIWFIRSTPRIV + 0x1C)
#define RTPRIV_IOCTL_GET_DRIVER_INFO (SIOCIWFIRSTPRIV + 0x1D)

#define RT_MAC_ADDR_LEN	     6
#define RT_MAX_NUMBER_OF_MAC 75

#define RT_RATE_MODE_CCK	  0
#define RT_RATE_MODE_OFDM	  1
#define RT_RATE_MODE_HTMIX	  2
#define RT_RATE_MODE_HTGREENFIELD 3
#define RT_RATE_MODE_VHT	  4

#define RT_IWINFO_CIPHER_NONE	(1 << 0)
#define RT_IWINFO_CIPHER_TKIP	(1 << 1)
#define RT_IWINFO_CIPHER_CCMP	(1 << 2)
#define RT_IWINFO_CIPHER_WEP40	(1 << 3)
#define RT_IWINFO_CIPHER_WEP104 (1 << 4)

#define RT_IWINFO_KMGMT_NONE (1 << 0)
#define RT_IWINFO_KMGMT_PSK  (1 << 1)
#define RT_IWINFO_KMGMT_SAE  (1 << 2)
#define RT_IWINFO_KMGMT_OWE  (1 << 3)

#define RT_IWINFO_WPA_V1 (1 << 0)
#define RT_IWINFO_WPA_V2 (1 << 1)
#define RT_IWINFO_WPA_V3 (1 << 2)

#define RT_IWINFO_AUTH_OPEN   (1 << 0)
#define RT_IWINFO_AUTH_SHARED (1 << 1)

enum rt_iwinfo_channelwidth {
	RT_IWINFO_BAND_WIDTH_20,
	RT_IWINFO_BAND_WIDTH_40,
	RT_IWINFO_BAND_WIDTH_80,
	RT_IWINFO_BAND_WIDTH_160,
	RT_IWINFO_BAND_WIDTH_10,
	RT_IWINFO_BAND_WIDTH_5,
	RT_IWINFO_BAND_WIDTH_8080,
	RT_IWINFO_BAND_WIDTH_BOTH,
	RT_IWINFO_BAND_WIDTH_25,
	__RT_IWINFO_BAND_MAX,
};

enum rt_iwinfo_cmd {
	RT_IWINFO_FREQLIST = 0,
	RT_IWINFO_ASSOCLIST,
	RT_IWINFO_COUNTRY,
	RT_IWINFO_COUNTRY_LIST,
	RT_IWINFO_HTMODE_LIST,
	RT_IWINFO_HTMODE,
	RT_IWINFO_ENCRYPTION,
	RT_IWINFO_STATS,
};

struct rt_iwinfo_msg {
	union {
		uint8_t cmd;
		uint16_t count;
	} header;
	uint8_t data[0];
};

struct rt_iwinfo_freq_entry {
	uint8_t channel;
	uint32_t mhz;
};

struct rt_iwinfo_signal_info {
	int8_t signal;
	int8_t noise;
	int8_t quality;
};

struct rt_iwinfo_rate {
	uint32_t rate;
	uint8_t mcs;
	uint8_t is_40mhz : 1;
	uint8_t is_short_gi : 1;
	uint8_t is_ht : 1;
	uint8_t mhz;
};

struct rt_iwinfo_sta_entry {
	uint8_t mac[6];
	struct rt_iwinfo_signal_info sig_noise;
	uint32_t inactive;
	uint32_t connected_time;
	uint32_t rx_packets;
	uint32_t tx_packets;
	uint32_t rx_bytes;
	uint32_t tx_bytes;
	struct rt_iwinfo_rate rx_rate;
	struct rt_iwinfo_rate tx_rate;
};

struct rt_iwinfo_htmode {
	uint16_t rt_ht;
};

struct rt_iwinfo_country_entry {
	uint16_t iso3166;
	char ccode[2];
};

struct rt_iwinfo_crypto_entry {
	uint8_t wpa_version;
	uint16_t group_ciphers;
	uint16_t pair_ciphers;
	uint8_t auth_suites;
	uint8_t auth_algs;
};

#endif // RALINK_H
