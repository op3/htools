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

#ifndef HUTILS_UDP_H
#define HUTILS_UDP_H

#include <hconf/include/hutils/udp.h>
#include <stdlib.h>
#include <hutils/cdecls.h>
#include <hutils/funcattr.h>
#include <hutils/stdint.h>

#if defined(HCONF_mSOCKET_H_bSOCKET_H)
#	include <socket.h>
#elif defined(HCONF_mSOCKET_H_bSYS_SOCKET_H)
#	include <sys/socket.h>
#elif defined(HCONF_mSOCKET_H_bNONE)
#endif

#if defined(HCONF_mIPPROTO_UDP_bNETINET_IN_H)
#	include <netinet/in.h>
#elif defined(HCONF_mIPPROTO_UDP_bNOWARN_NETINET_IN_H)
/* HCONF_CPPFLAGS=-D__NO_INCLUDE_WARN__ */
#	include <netinet/in.h>
#endif
#if defined(HCONFING_mIPPROTO_UDP)
HCONF_TEST
{
	return IPPROTO_UDP;
}
#endif

#if defined(HCONF_mUDP_LOOKUP_bGETADDRINFO)
#	include <netdb.h>
#	if defined(HCONFING_mUDP_LOOKUP)
HCONF_TEST
{
	return getaddrinfo(0, 0, 0, 0);
}
#	endif
#elif defined(HCONF_mUDP_LOOKUP_bGETHOSTBYNAME_SOCKLEN)
/* HCONF_LIBS=-lnetinet */
#	include <netdb.h>
#	define socklen_t int
#	if defined(HCONFING_mUDP_LOOKUP)
HCONF_TEST
{
	socklen_t len;

	gethostbyname(0);
	return recvfrom(0, 0, 0, 0, 0, &len);
}
#	endif
#endif

#if defined(HCONF_mUDP_EVENT_bPOLL)
#	include <poll.h>
#	if defined(HCONFING_mUDP_EVENT)
#		define HCONF_TEST_RUN
HCONF_TEST
{
	struct pollfd fds[1];
	fds[0].fd = 0;
	fds[0].events = POLLIN;
	return -1 != poll(fds, 1, 0);
}
#	endif
#elif defined(HCONF_mUDP_EVENT_bSYS_SELECT_H)
#	include <sys/select.h>
#	if defined(HCONFING_mUDP_EVENT)
HCONF_TEST
{
	return select(0, NULL, NULL, NULL, NULL);
}
#	endif
#elif defined(HCONF_mUDP_EVENT_bSELECT_TIME_H)
#	include <time.h>
#	if defined(HCONFING_mUDP_EVENT)
HCONF_TEST
{
	return select(0, NULL, NULL, NULL, NULL);
}
#	endif
#endif

CDECLS_BEGIN

enum {
	UDP_IPV4 = 0x0,
	UDP_IPV6 = 0x1
};

struct UDPAddress;
struct UDPClient;
struct UDPServer;
struct UDPDatagram {
	uint8_t	buf[512];
	size_t	size;
};

void			udp_address_free(struct UDPAddress **);
struct UDPClient	*udp_client_create(int, char const *, uint16_t)
	FUNC_RETURNS;
void			udp_client_free(struct UDPClient **);
int			udp_client_receive(struct UDPClient const *, struct
    UDPDatagram *, double);
int			udp_client_send(struct UDPClient const *, struct
    UDPDatagram const *);
struct UDPServer	*udp_server_create(int, uint16_t) FUNC_RETURNS;
void			udp_server_free(struct UDPServer **);
int			udp_server_receive(struct UDPServer const *, struct
    UDPAddress **, struct UDPDatagram *, double);
int			udp_server_send(struct UDPServer const *, struct
    UDPAddress const *, struct UDPDatagram const *);
int			udp_server_write(struct UDPServer const *, void const
    *, size_t);
int			udp_setup(void) FUNC_RETURNS;
void			udp_shutdown(void);

CDECLS_END

#endif
