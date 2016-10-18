/*
 * Copyright (c) 2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, UDPECT, INUDPECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <hutils/udp.h>

#if defined(HCONF_mUDP_bGETADDRINFO)
#	define GETADDRINFO
#	include <fcntl.h>
#	include <time.h>
#	include <netdb.h>
#	include <stdarg.h>
#	include <stdio.h>
#	include <unistd.h>
#	include <hutils/err.h>
#	define INVALID_SOCKET -1
#	define SOCKET int
#	define SOCKET_ERROR -1
static void
gaif(int a_error, char const *a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	vfprintf(stderr, a_fmt, args);
	va_end(args);
	fprintf(stderr, ": %s\n", gai_strerror(a_error));
}
static int
set_non_blocking(SOCKET a_socket)
{
	return 0 == fcntl(a_socket, F_SETFL, O_NONBLOCK);
}
static void
warnf(char const *a_fmt, ...)
{
	va_list args;

	va_start(args, a_fmt);
	hutils_vwarn(a_fmt, args);
	va_end(args);
}
#elif defined(_MSC_VER)
#	define GETADDRINFO
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	include <stdio.h>
#	define close closesocket
static int
set_non_blocking(SOCKET a_socket)
{
	u_long non_blocking;

	non_blocking = 1;
	return NO_ERROR == ioctlsocket(a_socket, FIONBIO, &non_blocking);
}
static void
warnf(char const *a_fmt, ...)
{
	va_list args;
	LPTSTR s;

	va_start(args, a_fmt);
	vfprintf(stderr, a_fmt, args);
	va_end(args);
	fprintf(stderr, ": ");
	s = NULL;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER |
	    FORMAT_MESSAGE_FROM_SYSTEM, NULL, WSAGetLastError(), 0,
	    (LPTSTR)&s, 0, NULL);
	fprintf(stderr, "%s\n", s);
	LocalFree(s);
}
#endif

#if defined(GETADDRINFO)

#include <errno.h>
#include <hutils/err.h>
#include <hutils/macros.h>
#include <hutils/memory.h>
#include <hutils/string.h>

struct UDPAddress {
	struct	sockaddr_storage addr;
	socklen_t	len;
};
struct UDPClient {
	SOCKET	socket;
};
struct UDPServer {
	SOCKET	socket;
};

static int	get_family(int) FUNC_RETURNS;
static int	receive_datagram(SOCKET, struct UDPDatagram *, struct
    UDPAddress *, double);
static int	send_datagram(SOCKET, struct UDPDatagram const *);

static int g_is_setup;

int
get_family(int a_flags)
{
	return UDP_IPV4 == ((UDP_IPV4 | UDP_IPV6) & a_flags) ? AF_INET :
	    AF_INET6;
}

int
receive_datagram(SOCKET a_socket, struct UDPDatagram *a_dgram, struct
    UDPAddress *a_addr, double a_timeout)
{
	fd_set socks;
	struct timeval timeout;
	int ret;

	a_dgram->size = 0;
	FD_ZERO(&socks);
	FD_SET(a_socket, &socks);
	timeout.tv_sec = (long)a_timeout;
	timeout.tv_usec = (long)(1e6 * (a_timeout - timeout.tv_sec));
	ret = select(a_socket + 1, &socks, NULL, NULL, &timeout);
	if (0 == ret) {
		return 1;
	}
	if (SOCKET_ERROR == ret) {
		if (EINTR == errno) {
			return 1;
		}
		warnf("select");
		return 0;
	}
	a_addr->len = sizeof a_addr->addr;
	ZERO(a_addr->addr);
	ret = recvfrom(a_socket, (char *)a_dgram->buf, LENGTH(a_dgram->buf),
	    0, (struct sockaddr *)&a_addr->addr, &a_addr->len);
	if (SOCKET_ERROR == ret) {
		if (EAGAIN == errno || EWOULDBLOCK != errno) {
			return 1;
		}
		warnf("recv");
		return 0;
	}
	a_dgram->size = ret;
	return 1;
}

int
send_datagram(SOCKET a_socket, struct UDPDatagram const *a_dgram)
{
	if (SOCKET_ERROR == send(a_socket, (char *)a_dgram->buf,
	    a_dgram->size, 0)) {
		warnf("send");
		return 0;
	}
	return 1;
}

struct UDPClient *
udp_client_create(int a_flags, char const *a_hostname, uint16_t a_port)
{
	struct addrinfo addri;
	char port_str[10];
	struct addrinfo *result, *p;
	struct UDPClient *client;
	SOCKET sock;
	int ret;

	if (!g_is_setup) {
		fprintf(stderr, "udp_client_create called outside "
		    "udp_setup/udp_shutdown.\n");
		return NULL;
	}

	ZERO(addri);
	addri.ai_family = get_family(a_flags);
	addri.ai_socktype = SOCK_DGRAM;
	addri.ai_protocol = IPPROTO_UDP;
	snprintf(port_str, sizeof port_str, "%d", a_port);
	ret = getaddrinfo(a_hostname, port_str, &addri, &result);
	if (0 != ret) {
		gaif(ret, "%s:%s", a_hostname, port_str);
		return NULL;
	}
	sock = INVALID_SOCKET;
	for (p = result; NULL != p; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (INVALID_SOCKET == sock) {
			warnf("socket(%s:%s)", a_hostname, port_str);
			continue;
		}
		if (0 != connect(sock, p->ai_addr, p->ai_addrlen)) {
			warnf("connect(%s:%s)", a_hostname, port_str);
			close(sock);
			sock = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (INVALID_SOCKET == sock) {
		return NULL;
	}
	if (!set_non_blocking(sock)) {
		warnf("set_non_blocking(%s:%s)", a_hostname, port_str);
		close(sock);
		return NULL;
	}
	CALLOC(client, 1);
	client->socket = sock;
	return client;
}

void
udp_client_free(struct UDPClient **a_client)
{
	struct UDPClient *client;

	client = *a_client;
	if (NULL == client) {
		return;
	}
	close(client->socket);
	FREE(*a_client);
}

int
udp_client_receive(struct UDPClient const *a_client, struct UDPDatagram
    *a_dgram, double a_timeout)
{
	struct UDPAddress addr;

	return receive_datagram(a_client->socket, a_dgram, &addr, a_timeout);
}

int
udp_client_send(struct UDPClient const *a_client, struct UDPDatagram
    const *a_dgram)
{
	return send_datagram(a_client->socket, a_dgram);
}

struct UDPServer *
udp_server_create(int a_flags, uint16_t a_port)
{
	struct addrinfo addri;
	char port_str[10];
	struct addrinfo *result, *p;
	struct UDPServer *server;
	int sock;

	if (!g_is_setup) {
		fprintf(stderr, "udp_client_create called outside "
		    "udp_setup/udp_shutdown.\n");
		return NULL;
	}

	ZERO(addri);
	addri.ai_family = get_family(a_flags);
	addri.ai_socktype = SOCK_DGRAM;
	addri.ai_flags = AI_PASSIVE;
	addri.ai_protocol = IPPROTO_UDP;
	snprintf(port_str, sizeof port_str, "%d", a_port);
	if (0 != getaddrinfo(NULL, port_str, &addri, &result)) {
		warnf("NULL:%s", port_str);
		return NULL;
	}
	sock = INVALID_SOCKET;
	for (p = result; NULL != p; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (INVALID_SOCKET == sock) {
			warnf("socket(NULL:%s)", port_str);
			continue;
		}
		if (0 != bind(sock, p->ai_addr, p->ai_addrlen)) {
			warnf("connect(NULL:%s)", port_str);
			close(sock);
			sock = INVALID_SOCKET;
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (INVALID_SOCKET == sock) {
		fprintf(stderr, "Could not create any socket.\n");
		return NULL;
	}
	if (!set_non_blocking(sock)) {
		warnf("set_non_blocking(NULL:%s)", port_str);
		close(sock);
		return NULL;
	}
	CALLOC(server, 1);
	server->socket = sock;
	return server;
}

void
udp_server_free(struct UDPServer **a_server)
{
	struct UDPServer *server;

	server = *a_server;
	if (NULL == server) {
		return;
	}
	close(server->socket);
	FREE(*a_server);
}

int
udp_server_receive(struct UDPServer const *a_server, struct UDPAddress
    **a_addr, struct UDPDatagram *a_dgram, double a_timeout)
{
	struct UDPAddress addr, *paddr;

	if (!receive_datagram(a_server->socket, a_dgram, &addr, a_timeout)) {
		return 0;
	}
	if (0 != a_dgram->size) {
		CALLOC(paddr, 1);
		COPY(*paddr, addr);
		*a_addr = paddr;
	}
	return 1;
}

int
udp_server_send(struct UDPServer const *a_server, struct UDPDatagram
    const *a_dgram)
{
	return send_datagram(a_server->socket, a_dgram);
}

int
udp_setup()
{
#if defined(_MSC_VER)
	struct WSAData wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		warnf("WSAStartup");
		return 0;
	}
#endif
	g_is_setup = 1;
	return 1;
}

void
udp_shutdown()
{
#if defined(_MSC_VER)
	WSACleanup();
#endif
	g_is_setup = 0;
}

#endif
