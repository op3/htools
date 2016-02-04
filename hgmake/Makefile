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

include $(HTOOLS_PATH)/hgmake/build_dir.mk
include $(HTOOLS_PATH)/hgmake/strict_cflags.mk
include $(HTOOLS_PATH)/hgmake/cmds.mk

CFLAGS:=$(CFLAGS) -ggdb
CPPFLAGS:=-D_BSD_SOURCE -D_POSIX_C_SOURCE=200809L -I. -I../hutils/include
LDFLAGS:=

HCONF_CONF:=$(BUILD_DIR)/hconf_conf
HCONF_MERGE:=$(BUILD_DIR)/hconf_merge

.PHONY: all
all: $(HCONF_CONF) $(HCONF_MERGE)

$(HCONF_CONF): $(BUILD_DIR)/hconf.o $(BUILD_DIR)/hconf_conf.o
	$(MKDIR)
	gcc $(LDFLAGS) -o $@ $^

$(HCONF_MERGE): $(BUILD_DIR)/hconf.o $(BUILD_DIR)/hconf_merge.o
	$(MKDIR)
	gcc $(LDFLAGS) -o $@ $^

$(BUILD_DIR)/%.o: %.c Makefile hconf.h
	$(MKDIR)
	gcc -c $(CPPFLAGS) $(CFLAGS) -o $@ $<

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)
