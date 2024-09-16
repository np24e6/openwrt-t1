#include <libsera/srnethttp.h>
#include <libsera/srutils.h>
#include <libsera/sragent.h>

#include "integrate.h"

using namespace std;

const std::string currentDateTime()
{
	time_t now = time(0);
	struct tm tstruct;
	char buf[80];
	tstruct = *localtime(&now);

	strftime(buf, sizeof(buf), "%Y-%m-%d.%X", &tstruct);

	return buf;
}

int Integrate::integrate(const SrAgent &agent, const string &srv, const string &srt)
{
	SrNetHttp http(agent.server() + "/s", srv, agent.auth());
	if (registerSrTemplate(http, xid, srt) != 0) {
		return -1;
	}

	http.clear();
	if (http.post("100," + agent.deviceID()) <= 0) {
		return -1;
	}

	SmartRest sr(http.response());
	SrRecord r = sr.next();

	if (r.size() && (r[0].second == "50")) {

		http.clear();

		if (http.post("101,RouterDevice") <= 0) {
			return -1;
		}

		sr.reset(http.response());
		r = sr.next();

		if ((r.size() != 3) && (r[0].second != "501")) {
			return -1;
		}

		id = r[2].second;
		string s = "102," + id + "," + agent.deviceID();

		if (http.post(s) <= 0) {
			return -1;
		}

		http.post("109," + id + ",CONNECTED");
		http.post("110," + id + "," + currentDateTime() + ",AVAILABLE");

		return 0;

	} else if ((r.size() == 3) && (r[0].second == "500")) {

		id = r[2].second;
		return 0;
	}

	return -1;
}