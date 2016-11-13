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
HTOOLS_PATH=..

include ../hgmake/build_dir.mk

CPPFLAGS:=$(CPPFLAGS) -I$(BUILD_DIR) -I. -Iinclude -I../hutils/include \
	-I../hutils/$(BUILD_DIR)
include ../hgmake/strict_cflags.mk

ifeq (release,$(BUILD_MODE))
	CFLAGS:=$(CFLAGS) -O3
endif
ifeq (debug,$(BUILD_MODE))
	CFLAGS:=$(CFLAGS) -ggdb
endif

SRC:=$(wildcard src/*.c)
OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
HCONF_PROJECTS:=../hutils
HCONF_SRC:=src/htest.h

HTEST_SRC:=$(wildcard tests/*.c)
HTEST_PRE=$(MKDIR_VERB)
HTEST_SUITE_SRC=$<
HTEST_SUITE_DST=$@

.PHONY: all
all: $(BUILD_DIR)/test
	-@./$< -c
	@echo "The result should be 7/11!"

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

include ../hgmake/hconf.mk
include htest.mk

$(BUILD_DIR)/test: $(BUILD_DIR)/libhtest.a $(HTEST_OBJ)
	$(MKDIR)
	$(LD_E) -L$(HTOOLS_PATH)/hutils/$(BUILD_DIR) -lhutils

$(BUILD_DIR)/libhtest.a: $(OBJ)
	$(MKDIR)
	$(AR_A)

$(BUILD_DIR)/%.o: %.c Makefile htest.mk $(HCONF_CACHE)
	$(MKDIR)
	$(CC_O)

%.o: %.c Makefile htest.mk $(HCONF_CACHE)
	$(MKDIR)
	$(CC_O)

-include $(patsubst %.o,%.d,$(OBJ) $(HTEST_OBJ))
