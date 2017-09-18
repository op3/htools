# Copyright (c) 2015-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

include ../hgmake/build_dir.mk

CPPFLAGS:=$(CPPFLAGS)\
	-I$(BUILD_DIR)\
	-I.\
	-Iinclude\
	-I../hutils/$(BUILD_DIR)\
	-I../hutils/include\
	-I../htest/include
include ../hgmake/strict_cflags.mk

ifeq (release,$(BUILD_MODE))
	CFLAGS:=$(CFLAGS) -O3
else ifeq (fpic,$(BUILD_MODE))
	CFLAGS:=$(CFLAGS) -fPIC
else
	CFLAGS:=$(CFLAGS) -ggdb
endif

TARGET:=$(BUILD_DIR)/libhwt.a
TEST:=$(BUILD_DIR)/test

SRC:=$(wildcard src/*.c)
HTEST_SRC:=$(wildcard tests/*.c)

OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
TEST_OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(HTEST_SRC))

CPPFLAGS:=$(CPPFLAGS)
CFLAGS:=$(CFLAGS)
LDFLAGS:=$(LDFLAGS)

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

HCONF_PROJECTS:=../hutils
include ../hgmake/hconf.mk

HTEST_DEP:=$(HCONF_CACHE)
HTEST_PRE=$(MKDIR)
include ../htest/htest.mk
TEST_OBJ:=$(TEST_OBJ) $(patsubst %.c,%.o,$(BUILD_DIR)/$(HTEST_TESTS))

$(TARGET): $(OBJ)
	$(QUIET)echo "AR    $@" &&\
	$(MKDIR) &&\
	ar rcs $@ $^

$(TEST): $(TEST_OBJ) $(TARGET)
	$(QUIET)echo "LD    $@" &&\
	$(MKDIR) &&\
	$(LD_E)\
		-L../hutils/$(BUILD_DIR) -lhutils \
		-L$(HTOOLS_PATH)/htest/$(BUILD_DIR) -lhtest $(COV_LIBS)

$(BUILD_DIR)/%.o: %.c Makefile $(HCONF_CACHE)
	$(QUIET)echo "CC    $@" &&\
	$(MKDIR) &&\
	$(CC_O) $(COV_CPPFLAGS) $(COV_CFLAGS)

-include $(patsubst %.o,%.d,$(OBJ) $(TEST_OBJ))
