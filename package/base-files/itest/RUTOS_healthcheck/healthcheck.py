import json
import re
from ast import literal_eval
from collections import defaultdict
from os.path import dirname, realpath
from requests import codes, get as http_get, post as http_post, Response
from tbot import testcase
from tbot.log import (
    c,
    message as log_message,
    warning as log_warning,
    with_verbosity,
    Verbosity,
)
from tbot.machine.linux import LinuxShell, Pipe, RedirBoth
from typing import Union


def log_error(colored_str: str, normal_str: str = "") -> None:
    log_message(c(colored_str).red.bold + normal_str)


def exec_with_output(lab: LinuxShell, *args) -> None:
    retcode, out = lab.exec(*args)
    if retcode != 0:
        log_message(out)

    assert retcode == 0


def get_ubus_resp(lab: LinuxShell, service: str, method: str) -> dict:
    return json.loads(lab.exec0("ubus", "call", service, method))


@testcase
def wait_for_init_done(lab: LinuxShell) -> None:
    res, _ = lab.exec(
        "ls", lab.fsroot / "tmp/run/init-done", RedirBoth(lab.fsroot / "dev/null")
    )

    with with_verbosity(Verbosity.QUIET):
        while res != 0:
            res, _ = lab.exec(
                "ls",
                lab.fsroot / "tmp/run/init-done",
                RedirBoth(lab.fsroot / "dev/null"),
            )


@testcase
def check_dmesg_for_segfaults(lab: LinuxShell) -> None:
    segfaults = defaultdict(list)

    dmesg_log = lab.exec0("dmesg")
    segfault_lines = re.findall(r"[^\s]*(?:\[\d+\])?:\s+segfault at\s+.*", dmesg_log)
    for line in segfault_lines:
        service_name, pid, message = re.match(
            r"^([^\[\s:]+)\[(\d+)\]:\s+segfault\s+(at.*)", line
        ).groups()

        segfaults[service_name].append((pid, message))

    for service in segfaults.keys():
        log_error(f"Segfault in {service}")
        [log_message(f"  {message} (PID {pid})") for pid, message in segfaults[service]]

    assert not segfaults


@testcase
def check_ps(lab: LinuxShell) -> None:
    missing = False

    services = [
        "dnsmasq",
        "dropbear",
        "gsmd",
        "ip_blockd",
        "logd",
        "mobifd",
        "netifd",
        "port_eventsd",
        "procd",
        "rpcd",
        "ubusd",
        "udhcpc",
        "uhttpd",
    ]

    command_lines = "\n".join(
        [" ".join(i.split()[4:]) for i in lab.exec0("ps").split("\n")]
    )

    for service in services:
        p_match = re.findall(
            r"^(?:[^\s\/]+\s+)?([\/\w]*" + re.escape(service) + r"[\/\w]*)(?:[ \t]|$)",
            command_lines,
            re.M,
        )

        if len(p_match) == 0:
            log_error(f"Process '{service}' is not running")
            missing = True

    assert not missing


@testcase
def check_ubus_methods(lab: LinuxShell) -> None:
    missing = False
    changed = False
    new = False

    with open(dirname(realpath(__file__)) + "/ubus_methods.json") as f:
        ubus_info = literal_eval(f.read())

    ubus_objects = lab.exec0("ubus", "list").split("\n")

    for service, known_methods in ubus_info.items():
        if service not in ubus_objects:
            log_error(f"Missing '{service}' service on ubus")
            missing = True
            continue

        current_methods = literal_eval(
            "{"
            + ",".join(lab.exec0("ubus", "-v", "list", service).split("\n")[1:])
            + "}"
        )

        current_keys = current_methods.keys()
        known_keys = known_methods.keys()

        for k in known_keys:
            if k not in current_keys:
                log_error(f"{service}:", f" missing '{k}' method on ubus")
                missing = True
            elif known_methods[k] != current_methods[k]:
                log_error(f"{service}:", f" changed parameters of '{k}' method")
                changed = True

        for c in current_keys:
            if c not in known_keys:
                log_warning(f"{service}: new '{c}' method on ubus")
                new = True

    if new:
        log_warning("Please update the test to include new ubus methods")

    assert not changed
    assert not missing


@testcase
def check_ubus_mobifd_responsive(lab: LinuxShell) -> None:
    assert get_ubus_resp(lab, "mobifd", "reload")["state"] == "OK"


@testcase
def check_ubus_gsmd_responsive(lab: LinuxShell) -> None:
    assert get_ubus_resp(lab, "gsm", "info")["mdm_stats"]["num_modems"] == 0


@testcase
def check_vuci_up(lab: LinuxShell) -> None:
    name = lab.exec0("ubus", "call", "vuci.login", "get_routername")

    assert name != ""


@testcase
def check_IPv4_address(lab: LinuxShell) -> None:
    board_json_file = "etc/board.json"
    interface_name = "br-lan"

    board_json = json.loads(lab.exec0("cat", lab.fsroot / board_json_file))
    current_br_lan_ip = re.sub(
        r"^[' \t]*\n",
        "",
        lab.exec0(
            "ip",
            "address",
            "show",
            interface_name,
            Pipe,
            "grep",
            "-o",
            "inet [^ /]*",
            Pipe,
            "awk",
            "{print $2}",
        ),
    ).strip()

    assert board_json["network"]["lan"]["default_ip"] == current_br_lan_ip


