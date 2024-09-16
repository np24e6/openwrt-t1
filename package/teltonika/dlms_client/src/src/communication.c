#include "master.h"

/*
	Original: https://github.com/Gurux/GuruxDLMS.c/blob/master/GuruxDLMSClientExample/src/communication.c
	Adjusted to our needs
*/

#include <netdb.h> //Add support for sockets
#include <sys/ioctl.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <termios.h>
#include <poll.h>

#define IO_TIMEOUT	    3 // timeout in seconds for reading/writing
#define RECEIVE_BUFFER_SIZE 200

PRIVATE int com_open_tcp_connection(connection *c);
PRIVATE uint8_t com_is_IPv6_address(const char *pAddress);
PRIVATE int com_check_if_connected_to_server(int fd);
PRIVATE int com_open_serial_connection(physical_device *dev);
PRIVATE int com_initialize_serial_settings(connection *c);

PRIVATE int com_read_data_block(connection *c, dlmsSettings *s, message *m, gxReplyData *reply);
PRIVATE int read_dlms_packet(connection *c, dlmsSettings *s, gxByteBuffer *data, gxReplyData *reply);
PRIVATE int read_data(connection *c, int *index);
PRIVATE int com_read_serial_port(connection *c, unsigned char eop);
PRIVATE int send_data(connection *c, gxByteBuffer *data);
PRIVATE int com_disconnect(connection *c, dlmsSettings *s);

//!< challenge can be NULL if request is not ciphered.
PRIVATE int com_handle_aarq_request(connection *c, dlmsSettings *s, message *m, gxReplyData *reply, gxByteBuffer *challenge);
PRIVATE int com_handle_snrm_request(connection *c, dlmsSettings *s, message *m, gxReplyData *reply, gxByteBuffer *challenge);
PRIVATE void com_handle_init_buffers(connection *c, dlmsSettings *s);
PRIVATE void com_initialize_buffers(connection *c, int size);

struct baudrate {
	int bps;
	uint32_t baud;
};

PRIVATE struct baudrate baudrates[] = {
	{ 300, B300 },	       { 600, B600 },	      { 1200, B1200 },	     { 1800, B1800 },
	{ 2400, B2400 },       { 4800, B4800 },	      { 9600, B9600 },	     { 19200, B19200 },
	{ 38400, B38400 },     { 57600, B57600 },     { 115200, B115200 },   { 230400, B230400 },
	{ 460800, B460800 },   { 500000, B500000 },   { 576000, B576000 },   { 921600, B921600 },
	{ 1000000, B1000000 }, { 1152000, B1152000 }, { 1500000, B1500000 }, { 2000000, B2000000 },
	{ 2500000, B2500000 }, { 3000000, B3000000 }, { 3500000, B3500000 }, { 4000000, B4000000 },
};

PUBLIC int com_open_connection(physical_device *dev)
{
	return (dev->connection->type == TCP) ? com_open_tcp_connection(dev->connection) :
						com_open_serial_connection(dev);
}

