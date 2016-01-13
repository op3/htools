/*
 * Copyright (c) 2015, 2016
 * Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
