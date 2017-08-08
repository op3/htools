# Copyright (c) 2016-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

.PHONY: all
all:
	$(MAKE) -C hgmake
	$(MAKE) -C hutils
	$(MAKE) -C htest
	$(MAKE) -C hutils test

.PHONY: clean
clean:
	$(MAKE) -C hgmake $@
	$(MAKE) -C hutils $@
	$(MAKE) -C htest $@

.PHONY: cov cov_files cov_funcs cov_anno
cov cov_files cov_funcs cov_anno:
	$(MAKE) -C hutils $@

.PHONY: mega
mega:
	$(MAKE)
	$(MAKE) BUILD_MODE=release
	$(MAKE) BUILD_MODE=cov
	CC=clang $(MAKE)
	CC=clang $(MAKE) BUILD_MODE=release
	CC=clang $(MAKE) BUILD_MODE=cov

.PHONY: mega-clean
mega-clean:
	$(MAKE) clean
	$(MAKE) BUILD_MODE=release clean
	$(MAKE) BUILD_MODE=cov clean
	CC=clang $(MAKE) clean
	CC=clang $(MAKE) BUILD_MODE=release clean
	CC=clang $(MAKE) BUILD_MODE=cov clean
