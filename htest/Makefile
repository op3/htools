# Copyright (c) 2014-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

HTOOLS_PATH=..

include $(HTOOLS_PATH)/hgmake/build_dir.mk
CPPFLAGS+=-I$(BUILD_DIR)\
	  -I.\
	  -Iinclude\
	  -I$(HTOOLS_PATH)/hutils/include\
	  -I$(HTOOLS_PATH)/hutils/$(BUILD_DIR)
include $(HTOOLS_PATH)/hgmake/strict_cflags.mk
ifeq (release,$(BUILD_MODE))
	CPPFLAGS+=-DNDEBUG
	CFLAGS+=-O3
else
	ifeq (fpic,$(BUILD_MODE))
		CFLAGS+=-fPIC
	else
		CFLAGS+=-ggdb
	endif
endif

SRC:=$(wildcard src/*.c)
HTEST_SRC:=$(wildcard tests/*.c)

OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(SRC))
HTEST_OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(HTEST_SRC))

CPPFLAGS:=$(CPPFLAGS)
CFLAGS:=$(CFLAGS)
LDFLAGS:=$(LDFLAGS)
LIBS:=$(LIBS)

.PHONY: all
all: $(BUILD_DIR)/test
	-@./$< -c
	@echo "The result should be 7/10!"

.PHONY: clean
clean:
	rm -rf $(BUILD_DIR)

HCONF_PROJECTS=$(HTOOLS_PATH)/hutils
HCONF_SRC=src/htest.h
HCONF_DEP=Makefile
include $(HTOOLS_PATH)/hgmake/hconf.mk

HTEST_PRE=$(MKDIR)
include htest.mk
HTEST_OBJ:=$(HTEST_OBJ) $(patsubst %.c,%.o,$(BUILD_DIR)/$(HTEST_TESTS))

COV_SRC:=$(SRC)
include $(HTOOLS_PATH)/hgmake/cov.mk

$(BUILD_DIR)/test: $(BUILD_DIR)/libhtest.a $(HTEST_OBJ)
	$(QUIET)echo "LD    $@" &&\
	$(MKDIR) &&\
	$(LD_E) -L$(HTOOLS_PATH)/hutils/$(BUILD_DIR) -lhutils $(COV_LIBS)

$(BUILD_DIR)/libhtest.a: $(OBJ)
	$(QUIET)echo "AR    $@" &&\
	$(MKDIR) &&\
	ar rcs $@ $^

$(BUILD_DIR)/%.o %.o: %.c Makefile htest.mk $(HCONF_CACHE)
	$(QUIET)echo "CC    $@" &&\
	$(MKDIR) &&\
	$(CC_O) $(COV_CPPFLAGS) $(COV_CFLAGS)

-include $(patsubst %.o,%.d,$(OBJ) $(HTEST_OBJ))
