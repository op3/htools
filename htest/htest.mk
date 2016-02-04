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
#  $(HTEST_GEN)       = source file in which to generate the suite list.
#  $(HTEST_SRC)       = test source files.
#  $(HTEST_SUITE_PRE) = called before creating suite file (e.g. mkdir).
#  $(HTEST_SUITE_SRC) = source file pattern (e.g. $<).
#  $(HTEST_SUITE_DST  = suite file pattern (e.g. $@).
# Output:
#  $(HTEST_SUITE)     = test suite cache files.

HTEST_CC_E=$(CC) -E $(HTEST_SUITE_SRC) $(CPPFLAGS) | \
	sed -n 's/.* htest_suite_header_\([^(]*\)_(.*/\1/p' > $(HTEST_SUITE_DST)
ifeq (1,$(V))
 HTEST_CC_E_V=
 HTEST_GEN_V=
else
 HTEST_CC_E_V=@echo "SUITE $@" &&
 HTEST_GEN_V=@echo "TESTS $@" &&
endif

HTEST_SUITE:=$(addprefix $(BUILD_DIR)/,$(HTEST_SRC:.c=.suite))

$(HTEST_GEN): $(HTEST_SUITE)
	$(HTEST_GEN_V)echo "#include <htest/htest.h>" > $@ &&\
	cat $^ | sed 's/^\(.*\)$$/HTEST_SUITE_PROTO(\1);/' >> $@ &&\
	echo "HTEST_SUITE_LIST_BEGIN" >> $@ &&\
	cat $^ | sed 's/^\(.*\)$$/ HTEST_SUITE_LIST_ADD(\1)/' >> $@ &&\
	echo "HTEST_SUITE_LIST_END" >> $@

$(BUILD_DIR)/%.suite: %.c
	$(HTEST_CC_E_V)$(HTEST_SUITE_PRE);\
	$(HTEST_CC_E)

.PHONY: clean_htest
clean_htest:
	rm -f $(HTEST_GEN) $(HTEST_SUITE)
