#ifndef HUTILS_CONFIG_H
#define HUTILS_CONFIG_H

#include <hutils/macros.h>

struct Config;
struct ConfigCollection;
struct ConfigSection;

void			config_collection_free(struct ConfigCollection **);
struct ConfigSection	*config_collection_get_section(struct ConfigCollection
    *, char const *) FUNC_RETURNS;
struct ConfigCollection	*config_collection_load_from_file(char const *)
	FUNC_RETURNS;
struct ConfigCollection	*config_collection_load_from_memory(char const *,
    size_t) FUNC_RETURNS;
int			config_collection_write(struct ConfigCollection const
    *, char const *) FUNC_RETURNS;
double			config_getd(struct Config const *) FUNC_RETURNS;
int32_t			config_geti32(struct Config const *) FUNC_RETURNS;
char const		*config_gets(struct Config const *) FUNC_RETURNS;
struct Config		*config_section_getd_config(struct ConfigSection *,
    char const *, double) FUNC_RETURNS;
struct Config		*config_section_geti32_config(struct ConfigSection *,
    char const *, int32_t) FUNC_RETURNS;
struct Config		*config_section_gets_config(struct ConfigSection *,
    char const *, char const *) FUNC_RETURNS;
void			config_setd(struct Config *, double);
void			config_seti32(struct Config *, int32_t);
void			config_sets(struct Config *, char const *);

#endif
