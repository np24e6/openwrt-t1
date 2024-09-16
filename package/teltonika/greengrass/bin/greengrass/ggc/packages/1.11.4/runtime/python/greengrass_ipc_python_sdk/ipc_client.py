#
# Copyright 2010-2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
#

import collections
import functools
import json
import logging
import sys

try:
    # Python 3
    from urllib.request import urlopen, Request
    from urllib.error import URLError, HTTPError
except ImportError:
    # Python 2
    from urllib2 import urlopen, Request, URLError, HTTPError

from greengrass_common.env_vars import AUTH_TOKEN, GG_DAEMON_PORT

runtime_logger = logging.getLogger(__name__)

HEADER_INVOCATION_ID = 'X-Amz-InvocationId'
HEADER_CLIENT_CONTEXT = 'X-Amz-Client-Context'
HEADER_AUTH_TOKEN = 'Authorization'
HEADER_INVOCATION_TYPE = 'X-Amz-Invocation-Type'
HEADER_FUNCTION_ERR_TYPE = 'X-Amz-Function-Error'
IPC_API_VERSION = '2016-11-01'


def _is_python_3():
    return sys.version_info[0] == 3


def _prepare_payload(payload):
    """
    We support sending either binary data or UTF-8 encoded json. This is a
    helper method to encode Python3 unicode string data to UTF-8 so the
    customer doesn't need to.

    There is no way for us to differentiate between string and binary data
    on Python2, so we do not attempt to do any encoding there.

    Python3 users are expected to send `bytes` if they do not wish their data
    to be UTF-8 encoded.
    """
    if payload is None:
        return b''

    if _is_python_3() and isinstance(payload, str):
        return payload.encode('utf-8')
    return payload


def wrap_urllib_exceptions(func):
    @functools.wraps(func)
    def wrapped(*args, **kwargs):
        try:
            return func(*args, **kwargs)
        except HTTPError as httpError:
            error_code = httpError.code
            reason = httpError.reason
            data = httpError.read()
            runtime_logger.error("HTTP Error {}:{}, {}".format(error_code, reason, data))
            raise IPCException(str(httpError))
        except URLError as e:
            runtime_logger.exception(e)
            raise IPCException(str(e))

    return wrapped


class IPCException(Exception):
    pass


WorkItem = collections.namedtuple('WorkItem', ['invocation_id', 'payload', 'client_context'])
GetWorkResultOutput = collections.namedtuple('GetWorkResultOutput', ['payload', 'func_err'])


class InitializationResult(collections.namedtuple('InitializationResult', ['function_error'])):
    """
    :param success: bool
    :param function_error: an Exception, a str that represents the error or
        something that implements __str__. If there's no error, then it'll be None.
    """

    # ensure that instances of this class won't create __dict__
    # (normally associated with objects) and they'll use the same amount of
    # memory as a regular tuple.
    __slots__ = ()

    def __str__(self):
        if self.function_error and isinstance(self.function_error, Exception):
            return json.dumps({
                'Success': False,
                'Response': '{}("{}")'.format(
                    self.function_error.__class__.__name__,
                    self.function_error
                )
            }, ensure_ascii=False)
        elif self.function_error:
            return json.dumps({
                'Success': False,
                'Response': '{}'.format(self.function_error)
            }, ensure_ascii=False)
        else:
            return json.dumps({
                'Success': True,
                'Response': ''
            }, ensure_ascii=False)


class PutRequest(Request):
    """
    class to handle HTTP PUT
    """

    def __init__(self, *args, **kwargs):
        return Request.__init__(self, *args, **kwargs)

    def get_method(self, *args, **kwargs):
        return 'PUT'