PRIVATE int com_open_tcp_connection(connection *c)
{
	int ret			    = 0;
	int addSize		    = 0;
	int flags		    = 0;
	struct sockaddr *add	    = NULL;
	struct sockaddr_in6 addrIP6 = { 0 };
	struct sockaddr_in addIP4   = { 0 };
	char *address		    = c->parameters.tcp.host;
	int port		    = c->parameters.tcp.port;
	int family		    = com_is_IPv6_address(address) ? AF_INET6 : AF_INET;

	c->socket  = socket(family, SOCK_STREAM, IPPROTO_IP);
	if (c->socket == -1) {
		log(L_ERROR, "Socket creation failed: %s\n", strerror(errno));
		return DLMS_ERROR_TYPE_COMMUNICATION_ERROR;
	}

	if (family == AF_INET) {
		addIP4.sin_port	       = htons(port);
		addIP4.sin_family      = AF_INET;
		addIP4.sin_addr.s_addr = inet_addr(address);
		addSize		       = sizeof(struct sockaddr_in);
		add		       = (struct sockaddr *)&addIP4;

		if (addIP4.sin_addr.s_addr == INADDR_NONE) {
			struct hostent *Hostent = gethostbyname(address);
			if (Hostent == NULL) {
				log(L_ERROR, "Failed to get host by name: %s\n", strerror(errno));
				return DLMS_ERROR_TYPE_COMMUNICATION_ERROR;
			};
			addIP4.sin_addr = *(struct in_addr *)(void *)Hostent->h_addr_list[0];
		};
	} else {
		addrIP6.sin6_port   = htons(port);
		addrIP6.sin6_family = AF_INET6;
		ret		    = inet_pton(family, address, &(addrIP6.sin6_addr));
		if (ret == -1) {
			log(L_ERROR, "Failed to get host by name: %s\n", strerror(errno));
			return DLMS_ERROR_TYPE_COMMUNICATION_ERROR;
		};
		add	= (struct sockaddr *)&addrIP6;
		addSize = sizeof(struct sockaddr_in6);
	}

	struct timeval tv = { IO_TIMEOUT, 0 };
	setsockopt(c->socket, SOL_SOCKET, SO_RCVTIMEO, (const char *)&tv, sizeof(tv));
	setsockopt(c->socket, SOL_SOCKET, SO_SNDTIMEO, (const char *)&tv, sizeof(tv));

	flags = fcntl(c->socket, F_GETFL, 0);
	fcntl(c->socket, F_SETFL, O_NONBLOCK);
	//Connect to the meter.
	if (connect(c->socket, add, addSize) == -1) {
		if (errno == EINPROGRESS && com_check_if_connected_to_server(c->socket)) {
			log(L_ERROR, "Failed to connect to server: %s", strerror(errno));
			return DLMS_ERROR_TYPE_COMMUNICATION_ERROR;
		}
	}

	fcntl(c->socket, F_SETFL, flags);
	return DLMS_ERROR_CODE_OK;
}

PRIVATE uint8_t com_is_IPv6_address(const char *pAddress)
{
	return (strstr(pAddress, ":") != NULL);
}

PRIVATE int com_check_if_connected_to_server(int fd)
{
	struct pollfd fds[1] = { [0] = { .fd = fd, .events = POLLOUT } };
	int tries	     = 0;
	while (tries < 25) {
		int rc = poll(fds, 1, 0);

		if (rc < 0) {
			log(L_ERROR,, "Poll error while trying to connect: %s\n", strerror(errno));
			break;
		}

		if ((fds[0].revents & POLLERR) || !fds[0].revents) {
			usleep(25);
			tries++;
			continue;
		}

		if (fds[0].revents & POLLOUT) {
			return 0; //!< connected
		}
	}

	return 1;
}

PRIVATE int com_initialize_mode_e(connection *c)
{
	struct termios options = { 0 };
	int baudrate = B9600;

	bb_clear(&c->data);
	bb_init(&c->data);
	bb_capacity(&c->data, 500);

	for (size_t i = 0; i < ARRAY_SIZE(baudrates); i++) {
		if (c->parameters.serial.baudrate == baudrates[i].bps) {
			baudrate = baudrates[i].baud;
			break;
		}
	}

	options.c_cflag |= PARENB;
	options.c_cflag &= ~PARODD;
	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag |= CS7;

	cfsetospeed(&options, B300);
	cfsetispeed(&options, B300);

	options.c_lflag = 0;
	options.c_cc[VMIN] = 1;
	options.c_cc[VTIME] = 5;

	if (tcsetattr(c->socket, TCSAFLUSH, &options) != 0) {
		log(L_ERROR, "Failed to set termios settings");
		return DLMS_ERROR_CODE_INVALID_PARAMETER;
	}

	char buff[16] = "/?!\r\n";
	int len = strlen(buff);
	if (write(c->socket, buff, len) != len) {
		log(L_ERROR, "Failed to write initialisation message");
		return DLMS_ERROR_CODE_SEND_FAILED;
	}

	if (com_read_serial_port(c, '\n') != 0) {
		// this might fail if device was already initiated with /?!\r\n.
		log(L_ERROR, "Failed to read from serial port");
		goto err;
	}

	unsigned char ch = 0;
	if (bb_getUInt8(&c->data, &ch) != 0 || ch != '/') {
		return DLMS_ERROR_CODE_SEND_FAILED;
	}

	//Get used baud rate.
	if (bb_getUInt8ByIndex(&c->data, 4, &ch) != 0) {
		return DLMS_ERROR_CODE_SEND_FAILED;
	}

	switch (ch) {
	case '0':
		baudrate = B300;
		break;
	case '1':
		baudrate = B600;
		break;
	case '2':
		baudrate = B1200;
		break;
	case '3':
		baudrate = B2400;
		break;
	case '4':
		baudrate = B4800;
		break;
	case '5':
		baudrate = B9600;
		break;
	case '6':
		baudrate = B19200;
		break;
	default:
		log(L_ERROR, "Could not parse baudrate");
		return DLMS_ERROR_CODE_INVALID_PARAMETER;
	}

err:
	buff[0] = 0x06;     // ACK
	buff[1] = '2'; 	    // "2" HDLC protocol procedure (Mode E)
	buff[2] = (char)ch; // baud rate
	buff[3] = '2';      // binary mode
	buff[4] = 0x0D;
	buff[5] = 0x0A;
	len = 6;

	if (write(c->socket, buff, 6) != len) {
		log(L_ERROR, "Failed to write acknowledgment message");
		return DLMS_ERROR_CODE_SEND_FAILED;
	}

	usleep(1000000); // This sleep is in standard. Do not remove.
	options.c_cflag = CS8 | CREAD | CLOCAL;
	cfsetospeed(&options, baudrate);
	cfsetispeed(&options, baudrate);

	if (tcsetattr(c->socket, TCSAFLUSH, &options) != 0) {
		log(L_ERROR, "Failed to set termios settings");
		return DLMS_ERROR_CODE_INVALID_PARAMETER;
	}

	return 0;
}

