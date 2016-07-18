# Copyright (c) 2014-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

include $(HTOOLS_PATH)/hgmake/build_dir.mk

CPPFLAGS:=$(CPPFLAGS) -Iinclude -I$(BUILD_DIR) -I$(HTOOLS_PATH)/hutils/$(BUILD_DIR) -I$(HTOOLS_PATH)/hutils/include
include $(HTOOLS_PATH)/hgmake/strict_cflags.mk

ifeq (release,$(BUILD_MODE))
 CFLAGS:=$(CFLAGS) -O3
endif
ifeq (debug,$(BUILD_MODE))
 CFLAGS:=$(CFLAGS) -ggdb
endif

include $(HTOOLS_PATH)/hgmake/cmds.mk

SRC:=$(wildcard src/*.c)
OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))

HTEST_GEN:=$(BUILD_DIR)/tests/test_all.c
HTEST_SRC:=$(wildcard tests/*.c)
HTEST_SUITE_PRE=$(MKDIR)
HTEST_SUITE_SRC=$<
HTEST_SUITE_DST=$@
TEST_OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(HTEST_SRC)) $(HTEST_GEN:.c=.o)

.PHONY: all
all: $(BUILD_DIR)/test
	@./$< -c
	@echo "The result should be 7/11!"

$(BUILD_DIR)/test: $(BUILD_DIR)/libhtest.a $(TEST_OBJ)
	$(MKDIR_V)
	$(LD_E_V) -L$(HTOOLS_PATH)/hutils/$(BUILD_DIR) -lhutils

$(BUILD_DIR)/libhtest.a: $(OBJ)
	$(MKDIR_V)
	$(AR_A_V)

HCONF_CACHES:=../hutils
HCONF_FILES:=src/htest.c
include $(HTOOLS_PATH)/hgmake/hconf.mk

$(BUILD_DIR)/%.o: %.c Makefile htest.mk $(HCONF_CACHE)
	$(MKDIR_V)
	$(CC_O_V)

include htest.mk

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(patsubst %.o,%.d,$(OBJ) $(TEST_OBJ))
