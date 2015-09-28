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
LD=gcc
CC=gcc
ifeq (release,$(BUILD_TYPE))
 CFLAGS:=$(CFLAGS) -O3
else
 ifeq (gcov,$(BUILD_TYPE))
  CPPFLAGS:=$(CPPFLAGS) -DDO_GCOV_FLUSH
  CFLAGS:=$(CFLAGS) --coverage
  LDFLAGS:=$(LDFLAGS) --coverage -L/usr/lib/gcc/$(GCC_MACHINE)/$(GCC_VERSION) -lgcov
 else
  BUILD_TYPE=debug
  CFLAGS:=$(CFLAGS) -ggdb
 endif
endif
GCC_MACHINE:=$(shell gcc -dumpmachine)
GCC_VERSION:=$(shell gcc -dumpversion)
BUILD_DIR:=build_$(GCC_MACHINE)_$(GCC_VERSION)_$(BUILD_TYPE)

CPPFLAGS:=$(CPPFLAGS) -Iinclude -I../hutils/include -D_GNU_SOURCE
CFLAGS:=$(CFLAGS) -ansi -pedantic-errors -Wall -Werror -Wmissing-prototypes -Wshadow -Wstrict-prototypes
ifneq (,$(findstring inux,$(GCC_MACHINE)))
 CFLAGS:=$(CFLAGS) -Wextra -Wswitch-enum
else
 CFLAGS:=$(CFLAGS) -W
endif

AR_A=$(AR) rcs $@ $^
LD_E=$(LD) -o $@ $^ $(LDFLAGS)
CC_O=$(CC) -c -o $@ $< -MMD $(CPPFLAGS) $(CFLAGS)
MV_D=file=$(patsubst %.c,%.d,$(<F)); if test -f $$file; then\
	echo -n $(@D)/ | cat - $$file > $(@:.o=.d);\
	rm -f $$file;\
fi
MKDIR=test -d $(@D) || mkdir -p $(@D)

ifeq ($(V),1)
 AR_A_V=$(AR_A)
 LD_E_V=$(LD_E)
 CC_O_V=$(CC_O)
 MV_D_V=$(MV_D)
 MKDIR_V=$(MKDIR)
else
 AR_A_V=@echo "AR    $@" && $(AR_A)
 LD_E_V=@echo "LD    $@" && $(LD_E)
 CC_O_V=@echo "CC    $@" && $(CC_O)
 MV_D_V=@$(MV_D)
 MKDIR_V=@$(MKDIR)
endif

HTEST_GEN:=$(BUILD_DIR)/tests/test_all.c
HTEST_SRC:=$(wildcard tests/*.c)
HTEST_SUITE_PRE=$(MKDIR)
HTEST_SUITE_SRC=$<
HTEST_SUITE_DST=$@

LIB_SRC:=$(wildcard src/*.c)
LIB_OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(LIB_SRC))
TEST_OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(HTEST_SRC)) $(HTEST_GEN:.c=.o)
ALL_DEP:=$(patsubst %.o,%.d,$(LIB_OBJ) $(TEST_OBJ))

.PHONY: all
all: $(BUILD_DIR)/test
	@./$< -c
	@echo "The result should be 7/11!"

include htest.mk

$(BUILD_DIR)/libhtest.a: $(LIB_OBJ)
	$(MKDIR_V)
	$(AR_A_V)

$(BUILD_DIR)/test: $(BUILD_DIR)/libhtest.a $(TEST_OBJ)
	$(MKDIR_V)
	$(LD_E_V)

$(BUILD_DIR)/%.o: %.c Makefile.gnu htest.mk
	$(MKDIR_V)
	$(CC_O_V)
	$(MV_D_V)

%.o: %.c Makefile.gnu htest.mk
	$(MKDIR_V)
	$(CC_O_V)
	$(MV_D_V)

.PHONY: clean
clean: clean_htest
	rm -rf $(BUILD_DIR)

-include $(ALL_DEP)
