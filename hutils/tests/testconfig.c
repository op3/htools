/*
 * Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

#include <hutils/config.h>
#include <htest/htest.h>

HTEST(MissingSection)
{
	char const c_cfg[] = "name=value";
	struct ConfigCollection *coll;

	coll = config_collection_load_from_memory(c_cfg, 0);
	HTRY_PTR(NULL, ==, coll);
	config_collection_free(&coll);
}

HTEST(MixedSections)
{
	char const c_cfg1[] = "[section";
	char const c_cfg2[] = "section]";
	char const c_cfg3[] = "[section]";
	struct ConfigCollection *coll;

	coll = config_collection_load_from_memory(c_cfg1, 0);
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg2, 0);
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg3, 0);
	HTRY_PTR(NULL, !=, coll);
	config_collection_free(&coll);
}

HTEST(MixedConfigs)
{
	char const c_cfg1[] = "[section] name";
	char const c_cfg2[] = "[section] =";
	char const c_cfg3[] = "[section] name=";
	char const c_cfg4[] = "[section] name value";
	char const c_cfg5[] = "[section] =value";
	char const c_cfg6[] = "[section] name=value";
	struct ConfigCollection *coll;

	coll = config_collection_load_from_memory(c_cfg1, 0);
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg2, 0);
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg3, 0);
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg4, 0);
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg5, 0);
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg6, 0);
	HTRY_PTR(NULL, !=, coll);
	config_collection_free(&coll);
}

HTEST(ValueConversion)
{
	struct ConfigCollection *coll;
	struct ConfigSection *section;
	struct Config *config;

	coll = config_collection_load_from_file("tests/values.cfg");
	section = config_collection_get_section(coll, "Values");

	config = config_section_get_config(section, "a");
	HTRY_I(1, ==, config_geti32(config));
	HTRY_DBL(1.0, ==, config_getd(config));
	HTRY_STR("1", ==, config_gets(config));

	config = config_section_get_config(section, "b");
	HTRY_I(1, ==, config_geti32(config));
	HTRY_DBL(1.1, ==, config_getd(config));
	HTRY_STR("1.1", ==, config_gets(config));

	config = config_section_get_config(section, "c");
	HTRY_I(0, ==, config_geti32(config));
	HTRY_DBL(1e-4, ==, config_getd(config));
	HTRY_STR(".1e-3", ==, config_gets(config));

	config = config_section_get_config(section, "d");
	HTRY_I(1, ==, config_geti32(config));
	HTRY_DBL(1.1, ==, config_getd(config));
	HTRY_STR("1.1a", ==, config_gets(config));

	config = config_section_get_config(section, "e");
	HTRY_I(0, ==, config_geti32(config));
	HTRY_DBL(0.0, ==, config_getd(config));
	HTRY_STR("a1.1", ==, config_gets(config));

	config_collection_free(&coll);
}

HTEST_SUITE(Config)
{
	HTEST_ADD(MissingSection);
	HTEST_ADD(MixedSections);
	HTEST_ADD(MixedConfigs);
	HTEST_ADD(ValueConversion);
}
