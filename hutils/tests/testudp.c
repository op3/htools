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

HTEST(Server)
{
	struct UDPServer *server;

	server = udp_server_create(12345);
	HTRY_PTR(NULL, !=, server);
	udp_server_free(&server);
}

HTEST(Client)
{
	struct UDPClient *client;

	client = udp_client_create("localhost", 12345);
	HTRY_PTR(NULL, !=, client);
	udp_client_free(&client);
}

HTEST(ServerClient)
{
#define STRING "WhereMyDataAt?"
	struct UDPAddress *address;
	struct UDPDatagram datagram;
	struct UDPServer *server;
	char *s;

	/*
	 * NOTE: One cannot reliably test UDP, but localhost should generally
	 * be ok.
	 */

	s = (char *)datagram.buf;
	server = udp_server_create(12345);

	{
		struct UDPClient *client;

		client = udp_client_create("localhost", 12345);
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
	FREE(address);

	ZERO(datagram);
	udp_server_receive(server, &address, &datagram, 0.1);
	HTRY_PTR(NULL, ==, address);
	HTRY_I(0, ==, datagram.size);

	udp_server_free(&server);
}

HTEST_SUITE(UDP)
{
	if (!udp_setup()) {
		fprintf(stderr, "Could not setup UDP.\n");
	}
	HTEST_ADD(Server);
	HTEST_ADD(Client);
	HTEST_ADD(ServerClient);
	udp_shutdown();
}
