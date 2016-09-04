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

#include <hutils/getopt.h>
#include <htest/htest.h>

HTEST(None)
{
	char *argv[1];

	argv[0] = "arg0";
	optind = 1;
	HTRY_I(-1, ==, getopt(LENGTH(argv), argv, "a"));
}

HTEST(SinglesAndCombos)
{
	char *argv[5];

	argv[0] = "arg0";
	argv[1] = "-a";
	argv[2] = "-b";
	argv[3] = "-ab";
	argv[4] = "-c";
	optind = 1;
	HTRY_I('a', ==, getopt(LENGTH(argv), argv, "ab"));
	HTRY_I('b', ==, getopt(LENGTH(argv), argv, "ab"));
	HTRY_I('a', ==, getopt(LENGTH(argv), argv, "ab"));
	HTRY_I('b', ==, getopt(LENGTH(argv), argv, "ab"));
	HTRY_I('?', ==, getopt(LENGTH(argv), argv, "ab"));
}

HTEST(DoubleDash)
{
	char *argv[4];

	argv[0] = "arg0";
	argv[1] = "-a";
	argv[2] = "--";
	argv[3] = "-b";
	optind = 1;
	HTRY_I('a', ==, getopt(LENGTH(argv), argv, "ab"));
	HTRY_I(-1, ==, getopt(LENGTH(argv), argv, "ab"));
}

HTEST(Values)
{
	char *argv[4];

	argv[0] = "arg0";
	argv[1] = "-a";
	argv[2] = "value1";
	argv[3] = "-avalue2";
	optind = 1;
	HTRY_I('a', ==, getopt(LENGTH(argv), argv, "a:b"));
	HTRY_STR("value1", ==, optarg);
	HTRY_I('a', ==, getopt(LENGTH(argv), argv, "a:b"));
	HTRY_STR("value2", ==, optarg);
}

HTEST_SUITE(Getopt)
{
	HTEST_ADD(None);
	HTEST_ADD(SinglesAndCombos);
	HTEST_ADD(DoubleDash);
	HTEST_ADD(Values);
}
