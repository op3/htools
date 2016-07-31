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

AR?=ar
CC?=gcc
LD?=gcc

include ../hgmake/build_dir.mk

CPPFLAGS:=$(CPPFLAGS) -I$(BUILD_DIR) -Iinclude -I../htest/include
include ../hgmake/strict_cflags.mk
ifeq (release,$(BUILD_MODE))
 CFLAGS:=$(CFLAGS) -O3
else
 CFLAGS:=$(CFLAGS) -ggdb
endif

TARGET:=$(BUILD_DIR)/libhutils.a
SRC:=$(wildcard src/*.c)
OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
HCONF_SRC:=$(patsubst %,include/hutils/%.h,gcc funcattr err dir fmtmod \
	getopt macros math stdint string thread time)
HCONF_DEP:=Makefile

TEST:=$(BUILD_DIR)/test
HTEST_GEN:=$(BUILD_DIR)/tests/tests.c
HTEST_SRC:=$(wildcard tests/*.c)
HTEST_SUITE_PRE=$(MKDIR_VERB)
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

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

include ../hgmake/hconf.mk
include ../htest/htest.mk

$(TARGET): $(OBJ)
	$(MKDIR)
	$(AR_A)

$(TEST): $(TEST_OBJ) $(TARGET)
	$(MKDIR)
	$(LD_E) -L../htest/$(BUILD_DIR) -lhtest

$(BUILD_DIR)/%.o: %.c Makefile $(HCONF_CACHE)
	$(MKDIR)
	$(CC_O)

-include $(patsubst %.o,%.d,$(OBJ) $(TEST_OBJ))
