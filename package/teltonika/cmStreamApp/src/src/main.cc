#include <thread>
#include "utils.h"

using namespace std;

int main()
{
	int ret = EXIT_SUCCESS;
	config cnf = {};

	if (!scan_config(CONFIG, CONF_OPTIONS_MAX, &cnf)) {
		return CONFIG_SCAN_ERR;
	}

	if (get_router_data(&cnf)) {
		ret = ROUTER_STR_ERR;
		goto release;
	}

	if (cnf.services == 2) {
		thread t1(connect_to, cnf.cloud[cnf.services - 2], cnf.router_str);
		t1.detach();
	}

	ret = connect_to(cnf.cloud[cnf.services - 1], cnf.router_str);

release:
	clean_up(&cnf);

	return ret;
}