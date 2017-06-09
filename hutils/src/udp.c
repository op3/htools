/*
 * Copyright (c) 2016-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
#include <errno.h>
#if defined(HCONF_mUDP_EVENT_bSYS_SELECT_H) || \
    defined(HCONF_mUDP_EVENT_bSELECT_TIME_H)
#	include <string.h>
#	include <hutils/macros.h>
#endif

#if defined(HCONF_mUDP_LOOKUP_bGETADDRINFO)
#	define GETADDRINFO
#	include <fcntl.h>
#	include <time.h>
#	include <stdarg.h>
#	include <stdio.h>
#	include <unistd.h>
#	include <hutils/err.h>
#	define INVALID_SOCKET -1
#	define SOCKADDR_STORAGE sockaddr_storage
#	define SOCKET int
#	define SOCKET_ERROR -1
FUNC_PRINTF(2, 0) static void
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
#elif defined(_MSC_VER)
#	define GETADDRINFO
#	include <winsock2.h>
#	include <ws2tcpip.h>
#	include <stdio.h>
#	define SOCKADDR_STORAGE sockaddr_storage
#	define close closesocket
static int
set_non_blocking(SOCKET a_socket)
{
	u_long non_blocking;

	non_blocking = 1;
	return NO_ERROR == ioctlsocket(a_socket, FIONBIO, &non_blocking);
}
/*
static void
hutils_warn(char const *a_fmt, ...)
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
*/
#else
#	define GETHOSTBYNAME
#	include <fcntl.h>
#	include <stdarg.h>
#	include <stdio.h>
#	include <unistd.h>
#	include <hutils/err.h>
#	define INVALID_SOCKET -1
#	define SOCKADDR_STORAGE sockaddr_in
#	define SOCKET int
#	define SOCKET_ERROR -1
static int
set_non_blocking(SOCKET a_socket)
{
	return 0 == fcntl(a_socket, F_SETFL, O_NONBLOCK);
}
#endif

/*
 * A few things can happen:
 *  1) Error -> warn(), return 0.
 *  2) Interruption/timeout -> return 0.
 *  3) Socket -> return 1;
 *  4) Extra -> return 2;
 */
static int
event_wait(SOCKET a_socket, int a_fd_extra, double a_timeout)
{
#if defined(HCONF_mUDP_EVENT_bPOLL)
	struct pollfd pfd[2];
	int nfds, ret;

	pfd[0].fd = a_socket;
	pfd[0].events = POLLIN;
	nfds = 1;
	if (0 < a_fd_extra) {
		pfd[1].fd = a_fd_extra;
		pfd[1].events = POLLIN;
		nfds = 2;
	}
	ret = poll(pfd, nfds, a_timeout * 1e3);
	if (-1 == ret && EINTR != errno) {
		hutils_warn("poll");
	}
	if (1 > ret) {
		return 0;
	}
	if (-1 != a_fd_extra && POLLIN & pfd[1].revents) {
		return 2;
	}
	return 1;
#elif defined(HCONF_mUDP_EVENT_bSYS_SELECT_H) || \
	defined(HCONF_mUDP_EVENT_bSELECT_TIME_H)
	fd_set socks;
	struct timeval timeout;
	int nfds, ret;

	FD_ZERO(&socks);
	FD_SET(a_socket, &socks);
	if (0 < a_fd_extra) {
		FD_SET(a_fd_extra, &socks);
	}
	nfds = MAX(a_socket, a_fd_extra);
	timeout.tv_sec = (long)a_timeout;
	timeout.tv_usec = (long)(1e6 * (a_timeout - timeout.tv_sec));
	ret = select(nfds + 1, &socks, NULL, NULL, &timeout);
	if (-1 == ret && EINTR != errno) {
		hutils_warn("select");
	}
	if (1 > ret) {
		return 0;
	}
	if (-1 != a_fd_extra && FD_ISSET(a_fd_extra, &socks)) {
		return 2;
	}
	return 1;
#endif
}

#include <hutils/err.h>
#include <hutils/macros.h>
#include <hutils/memory.h>
#include <hutils/string.h>

struct UDPAddress {
	struct	SOCKADDR_STORAGE addr;
	socklen_t	len;
};
struct UDPClient {
	SOCKET	socket;
};
struct UDPServer {
	SOCKET	socket;
	int	pfd[2];
};

static int	get_family(int) FUNC_RETURNS;
static int	receive_datagram(SOCKET, int, struct UDPDatagram *, struct
    UDPAddress **, double);

static int g_is_setup;

