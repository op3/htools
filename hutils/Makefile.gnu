# Copyright (c) 2014 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
LD=gcc
CC=gcc
ifeq (release,$(BUILD_TYPE))
 CFLAGS:=$(CFLAGS) -O3
else
 BUILD_TYPE=debug
 CFLAGS:=$(CFLAGS) -ggdb
endif
GCC_MACHINE:=$(shell gcc -dumpmachine)
GCC_VERSION:=$(shell gcc -dumpversion)
BUILD_DIR:=build_$(GCC_MACHINE)_$(GCC_VERSION)_$(BUILD_TYPE)

CPPFLAGS:=$(CPPFLAGS) -Iinclude -I$(HTEST_PATH)/include -D_GNU_SOURCE
CFLAGS:=$(CFLAGS) -ansi -pedantic-errors -Wall -Werror -Wextra -Wmissing-prototypes -Wshadow -Wstrict-prototypes
ifneq (,$(findstring inux,$(GCC_MACHINE)))
 CFLAGS:=$(CFLAGS) -Wswitch-enum
endif
LDFLAGS:=-L$(HTEST_PATH)/$(BUILD_DIR) -lhtest -lm -lpthread

AR_A=$(AR) rcs $@ $^
LD_E=$(LD) -o $@ $^ $(LDFLAGS)
CC_O=$(CC) -c -o $@ $< -MMD $(CPPFLAGS) $(CFLAGS)
MKDIR=test -d $(@D) || mkdir -p $(@D)

ifeq (1,$(V))
 AR_A_V=$(AR_A)
 LD_E_V=$(LD_E)
 CC_O_V=$(CC_O)
 MKDIR_V=$(MKDIR)
else
 AR_A_V=@echo "AR    " $@ && $(AR_A)
 LD_E_V=@echo "LD    " $@ && $(LD_E)
 CC_O_V=@echo "CC    " $@ && $(CC_O)
 MKDIR_V=@$(MKDIR)
endif

TARGET:=$(BUILD_DIR)/libhutils.a
SRC:=$(filter-out %getopt.c,$(wildcard src/*.c))
OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))

TEST:=$(BUILD_DIR)/test
HTEST_SRC:=$(wildcard tests/*.c)
HTEST_GEN:=$(BUILD_DIR)/tests/tests.c
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

$(BUILD_DIR)/%.o: %.c Makefile.gnu
	$(MKDIR_V)
	$(CC_O_V)

%.o: %.c Makefile.gnu
	$(MKDIR_V)
	$(CC_O_V)

include $(HTEST_PATH)/htest.mk

.PHONY: clean
clean: clean_htest
	rm -rf $(BUILD_DIR)

-include $(OBJ:.o=.d) $(TEST_OBJ:.o=.d)
