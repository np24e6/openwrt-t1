/*
 * ntpclient.c - NTP client
 *
 * Copyright (C) 1997, 1999, 2000, 2003, 2006, 2007, 2010, 2015  Larry Doolittle  <larry@doolittle.boa.org>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License (Version 2,
 *  June 1991) as published by the Free Software Foundation.  At the
 *  time of writing, that license was published by the FSF with the URL
 *  http://www.gnu.org/copyleft/gpl.html, and is incorporated herein by
 *  reference.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  Possible future improvements:
 *      - Write more documentation  :-(
 *      - Support leap second processing
 *      - Support IPv6
 *      - Support multiple (interleaved) servers
 *
 *  Compile with -DPRECISION_SIOCGSTAMP if your machine really has it.
 *  Older kernels (before the tickless era, pre 3.0?) only give an answer
 *  to the nearest jiffy (1/100 second), not so interesting for us.
 *
 *  If the compile gives you any flak, check below in the section
 *  labelled "XXX fixme - non-automatic build configuration".
 */

#include "phaselock.h"
#include "ntpclient.h"
#include "net.h"
#include "debug.h"
#include "log.h"

#include <libubus.h>
#include <uci.h>

#include <math.h> // trunc()
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> /* gethostbyname */
#include <arpa/inet.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <utime.h>

#ifdef MOBILE_SUPPORT
#include <libgsm.h>
#endif

#ifdef PRECISION_SIOCGSTAMP
#include <sys/ioctl.h>
#ifdef __GLIBC__
#include <linux/sockios.h>
#endif
#endif
#ifdef USE_OBSOLETE_GETTIMEOFDAY
#include <sys/time.h>
#endif

/* Default to the RFC-4330 specified value */
#ifndef MIN_INTERVAL
#define MIN_INTERVAL (60)
#endif

#define RETRY_INTERVAL (60)

#ifdef ENABLE_DEBUG
#define DEBUG_OPTION "d"
int g_debug = 0;
#else
#define DEBUG_OPTION
#endif

#ifdef ENABLE_REPLAY
#define REPLAY_OPTION "r"
#else
#define REPLAY_OPTION
#endif

#include <stdint.h>

/* XXX fixme - non-automatic build configuration */
#ifdef __linux__
#include <sys/utsname.h>
#include <sys/time.h>
#include <sys/timex.h>
#include <netdb.h>
#else
extern struct hostent *gethostbyname(const char *name);
extern int h_errno;
#define herror(hostname) ERR("Error %d looking up hostname %s", h_errno, hostname)
#endif
/* end configuration for host systems */

#define JAN_1970 0x83aa7e80 /* 2208988800 1970 - 1900 in seconds */
#define NTP_PORT (123)

/* How to multiply by 4294.967296 quickly (and not quite exactly)
 * without using floating point or greater than 32-bit integers.
 * If you want to fix the last 12 microseconds of error, add in
 * (2911*(x))>>28)
 */
#define NTPFRAC(x) (4294 * (x) + ((1981 * (x)) >> 11))

/* The reverse of the above, needed if we want to set our microsecond
 * clock (via clock_settime) based on the incoming time in NTP format.
 * Basically exact.
 */
#define USEC(x) (((x) >> 12) - 759 * ((((x) >> 10) + 32768) >> 16))

/* Converts NTP delay and dispersion, apparently in seconds scaled
 * by 65536, to microseconds.  RFC-1305 states this time is in seconds,
 * doesn't mention the scaling.
 * Should somehow be the same as 1000000 * x / 65536
 */
#define sec2u(x) ((x)*15.2587890625)

#define SYSFIXTIME_PATH "/etc/init.d/sysfixtime"

#ifdef MOBILE_SUPPORT
#define OPTION_PATH_NTPCLIENT	"ntpclient.@ntpclient[0].zoneName"
#define OPTION_PATH_SYSTEM_TMZ	"system.system.timezone"
#define OPTION_PATH_SYSTEM	"system.system.zoneName"
#define TIMEZONE_FILE_PATH	"/tmp/TZ"
#define TIMEZONE_STR_LEN	(16)
#define ETC_ZONENAME		"Etc/GMT"
#endif

static int get_current_freq(void)
{
	/* OS dependent routine to get the current value of clock frequency.
	 */
#ifdef __linux__
	struct timex txc;
	txc.modes = 0;
	if (adjtimex(&txc) < 0) {
		perror("adjtimex");
		exit(1);
	}
	return txc.freq;
#else
	return 0;
#endif
}