int
get_family(int a_flags)
{
#if defined(AF_INET6)
	return UDP_IPV4 == ((UDP_IPV4 | UDP_IPV6) & a_flags) ? AF_INET :
	    AF_INET6;
#else
	if (UDP_IPV4 != a_flags) {
		hutils_warnx("Platform only support IPv4, something else "
		    "requested.");
	}
	return AF_INET;
#endif
}

/*
 * Return:
 *  1) Error -> return 0.
 *  2) Timeout -> return 1, size = 0.
 *  3) Data -> return 1, size != 0.
 */
int
receive_datagram(SOCKET a_socket, int a_fd_extra, struct UDPDatagram *a_dgram,
    struct UDPAddress **a_addr, double a_timeout)
{
	struct UDPAddress addr;
	int ret;

	if (NULL != a_addr) {
		*a_addr = NULL;
	}
	a_dgram->size = 0;
	ret = event_wait(a_socket, a_fd_extra, a_timeout);
	if (0 == ret) {
		return 1;
	}
	if (2 == ret) {
		a_dgram->size = read(a_fd_extra, a_dgram->buf,
		    LENGTH(a_dgram->buf));
		return 1;
	}
	addr.len = sizeof addr.addr;
	ZERO(addr.addr);
	ret = recvfrom(a_socket, (char *)a_dgram->buf, LENGTH(a_dgram->buf),
	    0, (struct sockaddr *)&addr.addr, &addr.len);
	if (SOCKET_ERROR == ret) {
		if (EAGAIN == errno || EWOULDBLOCK == errno) {
			return 1;
		}
		hutils_warn("recvfrom");
		return 0;
	}
	a_dgram->size = ret;
	if (0 != ret && NULL != a_addr) {
		struct UDPAddress *paddr;

		CALLOC(paddr, 1);
		COPY(*paddr, addr);
		*a_addr = paddr;
	}
	return 1;
}

void
udp_address_free(struct UDPAddress **a_address)
{
	FREE(*a_address);
}

