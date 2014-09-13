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

# Input:
#  $(BUILD_DIR)   = where to put generated stuff.
#  $(MKDIR)       = creates directory $(@D) if it does not exist.
#  $(HTEST_GEN)   = source file in which to generate the suite list.
#  $(HTEST_SRC)   = test source files.
# Output:
#  $(HTEST_SUITE) = test suite cache files.

HTEST_CC_E=$(CC) -E $< $(CPPFLAGS) | \
	sed -n 's/.* htest_suite_header_\([^(]*\)_(.*/\1/p' > $@
ifeq (1,$(V))
HTEST_CC_E_V=$(HTEST_CC_E)
HTEST_QUIET=
HTEST_MKDIR_V=$(MKDIR)
else
HTEST_CC_E_V=@echo "SUITE $@" && $(HTEST_CC_E)
HTEST_QUIET=@echo "TESTS $@" && 
HTEST_MKDIR_V=@$(MKDIR)
endif

HTEST_SUITE:=$(addprefix $(BUILD_DIR)/,$(HTEST_SRC:.c=.suite))

$(HTEST_GEN): $(HTEST_SUITE)
	$(HTEST_QUIET)echo "#include <htest/htest.h>" > $@ &&\
	cat $^ | sed 's/^\(.*\)$$/HTEST_SUITE_PROTO(\1);/' >> $@ &&\
	echo "HTEST_SUITE_LIST_BEGIN" >> $@ &&\
	cat $^ | sed 's/^\(.*\)$$/ HTEST_SUITE_LIST_ADD(\1)/' >> $@ &&\
	echo "HTEST_SUITE_LIST_END" >> $@

$(BUILD_DIR)/%.suite: %.c
	$(HTEST_MKDIR_V)
	$(HTEST_CC_E_V)

.PHONY: clean_htest
clean_htest:
	rm -f $(HTEST_GEN) $(HTEST_SUITE)
