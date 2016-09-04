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

#include <hutils/aabb3.h>
#include <math.h>
#include <htest/htest.h>

HTEST(Min)
{
	struct AABB3f const c_aabb = {
		{1.0f, 2.0f, 3.0f},
		{4.0f, 5.0f, 6.0f}
	};
	struct Vector3f const c_v0 = {2.0f, 3.0f, 4.0f};
	struct Vector3f const c_v1 = {2.0f, 3.0f, 7.0f};
	struct Vector3f const c_v2 = {5.0f, 3.0f, 7.0f};
	struct Vector3f const c_v3 = {5.0f, 6.0f, 7.0f};

	HTRY_FLT(0.0f, ==, aabb3f_get_distance(&c_aabb, &c_v0));
	HTRY_FLT(1.0f, ==, aabb3f_get_distance(&c_aabb, &c_v1));
	HTRY_FLT((float)sqrt(2), ==, aabb3f_get_distance(&c_aabb, &c_v2));
	HTRY_FLT((float)sqrt(3), ==, aabb3f_get_distance(&c_aabb, &c_v3));
}

HTEST_SUITE(AABB3)
{
	HTEST_ADD(Min);
}
