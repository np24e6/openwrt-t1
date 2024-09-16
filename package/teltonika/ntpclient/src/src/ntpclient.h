#ifndef NTPCLIENT_H
#define NTPCLIENT_H

#ifndef NTP_MAX_SERVERS
#define NTP_MAX_SERVERS (1)
#endif

#include <libubus.h>
#include <uci.h>

#ifdef MOBILE_SUPPORT
#include <libgsm.h>
#endif

struct ntptime {
	unsigned int coarse;
	unsigned int fine;
};

#ifdef MOBILE_SUPPORT
struct modem_sync {
	struct ubus_context *ubus_ctx;
	struct uci_context *uci_ctx;
	int time_sync_enabled;
	int timezone_sync_enabled;
	lgsm_time_t *modem_time;
};
#endif

struct ntp_control {
	short int udp_local_port;
	int probes_sent;
	int live;
	int set_clock; /* non-zero presumably needs root privs */
	int probe_count;
	int failover_cnt;
	int cycle_time;
	int retry_interval;
	int goodness;
	int cross_check;
	int hosts;
	int save_time;
#ifdef MOBILE_SUPPORT
	struct modem_sync modem_sync;
#endif
	uint32_t time_of_send[2];
	char *hostnames[NTP_MAX_SERVERS];
	char *hotplug_script;
};

/* global tuning parameter */
extern double min_delay;

#endif