static int set_freq(int new_freq)
{
	/* OS dependent routine to set a new value of clock frequency.
	 */
#ifdef __linux__
	struct timex txc;
	txc.modes = ADJ_FREQUENCY;
	txc.freq  = new_freq;
	if (adjtimex(&txc) < 0) {
		perror("adjtimex");
		exit(1);
	}
	return txc.freq;
#else
	return 0;
#endif
}

static void run_script(char *hotplug_script)
{
	pid_t pid;
	char *envp[] = { "ACTION=stratum", NULL };

	if (!hotplug_script) {
		DBG("No hotplug script provided");
		return;
	}

	if (access(hotplug_script, F_OK)) {
		DBG("Cannot find hotplug script %s", hotplug_script);
		return;
	}

	pid = fork();

	if (pid == 0) {
		execvpe(hotplug_script, NULL, envp);
		exit(EXIT_SUCCESS);
	} else if (pid == -1) {
		DBG("Hotplug script fork failed");
	}
}

static void set_time(struct ntptime *new, struct ntp_control *ntpc)
{
#ifndef USE_OBSOLETE_GETTIMEOFDAY
	/* POSIX 1003.1-2001 way to set the system clock
	 */
	struct timespec tv_set;
	/* it would be even better to subtract half the slop */
	tv_set.tv_sec = new->coarse - JAN_1970;
	/* divide xmttime.fine by 4294.967296 */
	tv_set.tv_nsec = USEC(new->fine) * 1000;
	if (clock_settime(CLOCK_REALTIME, &tv_set) < 0) {
		perror("clock_settime");
		exit(1);
	}
	/* Save time in case correct power-off sequence is skipped */
	if (ntpc->save_time == 1) {
		utime(SYSFIXTIME_PATH, NULL);
	}
	run_script(ntpc->hotplug_script);
	DBG("set time to %" PRId64 ".%.9" PRId64 "", (int64_t)tv_set.tv_sec, (int64_t)tv_set.tv_nsec);
#else
	/* Traditional Linux way to set the system clock
	 */
	struct timeval tv_set;
	/* it would be even better to subtract half the slop */
	tv_set.tv_sec = new->coarse - JAN_1970;
	/* divide xmttime.fine by 4294.967296 */
	tv_set.tv_usec = USEC(new->fine);
	if (settimeofday(&tv_set, NULL) < 0) {
		perror("settimeofday");
		exit(1);
	}
	/* Save time in case correct power-off sequence is skipped */
	if (ntpc->save_time == 1) {
		utime(SYSFIXTIME_PATH, NULL);
	}
	run_script(ntpc->hotplug_script);
	DBG("set time to %" PRId64 ".%.6" PRId64 "", (int64_t)tv_set.tv_sec, (int64_t)tv_set.tv_usec);
#endif
}

static void ntpc_gettime(uint32_t *time_coarse, uint32_t *time_fine)
{
#ifndef USE_OBSOLETE_GETTIMEOFDAY
	/* POSIX 1003.1-2001 way to get the system time
	 */
	struct timespec now;
	clock_gettime(CLOCK_REALTIME, &now);
	*time_coarse = now.tv_sec + JAN_1970;
	*time_fine   = NTPFRAC(now.tv_nsec / 1000);
#else
	/* Traditional Linux way to get the system time
	 */
	struct timeval now;
	gettimeofday(&now, NULL);
	*time_coarse = now.tv_sec + JAN_1970;
	*time_fine   = NTPFRAC(now.tv_usec);
#endif
}

static void get_packet_timestamp(int usd, struct ntptime *udp_arrival_ntp)
{
#ifdef PRECISION_SIOCGSTAMP
	struct timeval udp_arrival;
	if (ioctl(usd, SIOCGSTAMP, &udp_arrival) < 0) {
		perror("ioctl-SIOCGSTAMP");
		ntpc_gettime(&udp_arrival_ntp->coarse, &udp_arrival_ntp->fine);
	} else {
		udp_arrival_ntp->coarse = udp_arrival.tv_sec + JAN_1970;
		udp_arrival_ntp->fine	= NTPFRAC(udp_arrival.tv_usec);
	}
#else
	(void)usd; /* not used */
	ntpc_gettime(&udp_arrival_ntp->coarse, &udp_arrival_ntp->fine);
#endif
}

