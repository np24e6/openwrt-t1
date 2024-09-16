#
# Copyright 2010-2016 Amazon.com, Inc. or its affiliates. All Rights Reserved.
#
# this log appender is shared among all components of python lambda runtime, including:
# greengrass_common/greengrass_message.py, greengrass_ipc_python_sdk/ipc_client.py,
# greengrass_ipc_python_sdk/utils/exponential_backoff.py, lambda_runtime/lambda_runtime.py.
# so that all log records will be emitted to local Cloudwatch.
import logging
import inspect
import time
import os
import sys

# https://docs.python.org/2/library/logging.html#logrecord-attributes
LOCALWATCH_FORMAT = '%(filename)s:%(lineno)d,%(message)s\n'


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
        return payload  # If the input payload is None, we return as is.

    if _is_python_3() and isinstance(payload, str):
        return payload.encode('utf-8')
    return payload


class LocalwatchLogHandler(logging.Handler):

    def __init__(self):
        # Set the log level here for client-side filtering
        logging.Handler.__init__(self, level=self._min_log_level())

        # Set the log level to pipe fd read from environment variables
        get_pipe_fd = lambda name: int(os.environ[name])
        self.log_level_to_pipe_fd = {
            "INFO": get_pipe_fd("_GG_LOG_FD_INFO"),
            "ERROR": get_pipe_fd("_GG_LOG_FD_ERROR"),
            "DEBUG": get_pipe_fd("_GG_LOG_FD_DEBUG"),
            "WARNING": get_pipe_fd("_GG_LOG_FD_WARN"),
            "CRITICAL": get_pipe_fd("_GG_LOG_FD_FATAL")
        }

        self.min_log_level = self._min_log_level()

    def emit(self, record):
        os.write(self.log_level_to_pipe_fd[record.levelname], _prepare_payload(self.format(record)))

    def flush(self):
        pass

    def _min_log_level(self):
        min_log_level = os.getenv('LOG_LEVEL', "NOTSET")
        # For non-matching levels, convert to equivalent logging Python's levels
        if min_log_level == "WARN":
            return logging.WARNING
        elif min_log_level == "FATAL":
            return logging.CRITICAL
        elif min_log_level in set(["TRACE", "NOTSET"]):
            return logging.NOTSET
        elif min_log_level == "INFO":
            return logging.INFO
        elif min_log_level == "DEBUG":
            return logging.DEBUG
        elif min_log_level == "ERROR":
            return logging.ERROR
        else:
            raise ValueError("Could not identify the log level: {}".format(min_log_level))


class StdioLogWriter:
    def __init__(self, default_log_level, localwatch_log_handler):
        self.default_log_level = default_log_level
        self.default_log_level_name = logging.getLevelName(self.default_log_level)

        self.localwatch_log_handler = localwatch_log_handler

    def flush(self):
        pass

    def write(self, data):
        data = str(data)
        if data == "\n":
            # when print(data) is invoked, it invokes write() twice. First,
            # writes the data, then writes a new line. This is to avoid
            # emitting log record with just a new-line character.
            return

        # creates https://docs.python.org/2/library/logging.html#logrecord-objects
        file_name, line_number = inspect.getouterframes(inspect.currentframe())[1][1:3]
        # note that it attaches a newline at the end for the pipe
        record = logging.makeLogRecord({"msg": data,
                                        "filename": os.path.basename(file_name),
                                        "lineno": line_number,
                                        "levelname": self.default_log_level_name})

        self.localwatch_log_handler.emit(record)
