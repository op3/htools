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

CPPFLAGS:=$(CPPFLAGS) -Iinclude -I$(HUTILS_PATH)/$(BUILD_DIR) -I$(HUTILS_PATH)/include
include $(HGMAKE_PATH)/strict_cflags.mk

ifeq (release,$(BUILD_TYPE))
 CFLAGS:=$(CFLAGS) -O3
else
 ifeq (gcov,$(BUILD_TYPE))
  CPPFLAGS:=$(CPPFLAGS) -DDO_GCOV_FLUSH
  CFLAGS:=$(CFLAGS) --coverage
  LDFLAGS:=$(LDFLAGS) --coverage -L/usr/lib/gcc/$(GCC_MACHINE)/$(GCC_VERSION) -lgcov
 else
  CFLAGS:=$(CFLAGS) -ggdb
 endif
endif

include $(HGMAKE_PATH)/hconf.mk
include $(HGMAKE_PATH)/config_cache.mk
include $(HGMAKE_PATH)/cmds.mk

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
	$(LD_E_V)

$(BUILD_DIR)/libhtest.a: $(OBJ)
	$(MKDIR_V)
	$(AR_A_V)

$(BUILD_DIR)/%.o: %.c htest.mk $(CONFIG_CACHE)
	$(MKDIR_V)
	$(CC_O_V)
	$(MV_D_V)

include htest.mk

.PHONY: clean
clean: clean_htest
	$(RM_V) $(BUILD_DIR)

$(CONFIG_CACHE): Makefile
	$(CONF_V)
	$(MKDIR_V)
	$(CONFIG_TEST_BEGIN)
	$(CONFIG_TEST_CC)
	$(CONFIG_TEST_END)

-include $(patsubst %.o,%.d,$(OBJ) $(TEST_OBJ))