static int check_source(int data_len, struct sockaddr_in *sa_in, unsigned int sa_len,
			struct ntp_control *ntpc)
{
	struct sockaddr *sa_source = (struct sockaddr *)sa_in;
	(void)sa_len; /* not used */
	DBG("packet of length %d received", data_len);
	if (sa_source->sa_family == AF_INET) {
		DBG("Source: INET Port %d host %s", ntohs(sa_in->sin_port), inet_ntoa(sa_in->sin_addr));
	} else {
		DBG("Source: Address family %d", sa_source->sa_family);
	}
	/* we could check that the source is the server we expect, but
	 * Denys Vlasenko recommends against it: multihomed hosts get it
	 * wrong too often. */
#if 0
	if (memcmp(ntpc->serv_addr, &(sa_in->sin_addr), 4)!=0) {
		return 1;  /* fault */
	}
#else
	(void)ntpc; /* not used */
#endif
	if (NTP_PORT != ntohs(sa_in->sin_port)) {
		return 1; /* fault */
	}
	return 0;
}

static double ntpdiff(struct ntptime *start, struct ntptime *stop)
{
	int a;
	unsigned int b;
	a = stop->coarse - start->coarse;
	if (stop->fine >= start->fine) {
		b = stop->fine - start->fine;
	} else {
		b = start->fine - stop->fine;
		b = ~b;
		a -= 1;
	}

	return a * 1.e6 + b * (1.e6 / 4294967296.0);
}

/* Does more than print, so this name is bogus.
 * It also makes time adjustments, both sudden (-s)
 * and phase-locking (-l).
 * sets *error to the number of microseconds uncertainty in answer
 * returns 0 normally, 1 if the message fails sanity checks
 */
static int rfc1305print(uint32_t *data, struct ntptime *arrival, struct ntp_control *ntpc, int *error)
{
	/* straight out of RFC-1305 Appendix A */
	int li, vn, mode, stratum, poll, prec;
	int delay, disp, refid;
	struct ntptime reftime, orgtime, rectime, xmttime;
	double el_time, st_time, skew1, skew2;
	int freq;
#ifdef ENABLE_DEBUG
	const char *drop_reason = NULL;
#endif

#define Data(i) ntohl(((uint32_t *)data)[i])
	li	= Data(0) >> 30 & 0x03;
	vn	= Data(0) >> 27 & 0x07;
	mode	= Data(0) >> 24 & 0x07;
	stratum = Data(0) >> 16 & 0xff;
	poll	= Data(0) >> 8 & 0xff;
	prec	= Data(0) & 0xff;
	if (prec & 0x80)
		prec |= 0xffffff00;
	delay	       = Data(1);
	disp	       = Data(2);
	refid	       = Data(3);
	reftime.coarse = Data(4);
	reftime.fine   = Data(5);
	orgtime.coarse = Data(6);
	orgtime.fine   = Data(7);
	rectime.coarse = Data(8);
	rectime.fine   = Data(9);
	xmttime.coarse = Data(10);
	xmttime.fine   = Data(11);
#undef Data

	DBG("LI=%d  VN=%d  Mode=%d  Stratum=%d  Poll=%d  Precision=%d", li, vn, mode, stratum, poll, prec);
	DBG("Delay=%.1f  Dispersion=%.1f  Refid=%u.%u.%u.%u", sec2u(delay), sec2u(disp), refid >> 24 & 0xff,
	    refid >> 16 & 0xff, refid >> 8 & 0xff, refid & 0xff);
	DBG("Reference %u.%.6u", reftime.coarse, USEC(reftime.fine));
	DBG("(sent)    %u.%.6u", ntpc->time_of_send[0], USEC(ntpc->time_of_send[1]));
	DBG("Originate %u.%.6u", orgtime.coarse, USEC(orgtime.fine));
	DBG("Receive   %u.%.6u", rectime.coarse, USEC(rectime.fine));
	DBG("Transmit  %u.%.6u", xmttime.coarse, USEC(xmttime.fine));
	DBG("Our recv  %u.%.6u", arrival->coarse, USEC(arrival->fine));

	el_time = ntpdiff(&orgtime, arrival); /* elapsed */
	st_time = ntpdiff(&rectime, &xmttime); /* stall */
	skew1	= ntpdiff(&orgtime, &rectime);
	skew2	= ntpdiff(&xmttime, arrival);
	freq	= get_current_freq();
	DBG("Total elapsed: %9.2f\n"
	    "Server stall:  %9.2f\n"
	    "Slop:          %9.2f",
	    el_time, st_time, el_time - st_time);
	DBG("Skew:          %9.2f\n"
	    "Frequency:     %9d\n"
	    " day   second     elapsed    stall     skew  dispersion  freq",
	    (skew1 - skew2) / 2, freq);

	/* error checking, see RFC-4330 section 5 */
#ifdef ENABLE_DEBUG
#define FAIL(x)                                                                                              \
	do {                                                                                                 \
		drop_reason = (x);                                                                           \
		goto fail;                                                                                   \
	} while (0)
#else
#define FAIL(x) goto fail;
#endif
	if (ntpc->cross_check) {
		if (li == 3)
			FAIL("LI==3"); /* unsynchronized */
		if (vn < 3)
			FAIL("VN<3"); /* RFC-4330 documents SNTP v4, but we interoperate with NTP v3 */
		if (mode != 4)
			FAIL("MODE!=3");
		if (orgtime.coarse != ntpc->time_of_send[0] || orgtime.fine != ntpc->time_of_send[1])
			FAIL("ORG!=sent");
		if (xmttime.coarse == 0 && xmttime.fine == 0)
			FAIL("XMT==0");
		if (delay > 65536 || delay < -65536)
			FAIL("abs(DELAY)>65536");
		if (disp > 65536 || disp < -65536)
			FAIL("abs(DISP)>65536");
		if (stratum == 0)
			FAIL("STRATUM==0"); /* kiss o' death */
#undef FAIL
	}

	/* XXX should I do this if debug flag is set? */
	if (ntpc->set_clock) { /* you'd better be root, or ntpclient will exit here! */
		set_time(&xmttime, ntpc);
	}

	/* Not the ideal order for printing, but we want to be sure
	 * to do all the time-sensitive thinking (and time setting)
	 * before we start the output, especially fflush() (which
	 * could be slow).  Of course, if debug is turned on, speed
	 * has gone down the drain anyway. */
	if (ntpc->live) {
		int new_freq;
		new_freq =
			contemplate_data(arrival->coarse, (skew1 - skew2) / 2, el_time + sec2u(disp), freq);
		if (!g_debug && new_freq != freq)
			set_freq(new_freq);
	}
	LOG("%d %.5d.%.3d  %8.1f %8.1f  %8.1f %8.1f %9d", arrival->coarse / 86400, arrival->coarse % 86400,
	    arrival->fine / 4294967, el_time, st_time, (skew1 - skew2) / 2, sec2u(disp), freq);
	*error = el_time - st_time;

	return 0;
fail:
#ifdef ENABLE_DEBUG
	LOG("%d %.5d.%.3d  rejected packet: %s", arrival->coarse / 86400, arrival->coarse % 86400,
	    arrival->fine / 4294967, drop_reason);
#else
	LOG("%d %.5d.%.3d  rejected packet", arrival->coarse / 86400, arrival->coarse % 86400,
	    arrival->fine / 4294967);
#endif
	return 1;
}

