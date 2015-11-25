# Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

# HCONF_FILES = list of files to be hconf:ed.

ifndef SED
 SED:=sed
endif

HCONF_CACHE:=$(BUILD_DIR)/hconf_cache
HCONF_LOG:=$(BUILD_DIR)/hconf_log

HCONF_CC=$(shell $(SED) -n 1p $(HCONF_CACHE))
HCONF_CPPFLAGS=$(shell $(SED) -n 2p $(HCONF_CACHE))
HCONF_CFLAGS=$(shell $(SED) -n 3p $(HCONF_CACHE))
HCONF_LDFLAGS=$(shell $(SED) -n 4p $(HCONF_CACHE))
HCONF_LIBS=$(shell $(SED) -n 5p $(HCONF_CACHE))

define HCONF_RULE
$$(BUILD_DIR)/hconf/$$(FILE:.c=.h): $$(FILE)
	$$(MKDIR_V)
	$$(HCONF_V) $$(BUILD_DIR) $$<
endef

$(foreach FILE,$(HCONF_FILES),$(eval $(HCONF_RULE)))

$(HCONF_CACHE): Makefile $(addprefix $(BUILD_DIR)/hconf/,$(HCONF_FILES:.c=.h))
	$(MKDIR_V)
	$(QUIET_V)cmd="ccache -h";out=`$$cmd 2>&1`;ret=$$?;echo "$$cmd: $$out" > $(HCONF_LOG);\
	cc=gcc;\
	if [ 0 -eq $$ret ]; then\
		cc="ccache gcc";\
	fi;\
	echo $$cc > $@.tmp;\
	echo >> $@.tmp;\
	echo >> $@.tmp;\
	echo >> $@.tmp;\
	echo >> $@.tmp;\
	paste -d' ' $@.tmp $(filter %.mk,$(^:.h=.mk)) | tr -s " " > $@.tmp2;\
	test -f $@ && diff $@ $@.tmp2 > /dev/null;\
	if [ 1 -eq $$? ]; then\
		mv $@.tmp2 $@;\
	fi
