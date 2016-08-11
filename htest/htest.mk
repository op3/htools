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

# Input:
#  $(BUILD_DIR)       = where to put generated stuff.
#  $(HTEST_SRC)       = test source files.
#  $(HTEST_PRE)       = called before creating a file (e.g. mkdir).
#  $(HTEST_SUITE_SRC) = source file pattern (e.g. $<).
#  $(HTEST_SUITE_DST) = suite file pattern (e.g. $@).
# Output:
#  $(HTEST_TESTS)     = will contain the suite list.
#  $(HTEST_SUITES)    = test suite cache files.
#  $(HTEST_OBJ)       = test source filenames turned into object filenames.

HTEST_CC_E=$(CC) -E $(HTEST_SUITE_SRC) $(CPPFLAGS) 2>/dev/null | \
	sed -n 's/.* htest_suite_header_\([^(]*\)_(.*/\1/p' > $(HTEST_SUITE_DST)
ifneq (,$(V))
	HTEST_CC_E_PRE=
	HTEST_TESTS_PRE=
else
	HTEST_CC_E_PRE=@echo "SUITE $@" &&
	HTEST_TESTS_PRE=@echo "TESTS $@" &&
endif

HTEST_TESTS:=$(BUILD_DIR)/htest/tests.c
HTEST_SUITES:=$(patsubst %.c,$(BUILD_DIR)/%.suite,$(HTEST_SRC))
HTEST_OBJ:=$(patsubst %.c,$(BUILD_DIR)/%.o,$(HTEST_SRC)) $(HTEST_TESTS:.c=.o)

$(HTEST_TESTS): $(HTEST_SUITES)
	$(HTEST_TESTS_PRE)$(HTEST_PRE);\
	echo "#include <htest/htest.h>" > $@;\
	if [ "$^" ]; then\
		cat $^ | sed 's/^\(.*\)$$/HTEST_SUITE_PROTO(\1);/';\
		echo "HTEST_SUITE_LIST_BEGIN";\
		cat $^ | sed 's/^\(.*\)$$/ HTEST_SUITE_LIST_ADD(\1)/';\
		echo "HTEST_SUITE_LIST_END";\
	else\
		echo;\
	fi >> $@

$(BUILD_DIR)/%.suite: %.c
	$(HTEST_CC_E_PRE)$(HTEST_PRE);\
	$(HTEST_CC_E)

.PHONY: clean_htest
clean_htest:
	rm -f $(HTEST_TESTS) $(HTEST_SUITES)
