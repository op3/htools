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
LD=gcc
MKDIR=test -d $(@D) || mkdir -p $(@D)

ifeq (1,$(V))
AR_V=$(AR)
CC_V=$(CC)
LD_V=$(LD)
MKDIR_V=$(MKDIR)
else
AR_V=@echo "AR   " $@ && $(AR)
CC_V=@echo "CC   " $@ && $(CC)
LD_V=@echo "LD   " $@ && $(LD)
MKDIR_V=@$(MKDIR)
endif

SUFFIX:=$(shell uname -s)_$(shell uname -m)
BUILD_DIR:=build_$(SUFFIX)

CPPFLAGS:=-Iinclude -I../htest/include
CFLAGS:=-ansi -ggdb -MMD -pedantic-errors -Wall -Werror -Wextra -Wmissing-declarations -Wmissing-prototypes -Wshadow -Wstrict-prototypes
LDFLAGS:=-lhtest -lm

TARGET:=$(BUILD_DIR)/libhutils.a
SRC:=$(wildcard src/*.c)
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
	$(AR_V) rcs $@ $^

$(TEST): $(OBJ) $(TEST_OBJ)
	$(MKDIR_V)
	$(LD_V) -o $@ $^ -L$(HTEST_PATH)/build_$(SUFFIX) $(LDFLAGS)

$(BUILD_DIR)/%.o: %.c Makefile
	$(MKDIR_V)
	$(CC_V) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

%.o: %.c Makefile
	$(MKDIR_V)
	$(CC_V) -c -o $@ $< $(CPPFLAGS) $(CFLAGS)

clean:
	rm -rf $(BUILD_DIR)

include $(HTEST_PATH)/htest.mk
-include $(OBJ:.o=.d) $(TEST_OBJ:.o=.d)
