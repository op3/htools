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

# BUILD_DIR    = Generated files will be put in here.
# HCONF_CACHES = List of hconf caches from other projects to be included.
# HCONF_SRC    = List of files to be hconf:ed.
# HCONF_DEP    = List of files to depend on.

SED?=sed

HCONF_CONF:=$(HTOOLS_PATH)/hgmake/$(BUILD_DIR)/hconf_conf
HCONF_MERGE:=$(HTOOLS_PATH)/hgmake/$(BUILD_DIR)/hconf_merge

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

AR_A_VERB=$(AR) rcs $@ $(filter %.o,$^)
CC_O_VERB=$(HCONF_CC) -c -o $@ $< -MMD $(CPPFLAGS) $(HCONF_CPPFLAGS) $(CFLAGS) $(HCONF_CFLAGS)
CC_PRINCESS_O_VERB=$(HCONF_CC) -c -o $@ $< -MMD $(CPPFLAGS) $(HCONF_CPPFLAGS) $(filter-out -ansi% -pedantic% -W%,$(CFLAGS) $(HCONF_CFLAGS))
LD_E_VERB=$(HCONF_CC) -o $@ $(filter %.o %.a,$+) $(HCONF_LDFLAGS) $(LDFLAGS) $(HCONF_LIBS) $(LIBS)
MKDIR_VERB=[ -d $(@D) ] || mkdir -p $(@D)

ifneq (,$(V))
 AR_A_PRE=
 CC_O_PRE=
 CC_PRINCESS_O_PRE=
 HCONF_CONF_V=-v
 LD_E_PRE=
 QUIET=
else
 AR_A_PRE=@echo "AR    $@";
 CC_O_PRE=@echo "CC    $@";
 CC_PRINCESS_O_PRE=@echo "CCP   $@";
 HCONF_CONF_V=
 LD_E_PRE=@echo "LD    $@";
 QUIET=@
endif
AR_A=$(AR_A_PRE)$(AR_A_VERB)
CC_O=$(CC_O_PRE)$(CC_O_VERB)
CC_PRINCESS_O=$(CC_PRINCESS_O_PRE)$(CC_PRINCESS_O_VERB)
LD_E=$(LD_E_PRE)$(LD_E_VERB)
MKDIR=$(QUIET)$(MKDIR_VERB)

HCONF_CACHES_FILES:=$(addsuffix /$(HCONF_CACHE),$(HCONF_CACHES))
CPPFLAGS:=$(CPPFLAGS) $(patsubst %,-I%/$(BUILD_DIR),$(HCONF_CACHES))

export CC
$(HCONF_CACHE): $(HCONF_CACHES_FILES) $(HCONF_SRC)
	$(MKDIR)
	$(QUIET)(echo;echo $(CPPFLAGS);echo $(CFLAGS);echo;echo;) > $@.tmp;\
	if [ "$(HCONF_CACHES_FILES)" ]; then\
		$(HCONF_MERGE) $@.tmp $(HCONF_CACHES_FILES) > $@.tmp2;\
		mv $@.tmp2 $@.tmp;\
	fi;\
	for i in $(HCONF_SRC) x; do\
		[ "x" = "$$i" ] && break;\
		h=`echo $$i | sed 's/\.[0-9A-Za-z]*$$/.h/'`;\
		h_fin=$(BUILD_DIR)/hconf/$$h;\
		h_tmp=$(BUILD_DIR)/_hconf/hconf/$$h;\
		hc_fin=$(BUILD_DIR)/hconf/$$i.hconf;\
		hc_tmp=$(BUILD_DIR)/_hconf/hconf/$$i.hconf;\
		do_rebuild=;\
		if [ ! -f $$hc_fin -o $$hc_fin -ot $$i ]; then\
			do_rebuild=1;\
		else\
			for d in $(HCONF_DEP) x; do\
				[ "x" = "$$d" ] && break;\
				[ $$hc_fin -ot $$d ] && do_rebuild=1;\
			done;\
		fi;\
		if [ "$$do_rebuild" ]; then\
			echo HCONF $$i;\
			$(HCONF_CONF) $(HCONF_CONF_V) -d $(BUILD_DIR) -h $@.tmp -i $$i;\
			if [ 0 -ne $$? ]; then\
				cat $(BUILD_DIR)/_hconf/hconf/$$i.log;\
				exit 1;\
			fi;\
		else\
			$(HCONF_MERGE) $@.tmp $$hc_fin > $$hc_tmp;\
		fi;\
		rm -f $$h_fin $$hc_fin $@.tmp;\
		dname=`dirname $$hc_fin`;\
		[ -d $$dname ] || mkdir -p $$dname;\
		cp $$h_tmp $$h_fin;\
		cp $$hc_tmp $$hc_fin && cp $$hc_tmp $@.tmp;\
	done;\
	[ -f $@ ] && diff $@ $@.tmp > /dev/null;\
	if [ 0 -eq $$? ]; then touch $@; else mv -f $@.tmp $@; fi
