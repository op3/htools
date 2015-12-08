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

# HCONF_FILES = list of files to be hconf:ed.

ifndef SED
 SED:=sed
endif

HCONF_CACHE:=$(BUILD_DIR)/hconf.cache
HCONF_LOG:=$(BUILD_DIR)/hconf.log

HCONF_CC=$(shell $(SED) -n 1p $(HCONF_CACHE))
HCONF_CPPFLAGS=$(shell $(SED) -n 2p $(HCONF_CACHE))
HCONF_CFLAGS=$(shell $(SED) -n 3p $(HCONF_CACHE))
HCONF_LDFLAGS=$(shell $(SED) -n 4p $(HCONF_CACHE))
HCONF_LIBS=$(shell $(SED) -n 5p $(HCONF_CACHE))

HCONF_TMP_CC=`$(SED) -n 1p $@.tmp`
HCONF_TMP_CPPFLAGS=`$(SED) -n 2p $@.tmp`
HCONF_TMP_CFLAGS=`$(SED) -n 3p $@.tmp`
HCONF_TMP_LDFLAGS=`$(SED) -n 4p $@.tmp`
HCONF_TMP_LIBS=`$(SED) -n 5p $@.tmp`

HCONF_CACHES_FILES:=$(addsuffix /$(HCONF_CACHE),$(HCONF_CACHES))

$(HCONF_CACHE): Makefile $(HCONF_CACHES_FILES) $(HCONF_FILES)
	$(MKDIR_V)
	$(QUIET_V)cmd="ccache -h";out=`$$cmd 2>&1`;ret=$$?;echo "$$cmd: $$out" > $(HCONF_LOG);\
	ccache=;\
	[ 0 -eq $$ret ] && ccache="ccache ";\
	echo $${ccache}$(CC) > $@.tmp;\
	echo "$(CPPFLAGS)" >> $@.tmp;\
	echo "$(CFLAGS)" >> $@.tmp;\
	echo "$(LDFLAGS)" >> $@.tmp;\
	echo "$(LIBS)" >> $@.tmp;\
	if [ "$(HCONF_CACHES_FILES)" ]; then\
		$(HTOOLS_PATH)/hgmake/hconf_merge.sh $@.tmp $(HCONF_CACHES_FILES) > $@.tmp2;\
		mv $@.tmp2 $@.tmp;\
	fi;\
	verbose=;\
	[ "x1" = "x$V" ] && verbose=-v;\
	for i in $(filter %.c %.h,$^); do\
		mk=$(BUILD_DIR)/hconf/`echo $$i | sed 's/\.[c|h]$$/.mk/'`;\
		if [ ! -f $$mk -o $$mk -ot $$i ]; then\
			$(HTOOLS_PATH)/hgmake/hconf.sh $$verbose $@.tmp $(BUILD_DIR) $$i;\
			[ 0 -ne $$? ] && exit 1;\
		fi;\
		$(HTOOLS_PATH)/hgmake/hconf_merge.sh $@.tmp $$mk > $@.tmp2;\
		mv $@.tmp2 $@.tmp;\
	done;\
	[ -f $@ ] && diff $@ $@.tmp > /dev/null;\
	if [ 1 -eq $$? ]; then\
		mv -f $@.tmp $@;\
	else\
		touch $@;\
	fi
