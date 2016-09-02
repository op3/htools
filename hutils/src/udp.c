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

#include <sys/fcntl.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <hutils/err.h>
#include <hutils/macros.h>
#include <hutils/memory.h>
#include <hutils/string.h>

struct UDPAddress {
	struct	sockaddr_storage addr;
	socklen_t	len;
};
struct UDPClient {
	int	socket;
};
struct UDPServer {
	int	socket;
};

struct UDPClient *
udp_client_create(char const *const a_hostname, uint16_t const a_port)
{
	struct addrinfo addri;
	char port_str[10];
	struct addrinfo *result, *p;
	struct UDPClient *client;
	int sock;

	ZERO(addri);
	addri.ai_socktype = SOCK_DGRAM;
	addri.ai_protocol = IPPROTO_UDP;
	snprintf(port_str, sizeof port_str, "%d", a_port);
	if (0 != getaddrinfo(a_hostname, port_str, &addri, &result)) {
		warn("%s:%s", a_hostname, port_str);
		return NULL;
	}
	sock = -1;
	for (p = result; NULL != p; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (-1 == sock) {
			warnx("socket(%s:%s)", a_hostname, port_str);
			continue;
		}
		if (0 != connect(sock, p->ai_addr, p->ai_addrlen)) {
			warnx("connect(%s:%s)", a_hostname, port_str);
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (-1 == sock) {
		return NULL;
	}
	if (-1 == fcntl(sock, F_SETFL, O_NONBLOCK)) {
		warnx("fcntl(%s:%s, F_SETFL, O_NONBLOCK)", a_hostname,
		    port_str);
		close(sock);
		return NULL;
	}
	CALLOC(client, 1);
	client->socket = sock;
	return client;
}

void
udp_client_free(struct UDPClient **const a_client)
{
	struct UDPClient *client;

	client = *a_client;
	if (NULL == client) {
		return;
	}
	close(client->socket);
	FREE(*a_client);
}

void
udp_client_receive(struct UDPClient const *const a_client, struct UDPDatagram
    *const a_dgram, double const a_timeout)
{
	fd_set socks;
	struct timeval timeout;
	ssize_t size;
	int ret;

	a_dgram->size = 0;
	FD_ZERO(&socks);
	FD_SET(a_client->socket, &socks);
	timeout.tv_sec = a_timeout;
	timeout.tv_sec = 1e6 * (a_timeout - timeout.tv_sec);
	ret = select(a_client->socket + 1, &socks, NULL, NULL, &timeout);
	if (0 == ret) {
		return;
	}
	if (-1 == ret) {
		if (EINTR == errno) {
			return;
		}
		warn("select");
		return;
	}
	size = recv(a_client->socket, a_dgram->buf, LENGTH(a_dgram->buf), 0);
	if (-1 == size) {
		if (EAGAIN != errno && EWOULDBLOCK != errno) {
			warn("recv");
		}
		return;
	}
	a_dgram->size = size;
}

void
udp_client_send(struct UDPClient const *const a_client, struct UDPDatagram
    const *const a_dgram)
{
	if (-1 == send(a_client->socket, a_dgram->buf, a_dgram->size, 0)) {
		warn("send");
	}
}

struct UDPServer *
udp_server_create(uint16_t const a_port)
{
	struct addrinfo addri;
	char port_str[10];
	struct addrinfo *result, *p;
	struct UDPServer *server;
	int sock;

	ZERO(addri);
	addri.ai_socktype = SOCK_DGRAM;
	addri.ai_flags = AI_PASSIVE;
	addri.ai_protocol = IPPROTO_UDP;
	snprintf(port_str, sizeof port_str, "%d", a_port);
	if (0 != getaddrinfo(NULL, port_str, &addri, &result)) {
		warn("NULL:%s", port_str);
		return NULL;
	}
	sock = -1;
	for (p = result; NULL != p; p = p->ai_next) {
		sock = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
		if (-1 == sock) {
			warnx("socket(NULL:%s)", port_str);
			continue;
		}
		if (0 != bind(sock, p->ai_addr, p->ai_addrlen)) {
			warnx("connect(NULL:%s)", port_str);
			continue;
		}
		break;
	}
	freeaddrinfo(result);
	if (-1 == sock) {
		return NULL;
	}
	if (-1 == fcntl(sock, F_SETFL, O_NONBLOCK)) {
		warnx("fcntl(NULL:%s, F_SETFL, O_NONBLOCK)", port_str);
		close(sock);
		return NULL;
	}
	CALLOC(server, 1);
	server->socket = sock;
	return server;
}

void
udp_server_free(struct UDPServer **const a_server)
{
	struct UDPServer *server;

	server = *a_server;
	if (NULL == server) {
		return;
	}
	close(server->socket);
	FREE(*a_server);
}

void
udp_server_receive(struct UDPServer const *const a_server, struct UDPAddress
    **const a_addr, struct UDPDatagram *const a_dgram, double const a_timeout)
{
	fd_set socks;
	struct timeval timeout;
	struct UDPAddress addr, *paddr;
	ssize_t size;
	int ret;

	*a_addr = NULL;
	a_dgram->size = 0;
	FD_ZERO(&socks);
	FD_SET(a_server->socket, &socks);
	timeout.tv_sec = a_timeout;
	timeout.tv_usec = 1e6 * (a_timeout - timeout.tv_sec);
	ret = select(a_server->socket + 1, &socks, NULL, NULL, &timeout);
	if (0 == ret) {
		return;
	}
	if (-1 == ret) {
		if (EINTR == errno) {
			return;
		}
		warn("select");
		return;
	}
	addr.len = sizeof addr.addr;
	ZERO(addr.addr);
	size = recvfrom(a_server->socket, a_dgram->buf, LENGTH(a_dgram->buf),
	    0, (struct sockaddr *)&addr.addr, &addr.len);
	if (-1 == size) {
		if (EAGAIN != errno && EWOULDBLOCK != errno) {
			warn("recv");
		}
		return;
	}
	CALLOC(paddr, 1);
	COPY(*paddr, addr);
	*a_addr = paddr;
	a_dgram->size = size;
}

void
udp_server_send(struct UDPServer const *const a_server, struct UDPDatagram
    const *const a_dgram)
{
	if (-1 == send(a_server->socket, a_dgram->buf, a_dgram->size, 0)) {
		warn("send");
	}
}

#endif