static void send_packet(int usd, uint32_t time_sent[2])
{
	uint32_t data[12];
#define LI	0
#define VN	3
#define MODE	3
#define STRATUM 0
#define POLL	4
#define PREC	-6

	DBG("Sending ...");
	if (sizeof data != 48) {
		ERR("size error");
		return;
	}
	memset(data, 0, sizeof data);
	data[0] =
		htonl((LI << 30) | (VN << 27) | (MODE << 24) | (STRATUM << 16) | (POLL << 8) | (PREC & 0xff));
	data[1] = htonl(1 << 16); /* Root Delay (seconds) */
	data[2] = htonl(1 << 16); /* Root Dispersion (seconds) */
	ntpc_gettime(time_sent, time_sent + 1);
	data[10] = htonl(time_sent[0]); /* Transmit Timestamp coarse */
	data[11] = htonl(time_sent[1]); /* Transmit Timestamp fine   */
	send(usd, data, 48, 0);
}

static int find_server(struct ntp_control *ntpc)
{
	int socket = -1;

	for (int i = 0; i < ntpc->hosts; i++) {
		socket = net_setup(ntpc->hostnames[i], NTP_PORT, ntpc->udp_local_port);
		if (socket > -1) {
			LOG("Working with server %s", ntpc->hostnames[i]);
			break;
		}
	}
	return socket;
}

static void ntpc_sleep(time_t seconds)
{
	struct timeval tv;
	int i;
	tv.tv_sec  = seconds;
	tv.tv_usec = 0;
	do {
		i = select(1, NULL, NULL, NULL, &tv);
	} while (i == -1 && errno == EINTR);
}

