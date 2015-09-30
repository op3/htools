# Copyright (c) 2015
# Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

ifndef CONFIG_CACHE
 CONFIG_CACHE:=$(BUILD_DIR)/config_cache
endif
ifndef CONFIG_LOG
 CONFIG_LOG:=$(BUILD_DIR)/config_log
endif
ifndef CONFIG_TMP
 CONFIG_TMP:=$(BUILD_DIR)/config_tmp
endif
ifndef SED
 SED:=sed
endif
CONFIG_CC=$(shell $(SED) -n 1p $(CONFIG_CACHE))
CONFIG_CPPFLAGS=$(shell $(SED) -n 2p $(CONFIG_CACHE))
CONFIG_CFLAGS=$(shell $(SED) -n 3p $(CONFIG_CACHE))
CONFIG_LDFLAGS=$(shell $(SED) -n 4p $(CONFIG_CACHE))

CONFIG_ADD_CPPFLAGS=$(SED) "2s,$$, $1," $(CONFIG_TMP) > $(CONFIG_TMP)2 && mv $(CONFIG_TMP)2 $(CONFIG_TMP)
CONFIG_ADD_CFLAGS=$(SED) "3s,$$, $1," $(CONFIG_TMP) > $(CONFIG_TMP)2 && mv $(CONFIG_TMP)2 $(CONFIG_TMP)
CONFIG_ADD_LDFLAGS=$(SED) "4s,$$, $1," $(CONFIG_TMP) > $(CONFIG_TMP)2 && mv $(CONFIG_TMP)2 $(CONFIG_TMP)

CONFIG_TEST_BEGIN=\
	$(QUIET_V)echo > $(CONFIG_LOG);\
	echo > $(CONFIG_TMP);\
	echo >> $(CONFIG_TMP);\
	echo >> $(CONFIG_TMP);\
	echo >> $(CONFIG_TMP)

CONFIG_TEST_END=\
	$(QUIET_V)mv $(CONFIG_TMP) $@

CONFIG_TEST_CC=\
	$(QUIET_V)cmd="ccache -h";out=`$$cmd 2>&1`;ret=$$?;echo "$$cmd: $$out" >> $(CONFIG_LOG);\
	pre=;\
	if [ 0 -eq $$ret ]; then\
		pre="ccache ";\
	fi;\
	$(SED) "1s,$$,$${pre}$(CC)," $(CONFIG_TMP) > $(CONFIG_TMP)2 && mv $(CONFIG_TMP)2 $(CONFIG_TMP)
