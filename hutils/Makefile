# Copyright (c) 2014, 2015
# Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

AR=ar
CC=gcc
LD=gcc

include $(HGMAKE_PATH)/build_dir.mk

CPPFLAGS:=$(CPPFLAGS) -I$(BUILD_DIR) -Iinclude -I$(HTEST_PATH)/include -D_GNU_SOURCE
include $(HGMAKE_PATH)/strict_cflags.mk
CFLAGS:=$(CFLAGS) -Wswitch-enum
LDFLAGS:=-L$(HTEST_PATH)/$(BUILD_DIR) -lhtest -lm -lpthread

ifeq (release,$(BUILD_TYPE))
 CFLAGS:=$(CFLAGS) -O3
else
 CFLAGS:=$(CFLAGS) -ggdb
endif

HCONF=CC="$(CONFIG_CC)" CPPFLAGS="$(CPPFLAGS) $(CONFIG_CPPFLAGS)" CFLAGS="$(CFLAGS) $(CONFIG_CFLAGS)" $(HGMAKE_PATH)/hconf.sh

include $(HGMAKE_PATH)/config_cache.mk

AR_A=$(AR) rcs $@ $^
LD_E=$(LD) -o $@ $^ $(LDFLAGS)
CC_O=$(CONFIG_CC) -c -o $@ $< -MMD $(CPPFLAGS) $(CFLAGS)
MKDIR=test -d $(@D) || mkdir -p $(@D)

ifeq (1,$(V))
 AR_A_V=$(AR_A)
 CC_O_V=$(CC_O)
 CONF_V=
 HCONF_V=$(HCONF) -v
 LD_E_V=$(LD_E)
 MKDIR_V=$(MKDIR)
 QUIET_V=
else
 AR_A_V=@echo "AR    " $@ && $(AR_A)
 CC_O_V=@echo "CC    " $@ && $(CC_O)
 CONF_V=@echo "CONF  " $@
 HCONF_V=@echo "HCONF " $@ && $(HCONF)
 LD_E_V=@echo "LD    " $@ && $(LD_E)
 MKDIR_V=@$(MKDIR)
 QUIET_V=@
endif

TARGET:=$(BUILD_DIR)/libhutils.a
SRC:=$(filter-out %getopt.c,$(wildcard src/*.c))
OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))

TEST:=$(BUILD_DIR)/test
HTEST_GEN:=$(BUILD_DIR)/tests/tests.c
HTEST_SRC:=$(wildcard tests/*.c)
HTEST_SUITE_PRE=$(MKDIR)
HTEST_SUITE_SRC=$<
HTEST_SUITE_DST=$@
TEST_OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(HTEST_SRC)) $(HTEST_GEN:.c=.o)

.PHONY: all
all: $(TARGET) test

.PHONY: test
test: $(TEST)
	@./$< -c

$(TARGET): $(OBJ)
	$(MKDIR_V)
	$(AR_A_V)

$(TEST): $(TEST_OBJ) $(OBJ)
	$(MKDIR_V)
	$(LD_E_V)

$(BUILD_DIR)/%.o: %.c Makefile $(CONFIG_CACHE)
	$(MKDIR_V)
	$(CC_O_V)

%.o: %.c Makefile $(CONFIG_CACHE)
	$(MKDIR_V)
	$(CC_O_V)

$(BUILD_DIR)/src/time.o: $(BUILD_DIR)/hconf/src/time.c.h

$(BUILD_DIR)/hconf/src/time.c.h: src/time.c
	$(HCONF_V) $(BUILD_DIR) $<

include $(HTEST_PATH)/htest.mk

.PHONY: clean
clean: clean_htest
	rm -rf $(BUILD_DIR)

-include $(OBJ:.o=.d) $(TEST_OBJ:.o=.d)

$(CONFIG_CACHE): Makefile
	$(CONF_V)
	$(MKDIR_V)
	$(CONFIG_TEST_BEGIN)
	$(CONFIG_TEST_CC)
	$(CONFIG_TEST_END)