#ifdef MOBILE_SUPPORT
static int format_timezone(int tmz, char *timezone, char *zonename)
{
	// 'double' type to have a negative 0 when offset is only negative minutes (e.g. -0:45)
	double gmt_offset_hours = trunc((double)tmz / 4);
	int gmt_offset_minutes = abs(tmz % 4 * 15);

	if (sprintf(timezone, "<%+.0f%02d>%.0f:%02d", gmt_offset_hours, gmt_offset_minutes,
		    -1 * gmt_offset_hours, gmt_offset_minutes) < 0) {
		ERR("Could not format GMT timezone");
		return 1;
	}

	if (gmt_offset_minutes != 0) {
		// There are no generic zonenames for timezones with 15 minute increments, so set it to exact offset value
		if (sprintf(zonename, "%+.0f:%02d", gmt_offset_hours, gmt_offset_minutes) < 0) {
			goto err;
		}
	} else if (gmt_offset_hours == 0) {
		strcpy(zonename, ETC_ZONENAME);
	} else if (sprintf(zonename, ETC_ZONENAME "%+.0f", -1 * gmt_offset_hours) < 0) {
		goto err;
	}

	return 0;

err:
	ERR("Could not format zonename");
	return 2;
}

static void uci_set_value(struct uci_context *uci_ctx, const char *option_path, const char *value)
{
	int ret;
	struct uci_ptr ptr = { 0 };

	char *path = strdup(option_path);
	if (!path) {
		ERR("Out of memory");
		return;
	}

	ret = uci_lookup_ptr(uci_ctx, &ptr, path, strchr(option_path, '@') != NULL);
	if (ret != UCI_OK) {
		ERR("Failed to lookup uci pointer for '%s'; ret=%d", option_path, ret);
		goto out;
	}

	ptr.value = value;

	ret = uci_set(uci_ctx, &ptr);
	if (ret != UCI_OK) {
		ERR("Failed to set '%s=%s'; ret=%d", option_path, value, ret);
		goto out;
	}

	ret = uci_commit(uci_ctx, &ptr.p, false);
	if (ret != UCI_OK) {
		ERR("Failed to commit uci changes for '%s'; ret=%d", option_path, ret);
		goto out;
	}

out:
	free(path);
}

static void set_timezone_from_modem(struct modem_sync *modem_data)
{
	char *timezone_str = calloc(TIMEZONE_STR_LEN, sizeof(char));
	char *zonename_str = calloc(TIMEZONE_STR_LEN, sizeof(char));

	if (!timezone_str || !zonename_str) {
		ERR("Failed to allocate buffer");
		goto out;
	}

	if (format_timezone(modem_data->modem_time->time_zone, timezone_str, zonename_str) ||
	    !timezone_str[0]) {
		ERR("Failed to parse timezone value=%d", modem_data->modem_time->time_zone);
		goto out;
	}

	DBG("Setting timezone to |%s|", timezone_str);
	FILE *fp = fopen(TIMEZONE_FILE_PATH, "w");
	if (!fp) {
		ERR("Could not open file %s", TIMEZONE_FILE_PATH);
		goto out;
	}
	fprintf(fp, "%s\n", timezone_str);
	fclose(fp);

	if (!modem_data->uci_ctx && !(modem_data->uci_ctx = uci_alloc_context())) {
		ERR("Failed to initialize the uci");
		goto out;
	}

	DBG("Setting zonename to |%s|", zonename_str);
	uci_set_value(modem_data->uci_ctx, OPTION_PATH_NTPCLIENT, zonename_str);
	uci_set_value(modem_data->uci_ctx, OPTION_PATH_SYSTEM, zonename_str);
	uci_set_value(modem_data->uci_ctx, OPTION_PATH_SYSTEM_TMZ, timezone_str);

out:
	free(timezone_str);
	free(zonename_str);
}

static void set_datetime_from_modem(struct ntp_control *ntpc)
{
	struct tm modem_time_tm = { 0 };

	DBG("Time string from modem: %s", ntpc->modem_sync.modem_time->time);
	strptime(ntpc->modem_sync.modem_time->time, "%y/%m/%d,%H:%M:%S", &modem_time_tm);

	// Convert GMT to local time
	modem_time_tm.tm_hour += ntpc->modem_sync.modem_time->time_zone / 4;
	modem_time_tm.tm_min += ntpc->modem_sync.modem_time->time_zone % 4 * 15;

	DBG("Parsed local time: %.24s", asctime(&modem_time_tm)); // .24 to not print '\n' at the end

	struct ntptime ntp_time = {
		// Add JAN_1970 to make compatible with set_time()
		.coarse = mktime(&modem_time_tm) + JAN_1970,
		.fine	= 0,
	};
	DBG("coarse=%d fine=%d", ntp_time.coarse, ntp_time.fine);

	set_time(&ntp_time, ntpc);
}

