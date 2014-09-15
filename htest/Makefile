# Copyright (c) 2014 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
#
# Permission to use, copy, modify, and distribute this software for any
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
CPPFLAGS=-Iinclude -D_GNU_SOURCE
CFLAGS=-ansi -ggdb -pedantic-errors -Wall -Werror -Wmissing-prototypes -Wshadow -Wstrict-prototypes -Wswitch-enum

SUFFIX:=$(shell uname -s)_$(shell uname -m)
BUILD_DIR:=build_$(SUFFIX)

AR_A=$(AR) rcs $@ $^
CC_EXE=$(CC) -o $@ $^ $(LDFLAGS)
CC_O=$(CC) -c -o $@ $< -MMD $(CPPFLAGS) $(CFLAGS)
MV_D=file=$(patsubst %.c,%.d,$(<F)); if test -f $$file; then\
	echo -n $(@D)/ | cat - $$file > $(@:.o=.d);\
	rm -f $$file;\
fi
MKDIR=test -d $(@D) || mkdir -p $(@D)

ifeq ($(V),1)
AR_A_V=$(AR_A)
CC_EXE_V=$(CC_EXE)
CC_O_V=$(CC_O)
MV_D_V=$(MV_D)
MKDIR_V=$(MKDIR)
else
AR_A_V=@echo "AR $@" && $(AR_A)
CC_EXE_V=@echo "LD $@" && $(CC_EXE)
CC_O_V=@echo "CC $@" && $(CC_O)
MV_D_V=@$(MV_D)
MKDIR_V=@$(MKDIR)
endif

HTEST_GEN:=$(BUILD_DIR)/tests/test_all.c
HTEST_SRC:=$(wildcard tests/*.c)

LIB_SRC:=$(wildcard src/*.c)
LIB_OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(LIB_SRC))
TEST_OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(HTEST_SRC)) $(HTEST_GEN:.c=.o)
ALL_DEP:=$(patsubst %.o,%.d,$(LIB_OBJ) $(TEST_OBJ))

all: $(BUILD_DIR)/test
	./$<

include htest.mk

$(BUILD_DIR)/libhtest.a: $(LIB_OBJ)
	$(AR_A_V)

$(BUILD_DIR)/test: $(BUILD_DIR)/libhtest.a $(TEST_OBJ)
	$(CC_EXE_V)

$(BUILD_DIR)/%.o: %.c Makefile htest.mk
	$(MKDIR_V)
	$(CC_O_V)
	$(MV_D_V)

%.o: %.c Makefile htest.mk
	$(MKDIR_V)
	$(CC_O_V)
	$(MV_D_V)

clean: clean_htest
	rm -rf $(BUILD_DIR)

-include $(ALL_DEP)
