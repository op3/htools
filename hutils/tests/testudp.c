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
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <hutils/udp.h>
#include <htest/htest.h>
#include <hutils/memory.h>
#include <hutils/string.h>

HTEST(ServerPreSetup)
{
	struct UDPServer *server;

	server = udp_server_create(UDP_IPV4, 12345);
	HTRY_PTR(NULL, ==, server);
}

HTEST(ClientPreSetup)
{
	struct UDPClient *client;

	client = udp_client_create(UDP_IPV4, "127.0.0.1", 12345);
	HTRY_PTR(NULL, ==, client);
}

HTEST(ServerPostSetup)
{
	struct UDPServer *server;

	server = udp_server_create(UDP_IPV4, 12345);
	HTRY_PTR(NULL, !=, server);
	udp_server_free(&server);
	HTRY_PTR(NULL, ==, server);
}

HTEST(ClientPostSetup)
{
	struct UDPClient *client;

	client = udp_client_create(UDP_IPV4, "127.0.0.1", 12345);
	HTRY_PTR(NULL, !=, client);
	udp_client_free(&client);
	HTRY_PTR(NULL, ==, client);
}

HTEST(ServerClient)
{
#define STRING "WhereMyDataAt?"
	struct UDPAddress *address;
	struct UDPDatagram datagram;
	struct UDPServer *server;
	char *s;

	/*
	 * NOTE:
	 * One cannot really test UDP, but 127.0.0.1 should generally be ok.
	 * IPv4-v6 interoperability is tricky, let's assume v4, also makes
	 * little practical sense to ask server/client what they are since
	 * they should be on different hosts.
	 */

	s = (char *)datagram.buf;
	server = udp_server_create(UDP_IPV4, 12345);

	{
		struct UDPClient *client;

		client = udp_client_create(UDP_IPV4, "127.0.0.1", 12345);
		HTRY_PTR(NULL, != , client);
		strlcpy(s, STRING, sizeof datagram.buf);
		datagram.size = sizeof(STRING);
		udp_client_send(client, &datagram);
		udp_client_free(&client);
	}

	ZERO(datagram);
	udp_server_receive(server, &address, &datagram, 0.1);
	HTRY_PTR(NULL, !=, address);
	HTRY_I(sizeof STRING, ==, datagram.size);
	HTRY_STR(STRING, ==, s);
	udp_address_free(&address);

	ZERO(datagram);
	udp_server_receive(server, &address, &datagram, 0.1);
	HTRY_PTR(NULL, ==, address);
	HTRY_I(0, ==, datagram.size);

	udp_server_free(&server);
}

HTEST(ServerWriting)
{
	struct UDPAddress *address;
	struct UDPDatagram datagram;
	struct UDPServer *server;
	uint8_t value;

	server = udp_server_create(UDP_IPV4, 12345);
	value = 2;
	udp_server_write(server, &value, sizeof value);
	HTRY_BOOL(udp_server_receive(server, &address, &datagram, 1.0));
	HTRY_PTR(NULL, ==, address);
	HTRY_I(1, ==, datagram.size);
	HTRY_I(2, ==, datagram.buf[0]);
	udp_server_free(&server);
}

HTEST_SUITE(UDP)
{
	HTEST_ADD(ServerPreSetup);
	HTEST_ADD(ClientPreSetup);
	if (!udp_setup()) {
		fprintf(stderr, "Could not setup UDP.\n");
	}
	HTEST_ADD(ServerPostSetup);
	HTEST_ADD(ClientPostSetup);
	HTEST_ADD(ServerClient);
	HTEST_ADD(ServerWriting);
	udp_shutdown();
}
