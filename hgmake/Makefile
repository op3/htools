# Copyright (c) 2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
# WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
# MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
# ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
# WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
# ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
# OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

include build_dir.mk
include strict_cflags.mk

CPPFLAGS:=$(CPPFLAGS) -D_BSD_SOURCE -D_DEFAULT_SOURCE -I. -I../hutils/include
# hutils hconfs the necessary funcattr, so we have to skip it here.
CFLAGS:=$(filter-out -Wformat=2,$(CFLAGS)) -ggdb
CONF:=$(BUILD_DIR)/hconf_conf
MERGE:=$(BUILD_DIR)/hconf_merge

.PHONY: all
all: $(CONF) $(MERGE)

include hconf.mk

$(CONF): $(addprefix $(BUILD_DIR)/,common.o hconf_conf.o)
	$(QUIET)echo "LD    $@" &&\
	$(MKDIR) &&\
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

$(MERGE): $(addprefix $(BUILD_DIR)/,common.o hconf_merge.o)
	$(QUIET)echo "LD    $@" &&\
	$(MKDIR) &&\
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)

$(BUILD_DIR)/%.o: %.c Makefile
	$(QUIET)echo "CC    $@" &&\
	$(MKDIR) &&\
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