struct UDPClient *
udp_client_create(int a_flags, char const *a_hostname, uint16_t a_port)
{
	struct UDPClient *client;
	SOCKET sock;

	if (!g_is_setup) {
		fprintf(stderr, "udp_client_create called outside "
		    "udp_setup/udp_shutdown.\n");
		return NULL;
	}

#if defined(GETADDRINFO)
	{
		struct addrinfo addri;
		char port_str[10];
		struct addrinfo *result, *p;
		int ret;

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
			sock = socket(p->ai_family, p->ai_socktype,
			    p->ai_protocol);
			if (INVALID_SOCKET == sock) {
				hutils_warn("socket(%s:%s)", a_hostname,
				    port_str);
				continue;
			}
			if (0 != connect(sock, p->ai_addr, p->ai_addrlen)) {
				hutils_warn("connect(%s:%s)", a_hostname,
				    port_str);
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
	}
#else
	{
		struct SOCKADDR_STORAGE server;
		struct hostent *host;

		/* Not const?! Wtf... */
		host = gethostbyname((char *)a_hostname);
		if (NULL == host) {
			fprintf(stderr, "gethostbyname(%s): %s.\n",
			    a_hostname, strerror(h_errno));
			return NULL;
		}
		sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (0 > sock) {
			fprintf(stderr, "socket: %s.\n", strerror(errno));
			return NULL;
		}
		server.sin_family = get_family(a_flags);
		memcpy(&server.sin_addr.s_addr, host->h_addr, host->h_length);
		server.sin_port = htons(a_port);
		if (0 != connect(sock, (void *)&server, sizeof server)) {
			fprintf(stderr, "connect: %s.\n", strerror(errno));
			close(sock);
			return NULL;
		}
	}
#endif
	if (!set_non_blocking(sock)) {
		hutils_warn("set_non_blocking(%s:%d)", a_hostname, a_port);
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
	return receive_datagram(a_client->socket, -1, a_dgram, NULL,
	    a_timeout);
}

int
udp_client_send(struct UDPClient const *a_client, struct UDPDatagram const
    *a_dgram)
{
	union {
		uint8_t	const *c;
		uint8_t	*u;
	} u;

	u.c = a_dgram->buf;
	/* TODO: SOCKET_ERROR in Windows, -1 everywhere else... */
	if (SOCKET_ERROR == send(a_client->socket, u.u, a_dgram->size, 0)) {
		hutils_warn("send");
		return 0;
	}
	return 1;
}

struct UDPServer *
udp_server_create(int a_flags, uint16_t a_port)
{
	struct UDPServer *server;
	int flags, has_pfd, sock;

	if (!g_is_setup) {
		fprintf(stderr, "udp_server_create called outside "
		    "udp_setup/udp_shutdown.\n");
		return NULL;
	}

	has_pfd = 0;
	sock = INVALID_SOCKET;
	CALLOC(server, 1);
	if (-1 == pipe(server->pfd)) {
		hutils_warn("pipe");
		goto udp_server_create_fail;
	}
	has_pfd = 1;
	flags = fcntl(server->pfd[0], F_GETFL);
	if (-1 == flags) {
		hutils_warn("fcntl_get(pfd[0])");
		goto udp_server_create_fail;
	}
	if (-1 == fcntl(server->pfd[0], F_SETFL, flags | O_NONBLOCK)) {
		hutils_warn("fcntl_set(pfd[0])");
		goto udp_server_create_fail;
	}
	flags = fcntl(server->pfd[1], F_GETFL);
	if (-1 == flags) {
		hutils_warn("fcntl_get(pfd[1])");
		goto udp_server_create_fail;
	}
	if (-1 == fcntl(server->pfd[1], F_SETFL, flags | O_NONBLOCK)) {
		hutils_warn("fcntl_set(pfd[1])");
		goto udp_server_create_fail;
	}

#if defined(GETADDRINFO)
	{
		struct addrinfo addri;
		char port_str[10];
		struct addrinfo *result, *p;

		snprintf(port_str, sizeof port_str, "%d", a_port);
		ZERO(addri);
		addri.ai_family = get_family(a_flags);
		addri.ai_socktype = SOCK_DGRAM;
		addri.ai_flags = AI_PASSIVE;
		addri.ai_protocol = IPPROTO_UDP;
		if (0 != getaddrinfo(NULL, port_str, &addri, &result)) {
			/* TODO Must use gai_strerror! */
			hutils_warn("NULL:%s", port_str);
			goto udp_server_create_fail;
		}
		for (p = result; NULL != p; p = p->ai_next) {
			sock = socket(p->ai_family, p->ai_socktype,
			    p->ai_protocol);
			if (INVALID_SOCKET == sock) {
				hutils_warn("socket(NULL:%s)", port_str);
				continue;
			}
			if (0 != bind(sock, p->ai_addr, p->ai_addrlen)) {
				hutils_warn("connect(NULL:%s)", port_str);
				close(sock);
				sock = INVALID_SOCKET;
				continue;
			}
			break;
		}
		freeaddrinfo(result);
		if (INVALID_SOCKET == sock) {
			hutils_warnx("Could not create socket.");
			goto udp_server_create_fail;
		}
	}
#else
	{
		struct SOCKADDR_STORAGE me;

		sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
		if (0 > sock) {
			fprintf(stderr, "socket: %s.", strerror(errno));
			return NULL;
		}
		ZERO(me);
		me.sin_family = get_family(a_flags);
		me.sin_addr.s_addr = htonl(INADDR_ANY);
		me.sin_port = htons(a_port);
		if (0 != bind(sock, (void *)&me, sizeof me)) {
			fprintf(stderr, "bind: %s.", strerror(errno));
			close(sock);
			return NULL;
		}
	}
#endif
	if (!set_non_blocking(sock)) {
		hutils_warn("set_non_blocking(NULL:%d)", a_port);
		close(sock);
		return NULL;
	}
	server->socket = sock;
	return server;

udp_server_create_fail:
	if (INVALID_SOCKET != sock) {
		close(sock);
	}
	if (has_pfd) {
		close(server->pfd[0]);
		close(server->pfd[1]);
	}
	FREE(server);
	return NULL;
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
	close(server->pfd[0]);
	close(server->pfd[1]);
	FREE(*a_server);
}

int
udp_server_receive(struct UDPServer const *a_server, struct UDPAddress
    **a_addr, struct UDPDatagram *a_dgram, double a_timeout)
{
	return receive_datagram(a_server->socket, a_server->pfd[0], a_dgram,
	    a_addr, a_timeout);
}

int
udp_server_send(struct UDPServer const *a_server, struct UDPAddress const
    *a_addr, struct UDPDatagram const *a_dgram)
{
	union {
		uint8_t	const *c;
		uint8_t	*u;
	} u;

	u.c = a_dgram->buf;
	if (SOCKET_ERROR == sendto(a_server->socket, u.u, a_dgram->size, 0,
	    (struct sockaddr const *)&a_addr->addr, a_addr->len)) {
		hutils_warn("send");
		return 0;
	}
	return 1;
}

int
udp_server_write(struct UDPServer const *a_server, void const *a_data, size_t
    a_data_len)
{
	if (-1 == write(a_server->pfd[1], a_data, a_data_len)) {
		hutils_warn("write(pfd[1])");
		return 0;
	}
	return 1;
}

int
udp_setup()
{
#if defined(_MSC_VER)
	struct WSAData wsa;

	if (WSAStartup(MAKEWORD(2, 2), &wsa)) {
		hutils_warn("WSAStartup");
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