PRIVATE int com_open_serial_connection(physical_device *dev)
{
	char *device = dev->connection->parameters.serial.device;
	dev->connection->socket = open(device, O_RDWR | O_NOCTTY | O_NONBLOCK);
	if (dev->connection->socket == -1) {
		log(L_DEBUG, "Failed to open serial port: %s\n", device);
		return DLMS_ERROR_TYPE_COMMUNICATION_ERROR;
	}

	if (!isatty(dev->connection->socket)) {
		log(L_DEBUG, "'%s' is not a serial port\n", device);
		return DLMS_ERROR_TYPE_COMMUNICATION_ERROR;
	}

	return (dev->settings.interfaceType != DLMS_INTERFACE_TYPE_HDLC_WITH_MODE_E) ?
		       com_initialize_serial_settings(dev->connection) :
		       com_initialize_mode_e(dev->connection);
}

PRIVATE int com_initialize_serial_settings(connection *c)
{
	int ret		       = 1;
	struct termios options = { 0 };
	int baudrate	       = 0;
	int databits	       = 0;

	for (size_t i = 0; i < ARRAY_SIZE(baudrates); i++) {
		if (c->parameters.serial.baudrate == baudrates[i].bps) {
			baudrate = baudrates[i].baud;
			break;
		}
	}

	switch (c->parameters.serial.databits) {
	case 5:
		databits = CS5;
		break;
	case 6:
		databits = CS6;
		break;
	case 7:
		databits = CS7;
		break;
	case 8:
		databits = CS8;
		break;
	default:
		log(L_ERROR, "Invalid databits\n");
		goto err;
	}

	char p = *c->parameters.serial.parity;
	switch (p) {
	case 'n':
		options.c_cflag &= ~PARENB;
		break;
	case 'o':
		options.c_cflag |= (PARENB | PARODD);
		break;
	case 'e':
		options.c_cflag |= PARENB;
		options.c_cflag &= ~PARODD;
		break;
	case 'm':
		options.c_cflag |= (PARENB | CMSPAR | PARODD);
		break;
	case 's':
		options.c_cflag |= (PARENB | CMSPAR);
		options.c_cflag &= ~PARODD;
		break;
	default:
		log(L_ERROR, "Invalid parity\n");
		goto err;
	}

	if (c->parameters.serial.stopbits == 1) {
		options.c_cflag &= ~CSTOPB;
	} else {
		options.c_cflag |= CSTOPB;
	}

	options.c_cflag = databits | CREAD | CLOCAL;

	cfsetospeed(&options, baudrate);
	cfsetispeed(&options, baudrate);

	if (!strncmp("rts/cts", c->parameters.serial.flow_control, strlen("rts/cts"))) {
		options.c_cflag |= CRTSCTS;
	} else if (!strncmp("xon/xoff", c->parameters.serial.flow_control, strlen("xon/xoff"))) {
		options.c_iflag |= (IXON | IXOFF);
	}

	// How long we are waiting reply charachter from serial port.
	options.c_cc[VMIN]  = 1;
	options.c_cc[VTIME] = 5;

	if (tcsetattr(c->socket, TCSANOW, &options) != 0) {
		log(L_ERROR, "Failed to set STTY settings");
		goto err;
	}

	ret = 0;
err:
	return ret;
}