static void get_time_from_modem(struct ubus_context *ubus_ctx, lgsm_time_t **modem_time)
{
	lgsm_structed_info_t rsp = { 0 };
	lgsm_err_t ret		 = LGSM_SUCCESS;

	for (uint32_t i = 0; i < 12; i++) {
		ret = lgsm_get_time(ubus_ctx, TIME_MODE_GMT, i, &rsp);
		if (ret == LGSM_SUCCESS && rsp.label != LGSM_LABEL_ERROR) {
			break;
		}
	}

	if (ret != LGSM_SUCCESS || rsp.label == LGSM_LABEL_ERROR) {
		ERR("Failed to get proper datetime from operator station: %s", lgsm_strerror(ret));
		goto end;
	}

	if (!*modem_time && !(*modem_time = calloc(1, sizeof(lgsm_time_t)))) {
		ERR("Couldn't allocate memory");
		goto end;
	}

	memcpy(*modem_time, &rsp.data.time, sizeof(lgsm_time_t));

end:
	handle_gsm_structed_info_free(&rsp);
}
#endif // MOBILE_SUPPORT

static int ntpc_handle_timeout(struct ntp_control *ntpc)
{
	// Don't sync from mobile if count of syncs is specified
	if (ntpc->probe_count) {
		return ntpc->probes_sent >= ntpc->probe_count;
	}

#ifdef MOBILE_SUPPORT
	if (!ntpc->modem_sync.modem_time || !ntpc->modem_sync.time_sync_enabled ||
	    ntpc->probes_sent < ntpc->failover_cnt) {
		return 0;
	}

	DBG("Will sync time from mobile operator");
	set_datetime_from_modem(ntpc);
#endif

	return 0;
}

static void primary_loop(struct ntp_control *ntpc)
{
	fd_set fds;
	struct sockaddr_in sa_xmit_in;
	int i, pack_len, error;
	socklen_t sa_xmit_len;
	struct timeval to;
	struct ntptime udp_arrival_ntp;
	static uint32_t incoming_word[325];
#define incoming	((char *)incoming_word)
#define sizeof_incoming (sizeof incoming_word)

	ntpc->probes_sent = 0;
	sa_xmit_len	  = sizeof sa_xmit_in;
	to.tv_sec	  = 0;
	to.tv_usec	  = 0;

	int usd	       = -1;
	int sleep_time = ntpc->retry_interval;

	for (;;) {
#ifdef MOBILE_SUPPORT
		if (ntpc->modem_sync.time_sync_enabled || ntpc->modem_sync.timezone_sync_enabled) {
			get_time_from_modem(ntpc->modem_sync.ubus_ctx, &ntpc->modem_sync.modem_time);
			DBG("Datetime received from GSM modem: |%s|", ntpc->modem_sync.modem_time->time);
		}

		DBG("sleep_time = %d", sleep_time);
		DBG("cycle_time = %d", ntpc->cycle_time);
		DBG("retry_interval = %d", ntpc->retry_interval);
		DBG("Finding server...");
#endif
		usd = find_server(ntpc);
		if (usd < 0) {
			ntpc->probes_sent++;
			if (ntpc_handle_timeout(ntpc)) {
				break;
			}
			sleep_time = ntpc->retry_interval;
			goto loopend;
		}

		//We have socket here
		send_packet(usd, ntpc->time_of_send);
		++ntpc->probes_sent;

		//Wait for reply
		to.tv_sec  = sleep_time;
		to.tv_usec = 0;
		FD_ZERO(&fds);
		FD_SET(usd, &fds);
		do {
			i = select(usd + 1, &fds, NULL, NULL, &to); // Wait on read or error
		} while (i == -1 && errno == EINTR);

		if (i == -1) {
			//real error here
			sleep_time = ntpc->retry_interval;
			goto loopend;
		}

		if (i == 0) {
			//timeout
			if (ntpc_handle_timeout(ntpc)) {
				break;
			}
			sleep_time = ntpc->retry_interval;
			goto loopend;
		}

		// We have something to read here.
		DBG("Receiving...");
		pack_len   = recvfrom(usd, incoming, sizeof_incoming, 0, (struct sockaddr *)&sa_xmit_in,
				      &sa_xmit_len);
		error	   = ntpc->goodness;
		sleep_time = ntpc->retry_interval;
		sleep_time = 0;
		if (pack_len < 0) {
			ERR("recvfrom error");
		} else if (pack_len > 0 && (unsigned)pack_len < sizeof_incoming) {
			if (ntpc->failover_cnt) {
				ntpc->probes_sent = 0;
			}
			get_packet_timestamp(usd, &udp_arrival_ntp);

			if (check_source(pack_len, &sa_xmit_in, sa_xmit_len, ntpc) != 0 ||
			    rfc1305print(incoming_word, &udp_arrival_ntp, ntpc, &error) != 0) {
				goto loopend;
			}

			sleep_time	     = ntpc->cycle_time;
			ntpc->retry_interval = RETRY_INTERVAL;
		} else {
			LOG("Ooops.  pack_len=%d", pack_len);
		}
		/* best rollover option: specify -g, -s, and -l.
		 * simpler rollover option: specify -s and -l, which
		 * triggers a magic -c 1 */
		if ((error < ntpc->goodness && ntpc->goodness != 0) ||
		    (ntpc->probes_sent >= ntpc->probe_count && ntpc->probe_count != 0)) {
			if (ntpc->failover_cnt) {
				ntpc->probes_sent = 0;
			}
			ntpc->set_clock = 0;
			if (!ntpc->live) {
				break;
			}
		}

loopend:
#ifdef MOBILE_SUPPORT
		if (ntpc->modem_sync.modem_time && ntpc->modem_sync.timezone_sync_enabled) {
			set_timezone_from_modem(&ntpc->modem_sync);
		}
#endif
		close(usd);
		DBG("Probes sent: %d", ntpc->probes_sent);
		DBG("Going to sleep %d sec.", sleep_time);
		ntpc_sleep(sleep_time);
		if (sleep_time < ntpc->cycle_time) {
			ntpc->retry_interval <<= 1;
			if (ntpc->retry_interval > ntpc->cycle_time) {
				ntpc->retry_interval = ntpc->cycle_time;
			}
		}
	} //for(;;)
	close(usd);
#undef incoming
#undef sizeof_incoming
}

