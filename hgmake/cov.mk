# Copyright (c) 2016, 2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

# Accepted only when BUILD_MODE=cov
# COV_SRC=files to consider for cov summaries.
# Currently gcov and lcov supported.

ifeq (cov,$(BUILD_MODE))
ifneq (,$(filter clang%,$(CC)))
 COV_CFLAGS=-fprofile-instr-generate -fcoverage-mapping
 COV_LIBS=
else
 ifneq (,$(or $(filter cc,$(CC)),$(filter %gcc,$(CC))))
  COV_CFLAGS=--coverage
  COV_LIBS=--coverage
 endif
endif

.PHONY: cov
cov:
	$(QUIET)echo Collecting coverage info...;\
	lines_total=0;\
	lines_ok_total=0;\
	for i in $(COV_SRC); do\
		dir=`dirname $$i`;\
		numbers=`gcov -n $$i -o $(BUILD_DIR)/$$dir 2> /dev/null | grep -A1 $$i | sed -n 's/[A-Za-z: ]*\([0-9.]*\).*of/\1/p'`;\
		[ "x" = "x$$numbers" ] && continue;\
		percentage=`echo $$numbers | awk '{print $$1}'`;\
		lines=`echo $$numbers | awk '{print $$2}'`;\
		lines_ok=`echo "$$percentage * $$lines / 100+1" | bc`;\
		lines_ok_total=`expr $$lines_ok_total + $$lines_ok`;\
		lines_total=`expr $$lines_total + $$lines`;\
	done;\
	percentage_total=`echo "100 * $$lines_ok_total / $$lines_total" | bc`;\
	echo Coverage: $$percentage_total% \($$lines_ok_total/$$lines_total lines\)

.PHONY: cov_files
cov_files:
	$(QUIET)echo Dumping gcov file-level info...;\
	for i in $(COV_SRC); do\
		echo $$i;\
		gcov -n $$i -o $(BUILD_DIR)/`dirname $$i`;\
	done

.PHONY: cov_funcs
cov_funcs:
	$(QUIET)echo Dumping gcov function-level info...;\
	for i in $(COV_SRC); do\
		echo $$i;\
		gcov -fn $$i -o $(BUILD_DIR)/`dirname $$i`;\
	done

.PHONY: cov_anno
cov_anno:
	$(QUIET)echo Generating gcov annotated files...;\
	for i in $(COV_SRC); do\
		gcov -lp $$i -o $(BUILD_DIR)/`dirname $$i`;\
	done;\
	[ -d $(BUILD_DIR)/gcov ] || mkdir -p $(BUILD_DIR)/gcov;\
	mv *.gcov $(BUILD_DIR)/gcov

else
.PHONY: cov cov_files cov_funcs cov_anno
cov cov_files cov_funcs cov_anno:
	$(QUIET)echo "cov*" targets available only with BUILD_MODE=cov. 1>&2 && exit 1
endif