PRIVATE int send_data(connection *c, gxByteBuffer *data)
{
	int ret = DLMS_ERROR_CODE_OK;

	switch (c->type) {
	case TCP:
		if (send(c->socket, (const char *)data->data, data->size, 0) == -1) {
			log(L_ERROR, "Failed to send data: %s\n", strerror(errno));
			ret = DLMS_ERROR_TYPE_COMMUNICATION_ERROR;
		}
		break;
	case SERIAL:
		if (write(c->socket, data->data, data->size) != (ssize_t)data->size) {
			log(L_ERROR, "Failed to write data: %s\n", strerror(errno));
			ret = DLMS_ERROR_TYPE_COMMUNICATION_ERROR;
		}
		break;
	default:
		log(L_ERROR, "Invalid connection type");
		ret = DLMS_ERROR_CODE_INVALID_PARAMETER;
	}

	return ret;
}

PRIVATE int read_data(connection *c, int *index)
{
	int ret = 0;
	if (c->type == SERIAL) {
		if ((ret = com_read_serial_port(c, 0x7E)) != 0) {
			log(L_ERROR, "com_read_serial_port has failed");
			return ret;
		}
	} else if (c->type == TCP) {
		uint32_t cnt = c->data.capacity - c->data.size;
		if (cnt < 1) {
			log(L_ERROR, "data.size is bigger than data.capacity");
			return DLMS_ERROR_CODE_OUTOFMEMORY;
		}

		ret = recv(c->socket, (char *)c->data.data + c->data.size, cnt, 0);
		if (ret == -1 || (ret == 0 && cnt != 0)) {
			log(L_ERROR, "read_data(): recv failure");
			return DLMS_ERROR_CODE_RECEIVE_FAILED;
		}
		c->data.size += ret;
	}

	if (g_debug_level <= L_DEBUG) {
		char *hex = hlp_bytesToHex(c->data.data + *index, c->data.size - *index);
		if (*index == 0) {
			log(L_DEBUG, "RX:\t%s\n", hex);
		} else {
			log(L_DEBUG, " %s", hex);
		}
		free(hex);
		*index = c->data.size;
	}

	return 0;
}

PRIVATE int com_read_serial_port(connection *c, unsigned char eop)
{
	//Read reply data.
	int ret, pos;
	uint cnt		 = 1;
	unsigned char eopFound	 = 0;
	int lastReadIndex	 = 0;
	unsigned short bytesRead = 0;
	unsigned short readTime	 = 0;
	do {
		//Get bytes available.
		ret = ioctl(c->socket, FIONREAD, &cnt);
		//If driver is not supporting this functionality.
		if (ret < 0) {
			cnt = RECEIVE_BUFFER_SIZE;
		} else if (cnt == 0) {
			//Try to read at least one byte.
			cnt = 1;
		}
		//If there is more data than can fit to buffer.
		if (cnt > RECEIVE_BUFFER_SIZE) {
			cnt = RECEIVE_BUFFER_SIZE;
		}
		bytesRead = read(c->socket, c->data.data + c->data.size, cnt);
		if (bytesRead == 0xFFFF) {
			//If there is no data on the read buffer.
			if (errno == EAGAIN) {
				if (readTime > c->wait_time) {
					log(L_ERROR, "Wait time is longer than read time");
					return DLMS_ERROR_CODE_RECEIVE_FAILED;
				}
				readTime += 100;
				bytesRead = 0;
			} else {
				log(L_ERROR, "com_read_serial_port() read failed");
				return DLMS_ERROR_TYPE_COMMUNICATION_ERROR | errno;
			}
		}
		c->data.size += (unsigned short)bytesRead;
		//Note! Some USB converters can return true for ReadFile and Zero as bytesRead.
		//In that case wait for a while and read again.
		if (bytesRead == 0) {
			usleep(40000);
			continue;
		}
		//Search eop.
		if (c->data.size > 5) {
			//Some optical strobes can return extra bytes.
			for (pos = c->data.size - 1; pos != lastReadIndex; --pos) {
				if (c->data.data[pos] == eop) {
					eopFound = 1;
					break;
				}
			}
			lastReadIndex = pos;
		}
	} while (eopFound == 0);
	return DLMS_ERROR_CODE_OK;
}

