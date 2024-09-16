import collections
import os

from greengrass_ipc_python_sdk.ipc_client import \
    IPCClient, IPCException

GreengrassServiceCallOutput = collections.namedtuple('GreengrassServiceCallOutput', ['response', 'error_response'])
GreengrassServiceRegistryEntry = collections.namedtuple('GreengrassServiceRegistryEntry', ['service_client', 'resolved_addr'])

"""
    :param context: context holds the metadata for the message. e.g. status_code, content-type
    :type context: bytes

    :param payload: payload holds the message main body
    :type payload: bytes
"""
GreengrassServiceMessage = collections.namedtuple('GreengrassServiceMessage', ['context', 'payload'])


class GreengrassRuntimeException(Exception):
    pass


class GreengrassServiceClient(object):
    """
        GreengrassServiceClient is the customer-facing facade class to be used to communicate with services
    """
    def __init__(self):
        self._lambda_client = _LambdaServiceClient()

    def call(self, address, message):
        """
            to be changed once the registry API is available
            call(self, address, message) is the API that's dedicated for send request to services that performs
            synchronous request-reply services. It blocks the caller until the response is received.
            :param address: address holds the identifier for the service to be called. e.g. Lambda ARN, customer-defined
            service alias.
            :type address: str

            :param message: message holds the GreengrassServiceMessage type message to be delivered to the service.
            :type message: GreengrassServiceMessage

            :returns service_call_output: service_call_output captures 3 fields - response, error_response, gg_err.
            service populates *only* response when the handling is successful
            service populates *only* error_response when the handling is unsuccessful
            greengrass populates *only* gg_err when error happened outside the target service that failed the call.
            :type service_call_output: GreengrassServiceCallOutput

            :raises GreengrassRuntimeException
        """
        self._client, resolved_addr = self._ipc_client(address)
        return self._client.send(resolved_addr, message)

    def _ipc_client(self, address):
        return self._lambda_client, address


class _LambdaServiceClient(object):
    """
        client behavior impl. for existing lambda services.
    """
    def __init__(self):
        self._ipc_client = IPCClient()

    def send(self, resolved_addr, message):
        """
            :param resolved_addr: must be an lambda ARN.
            :type  resolved_addr: tbd

            :param message: the message to be delivered
            :type message: GreengrassServiceMessage

            :return service_call_output: GreengrassServiceCallOutput

            :raises GreengrassRuntimeException
        """
        try:
            invocation_id = self._ipc_client.post_work(resolved_addr, message.payload, message.context)
            get_work_result_output = self._ipc_client.get_work_result(resolved_addr, invocation_id)
            if not get_work_result_output.func_err:
                # no error
                return GreengrassServiceCallOutput(
                    GreengrassServiceMessage(
                        '',
                        get_work_result_output.payload
                    ),
                    None
                )
            elif get_work_result_output.func_err == 'Handled':
                # service error
                return GreengrassServiceCallOutput(
                    None,
                    GreengrassServiceMessage(
                        '',
                        get_work_result_output.payload
                    )
                )
            else:
                # ipc error
                raise GreengrassRuntimeException(get_work_result_output.func_err)
        except IPCException as e:
            raise GreengrassRuntimeException(str(e))
