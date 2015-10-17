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

CPPFLAGS:=$(CPPFLAGS) -I$(BUILD_DIR) -Iinclude -I. -I$(HTEST_PATH)/include -D_GNU_SOURCE
include $(HGMAKE_PATH)/strict_cflags.mk
LDFLAGS:=-L$(HTEST_PATH)/$(BUILD_DIR) -lhtest -lm -lpthread

ifeq (release,$(BUILD_TYPE))
 CFLAGS:=$(CFLAGS) -O3
else
 CFLAGS:=$(CFLAGS) -ggdb
endif

include $(HGMAKE_PATH)/hconf.mk
include $(HGMAKE_PATH)/config_cache.mk
include $(HGMAKE_PATH)/cmds.mk

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
all: $(TARGET)

.PHONY: test
test: $(TEST)
	@./$< -c

.PHONY: splint
splint:
	splint -nullret +posixstrictlib $(CPPFLAGS) $(SRC)

$(TARGET): $(OBJ)
	$(MKDIR_V)
	$(AR_A_V)

$(TEST): $(TARGET) $(TEST_OBJ)
	$(MKDIR_V)
	$(LD_E_V) $(TARGET)

$(BUILD_DIR)/%.o: %.c Makefile $(CONFIG_CACHE) $(BUILD_DIR)/hconf/include/hutils/macros.h
	$(MKDIR_V)
	$(CC_O_V)

$(BUILD_DIR)/hconf/%.c.h: %.c $(BUILD_DIR)/hconf/include/hutils/macros.h
	$(HCONF_V) $(BUILD_DIR) $<

$(BUILD_DIR)/hconf/include/hutils/macros.h: include/hutils/macros.h $(CONFIG_CACHE)
	$(HCONF_V) $(BUILD_DIR) $<

$(BUILD_DIR)/src/dir.o: $(BUILD_DIR)/hconf/src/dir.c.h

$(BUILD_DIR)/src/thread.o: $(BUILD_DIR)/hconf/src/thread.c.h

$(BUILD_DIR)/src/time.o: $(BUILD_DIR)/hconf/src/time.c.h

include $(HTEST_PATH)/htest.mk

.PHONY: clean
clean: clean_htest
	rm -rf $(BUILD_DIR)

$(CONFIG_CACHE): Makefile
	$(CONF_V)
	$(MKDIR_V)
	$(CONFIG_TEST_BEGIN)
	$(CONFIG_TEST_CC)
	$(CONFIG_TEST_END)

-include $(patsubst %.o,%.d,$(OBJ) $(TEST_OBJ))