PRIVATE int read_dlms_packet(connection *c, dlmsSettings *s, gxByteBuffer *data, gxReplyData *reply)
{
	int ret		 = DLMS_ERROR_CODE_OK;
	int index	 = 0;
	reply->complete	 = 0;
	c->data.size	 = 0;
	c->data.position = 0;

	if (data->size == 0) {
		log(L_ERROR, "Message is empty");
		return DLMS_ERROR_CODE_OK;
	}

	if (g_debug_level <= L_DEBUG) {
		char *hex = bb_toHexString(data);
		log(L_DEBUG, "TX:\t%s", hex);
		free(hex);
	}

	ret = send_data(c, data);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to send data: %s\n", hlp_getErrorMessage(ret));
		return ret;
	}

	//Loop until packet is complete.
	unsigned char pos = 0;
	do {
		if ((ret = read_data(c, &index)) != 0) {
			if (ret != DLMS_ERROR_CODE_RECEIVE_FAILED || pos >= 3) {
				break;
			}
			log(L_DEBUG, "Data send failed. Try to resend %d/3", ++pos);
			if ((ret = send_data(c, data)) != DLMS_ERROR_CODE_OK) {
				break;
			}
		} else {
			ret = cl_getData(s, &c->data, reply);
			if (ret != 0 && ret != DLMS_ERROR_CODE_FALSE) {
				break;
			}
		}
	} while (reply->complete == 0);

	return ret;
}

PRIVATE int com_read_data_block(connection *c, dlmsSettings *s, message *m, gxReplyData *reply)
{
	int ret = DLMS_ERROR_CODE_OK;

	if (m->size == 0) {
		// log(L_ERROR, "There is no data to send");
		return ret;
	}

	gxByteBuffer rr;
	bb_init(&rr);
	for (int pos = 0; pos != m->size; ++pos) {
		//Send data.
		ret = read_dlms_packet(c, s, m->data[pos], reply);
		if (ret != DLMS_ERROR_CODE_OK) {
			log(L_ERROR, "Failed to read_dlms_packet: %s", hlp_getErrorMessage(ret));
			goto end;
		}
		//Check is there errors or more data from server
		while (reply_isMoreData(reply)) {
			ret = cl_receiverReady(s, reply->moreData, &rr);
			if (ret != DLMS_ERROR_CODE_OK) {
				log(L_ERROR, "Failed to cl_receiverReady: %s", hlp_getErrorMessage(ret));
				goto end;
			}

			ret = read_dlms_packet(c, s, &rr, reply);
			if (ret != DLMS_ERROR_CODE_OK) {
				log(L_ERROR, "[while] Failed to read_dlms_packet: %s", hlp_getErrorMessage(ret));
				goto end;
			}
			bb_clear(&rr);
		}
	}

end:
	bb_clear(&rr);
	return ret;
}

PUBLIC int com_update_invocation_counter(connection *c, dlmsSettings *s, const char *invocationCounter)
{
	int ret = DLMS_ERROR_CODE_OK;

	if (invocationCounter == NULL || s->cipher.security == DLMS_SECURITY_NONE) {
		return ret;
	}

	gxByteBuffer challenge	 = { 0 };
	message messages	 = { 0 };
	gxReplyData reply	 = { 0 };
	unsigned short add	 = s->clientAddress;
	DLMS_AUTHENTICATION auth = s->authentication;
	DLMS_SECURITY security	 = s->cipher.security;

	bb_init(&challenge);
	bb_set(&challenge, s->ctoSChallenge.data, s->ctoSChallenge.size);
	s->clientAddress   = 16;
	s->authentication  = DLMS_AUTHENTICATION_NONE;
	s->cipher.security = DLMS_SECURITY_NONE;

	log(L_DEBUG, "Updating invocation counter\n");

	//Get meter's send and receive buffers size.
	ret = com_handle_snrm_request(c, s, &messages, &reply, &challenge);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "handle_snrm_request failed: %s\r\n", hlp_getErrorMessage(ret));
		goto err;
	}

	ret = com_handle_aarq_request(c, s, &messages, &reply, &challenge);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "handle_aarq_request failed: %s\r\n", hlp_getErrorMessage(ret));
		goto err;
	}

	com_handle_init_buffers(c, s);

	gxData d = { 0 };
	cosem_init(BASE(d), DLMS_OBJECT_TYPE_DATA, invocationCounter);
	if ((ret = com_read(c, s, BASE(d), 2)) == 0) {
		s->cipher.invocationCounter = 1 + var_toInteger(&d.value);

		log(L_DEBUG, "Invocation counter: %lu (0x%lX)\r\n", s->cipher.invocationCounter,
		    s->cipher.invocationCounter);

		com_disconnect(c, s);
		bb_clear(&s->ctoSChallenge);
		bb_set(&s->ctoSChallenge, challenge.data, challenge.size);
	}

