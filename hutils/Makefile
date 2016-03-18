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

CPPFLAGS:=$(CPPFLAGS) -I$(BUILD_DIR) -Iinclude -I. -I../htest/include
include $(HTOOLS_PATH)/hgmake/strict_cflags.mk
LDFLAGS:=
LIBS:=-lm

ifeq (release,$(BUILD_TYPE))
 CFLAGS:=$(CFLAGS) -O3
else
 CFLAGS:=$(CFLAGS) -ggdb
endif

include $(HTOOLS_PATH)/hgmake/cmds.mk

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

$(TEST): $(TEST_OBJ) $(TARGET)
	$(MKDIR_V)
	$(LD_E_V) -L../htest/$(BUILD_DIR) -lhtest

HCONF_FILES:=\
	include/hutils/stdint.h \
	include/hutils/macros.h \
	include/hutils/err.h \
	include/hutils/fmtmod.h \
	include/hutils/snprintf.h \
	include/hutils/strdup.h \
	include/hutils/strndup.h \
	include/hutils/strsignal.h \
	src/dir.c \
	src/thread.c \
	src/time.c
include $(HTOOLS_PATH)/hgmake/hconf.mk

$(BUILD_DIR)/%.o: %.c Makefile $(HCONF_CACHE)
	$(MKDIR_V)
	$(CC_O_V)

include ../htest/htest.mk

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

-include $(patsubst %.o,%.d,$(OBJ) $(TEST_OBJ))
