# Copyright (c) 2015-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

# BUILD_DIR      = Generated files will be put in here.
# HCONF_PROJECTS = List of hconf:ed projects to be included.
# HCONF_SRC      = List of files to be hconf:ed.
# HCONF_DEP      = List of files to depend on.
# HCONF_EXT_PATH = Path(s) to hconf cache(s) for HCONF_EXT_* variables.

HCONF_CACHE:=$(BUILD_DIR)/hconf.cache
HCONF_CONF:=$(HTOOLS_PATH)/hgmake/$(BUILD_DIR)/hconf_conf
HCONF_MERGE:=$(HTOOLS_PATH)/hgmake/$(BUILD_DIR)/hconf_merge

# Hconf:ed variables in current build.
HCONF_CC=$(shell sed -n 1p $(HCONF_CACHE) | sed s/^$$/no-cc/)
HCONF_CPPFLAGS=$(shell sed -n 2p $(HCONF_CACHE))
HCONF_CFLAGS=$(shell sed -n 3p $(HCONF_CACHE))
HCONF_LDFLAGS=$(shell sed -n 4p $(HCONF_CACHE))
HCONF_LIBS=$(shell sed -n 5p $(HCONF_CACHE))

# If you don't want an hconf cache in your own build:
#  HCONF_EXT_PATH=$(HTOOLS_PATH)/hutils/$(BUILD_DIR) yadayada/$(BUILD_DIR)
#  include $(HTOOLS_PATH)/hgmake/hconf.mk
#  CPPFLAGS:=$(CPPFLAGS) $(HCONF_EXT_CPPFLAGS)
HCONF_EXT_CACHE:=$(HCONF_EXT_PATH)/$(HCONF_CACHE)
HCONF_EXT_CC=$(shell sed -n 1p $(HCONF_EXT_CACHE))
HCONF_EXT_CPPFLAGS=$(shell sed -n 2p $(HCONF_EXT_CACHE))
HCONF_EXT_CFLAGS=$(shell sed -n 3p $(HCONF_EXT_CACHE))
HCONF_EXT_LDFLAGS=$(shell sed -n 4p $(HCONF_EXT_CACHE))
HCONF_EXT_LIBS=$(shell sed -n 5p $(HCONF_EXT_CACHE))

# Common commands.
CPP_D=cppflags="$(HCONF_CPPFLAGS)" && sh $(HCONF_CACHE).ccd $@ $< $$cppflags > $(@:.o=.d) &&
CC_O=$(CPP_D)$(HCONF_CC) -c -o $@ $< $$cppflags $(CFLAGS) $(HCONF_CFLAGS)
CC_PRINCESS_O=$(CPP_D)$(HCONF_CC) -c -o $@ $< $$cppflags $(filter-out -ansi% -pedantic% -W%,$(CFLAGS) $(HCONF_CFLAGS))
LD_E=$(HCONF_CC) -o $@ $(filter %.o %.a,$+) $(HCONF_LDFLAGS) $(LDFLAGS) $(LIBS) $(HCONF_LIBS) $(LIBS_POST)
MKDIR=[ -d $(@D) ] || mkdir -p $(@D)

# "true" used to not be confused by e.g. "-" in GNU make recipes.
ifeq (,$(V))
 HCONF_CONF_V=
 QUIET=@
else
 HCONF_CONF_V=-v
 QUIET=true &&
endif

HCONF_PROJECTS_FILES:=$(addsuffix /$(HCONF_CACHE),$(HCONF_PROJECTS))
CPPFLAGS:=$(CPPFLAGS) $(patsubst %,-I%/$(BUILD_DIR),$(HCONF_PROJECTS))

$(HCONF_CACHE): $(HCONF_PROJECTS_FILES) $(HCONF_SRC) $(HCONF_CACHE).ccd
	$(QUIET)$(MKDIR) &&\
	export CC=$(CC) &&\
	(echo;echo $(CPPFLAGS) `sed -n 2p $(HCONF_CACHE).ccd`;echo $(CFLAGS);echo;echo;) > $@.tmp;\
	if [ "$(HCONF_PROJECTS_FILES)" ]; then\
		$(HCONF_MERGE) $@.tmp $(HCONF_PROJECTS_FILES) > $@.tmp2;\
		mv $@.tmp2 $@.tmp;\
	fi;\
	do_rebuild=;\
	for i in $(HCONF_SRC) x; do\
		[ "x" = "$$i" ] && break;\
		h=`echo $$i | sed 's/\.[0-9A-Za-z]*$$/.h/'`;\
		h_fin=$(BUILD_DIR)/hconf/$$h;\
		h_tmp=$(BUILD_DIR)/_hconf/hconf/$$h;\
		hc_fin=$(BUILD_DIR)/hconf/$$i.hconf;\
		hc_tmp=$(BUILD_DIR)/_hconf/hconf/$$i.hconf;\
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
	mv -f $@.tmp $@

# Either:
#  1) Line 1 = dep script
#  2) Line 2 = -MD
$(HCONF_CACHE).ccd:
	$(QUIET)echo "DEP   $@";\
	$(MKDIR) &&\
	cd $(@D) &&\
	[ -d _ccd ] || mkdir _ccd;\
	echo "int main(){return 0;}" > _ccd/main.c &&\
	gcc -MD -o _ccd/main.o _ccd/main.c;\
	if [ -f _ccd/main.d ]; then\
		(echo exit 0;echo "-MD") > $(@F) && exit 0;\
	fi;\
	if [ 1 -eq `gcc -E _ccd/main.c | grep ^\# | wc -l` ]; then\
		echo "(o=\$$1;shift;c=\$$1;shift;echo -n \$$o:;list=\`gcc -E \$$@ \$$c | sed -n 's/^#.*\"\\(.*\\)\".*/\1/p' | sort -u\`;for i in \$$list;do [ -f \$$i ]&&echo \$$i;done) | tr '\n' ' '" > $(@F) && exit 0;\
	fi;\
	exit 1