err:
	s->clientAddress   = add;
	s->authentication  = auth;
	s->cipher.security = security;

	bb_clear(&challenge);
	return ret;
}

PRIVATE int com_disconnect(connection *c, dlmsSettings *s)
{
	int ret		  = DLMS_ERROR_CODE_OK;
	gxReplyData reply = { 0 };
	message msg	  = { 0 };

	log(L_INFO, "Disconnecting");
	reply_init(&reply);
	mes_init(&msg);

	if ((ret = cl_disconnectRequest(s, &msg)) != 0 ||
	    (ret = com_read_data_block(c, s, &msg, &reply)) != 0) {
		log(L_DEBUG, "Disconnect failed");
	}

	reply_clear(&reply);
	mes_clear(&msg);
	return ret;
}

PUBLIC int com_initialize_connection(connection *c, dlmsSettings *s)
{

	int ret		  = DLMS_ERROR_CODE_OK;
	message messages  = { 0 };
	gxReplyData reply = { 0 };

	log(L_INFO, "Initialising connection ('%d') ('%s')\n", c->id, UTL_SAFE_STR(c->name));

	//Get meter's send and receive buffers size.
	ret = com_handle_snrm_request(c, s, &messages, &reply, NULL);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to handle SNRM request: %s\n", hlp_getErrorMessage(ret));
		goto err;
	}

	ret = com_handle_aarq_request(c, s, &messages, &reply, NULL);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to handle AARQ request: %s\n", hlp_getErrorMessage(ret));
		goto err;
	}

	com_handle_init_buffers(c, s);

	if (s->authentication <= DLMS_AUTHENTICATION_LOW) {
		goto err;
	}

	// Get challenge Is HLS authentication is used.
	ret = cl_getApplicationAssociationRequest(s, &messages);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to get AA request: %s\n", hlp_getErrorMessage(ret));
		goto err;
	}

	ret = com_read_data_block(c, s, &messages, &reply);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to read data block: %s\n", hlp_getErrorMessage(ret));
		goto err;
	}

	ret = cl_parseApplicationAssociationResponse(s, &reply.data);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to parse AA response: %s\n", hlp_getErrorMessage(ret));
		goto err;
	}

err:
	mes_clear(&messages);
	reply_clear(&reply);
	return ret;
}

PUBLIC int com_read(connection *c, dlmsSettings *s, gxObject *object, unsigned char attributeOrdinal)
{
	int ret		  = DLMS_ERROR_CODE_OK;
	message data	  = { 0 };
	gxReplyData reply = { 0 };

	mes_init(&data);
	reply_init(&reply);

	ret = cl_read(s, object, attributeOrdinal, &data);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to read object: %s", hlp_getErrorMessage(ret));
		goto err;
	}

	ret = com_read_data_block(c, s, &data, &reply);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to read COSEM object data block: %s", hlp_getErrorMessage(ret));
		goto err;
	}

	ret = cl_updateValue(s, object, attributeOrdinal, &reply.dataValue);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to update value: %s", hlp_getErrorMessage(ret));
		goto err;
	}

err:
	mes_clear(&data);
	reply_clear(&reply);
	return ret;
}

PUBLIC void com_close(connection *c, dlmsSettings *s)
{
	log(L_INFO, "Closing connection ('%d') ('%s')", c->id, UTL_SAFE_STR(c->name));

	if (!s->server) {
		gxReplyData reply = { 0 };
		message msg	  = { 0 };
		reply_init(&reply);
		mes_init(&msg);
		if (cl_releaseRequest(s, &msg) != 0 || com_read_data_block(c, s, &msg, &reply) != 0) {
			//Show error but continue close.
			log(L_DEBUG, "Release failed");
			goto err;
		}
		reply_clear(&reply);
		mes_clear(&msg);

		if (cl_disconnectRequest(s, &msg) != 0 || com_read_data_block(c, s, &msg, &reply) != 0) {
			//Show error but continue close.
			log(L_DEBUG, "Close failed");
			goto err;
		}
		reply_clear(&reply);
		mes_clear(&msg);
	}

err:
	com_close_socket(c);
}

