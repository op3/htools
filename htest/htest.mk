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

# Input:
#  $(BUILD_DIR)       = build directory.
#  $(HTEST_DEP)       = dependencies before doing anything.
#  $(HTEST_PRE)       = called before creating a file (e.g. mkdir).
#  $(HTEST_SRC)       = test source files.
#  $(HTEST_TESTS)     = generated code dump, htest/tests.c by default.
# Output:
#  $(HTEST_SUITES)    = suite cache files.

ifeq (gcov,$(BUILD_MODE))
 CPPFLAGS:=$(CPPFLAGS) -DHTEST_COV_FLUSH
endif
ifeq (,$(HTEST_PRE))
 HTEST_PRE=true
endif
ifeq (,$(HTEST_TESTS))
 HTEST_TESTS=htest/tests.c
endif

HTEST_CC_E=gcc -E $(filter-out %_FORTIFY_SOURCE%,$(CPPFLAGS)) $< | sed -n 's/.* htest_suite_header_\([^(]*\)_(.*/\1/p' > $@
ifeq (,$(V))
 HTEST_CC_E_PRE=@echo "SUITE $@" &&
 HTEST_TESTS_PRE=@echo "TESTS $@" &&
else
 HTEST_CC_E_PRE=true &&
 HTEST_TESTS_PRE=true &&
endif

HTEST_SUITES:=$(patsubst %.c,$(BUILD_DIR)/%.suite,$(HTEST_SRC))

$(BUILD_DIR)/$(HTEST_TESTS): $(HTEST_SUITES)
	$(HTEST_TESTS_PRE)$(HTEST_PRE) &&\
	echo "#include <htest/htest.h>" > $@ &&\
	if [ "$^" ]; then\
		cat $^ | awk '{print "HTEST_SUITE_PROTO("$$0");"}';\
		echo "HTEST_SUITE_LIST_BEGIN";\
		cat $^ | awk '{print "\tHTEST_SUITE_LIST_ADD("$$0")"}';\
		echo "HTEST_SUITE_LIST_END";\
	else\
		echo;\
	fi >> $@

$(BUILD_DIR)/%.suite: %.c $(HTEST_DEP)
	$(HTEST_CC_E_PRE)$(HTEST_PRE) && $(HTEST_CC_E)

.PHONY: htest_clean
htest_clean:
	rm -f $(BUILD_DIR)/$(HTEST_TESTS) $(HTEST_SUITES)
