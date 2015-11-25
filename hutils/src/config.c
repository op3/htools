#include <hutils/config.h>
#include <sys/queue.h>
#include <errno.h>
#include <stdio.h>
#include <hutils/lexer.h>
#include <hutils/memory.h>

TAILQ_HEAD(ConfigList, Config);
struct Config {
	char	*name;
	double	d;
	int32_t	i32;
	char	*str;
	TAILQ_ENTRY(Config)	next;
};
TAILQ_HEAD(SectionList, ConfigSection);
struct ConfigSection {
	char	*name;
	struct	ConfigList config_list;
	TAILQ_ENTRY(ConfigSection)	next;
};
struct ConfigCollection {
	struct	SectionList section_list;
};
struct MemoryData {
	char	const *buf;
	size_t	len;
	size_t	ofs;
};

struct Config		*create_config(struct ConfigSection *, char const *)
	FUNC_RETURNS;
struct ConfigSection	*create_section(struct ConfigCollection *, char const
    *) FUNC_RETURNS;
struct Config		*get_config(struct ConfigSection const *, char const
    *) FUNC_RETURNS;
struct ConfigSection	*get_section(struct ConfigCollection const *, char
    const *) FUNC_RETURNS;
struct ConfigCollection	*load(struct Lexer *) FUNC_RETURNS;
static size_t		memory_read(void *, char *, size_t) FUNC_RETURNS;

void
config_collection_free(struct ConfigCollection **const a_coll)
{
	struct ConfigCollection *coll;

	coll = *a_coll;
	if (NULL == coll) {
		return;
	}
	while (!TAILQ_EMPTY(&coll->section_list)) {
		struct ConfigSection *section;

		section = TAILQ_FIRST(&coll->section_list);
		TAILQ_REMOVE(&coll->section_list, section, next);
		while (!TAILQ_EMPTY(&section->config_list)) {
			struct Config *config;

			config = TAILQ_FIRST(&section->config_list);
			TAILQ_REMOVE(&section->config_list, config, next);
			FREE(config->name);
			FREE(config->str);
			FREE(config);
		}
		FREE(section->name);
		FREE(section);
	}
	FREE(*a_coll);
}

struct ConfigSection *
config_collection_get_section(struct ConfigCollection *const a_coll, char
    const *const a_name)
{
	struct ConfigSection *section;

	section = get_section(a_coll, a_name);
	if (NULL == section) {
		section = create_section(a_coll, a_name);
	}
	return section;
}

struct ConfigCollection *
config_collection_load_from_file(char const *const a_path)
{
	FILE *file;
	struct Lexer *lexer;
	struct ConfigCollection *coll;

	file = fopen(a_path, "rb");
	if (NULL == file) {
		CALLOC(coll, 1);
		TAILQ_INIT(&coll->section_list);
	} else {
		lexer = lexer_create(NULL, file);
		coll = load(lexer);
		fclose(file);
	}
	return coll;
}

struct ConfigCollection *
config_collection_load_from_memory(char const *const a_buf, size_t const
    a_len)
{
	struct MemoryData memory_data;
	struct Lexer *lexer;
	struct ConfigCollection *coll;

	memory_data.buf = a_buf;
	memory_data.len = 0 == a_len ? strlen(a_buf) : a_len;
	memory_data.ofs = 0;
	lexer = lexer_create(memory_read, &memory_data);
	coll = load(lexer);
	return coll;
}

int
config_collection_write(struct ConfigCollection const *const a_coll, char
    const *const a_path)
{
	time_t tt;
	struct tm tm;
	char buf[26];
	FILE *file;
	struct ConfigSection *section;

	if (NULL == a_coll) {
		return 0;
	}

	file = fopen(a_path, "wb");
	if (NULL == file) {
		return 0;
	}
	time(&tt);
	gmtime_r(&tt, &tm);
	fprintf(file, "# Created %s", asctime_r(&tm, buf));
	TAILQ_FOREACH(section, &a_coll->section_list, next) {
		struct Config *config;

		fprintf(file, "\n[%s]\n", section->name);
		TAILQ_FOREACH(config, &section->config_list, next) {
			fprintf(file, "%s=%s\n", config->name, config->str);
		}
	}
	return 1;
}

double
config_getd(struct Config const *const a_config)
{
	return a_config->d;
}

int32_t
config_geti32(struct Config const *const a_config)
{
	return a_config->i32;
}

char const *
config_gets(struct Config const *const a_config)
{
	return a_config->str;
}

struct Config *
config_section_getd_config(struct ConfigSection *const a_section, char const
    *const a_name, double const a_d)
{
	struct Config *config;

	config = get_config(a_section, a_name);
	if (NULL == config) {
		config = create_config(a_section, a_name);
		config_setd(config, a_d);
	}
	return config;
}

struct Config *
config_section_geti32_config(struct ConfigSection *const a_section, char const
    *const a_name, int32_t const a_i32)
{
	struct Config *config;

	config = get_config(a_section, a_name);
	if (NULL == config) {
		config = create_config(a_section, a_name);
		config_seti32(config, a_i32);
	}
	return config;
}

