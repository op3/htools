/*
 * Copyright (c) 2015-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/random.h>

double
random_getd(struct RandomSeed *a_seed)
{
	double d1, d2;

	d1 = random_getu32(a_seed) / 4294967296.0;
	d2 = random_getu32(a_seed) / (4294967296.0 * 4294967296.0);
	return d1 + d2;
}

float
random_getf(struct RandomSeed *a_seed)
{
	return random_getu32(a_seed) / 4294967296.0f;
}

/* By George Marsaglia. */
uint32_t
random_getu32(struct RandomSeed *a_seed)
{
	uint32_t *seed;
	uint32_t t;
	int i;

	seed = a_seed->seed;
	t = seed[0] ^ (seed[0] >> 7);
	for (i = 0; 4 > i; ++i) {
		seed[i] = seed[i + 1];
	}
	seed[4] = (seed[4] ^ (seed[4] << 6)) ^ (t ^ (t << 13));
	return (seed[1] + seed[1] + 1) * seed[4];
}