@testcase
def check_API_up(lab: LinuxShell) -> None:
    device_id = assert_http_success(
        http_get(f"http://{lab.hostname}/api/unauthorized/status"), "device_identifier"
    )

    assert device_id != ""
    assert device_id != "-"


@testcase
def check_internet_connection(lab: LinuxShell) -> None:
    exec_with_output(lab, "ping", "-c", "1", "1.1.1.1")


@testcase
def check_dns(lab: LinuxShell) -> None:
    exec_with_output(lab, "wget", "-O", lab.fsroot / "dev/null", "http://google.com")


def assert_http_success(resp: Response, data_key: str = "") -> Union[str, dict]:
    assert resp.status_code == codes.OK
    response_body = json.loads(resp.text)
    assert response_body["success"]

    if not data_key:
        return response_body

    assert data_key in response_body["data"].keys()
    value = response_body["data"][data_key]
    assert value is not None

    return value


def API_login(url_base: str):
    return assert_http_success(
        http_post(
            url_base + "/login",
            json={"username": "admin", "password": "admin01"},
        ),
        "token",
    )


# supported_methods = ["get", "post", "put", "delete"]
supported_methods = ["get"]


def test_endpoint(endpoint, url, token, method, expected_errors, endpoint_data) -> bool:
    if method not in supported_methods:
        return True

    success = True

    if method == "get":
        http_function = http_get
    else:
        log_message(f"'{method}' test not implemented - skipping '{endpoint}'")
        return True

    resp = http_function(
        url,
        headers={"Authorization": f"Bearer {token}"},
    )

    if not resp.text:
        log_error(
            "Error: ",
            f"did not receive a response body for '{method}' request from {endpoint}",
        )
        return False

    try:
        response_body = json.loads(resp.text)
    except json.decoder.JSONDecodeError:
        log_error("Error: failed to parse the response: \n", resp.text)
        return False

    if response_body["success"]:
        assert_http_success(resp)
        return True

    is_hidden_endpoint = (
        "x-web" in endpoint_data.keys()
        and "hidden" in endpoint_data["x-web"].keys()
        and endpoint_data["x-web"]["hidden"]
    )

    assert "errors" in response_body
    for err in response_body["errors"]:
        if (err["code"], err["error"]) in expected_errors:
            continue

        msg = f"{resp.status_code} from {endpoint}"
        # Don't fail on errors for hidden endpoints
        if is_hidden_endpoint:
            log_warning(f"{msg} (ignoring error for hidden endpoint)")
        else:
            success = False
            log_error(
                f"Error:   {msg}", f".\nFull URL: {url}\nResponse: {response_body}"
            )

    return success


dummy_section_id = "0"
API_errors = {
    0: (122, "Service does not exist in device"),
    1: (122, f"Configuration {dummy_section_id} not found"),
    2: (122, f"Interface '{dummy_section_id}' does not exist."),
    3: (113, f"Section: {dummy_section_id} for service does not exist"),
    4: (113, f"Parent section '{dummy_section_id}' does not exist"),
    5: (113, f"Device '{dummy_section_id}' not found"),
    6: (113, "SIM card section not found."),
    7: (113, "Provided modem does not exist"),
    8: (1, "Provided modem does not exist"),
    9: (1, "Modem does not exist."),
    # 10: (
    #     113,
    #     "Must be one of the following values [network, system, events, connections].",
    # ),
}


def is_available_for_rutos_devices(endpoint):
    if "x-web" not in endpoint.keys() or "devices" not in endpoint["x-web"].keys():
        return True

    r = re.compile(r"(?:RUT|TRB|OTD|ATR|TAP|TCR)")
    return any(filter(r.match, endpoint["x-web"]["devices"]))


@testcase
def check_API_endpoints(lab: LinuxShell) -> None:
    success = True

    with open(dirname(realpath(__file__)) + "/api_bundle.json") as f:
        api_docs = json.load(f)

    url_base = f"http://{lab.hostname}/api"
    expected_errors = [API_errors[0]]

    token = API_login(url_base)

    for path in api_docs["paths"]:
        if not is_available_for_rutos_devices(api_docs["paths"][path]):
            continue

        endpoint = path

        if re.match(r"^/(?:login)", endpoint):
            continue
        # # TODO: remove this when these redundant unused endpoints are removed
        # if re.match(r"^\/messages\/modem\/\{id\}.*", endpoint):
        #     log_warning(f"temporarily skipping '{endpoint}' endpoint")
        #     continue
        if re.search(r"\{id\}", endpoint):
            endpoint = re.sub(r"\{id\}", dummy_section_id, endpoint)
            [expected_errors.append(API_errors[i]) for i in range(1, 10)]
        endpoint = re.sub(r"\{event_type\}", "network", endpoint)
        endpoint = re.sub(r"\{interval\}", "month", endpoint)
        endpoint = re.sub(r"\{modem_id\}", dummy_section_id, endpoint)
        endpoint = re.sub(r"\{sim_id\}", dummy_section_id, endpoint)
        endpoint = re.sub(r"\{sms_id\}", dummy_section_id, endpoint)
        endpoint = re.sub(r"\{alarm_id\}", dummy_section_id, endpoint)

        if False in [
            test_endpoint(
                path,
                url_base + endpoint,
                token,
                method,
                expected_errors,
                api_docs["paths"][path],
            )
            for method in api_docs["paths"][path].keys()
        ]:
            success = False

    assert success