struct Config *
config_section_gets_config(struct ConfigSection *const a_section, char const
    *const a_name, char const *a_s)
{
	struct Config *config;

	config = get_config(a_section, a_name);
	if (NULL == config) {
		config = create_config(a_section, a_name);
		config_sets(config, a_s);
	}
	return config;
}

void
config_setd(struct Config *const a_config, double const a_d)
{
	int len, ret;

	FREE(a_config->str);
	a_config->d = a_d;
	a_config->i32 = a_d;
	len = 24;
	MALLOC(a_config->str, len);
	ret = snprintf(a_config->str, len, "%g", a_d);
	assert(len > ret);
}

void
config_seti32(struct Config *const a_config, int32_t const a_i32)
{
	int len, ret;

	FREE(a_config->str);
	a_config->d = a_i32;
	a_config->i32 = a_i32;
	len = 16;
	MALLOC(a_config->str, len);
	ret = snprintf(a_config->str, len, "%d", a_i32);
	assert(len > ret);
}

void
config_sets(struct Config *const a_config, char const *const a_str)
{
	FREE(a_config->str);
	a_config->d = strtod(a_str, NULL);
	a_config->i32 = strtol(a_str, NULL, 10);
	a_config->str = strdup(a_str);
}

struct Config *
create_config(struct ConfigSection *const a_section, char const *const a_name)
{
	struct Config *config;

	CALLOC(config, 1);
	config->name = strdup(a_name);
	TAILQ_INSERT_TAIL(&a_section->config_list, config, next);
	return config;
}

struct ConfigSection *
create_section(struct ConfigCollection *const a_coll, char const *const
    a_name)
{
	struct ConfigSection *section;

	section = get_section(a_coll, a_name);
	CALLOC(section, 1);
	section->name = strdup(a_name);
	TAILQ_INIT(&section->config_list);
	TAILQ_INSERT_TAIL(&a_coll->section_list, section, next);
	return section;
}

struct Config *
get_config(struct ConfigSection const *const a_section, char const *const
    a_name)
{
	struct Config *config;

	TAILQ_FOREACH(config, &a_section->config_list, next) {
		if (0 == strcmp(config->name, a_name)) {
			return config;
		}
	}
	return NULL;
}

struct ConfigSection *
get_section(struct ConfigCollection const *const a_coll, char const *const
    a_name)
{
	struct ConfigSection *section;

	TAILQ_FOREACH(section, &a_coll->section_list, next) {
		if (0 == strcmp(section->name, a_name)) {
			break;
		}
	}
	return section;
}

struct ConfigCollection *
load(struct Lexer *const a_lexer)
{
	struct ConfigCollection *coll;
	struct ConfigSection *last_section;

	CALLOC(coll, 1);
	TAILQ_INIT(&coll->section_list);
	last_section = NULL;
	for (;;) {
		struct LexerToken token;

		token.str = NULL;
#define LOAD_ERROR(descr) do {\
		fprintf(stderr, "%d:%d: "descr".\n", \
		    lexer_get_line_no(a_lexer), lexer_get_col_no(a_lexer));\
		FREE(token.str);\
		goto load_error;\
	} HUTILS_COND(while, 0)
		if (!lexer_token_get(a_lexer, &token)) {
			if (LEXER_ERROR == token.type) {
				LOAD_ERROR("Lexer parser error");
			}
			break;
		}
		if ('[' == token.str[0]) {
			FREE(token.str);
			if (!lexer_token_get(a_lexer, &token) ||
			    LEXER_ALNUM != token.type) {
				LOAD_ERROR("Missing section name");
			}
			last_section = get_section(coll, token.str);
			if (NULL != last_section) {
				LOAD_ERROR("Duplicated section");
			}
			last_section = create_section(coll, token.str);
			token.str = NULL;
			if (!lexer_token_get(a_lexer, &token) ||
			    ']' != token.str[0]) {
				LOAD_ERROR("Missing ']'");
			}
			FREE(token.str);
		} else if (LEXER_ALNUM == token.type) {
			struct Config *config;
			char *name;

			if (NULL == last_section) {
				LOAD_ERROR("Section required before config");
			}
			config = get_config(last_section, token.str);
			if (NULL != config) {
				LOAD_ERROR("Duplicated config");
			}
			name = token.str;
			token.str = NULL;
			if (!lexer_token_get(a_lexer, &token) ||
			    '=' != token.str[0]) {
				FREE(name);
				LOAD_ERROR("Missing '='");
			}
			FREE(token.str);
			if (!lexer_token_get(a_lexer, &token)) {
				FREE(name);
				LOAD_ERROR("Missing value");
			}
			config = create_config(last_section, name);
			FREE(name);
			config_sets(config, token.str);
			FREE(token.str);
		} else {
			LOAD_ERROR("Missing section or config name");
		}
	}
	return coll;

load_error:
	config_collection_free(&coll);
	return NULL;
}

size_t
memory_read(void *const a_user_data, char *const a_dst, size_t const a_max)
{
	struct MemoryData *memory_data;
	size_t len;

	memory_data = a_user_data;
	len = memory_data->len - memory_data->ofs;
	len = MIN(len, a_max);
	memmove(a_dst, memory_data->buf + memory_data->ofs, len);
	return len;
}