#ifdef ENABLE_REPLAY
static void do_replay(void)
{
	char line[100];
	int n, day, freq, absolute;
	float sec, el_time, st_time, disp;
	double skew, errorbar;
	int simulated_freq	    = 0;
	unsigned int last_fake_time = 0;
	double fake_delta_time	    = 0.0;

	while (fgets(line, sizeof line, stdin)) {
		n = sscanf(line, "%d %f %f %f %lf %f %d", &day, &sec, &el_time, &st_time, &skew, &disp,
			   &freq);
		if (n == 7) {
			fputs(line, stdout);
			absolute = day * 86400 + (int)sec;
			errorbar = el_time + disp;
			DBG("contemplate %u %.1f %.1f %d", absolute, skew, errorbar, freq);
			if (last_fake_time == 0)
				simulated_freq = freq;
			fake_delta_time +=
				(absolute - last_fake_time) * ((double)(freq - simulated_freq)) / 65536;
			DBG("fake %f %d ", fake_delta_time, simulated_freq);
			skew += fake_delta_time;
			freq	       = simulated_freq;
			last_fake_time = absolute;
			simulated_freq = contemplate_data(absolute, skew, errorbar, freq);
		} else {
			ERR("Replay input error");
			exit(2);
		}
	}
}
#endif

static void usage(char *argv0)
{
	LOG("Usage: %s [-c count]"
#ifdef ENABLE_DEBUG
	    " [-d]"
#endif
	    " [-f frequency] [-g goodness] -h hostname\n"
	    "\t[-i interval] [-l] [-p port] [-q min_delay]"
#ifdef ENABLE_REPLAY
	    " [-r]"
#endif
	    " [-s] [-t] [-S]\n",
	    argv0);
}

