#include "iwinfo_wext.h"
#include "api/ralink.h"

struct survey_table {
	char channel[4];
	char ssid[33];
	char bssid[20];
	char security[23];
	char *crypto;
	char signal[6];
};

#define MAX_SURVEY_CNT 128
struct survey_table st[MAX_SURVEY_CNT];
int survey_count	      = 0;
static const char probe_str[] = "Driver version:";

#define MIN(_a, _b) (((_a) < (_b)) ? (_a) : (_b))

#define ENC_COPY_FIELD(__iwinfo, __rt, __field)                                                                        \
	do {                                                                                                           \
		__iwinfo |= (((__rt)&RT_IWINFO_##__field) ? IWINFO_##__field : 0);                                     \
	} while (0)

static void ralink_ioctl_set(const char *name, const char *key, const char *val)
{
	int socket_id;
	struct iwreq wrq;
	char data[64];
	snprintf(data, 64, "%s=%s", key, val);
	socket_id = socket(AF_INET, SOCK_DGRAM, 0);
	strcpy(wrq.ifr_ifrn.ifrn_name, name);
	wrq.u.data.length  = strlen(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags   = 0;
	ioctl(socket_id, RTPRIV_IOCTL_SET, &wrq);
	close(socket_id);
}

static int ralink_do_iwinfo_ioctl(const char *ifname, unsigned cmd, char *msgbuf, unsigned msgbuf_len, unsigned *count,
				  char **data)
{
	int ret;
	int s;
	struct iwreq wrq;
	struct rt_iwinfo_msg *msg = (struct rt_iwinfo_msg *)msgbuf;

	strcpy(wrq.ifr_name, ifname);
	wrq.u.data.length  = (uint16_t)msgbuf_len;
	wrq.u.data.pointer = msgbuf;
	wrq.u.data.flags   = 0;
	msg->header.cmd	   = cmd;
	*data		   = NULL;
	*count		   = 0;

	s   = socket(AF_INET, SOCK_DGRAM, 0);
	ret = ioctl(s, RTPRIV_IOCTL_IWINFO, &wrq);
	close(s);

	if (ret)
		return -1;

	*data  = (char *)msg->data;
	*count = msg->header.count;

	return 0;
}

static int ralink_probe(const char *ifname);

static bool ralink_is_device(const char *ifname)
{
	return !strcmp(ifname, "radio0");
}

static const char *ralink_to_interface(const char *ifname)
{
	if (!ralink_is_device(ifname))
		return ifname;

	if (!ralink_probe("ra0"))
		return NULL;

	return "ra0";
}

static int ralink_probe(const char *ifname)
{
	char data[1024] = { 0 };
	int ret;
	int s;
	struct iwreq wrq;

	if (ralink_is_device(ifname))
		return 1;

	strcpy(wrq.ifr_name, ifname);
	wrq.u.data.length  = 1024;
	wrq.u.data.pointer = data;
	wrq.u.data.flags   = 0;
	s		   = socket(AF_INET, SOCK_DGRAM, 0);
	ret		   = ioctl(s, RTPRIV_IOCTL_GET_DRIVER_INFO, &wrq);
	close(s);

	if (ret)
		return 0;

	if (MIN(wrq.u.data.length, sizeof(probe_str)) != sizeof(probe_str))
		return 0;

	return strncmp(data, probe_str, sizeof(probe_str) - 1) == 0;
}

static void ralink_close(void)
{
	/* Nop */
}

static int ralink_get_mode(const char *ifname, int *buf)
{
	if (ralink_is_device(ifname))
		return -1;

	return wext_ops.mode(ifname, buf);
}

static int ralink_get_ssid(const char *ifname, char *buf)
{
	if (ralink_is_device(ifname))
		ifname = "apcli0";

	return wext_ops.ssid(ifname, buf);
}

static int ralink_get_bssid(const char *ifname, char *buf)
{
	if (ralink_is_device(ifname))
		return -1;

	return wext_ops.bssid(ifname, buf);
}

static int ralink_get_bitrate(const char *ifname, int *buf)
{
	if (ralink_is_device(ifname))
		return -1;

	return wext_ops.bitrate(ifname, buf);
}

static int ralink_get_channel(const char *ifname, int *buf)
{
	ifname = ralink_to_interface(ifname);
	if (!ifname)
		return -1;

	return wext_ops.channel(ifname, buf);
}

static int ralink_get_center_chan1(const char *ifname, int *buf)
{
	// TODO
	return -1;
}

static int ralink_get_center_chan2(const char *ifname, int *buf)
{
	// TODO
	return -1;
}

static int ralink_get_frequency(const char *ifname, int *buf)
{
	ifname = ralink_to_interface(ifname);
	if (!ifname)
		return -1;

	return wext_ops.frequency(ifname, buf);
}

static int ralink_get_txpower(const char *ifname, int *buf)
{
	ifname = ralink_to_interface(ifname);
	if (!ifname)
		return -1;

	return wext_ops.txpower(ifname, buf);
}

static int ralink_get_signal(const char *ifname, int *buf)
{
	*buf = 0;

	if (ralink_is_device(ifname))
		return -1;

	int ret;
	char msgbuf[IWINFO_BUFSIZE];
	char *data;
	unsigned count;
	struct rt_iwinfo_signal_info *ent;

	ret = ralink_do_iwinfo_ioctl(ifname, RT_IWINFO_STATS, msgbuf, sizeof(msgbuf), &count, &data);
	if (ret)
		return -1;

	ent = (struct rt_iwinfo_signal_info *)data;

	*buf = ent->signal;

	return 0;
}

static int ralink_get_noise(const char *ifname, int *buf)
{
	*buf = 0;

	if (ralink_is_device(ifname))
		return -1;

	int ret;
	char msgbuf[IWINFO_BUFSIZE];
	char *data;
	unsigned count;
	struct rt_iwinfo_signal_info *ent;

	if (ralink_is_device(ifname))
		return -1;

	ret = ralink_do_iwinfo_ioctl(ifname, RT_IWINFO_STATS, msgbuf, sizeof(msgbuf), &count, &data);
	if (ret)
		return -1;

	ent = (struct rt_iwinfo_signal_info *)data;

	*buf = ent->noise;

	return 0;
}

static int ralink_get_quality(const char *ifname, int *buf)
{
	int signal;

	if (ralink_is_device(ifname))
		return -1;

	if (ralink_get_signal(ifname, &signal))
		return -1;

	/* A positive signal level is usually just a quality
		 * value, pass through as-is */
	if (signal >= 0) {
		*buf = signal;
	} else {
		if (signal < -120)
			signal = -120;
		else if (signal > -20)
			signal = -20;

		*buf = (signal + 120);
	}

	return 0;
}

static int ralink_get_quality_max(const char *ifname, int *buf)
{
	if (ralink_is_device(ifname))
		return -1;

	return wext_ops.quality_max(ifname, buf);
}

static void set_rate(struct iwinfo_rate_entry *rate, struct rt_iwinfo_rate *rt_rate)
{
	rate->rate	  = rt_rate->rate;
	rate->mcs	  = rt_rate->mcs;
	rate->is_ht	  = rt_rate->is_ht;
	rate->is_40mhz	  = rt_rate->is_40mhz;
	rate->is_short_gi = rt_rate->is_short_gi;
	rate->mhz	  = rt_rate->mhz;
}

static int ralink_get_assoclist(const char *ifname, char *buf, int *len)
{
	int ret;
	int ii;
	char msgbuf[IWINFO_BUFSIZE] = {0};
	char *data;
	unsigned count;
	struct rt_iwinfo_sta_entry *ent;
	struct iwinfo_assoclist_entry *a = (struct iwinfo_assoclist_entry *)buf;

	if (ralink_is_device(ifname))
		return -1;

	ret = ralink_do_iwinfo_ioctl(ifname, RT_IWINFO_ASSOCLIST, msgbuf, sizeof(msgbuf), &count, &data);

	if (ret)
		return -1;

	ent  = (struct rt_iwinfo_sta_entry *)data;
	*len = 0;

	for (ii = 0; ii < count; ++ii) {
		memset(&a[ii], 0, sizeof(struct iwinfo_assoclist_entry));
		memset(&a[ii].rx_rate, 0, sizeof(struct iwinfo_rate_entry));
		memset(&a[ii].tx_rate, 0, sizeof(struct iwinfo_rate_entry));
		memcpy(a[ii].mac, ent[ii].mac, 6);
		a[ii].inactive = ent[ii].inactive;
		a[ii].signal   = ent[ii].sig_noise.signal;
		a[ii].noise    = ent[ii].sig_noise.noise;
		set_rate(&a[ii].tx_rate, &ent[ii].tx_rate);
		set_rate(&a[ii].rx_rate, &ent[ii].rx_rate);
		a[ii].rx_bytes	 = ent[ii].rx_bytes;
		a[ii].tx_bytes	 = ent[ii].tx_bytes;
		a[ii].rx_packets = ent[ii].rx_packets;
		a[ii].tx_packets = ent[ii].tx_packets;
		*len += sizeof(struct iwinfo_assoclist_entry);
	}

	return 0;
}

static int ralink_fixed_txpwrlist(char *buf, int *len)
{
	const int txpwrs_mw[] = { 1, 12, 25, 39, 50, 63, 79, 100 };

	struct iwinfo_txpwrlist_entry *entries = buf;

	for (int i = 0; i < ARRAY_SIZE(txpwrs_mw); i++) {
		entries[i].mw  = txpwrs_mw[i];
		entries[i].dbm = iwinfo_mw2dbm(txpwrs_mw[i]);
		*len += sizeof(struct iwinfo_txpwrlist_entry);
	}
	return 0;
}

static int ralink_get_txpwrlist(const char *ifname, char *buf, int *len)
{
	ifname = ralink_to_interface(ifname);
	*len   = 0;

	if (!ifname)
		return ralink_fixed_txpwrlist(buf, len);

	return wext_ops.txpwrlist(ifname, buf, len);
}

static struct rt_iwinfo_freq_entry ralink_fixed_freqlist[] = {
	{ .mhz = 2412, .channel = 1 },
	{ .mhz = 2417, .channel = 2 },
	{ .mhz = 2422, .channel = 3 },
	{ .mhz = 2427, .channel = 4 },
	{ .mhz = 2432, .channel = 5 },
	{ .mhz = 2437, .channel = 6 },
	{ .mhz = 2442, .channel = 7 },
	{ .mhz = 2447, .channel = 8 },
	{ .mhz = 2452, .channel = 9 },
	{ .mhz = 2457, .channel = 10 },
	{ .mhz = 2462, .channel = 11 }
};

static int ralink_get_freqlist(const char *ifname, char *buf, int *len)
{
	int ret;
	int ii;
	char msgbuf[IWINFO_BUFSIZE];
	char *data;
	unsigned count;
	struct rt_iwinfo_freq_entry *ent;
	struct iwinfo_freqlist_entry *f = (struct iwinfo_freqlist_entry *)buf;

	ifname = ralink_to_interface(ifname);
	if (!ifname) {
		ent   = ralink_fixed_freqlist;
		count = ARRAY_SIZE(ralink_fixed_freqlist);
	} else {
		ret = ralink_do_iwinfo_ioctl(ifname, RT_IWINFO_FREQLIST, msgbuf, sizeof(msgbuf), &count,
					     &data);

		if (ret)
			return -1;

		ent = (struct rt_iwinfo_freq_entry *)data;
	}

	*len = 0;
	for (ii = 0; ii < count; ++ii) {
		f[ii].mhz	 = ent[ii].mhz;
		f[ii].channel	 = ent[ii].channel;
		f[ii].flags	 = 0;
		f[ii].restricted = 0;
		*len += sizeof(struct iwinfo_freqlist_entry);
	}

	return 0;
}

static int ralink_get_country(const char *ifname, char *buf)
{
	int ret;
	char msgbuf[IWINFO_BUFSIZE];
	char *data;
	unsigned count;
	struct rt_iwinfo_country_entry *ent;

	ifname = ralink_to_interface(ifname);
	if (!ifname)
		return -1;

	ret = ralink_do_iwinfo_ioctl(ifname, RT_IWINFO_COUNTRY, msgbuf, sizeof(msgbuf), &count, &data);

	if (ret)
		return -1;

	ent = (struct rt_iwinfo_country_entry *)data;
	sprintf(buf, "%c%c", ent->iso3166 / 256, ent->iso3166 % 256);

	return 0;
}

static int ralink_fixed_countrylist(char *buf, int *len)
{
	const char ccodes[][2] = { "AL", "DZ", "AR", "AM", "AU", "AT", "AZ", "BH", "BY", "BE", "BZ", "BO",
				   "BR", "BN", "BG", "CA", "CL", "CN", "CO", "CR", "HR", "CY", "CZ", "DK",
				   "DO", "EC", "EG", "SV", "EE", "FI", "FR", "GE", "DE", "GR", "GT", "HN",
				   "HK", "HU", "IS", "IN", "ID", "IR", "IE", "IL", "IT", "JP", "JO", "KZ",
				   "KP", "KR", "KW", "LV", "LB", "LI", "LT", "LU", "MO", "MK", "MY", "MX",
				   "MC", "MA", "NL", "NZ", "NO", "OM", "PK", "PA", "PE", "PH", "PL", "PT",
				   "PR", "QA", "RO", "RU", "SA", "SG", "SK", "SI", "ZA", "ES", "SE", "CH",
				   "SY", "TW", "TH", "TT", "TN", "TR", "UA", "AE", "GB", "US", "UY", "UZ",
				   "VE", "VN", "YE", "ZW", "00" };

	struct iwinfo_country_entry *entries = (struct iwinfo_country_entry *)buf;
	*len = 0;

	for (int i = 0; i < ARRAY_SIZE(ccodes); i++) {
		snprintf(entries[i].ccode, sizeof(entries[i].ccode), "%.2s", ccodes[i]);
		entries[i].iso3166 = (uint16_t)(ccodes[i][1]) | ((uint16_t)(ccodes[i][0]) << 8);
		*len += sizeof(struct iwinfo_country_entry);
	}

	return 0;
}

static int ralink_get_countrylist(const char *ifname, char *buf, int *len)
{
	int ret;
	int ii;
	int s;
	struct iwreq wrq;
	char msgbuf[IWINFO_BUFSIZE];
	char *data;
	unsigned count;
	struct rt_iwinfo_country_entry *ent;
	struct iwinfo_country_entry *c = (struct iwinfo_country_entry *)buf;
	struct ISO3166_to_CCode *e, *p;

	ifname = ralink_to_interface(ifname);
	if (!ifname)
		return ralink_fixed_countrylist(buf, len);

	ret = ralink_do_iwinfo_ioctl(ifname, RT_IWINFO_COUNTRY_LIST, msgbuf, sizeof(msgbuf), &count, &data);

	if (ret)
		return -1;

	ent = (struct rt_iwinfo_country_entry *)data;

	*len = 0;
	for (ii = 0; ii < count; ++ii) {
		c[ii].iso3166 = ent[ii].iso3166;
		snprintf(c[ii].ccode, sizeof(c[ii].ccode), "%.2s", ent[ii].ccode);
		*len += sizeof(struct iwinfo_country_entry);
	}

	return 0;
}

static int ralink_fixed_hwmodelist(int *buf) {
	*buf = 0;
	*buf |= IWINFO_80211_B;
	*buf |= IWINFO_80211_G;
	*buf |= IWINFO_80211_N;
	return 0;
}

static int ralink_get_hwmodelist(const char *ifname, int *buf)
{
	char chans[IWINFO_BUFSIZE]	= { 0 };
	struct iwinfo_freqlist_entry *e = NULL;
	int len				= 0;

	ifname = ralink_to_interface(ifname);
	if (!ifname)
		return ralink_fixed_hwmodelist(buf);

	*buf = 0;

	if (!ralink_get_freqlist(ifname, chans, &len)) {
		for (e = (struct iwinfo_freqlist_entry *)chans; e->channel; e++) {
			if (e->channel <= 14) {
				*buf |= IWINFO_80211_B;
				*buf |= IWINFO_80211_G;
				*buf |= IWINFO_80211_N;
			} else {
				*buf |= IWINFO_80211_A;
			}
		}

		return 0;
	}

	return -1;
}

static int ralink_get_htmode(const char *ifname, int *buf)
{
	int ret;
	char msgbuf[IWINFO_BUFSIZE];
	char *data;
	unsigned count;
	struct rt_iwinfo_htmode *ent;

	ifname = ralink_to_interface(ifname);
	if (!ifname)
		return -1;

	ret = ralink_do_iwinfo_ioctl(ifname, RT_IWINFO_HTMODE, msgbuf, sizeof(msgbuf), &count, &data);

	if (ret)
		return -1;

	ent = (struct rt_iwinfo_htmode *)data;

	switch (ent->rt_ht) {
	case RT_IWINFO_BAND_WIDTH_20:
		*buf |= IWINFO_HTMODE_HT20;
		break;
	case RT_IWINFO_BAND_WIDTH_40:
		*buf |= IWINFO_HTMODE_HT40;
		break;
	case RT_IWINFO_BAND_WIDTH_80:
		*buf |= IWINFO_HTMODE_HE80;
		break;
	case RT_IWINFO_BAND_WIDTH_160:
		*buf |= IWINFO_HTMODE_HE160;
		break;
	case RT_IWINFO_BAND_WIDTH_10:
	case RT_IWINFO_BAND_WIDTH_5:
		*buf |= IWINFO_HTMODE_NOHT;
		break;
	case RT_IWINFO_BAND_WIDTH_8080:
		*buf |= IWINFO_HTMODE_HE80_80;
		break;
	default:
		break;
	}

	return 0;
}

static int ralink_fixed_htmodelist(int *buf) {
	*buf = 0;
	*buf |= IWINFO_HTMODE_HT20;
	*buf |= IWINFO_HTMODE_HT40;
	return 0;
}

static int ralink_get_htmodelist(const char *ifname, int *buf)
{
	int ret;
	char msgbuf[IWINFO_BUFSIZE];
	char *data;
	unsigned count;
	struct rt_iwinfo_htmode *ent;

	ifname = ralink_to_interface(ifname);
	if (!ifname)
		return ralink_fixed_htmodelist(buf);

	ret = ralink_do_iwinfo_ioctl(ifname, RT_IWINFO_HTMODE_LIST, msgbuf, sizeof(msgbuf), &count, &data);
	if (ret)
		return -1;

	ent = (struct rt_iwinfo_htmode *)data;

	switch (ent->rt_ht) {
	case RT_IWINFO_BAND_WIDTH_20:
	case RT_IWINFO_BAND_WIDTH_40:
		*buf |= IWINFO_HTMODE_HT20;
		*buf |= IWINFO_HTMODE_HT40;
		break;
	case RT_IWINFO_BAND_WIDTH_80:
		*buf |= IWINFO_HTMODE_HE20;
		*buf |= IWINFO_HTMODE_HE40;
		*buf |= IWINFO_HTMODE_HE80;
		break;
	case RT_IWINFO_BAND_WIDTH_160:
		*buf |= IWINFO_HTMODE_HE20;
		*buf |= IWINFO_HTMODE_HE40;
		*buf |= IWINFO_HTMODE_HE80;
		*buf |= IWINFO_HTMODE_HE160;
		break;
	case RT_IWINFO_BAND_WIDTH_10:
	case RT_IWINFO_BAND_WIDTH_5:
		*buf |= IWINFO_HTMODE_NOHT;
		break;
	case RT_IWINFO_BAND_WIDTH_8080:
		*buf |= IWINFO_HTMODE_HE20;
		*buf |= IWINFO_HTMODE_HE40;
		*buf |= IWINFO_HTMODE_HE80;
		*buf |= IWINFO_HTMODE_HE80_80;
		*buf |= IWINFO_HTMODE_HE160;
		break;
	default:
		break;
	}

	return 0;
}

static int ralink_get_encryption(const char *ifname, char *buf)
{
	int ret;
	char msgbuf[IWINFO_BUFSIZE];
	char *data;
	unsigned count;
	struct rt_iwinfo_crypto_entry *ent;
	struct iwinfo_crypto_entry *c = (struct iwinfo_crypto_entry *)buf;

	if (ralink_is_device(ifname))
		return -1;

	ret = ralink_do_iwinfo_ioctl(ifname, RT_IWINFO_ENCRYPTION, msgbuf, sizeof(msgbuf), &count, &data);

	if (ret)
		return -1;

	ent = (struct rt_iwinfo_crypto_entry *)data;

	if (ent->wpa_version & RT_IWINFO_WPA_V1)
		c->wpa_version |= (1 << 0);
	if (ent->wpa_version & RT_IWINFO_WPA_V2)
		c->wpa_version |= (1 << 1);
	if (ent->wpa_version & RT_IWINFO_WPA_V3)
		c->wpa_version |= (1 << 2);

	ENC_COPY_FIELD(c->group_ciphers, ent->group_ciphers, CIPHER_NONE);
	ENC_COPY_FIELD(c->group_ciphers, ent->group_ciphers, CIPHER_WEP40);
	ENC_COPY_FIELD(c->group_ciphers, ent->group_ciphers, CIPHER_TKIP);
	ENC_COPY_FIELD(c->group_ciphers, ent->group_ciphers, CIPHER_CCMP);
	ENC_COPY_FIELD(c->group_ciphers, ent->group_ciphers, CIPHER_WEP104);

	ENC_COPY_FIELD(c->pair_ciphers, ent->pair_ciphers, CIPHER_NONE);
	ENC_COPY_FIELD(c->pair_ciphers, ent->pair_ciphers, CIPHER_WEP40);
	ENC_COPY_FIELD(c->pair_ciphers, ent->pair_ciphers, CIPHER_TKIP);
	ENC_COPY_FIELD(c->pair_ciphers, ent->pair_ciphers, CIPHER_CCMP);
	ENC_COPY_FIELD(c->pair_ciphers, ent->pair_ciphers, CIPHER_WEP104);

	ENC_COPY_FIELD(c->auth_suites, ent->auth_suites, KMGMT_NONE);
	ENC_COPY_FIELD(c->auth_suites, ent->auth_suites, KMGMT_PSK);
	ENC_COPY_FIELD(c->auth_suites, ent->auth_suites, KMGMT_SAE);
	ENC_COPY_FIELD(c->auth_suites, ent->auth_suites, KMGMT_OWE);

	ENC_COPY_FIELD(c->auth_algs, ent->auth_algs, AUTH_OPEN);
	ENC_COPY_FIELD(c->auth_algs, ent->auth_algs, AUTH_SHARED);

	c->enabled = (c->auth_algs && c->pair_ciphers) || c->wpa_version;

	return 0;
}

static int ralink_get_phyname(const char *ifname, char *buf)
{
	/* No suitable api in wext */
	strcpy(buf, ifname);
	return 0;
}

static int ralink_get_mbssid_support(const char *ifname, int *buf)
{
	return -1;
}

static int ralink_get_hardware_id(const char *ifname, char *buf)
{
	// TODO
	struct iwinfo_hardware_id *id = (struct iwinfo_hardware_id *)buf;
	return iwinfo_hardware_id_from_mtd(id);
}

static const struct iwinfo_hardware_entry *ralink_get_hardware_entry(const char *ifname)
{
	struct iwinfo_hardware_id id;

	if (ralink_get_hardware_id(ifname, (char *)&id))
		return NULL;

	return iwinfo_hardware(&id);
}

static int ralink_get_hardware_name(const char *ifname, char *buf)
{
	sprintf(buf, "Ralink");
	return 0;
}

static int ralink_get_txpower_offset(const char *ifname, int *buf)
{
	const struct iwinfo_hardware_entry *hw;

	ifname = ralink_to_interface(ifname);
	if (!ifname)
		return -1;

	if (!(hw = ralink_get_hardware_entry(ifname)))
		return -1;

	*buf = hw->frequency_offset;
	return 0;
}

static int ralink_get_frequency_offset(const char *ifname, int *buf)
{
	const struct iwinfo_hardware_entry *hw;

	ifname = ralink_to_interface(ifname);
	if (!ifname)
		return -1;

	if (!(hw = ralink_get_hardware_entry(ifname)))
		return -1;

	*buf = hw->txpower_offset;
	return 0;
}

static void next_field(char **line, char *output, int n)
{
	char *l = *line;
	int i;

	memcpy(output, *line, n);
	*line = &l[n];

	for (i = n - 1; i > 0; i--) {
		if (output[i] != ' ')
			break;
		output[i] = '\0';
	}
}

static void wifi_site_survey(const char *ifname, char *essid, int print)
{
	char *s = malloc(IW_SCAN_MAX_DATA);
	int ret;
	int socket_id;
	struct iwreq wrq;
	char *line, *start;
	ralink_ioctl_set(ifname, "SiteSurvey", (essid ? essid : ""));
	sleep(5);
	memset(s, 0x00, IW_SCAN_MAX_DATA);
	strcpy(wrq.ifr_name, ifname);
	wrq.u.data.length  = IW_SCAN_MAX_DATA;
	wrq.u.data.pointer = s;
	wrq.u.data.flags   = 0;
	socket_id	   = socket(AF_INET, SOCK_DGRAM, 0);
	ret		   = ioctl(socket_id, RTPRIV_IOCTL_GSITESURVEY, &wrq);
	close(socket_id);
	if (ret != 0)
		goto out;
	if (wrq.u.data.length < 1)
		goto out;
	/* ioctl result starts with a newline, for some reason */
	start = s;
	while (*start == '\n')
		start++;
	line	     = strtok((char *)start, "\n");
	line	     = strtok(NULL, "\n");
	survey_count = 0;
	while (line && (survey_count < MAX_SURVEY_CNT)) {
		next_field(&line, st[survey_count].channel, sizeof(st->channel));
		next_field(&line, st[survey_count].ssid, sizeof(st->ssid));
		next_field(&line, st[survey_count].bssid, sizeof(st->bssid));
		next_field(&line, st[survey_count].security, sizeof(st->security));
		next_field(&line, st[survey_count].signal, sizeof(st->signal));
		line			= strtok(NULL, "\n");
		st[survey_count].crypto = strstr(st[survey_count].security, "/");
		if (st[survey_count].crypto) {
			*st[survey_count].crypto = '\0';
			st[survey_count].crypto++;
			if (print)
				printf("%s|%s|%s|%s\n", st[survey_count].channel, st[survey_count].ssid,
				       st[survey_count].bssid, st[survey_count].security);
		}
		survey_count++;
	}
	if (survey_count == 0 && !print)
		printf("No results");
out:
	free(s);
}

static int ralink_get_scanlist(const char *ifname, char *buf, int *len)
{
	struct iwinfo_scanlist_entry *e = (struct iwinfo_scanlist_entry *)buf;
	int i				= 0;

	ifname = ralink_to_interface(ifname);
	if (!ifname) {
		if (ralink_probe("apcli0"))
			ifname = "apcli0";
		else
			return -1;
	}

	survey_count = 0;
	*len         = 0;

	wifi_site_survey(ifname, NULL, 0);

	for (i = 0; i < survey_count; i++) {
		int j;
		for (j = 0; j < 6; j++) {
			e[i].mac[j] = (uint8_t)strtoul(&st[i].bssid[j * 3], NULL, 16);
		}
		strcpy(e[i].ssid, st[i].ssid);
		e[i].channel	 = atoi(st[i].channel);
		e[i].mode	 = IWINFO_OPMODE_MASTER;
		e[i].quality	 = atoi(st[i].signal);
		e[i].quality_max = 100;
		memset(&e[i].crypto, 0, sizeof(struct iwinfo_crypto_entry));
		memset(&e[i].ht_chan_info, 0, sizeof(struct iwinfo_scanlist_ht_chan_entry));
		memset(&e[i].vht_chan_info, 0, sizeof(struct iwinfo_scanlist_vht_chan_entry));
		if (strstr(st[i].security, "WPA")) {
			e[i].crypto.enabled = 1;
			e[i].crypto.auth_suites |= IWINFO_KMGMT_PSK;
		}
		if (!st[i].crypto)
			continue;
		if (strstr(st[i].crypto, "TKIP"))
			e[i].crypto.group_ciphers |= IWINFO_CIPHER_TKIP;
		if (strstr(st[i].crypto, "AES"))
			e[i].crypto.group_ciphers |= IWINFO_CIPHER_CCMP;
		if (strstr(st[i].security, "WPA3"))
			e[i].crypto.wpa_version = 3;
		else if (strstr(st[i].security, "WPA2"))
			e[i].crypto.wpa_version = 2;
		else if (strstr(st[i].security, "WPA"))
			e[i].crypto.wpa_version = 1;
	}
	*len = survey_count * sizeof(struct iwinfo_scanlist_entry);

	return 0;
}

static int ralink_get_survey(const char *ifname, char *buf, int *len)
{
	(void)buf, (void)ifname;
	*len = 0;

	return -1;
}

const struct iwinfo_ops ralink_ops = {
	.name		  = "ralink",
	.probe		  = ralink_probe,
	.channel	  = ralink_get_channel,
	.center_chan1	  = ralink_get_center_chan1,
	.center_chan2	  = ralink_get_center_chan2,
	.frequency	  = ralink_get_frequency,
	.frequency_offset = ralink_get_frequency_offset,
	.txpower	  = ralink_get_txpower,
	.txpower_offset	  = ralink_get_txpower_offset,
	.bitrate	  = ralink_get_bitrate,
	.signal		  = ralink_get_signal,
	.noise		  = ralink_get_noise,
	.quality	  = ralink_get_quality,
	.quality_max	  = ralink_get_quality_max,
	.mbssid_support	  = ralink_get_mbssid_support,
	.hwmodelist	  = ralink_get_hwmodelist,
	.htmodelist	  = ralink_get_htmodelist,
	.htmode		  = ralink_get_htmode,
	.mode		  = ralink_get_mode,
	.ssid		  = ralink_get_ssid,
	.bssid		  = ralink_get_bssid,
	.country	  = ralink_get_country,
	.hardware_id	  = ralink_get_hardware_id,
	.hardware_name	  = ralink_get_hardware_name,
	.encryption	  = ralink_get_encryption,
	.phyname	  = ralink_get_phyname,
	.assoclist	  = ralink_get_assoclist,
	.txpwrlist	  = ralink_get_txpwrlist,
	.scanlist	  = ralink_get_scanlist,
	.freqlist	  = ralink_get_freqlist,
	.countrylist	  = ralink_get_countrylist,
	.survey 	  = ralink_get_survey,
	.close		  = ralink_close,
};