int com_getKeepAlive(connection *c, dlmsSettings *s)
{
	int ret		  = DLMS_ERROR_CODE_OK;
	message data	  = { 0 };
	gxReplyData reply = { 0 };

	mes_init(&data);
	reply_init(&reply);

	ret = cl_getKeepAlive(s, &data);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to get keepalive: %s", hlp_getErrorMessage(ret));
		goto err;
	}

	ret = com_read_data_block(c, s, &data, &reply);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "com_getKeepAlive(): failed to read data block: %s", hlp_getErrorMessage(ret));
		goto err;
	}

err:
	mes_clear(&data);
	reply_clear(&reply);
	return ret;
}

PUBLIC int com_readRowsByEntry(connection *c, dlmsSettings *s, gxProfileGeneric *obj, int index, int count)
{
	int ret		  = DLMS_ERROR_CODE_OK;
	message data	  = { 0 };
	gxReplyData reply = { 0 };

	mes_init(&data);
	reply_init(&reply);

	ret = cl_readRowsByEntry(s, obj, index, count, &data);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_DEBUG, "Failed to read rows by entry %s\n", hlp_getErrorMessage(ret));
		goto err;
	}

	ret = com_read_data_block(c, s, &data, &reply);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_DEBUG, "Failed to read data block %s\n", hlp_getErrorMessage(ret));
		goto err;
	}

	ret = cl_updateValue(s, (gxObject *)obj, 2, &reply.dataValue);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_DEBUG, "Failed to update value %s\n", hlp_getErrorMessage(ret));
		goto err;
	}

err:
	mes_clear(&data);
	reply_clear(&reply);
	return ret;
}

PRIVATE int com_handle_snrm_request(connection *c, dlmsSettings *s, message *m, gxReplyData *reply, gxByteBuffer *challenge)
{
	int ret = DLMS_ERROR_CODE_OK;

	mes_init(m);
	reply_init(reply);

	ret = cl_snrmRequest(s, m);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_DEBUG, "Failed to send SNRM request: %s\n", hlp_getErrorMessage(ret));
		goto end;
	}

	ret = com_read_data_block(c, s, m, reply);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_DEBUG, "Failed to read SNRM data block: %s\n", hlp_getErrorMessage(ret));
		goto end;
	}

	ret = cl_parseUAResponse(s, &reply->data);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_DEBUG, "Failed to parse UA response: %s\n", hlp_getErrorMessage(ret));
		goto end;
	}

end:
	mes_clear(m);
	reply_clear(reply);
	if (challenge) {
		bb_clear(challenge);
	}

	return ret;
}

PRIVATE int com_handle_aarq_request(connection *c, dlmsSettings *s, message *m, gxReplyData *reply, gxByteBuffer *challenge)
{
	int ret = DLMS_ERROR_CODE_OK;

	ret = cl_aarqRequest(s, m);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to send AARQ request: %s\n", hlp_getErrorMessage(ret));
		goto end;
	}

	ret = com_read_data_block(c, s, m, reply);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to read AARQ data block: %s\n", hlp_getErrorMessage(ret));
		goto end;
	}

	ret = cl_parseAAREResponse(s, &reply->data);
	if (ret != DLMS_ERROR_CODE_OK) {
		log(L_ERROR, "Failed to parse AARE response: %s\n", hlp_getErrorMessage(ret));
		goto end;
	}

end:
	mes_clear(m);
	reply_clear(reply);
	if (challenge) {
		bb_clear(challenge);
	}

	return ret;
}

PRIVATE void com_handle_init_buffers(connection *c, dlmsSettings *s)
{
	if (s->maxPduSize == 0xFFFF) {
		com_initialize_buffers(c, s->maxPduSize);
	} else {
		//!< Allocate 50 bytes more because some meters count this wrong and send few bytes too many.
		com_initialize_buffers(c, 50 + s->maxPduSize);
	}
}

PRIVATE void com_initialize_buffers(connection *c, int size)
{
	if (size == 0) {
		bb_clear(&c->data);
	} else {
		bb_capacity(&c->data, size);
	}
}

PUBLIC void com_close_socket(connection *c)
{
	if (c->socket != -1) {
		close(c->socket);
		c->socket = -1;
	}
}
