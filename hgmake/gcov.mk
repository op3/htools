# Copyright (c) 2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

# Accepted only when BUILD_MODE=gcov
# GCOV_SRC=files to undergo gcov.

ifeq (gcov,$(BUILD_MODE))

ifeq (,$(GCOV))
GCOV=gcov
endif

.PHONY: gcov
gcov:
	$(QUIET)echo Collecting gcov info...;\
	lines_total=0;\
	lines_ok_total=0;\
	for i in $(GCOV_SRC); do\
		dir=`dirname $$i`;\
		numbers=`$(GCOV) -n $$i -o $(BUILD_DIR)/$$dir 2> /dev/null | $(SED) -n 's/[A-Za-z: ]*\([0-9.]*\).*of/\1/p'`;\
		[ "x" = "x$$numbers" ] && continue;\
		percentage=`echo $$numbers | awk '{print $$1}'`;\
		lines=`echo $$numbers | awk '{print $$2}'`;\
		lines_ok=`echo "$$percentage * $$lines / 100+1" | bc`;\
		lines_ok_total=`expr $$lines_ok_total + $$lines_ok`;\
		lines_total=`expr $$lines_total + $$lines`;\
	done;\
	percentage_total=`echo "100 * $$lines_ok_total / $$lines_total" | bc`;\
	echo Coverage: $$percentage_total% \($$lines_ok_total/$$lines_total lines\)

.PHONY: gcov_files
gcov_files:
	$(QUIET)echo Dumping gcov file-level info...;\
	for i in $(GCOV_SRC); do\
		echo $$i;\
		dir=`dirname $$i`;\
		$(GCOV) -n $$i -o $(BUILD_DIR)/$$dir 2> /dev/null;\
	done

.PHONY: gcov_funcs
gcov_funcs:
	$(QUIET)echo Dumping gcov function-level info...;\
	for i in $(GCOV_SRC); do\
		echo $$i;\
		dir=`dirname $$i`;\
		$(GCOV) -fn $$i -o $(BUILD_DIR)/$$dir 2> /dev/null;\
	done

.PHONY: gcov_anno
gcov_anno:
	$(QUIET)echo Generating gcov annotated files...;\
	for i in $(GCOV_SRC); do\
		dir=`dirname $$i`;\
		$(GCOV) -p $$i -o $(BUILD_DIR)/$$dir 2> /dev/null;\
	done;\
	[ -d $(BUILD_DIR)/gcov ] || mkdir -p $(BUILD_DIR)/gcov;\
	mv *.c.gcov $(BUILD_DIR)/gcov
endif
