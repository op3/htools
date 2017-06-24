/*
 * Copyright (c) 2015-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
#include <hutils/string.h>

static char *get_tmp_path(void);

char *
get_tmp_path()
{
	char const *tmpdir;
	char *path;

	tmpdir = getenv("TMPDIR");
	if (NULL == tmpdir) {
		tmpdir = "/tmp";
	}
	path = NULL;
	STRCTV_BEGIN &path, tmpdir, "/hutils_testconfig.cfg" STRCTV_END;
	return path;
}

HTEST(MissingFileOk)
{
	struct ConfigCollection *coll;

	coll = config_collection_load_from_file("nada.cfg");
	HTRY_PTR(NULL, !=, coll);
	config_collection_free(&coll);
}

HTEST(SectionAutoCreate)
{
	char const c_cfg[] = "";
	struct ConfigCollection *coll;
	struct ConfigSection *sec;

	coll = config_collection_load_from_memory(c_cfg, 0);
	sec = config_collection_get_section(coll, "section");
	HTRY_PTR(NULL, !=, sec);
	config_collection_free(&coll);
}

HTEST(DefaultGet)
{
	char const c_cfg[] = "";
	struct ConfigCollection *coll;
	struct ConfigSection *sec;
	struct Config *cfg;

	coll = config_collection_load_from_memory(c_cfg, 0);
	sec = config_collection_get_section(coll, "section");

	cfg = config_section_getd(sec, "d", 3.14);
	HTRY_DBL(3.14, ==, config_getd(cfg));
	HTRY_I(3, ==, config_geti32(cfg));
	HTRY_STR("3.14", ==, config_gets(cfg));

	cfg = config_section_getd(sec, "d", 2.72);
	HTRY_DBL(3.14, ==, config_getd(cfg));
	HTRY_I(3, ==, config_geti32(cfg));
	HTRY_STR("3.14", ==, config_gets(cfg));

	cfg = config_section_geti32(sec, "i32", -1);
	HTRY_DBL(-1.0, ==, config_getd(cfg));
	HTRY_I(-1, ==, config_geti32(cfg));
	HTRY_STR("-1", ==, config_gets(cfg));

	cfg = config_section_geti32(sec, "i32", 666);
	HTRY_DBL(-1.0, ==, config_getd(cfg));
	HTRY_I(-1, ==, config_geti32(cfg));
	HTRY_STR("-1", ==, config_gets(cfg));

	cfg = config_section_gets(sec, "s", "8ball");
	HTRY_DBL(8.0, ==, config_getd(cfg));
	HTRY_I(8, ==, config_geti32(cfg));
	HTRY_STR("8ball", ==, config_gets(cfg));

	cfg = config_section_gets(sec, "s", "bananas");
	HTRY_DBL(8.0, ==, config_getd(cfg));
	HTRY_I(8, ==, config_geti32(cfg));
	HTRY_STR("8ball", ==, config_gets(cfg));

	config_collection_free(&coll);
}

HTEST(MissingSection)
{
	char const c_cfg[] = "config=value";
	struct ConfigCollection *coll;

	coll = config_collection_load_from_memory(c_cfg, strlen(c_cfg));
	HTRY_PTR(NULL, ==, coll);
	config_collection_free(&coll);
}

HTEST(MixedSections)
{
	char const c_cfg1[] = "[section";
	char const c_cfg2[] = "section]";
	char const c_cfg3[] = "[]";
	char const c_cfg4[] = "[section]";
	struct ConfigCollection *coll;

	coll = config_collection_load_from_memory(c_cfg1, strlen(c_cfg1));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg2, strlen(c_cfg2));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg3, strlen(c_cfg3));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg4, strlen(c_cfg4));
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

	coll = config_collection_load_from_memory(c_cfg1, strlen(c_cfg1));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg2, strlen(c_cfg2));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg3, strlen(c_cfg3));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg4, strlen(c_cfg4));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg5, strlen(c_cfg5));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfg6, strlen(c_cfg6));
	HTRY_PTR(NULL, !=, coll);
	config_collection_free(&coll);
}

HTEST(MixedValues)
{
	char const c_cfgp0[] = "[p0] a0=0 a1=0. a2=.0 a3=+0 a4=-0";
	char const c_cfgp1[] = "[p1] a0=0e0 a1=0.e0 a2=.0e0 a3=0e+0 a4=0e-0";
	char const c_cfgp2[] = "[p2] s=\"a\"";
	char const c_cfgf0[] = "[f0] a=++0";
	char const c_cfgf1[] = "[f1] a=--0";
	char const c_cfgf2[] = "[f3] a=0e";
	char const c_cfgf3[] = "[f4] s=a\"";
	char const c_cfgf4[] = "[f5] s=\"a";
	struct ConfigCollection *coll;

	coll = config_collection_load_from_memory(c_cfgp0, strlen(c_cfgp0));
	HTRY_PTR(NULL, !=, coll);

	coll = config_collection_load_from_memory(c_cfgp1, strlen(c_cfgp1));
	HTRY_PTR(NULL, !=, coll);

	coll = config_collection_load_from_memory(c_cfgp2, strlen(c_cfgp2));
	HTRY_PTR(NULL, !=, coll);

	coll = config_collection_load_from_memory(c_cfgf0, strlen(c_cfgf0));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfgf1, strlen(c_cfgf1));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfgf2, strlen(c_cfgf2));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfgf3, strlen(c_cfgf3));
	HTRY_PTR(NULL, ==, coll);

	coll = config_collection_load_from_memory(c_cfgf4, strlen(c_cfgf4));
	HTRY_PTR(NULL, ==, coll);

	config_collection_free(&coll);
}

HTEST(ValueConversion)
{
	struct ConfigCollection *coll;
	struct ConfigSection *section;
	struct Config *config;

	coll = config_collection_load_from_file("tests/values.cfg");
	section = config_collection_get_section(coll, "Values");

	config = config_section_getd(section, "a", 0.0);
	HTRY_I(1, ==, config_geti32(config));
	HTRY_DBL(1.0, ==, config_getd(config));
	HTRY_STR("1", ==, config_gets(config));

	config = config_section_getd(section, "b", 0.0);
	HTRY_I(1, ==, config_geti32(config));
	HTRY_DBL(1.1, ==, config_getd(config));
	HTRY_STR("1.1", ==, config_gets(config));

	config = config_section_getd(section, "c", 0.0);
	HTRY_I(0, ==, config_geti32(config));
	HTRY_DBL(1e-4, ==, config_getd(config));
	HTRY_STR(".1e-3", ==, config_gets(config));

	config = config_section_getd(section, "d", 0.0);
	HTRY_I(1, ==, config_geti32(config));
	HTRY_DBL(1.1, ==, config_getd(config));
	HTRY_STR("1.1a", ==, config_gets(config));

	config = config_section_getd(section, "e", 0.0);
	HTRY_I(0, ==, config_geti32(config));
	HTRY_DBL(0.0, ==, config_getd(config));
	HTRY_STR("a1.1", ==, config_gets(config));

	config_collection_free(&coll);
}

HTEST(Duplications)
{
	char const c_cfg_no1[] = "[section] [section]";
	char const c_cfg_no2[] = "[section] config=0 config=1";
	char const c_cfg_yes1[] = "[section1] config=0 [section2] config=1";
	struct ConfigCollection *coll;

	coll = config_collection_load_from_memory(c_cfg_no1,
	    strlen(c_cfg_no1));
	HTRY_PTR(NULL, ==, coll);
	coll = config_collection_load_from_memory(c_cfg_no2,
	    strlen(c_cfg_no2));
	HTRY_PTR(NULL, ==, coll);
	coll = config_collection_load_from_memory(c_cfg_yes1,
	    strlen(c_cfg_yes1));
	HTRY_PTR(NULL, !=, coll);
	config_collection_free(&coll);
}

HTEST(WriteEmpty)
{
	char const c_cfg[] = "";
	struct ConfigCollection *coll;
	char *path;

	coll = config_collection_load_from_memory(c_cfg, 0);
	path = get_tmp_path();
	HTRY_BOOL(config_collection_write(coll, path));
	free(path);
	config_collection_free(&coll);
}

HTEST(WriteLoad)
{
	char const c_cfg[] = "[section] config=1";
	struct ConfigCollection *coll;
	struct ConfigSection *section;
	struct Config *config;
	char *path;

	path = get_tmp_path();

	coll = config_collection_load_from_memory(c_cfg, strlen(c_cfg));
	section = config_collection_get_section(coll, "section");
	config = config_section_geti32(section, "config", 0);
	HTRY_I(1, ==, config_geti32(config));
	HTRY_I(1, ==, config_collection_write(coll, path));
	config_collection_free(&coll);

	coll = config_collection_load_from_file(path);
	section = config_collection_get_section(coll, "section");
	config = config_section_geti32(section, "config", 0);
	HTRY_I(1, ==, config_geti32(config));
	config_collection_free(&coll);

	free(path);
}

/*HTEST(WriteStrangePath)
{
	char const c_cfg[] = "[section]";
	struct ConfigCollection *coll;

	coll = config_collection_load_from_memory(c_cfg, 0);
	HTRY_BOOL(!config_collection_write(coll,
	    "/user_must_not_write_here.cfg"));
	config_collection_free(&coll);
}*/

HTEST_SUITE(Config)
{
	HTEST_ADD(MissingFileOk);
	HTEST_ADD(SectionAutoCreate);
	HTEST_ADD(DefaultGet);
	HTEST_ADD(MissingSection);
	HTEST_ADD(MixedSections);
	HTEST_ADD(MixedConfigs);
	HTEST_ADD(MixedValues);
	HTEST_ADD(ValueConversion);
	HTEST_ADD(Duplications);
	HTEST_ADD(WriteEmpty);
	HTEST_ADD(WriteLoad);
/*	HTEST_ADD(WriteStrangePath);*/
}