int main(int argc, char *argv[])
{
	int c;
	int ret = EXIT_FAILURE;
	/* These parameters are settable from the command line
	   the initializations here provide default behavior */
	int initial_freq; /* initial freq value to use */
	struct ntp_control ntpc		      = { 0 };
	ntpc.udp_local_port		      = 0; /* default of 0 means kernel chooses */
	ntpc.live			      = 0;
	ntpc.set_clock			      = 0;
	ntpc.probe_count		      = 0; /* default of 0 means loop forever */
	ntpc.failover_cnt		      = 0;
	ntpc.cycle_time			      = 600; /* seconds */
	ntpc.retry_interval		      = RETRY_INTERVAL;
	ntpc.goodness			      = 0;
	ntpc.cross_check		      = 1;
	ntpc.hosts			      = 0;
	ntpc.save_time			      = 0;
#ifdef MOBILE_SUPPORT
	ntpc.modem_sync.time_sync_enabled     = 0;
	ntpc.modem_sync.timezone_sync_enabled = 0;
	ntpc.modem_sync.ubus_ctx	      = NULL;
	ntpc.modem_sync.uci_ctx		      = NULL;
#endif
	memset(ntpc.hostnames, 0, sizeof(ntpc.hostnames));

	for (;;) {
		c = getopt(argc, argv, "c:" DEBUG_OPTION "f:e:g:h:i:lp:q:k:" REPLAY_OPTION "stmzDS");
		if (c == EOF)
			break;
		switch (c) {
		case 'c':
			ntpc.probe_count = atoi(optarg);
			break;
#ifdef ENABLE_DEBUG
		case 'd':
			++g_debug;
			break;
#endif
		case 'e':
			ntpc.hotplug_script = optarg;
			break;
		case 'f':
			initial_freq = atoi(optarg);
			DBG("initial frequency %d", initial_freq);
			set_freq(initial_freq);
			break;
		case 'g':
			ntpc.goodness = atoi(optarg);
			break;
		case 'h':
			if (ntpc.hosts < NTP_MAX_SERVERS) {
				ntpc.hostnames[ntpc.hosts++] = optarg;
			}
			break;
		case 'k':
			ntpc.failover_cnt = strtol(optarg, NULL, 10);
			break;
		case 'i':
			ntpc.cycle_time = atoi(optarg);
			break;
		case 'l':
			(ntpc.live)++;
			break;
		case 'p':
			ntpc.udp_local_port = atoi(optarg);
			break;
		case 'q':
			min_delay = atof(optarg);
			break;
#ifdef ENABLE_REPLAY
		case 'r':
			do_replay();
			exit(0);
			break;
#endif
		case 's':
			ntpc.set_clock++;
			break;

		case 't':
			ntpc.cross_check = 0;
			break;

		case 'S':
			ntpc.save_time = 1;
			break;
#ifdef MOBILE_SUPPORT
		case 'm':
			ntpc.modem_sync.time_sync_enabled = 1;
			break;

		case 'z':
			ntpc.modem_sync.timezone_sync_enabled = 1;
			break;
#endif
		case 'D':
			daemon(0, 0);
			break;

		default:
			usage(argv[0]);
			ret = 1;
			goto end;
		}
	}
	if (!ntpc.hosts) {
		ntpc.failover_cnt = 0;
	}

#ifdef MOBILE_SUPPORT
	if (!(ntpc.modem_sync.ubus_ctx = ubus_connect(NULL))) {
		ERR("Couldn't connect to ubus");
		goto end;
	}
#endif

	if (ntpc.set_clock && !ntpc.live && !ntpc.goodness && !ntpc.probe_count) {
		ntpc.probe_count = 1;
	}

	/* respect only applicable MUST of RFC-4330 */
	if (ntpc.probe_count != 1 && ntpc.cycle_time < MIN_INTERVAL) {
		ntpc.cycle_time = MIN_INTERVAL;
	}

	DBG("Configuration:\n"
	    "  -c probe_count             %d\n"
	    "  -d (debug)                 %d\n"
	    "  -g goodness                %d\n"
	    "  -k failover_count          %d\n"
	    "  -i interval                %d\n"
	    "  -l live                    %d\n"
	    "  -p local_port              %d\n"
	    "  -q min_delay               %f\n"
	    "  -s set_clock               %d\n"
	    "  -x cross_check             %d\n"
#ifdef MOBILE_SUPPORT
	    "  -m time_sync_enabled       %d\n"
	    "  -z timezone_sync_enabled   %d\n"
#endif
	    "  -S save_time               %d",
	    ntpc.probe_count, g_debug, ntpc.goodness, ntpc.failover_cnt, ntpc.cycle_time, ntpc.live,
	    ntpc.udp_local_port, min_delay, ntpc.set_clock, ntpc.cross_check,
#ifdef MOBILE_SUPPORT
	    ntpc.modem_sync.time_sync_enabled, ntpc.modem_sync.timezone_sync_enabled,
#endif
	    ntpc.save_time);
	DBG("Hosts:");
	for (int i = 0; i < ntpc.hosts && ntpc.hostnames[i]; i++) {
		DBG("  %d. %s", i + 1, ntpc.hostnames[i]);
	}
	DBG("Compiled with NTP_MAX_SERVERS = %d", NTP_MAX_SERVERS);

	signal(SIGINT, exit);
	signal(SIGTERM, exit);

	primary_loop(&ntpc);

	ret = EXIT_SUCCESS;
end:
#ifdef MOBILE_SUPPORT
	if (ntpc.modem_sync.ubus_ctx) {
		ubus_free(ntpc.modem_sync.ubus_ctx);
	}
	if (ntpc.modem_sync.uci_ctx) {
		uci_free_context(ntpc.modem_sync.uci_ctx);
	}
	free(ntpc.modem_sync.modem_time);
#endif
	return ret;
}
