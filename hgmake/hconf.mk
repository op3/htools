# Copyright (c) 2015-2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
	echo "int main(){}" > $(BUILD_DIR)/hconf.main.c;\
	wextra=-Wextra;\
	$(CC) $$wextra -o /dev/null $(BUILD_DIR)/hconf.main.c &> /dev/null;\
	[ 0 -ne $$? ] && wextra=-W;\
	wswitchenum=-Wswitch-enum;\
	$(CC) $$wswitchenum -o /dev/null $(BUILD_DIR)/hconf.main.c &> /dev/null;\
	[ 0 -ne $$? ] && wswitchenum=;\
	echo $${ccache}$(CC) > $@.tmp;\
	echo "$(CPPFLAGS)" >> $@.tmp;\
	echo "$(CFLAGS) $$wextra $$wswitchenum" >> $@.tmp;\
	echo "$(LDFLAGS)" >> $@.tmp;\
	echo "$(LIBS)" >> $@.tmp;\
	if [ "$(HCONF_CACHES_FILES)" ]; then\
		$(HTOOLS_PATH)/hgmake/$(BUILD_DIR)/hconf_merge $@.tmp $(HCONF_CACHES_FILES) > $@.tmp2;\
		mv $@.tmp2 $@.tmp;\
	fi;\
	verbose=;\
	[ "x1" = "x$V" ] && verbose=-v;\
	if [ "$(HCONF_FILES)" ]; then\
		for i in $(HCONF_FILES); do\
			h=`echo $$i | sed 's/\.[0-9A-Za-z]*$$/.h/'`;\
			h_fin=$(BUILD_DIR)/hconf/$$h;\
			h_tmp=$(BUILD_DIR)/hconf_/hconf/$$h;\
			mk_fin=$(BUILD_DIR)/hconf/$$i.mk;\
			mk_tmp=$(BUILD_DIR)/hconf_/hconf/$$i.mk;\
			if [ ! -f $$mk_fin -o $$mk_fin -ot $$i -o $$mk_fin -ot Makefile ]; then\
				$(HTOOLS_PATH)/hgmake/$(BUILD_DIR)/hconf_conf $$verbose $@.tmp $(BUILD_DIR) $$i;\
				[ 0 -ne $$? ] && exit 1;\
			else\
				$(HTOOLS_PATH)/hgmake/$(BUILD_DIR)/hconf_merge $@.tmp $$mk_fin > $$mk_tmp;\
			fi;\
			rm -f $$h_fin $$mk_fin $@.tmp;\
			dname=`dirname $$mk_fin`;\
			[ -d $$dname ] || mkdir -p $$dname;\
			cp $$h_tmp $$h_fin;\
			cp $$mk_tmp $$mk_fin;\
			cp $$mk_tmp $@.tmp;\
		done;\
	fi;\
	[ -f $@ ] && diff $@ $@.tmp > /dev/null;\
	if [ 1 -eq $$? ]; then\
		mv -f $@.tmp $@;\
	else\
		touch $@;\
	fi