class IPCClient:
    """
    Client for IPC that provides methods for getting/posting work for functions,
    as well as getting/posting results of the work.
    """

    _GG_DAEMON_PORT = GG_DAEMON_PORT

    @staticmethod
    def set_gg_daemon_port(port):
        """
        The IPCClient sets the port based on an environment variable.
        This is useful during testing.
        :param port: New value of port used by the ipc client.
        :type port: str
        :return: None
        """
        IPCClient._GG_DAEMON_PORT = port
        pass

    def __init__(self, endpoint='localhost', port=None):
        """
        :param endpoint: Endpoint used to connect to IPC.
            Generally, IPC and functions always run on the same box,
            so endpoint should always be 'localhost' in production.
            You can override it for testing purposes.
        :type endpoint: str

        :param port: Deprecated. Will not be used.
        :type port: None
        """
        self.endpoint = endpoint
        self.port = int(IPCClient._GG_DAEMON_PORT)
        self.auth_token = AUTH_TOKEN

        if port is not None:
            runtime_logger.warning(
                'deprecated arg port={} will be ignored'.format(port)
            )

    @wrap_urllib_exceptions
    def post_work(self, function_arn, payload, client_context, invocation_type="RequestResponse"):
        """
        Send work item to specified :code:`function_arn`.

        :param function_arn: Arn of the Lambda function intended to receive the work for processing.
        :type function_arn: str

        :param payload: The data making up the work being posted.
        :type payload: str or bytes

        :param client_context: The base64 encoded client context byte string that will be provided to the Lambda
        function being invoked.
        :type client_context: bytes

        :returns: Invocation ID for obtaining result of the work.
        :type returns: str
        """
        url = self._get_url(function_arn)
        runtime_logger.info('Posting work for function [{}] to {}'.format(function_arn, url))

        request = Request(url, _prepare_payload(payload))
        request.add_header(HEADER_CLIENT_CONTEXT, client_context)
        request.add_header(HEADER_AUTH_TOKEN, self.auth_token)
        request.add_header(HEADER_INVOCATION_TYPE, invocation_type)

        response = urlopen(request)

        invocation_id = response.info().get(HEADER_INVOCATION_ID)
        runtime_logger.info('Work posted with invocation id [{}]'.format(invocation_id))
        return invocation_id

    @wrap_urllib_exceptions
    def get_work(self, function_arn):
        """
        Retrieve the next work item for specified :code:`function_arn`.

        :param function_arn: Arn of the Lambda function intended to receive the work for processing.
        :type function_arn: string

        :returns: Next work item to be processed by the function.
        :type returns: WorkItem
        """
        url = self._get_work_url(function_arn)
        runtime_logger.info('Getting work for function [{}] from {}'.format(function_arn, url))

        request = Request(url)
        request.add_header(HEADER_AUTH_TOKEN, self.auth_token)

        response = urlopen(request)

        invocation_id = response.info().get(HEADER_INVOCATION_ID)
        client_context = response.info().get(HEADER_CLIENT_CONTEXT)

        runtime_logger.info('Got work item with invocation id [{}]'.format(invocation_id))
        return WorkItem(
            invocation_id=invocation_id,
            payload=response.read(),
            client_context=client_context)

    @wrap_urllib_exceptions
    def post_work_result(self, function_arn, work_item):
        """
        Post the result of processing work item by :code:`function_arn`.

        :param function_arn: Arn of the Lambda function intended to receive the work for processing.
        :type function_arn: string

        :param work_item: The WorkItem holding the results of the work being posted.
        :type work_item: WorkItem

        :returns: None
        """
        url = self._get_work_url(function_arn)

        runtime_logger.info('Posting work result for invocation id [{}] to {}'.format(work_item.invocation_id, url))

        request = Request(url, _prepare_payload(work_item.payload))

        request.add_header(HEADER_INVOCATION_ID, work_item.invocation_id)
        request.add_header(HEADER_AUTH_TOKEN, self.auth_token)

        urlopen(request)

        runtime_logger.info('Posted work result for invocation id [{}]'.format(work_item.invocation_id))

    @wrap_urllib_exceptions
    def post_handler_err(self, function_arn, invocation_id, handler_err):
        """
        Post the error message from executing the function handler for :code:`function_arn`
        with specifid :code:`invocation_id`


        :param function_arn: Arn of the Lambda function which has the handler error message.
        :type function_arn: string

        :param invocation_id: Invocation ID of the work that is being requested
        :type invocation_id: string

        :param handler_err: the error message caught from handler
        :type handler_err: string
        """
        url = self._get_work_url(function_arn)

        runtime_logger.info('Posting handler error for invocation id [{}] to {}'.format(invocation_id, url))

        payload = json.dumps({
            "errorMessage": handler_err,
        }).encode('utf-8')

        request = Request(url, payload)
        request.add_header(HEADER_INVOCATION_ID, invocation_id)
        request.add_header(HEADER_FUNCTION_ERR_TYPE, "Handled")
        request.add_header(HEADER_AUTH_TOKEN, self.auth_token)

        urlopen(request)

        runtime_logger.info('Posted handler error for invocation id [{}]'.format(invocation_id))

    @wrap_urllib_exceptions
    def get_work_result(self, function_arn, invocation_id):
        """
        Retrieve the result of the work processed by :code:`function_arn`
        with specified :code:`invocation_id`.

        :param function_arn: Arn of the Lambda function intended to receive the work for processing.
        :type function_arn: string

        :param invocation_id: Invocation ID of the work that is being requested
        :type invocation_id: string

        :returns: The get work result output contains result payload and function error type if the invoking is failed.
        :type returns: GetWorkResultOutput
        """

        if function_arn == "arn:aws:lambda:::function:GGRouter":
            return GetWorkResultOutput(
                payload='',
                func_err='')

        url = self._get_url(function_arn)

        runtime_logger.info('Getting work result for invocation id [{}] from {}'.format(invocation_id, url))

        request = Request(url)
        request.add_header(HEADER_INVOCATION_ID, invocation_id)
        request.add_header(HEADER_AUTH_TOKEN, self.auth_token)

        response = urlopen(request)

        runtime_logger.info('Got result for invocation id [{}]'.format(invocation_id))

        payload = response.read()
        func_err = response.info().get(HEADER_FUNCTION_ERR_TYPE)

        return GetWorkResultOutput(
            payload=payload,
            func_err=func_err)

    def _get_url(self, function_arn):
        return 'http://{endpoint}:{port}/{version}/functions/{function_arn}'.format(
            endpoint=self.endpoint, port=self.port, version=IPC_API_VERSION, function_arn=function_arn
        )

    def _get_work_url(self, function_arn):
        return '{base_url}/work'.format(base_url=self._get_url(function_arn))

    def _put_init_result_url(self, function_arn):
        return '{base_url}/initialized'.format(base_url=self._get_url(function_arn))

    @wrap_urllib_exceptions
    def put_initialization_result(self, function_arn, func_error):
        url = self._put_init_result_url(function_arn)
        request = PutRequest(url, data=str(InitializationResult(func_error)).encode('utf-8'))
        request.add_header(HEADER_AUTH_TOKEN, self.auth_token)

        urlopen(request)
