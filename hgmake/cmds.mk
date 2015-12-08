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

ifndef SED
 SED:=sed
endif

AR_A=$(AR) rcs $@ $^
CC_O=$(HCONF_CC) -c -o $@ $< -MMD $(CPPFLAGS) $(HCONF_CPPFLAGS) $(CFLAGS) $(HCONF_CFLAGS)
HCONF=CC="$(CC)" \
      CPPFLAGS="$(CPPFLAGS)" \
      CFLAGS="$(CFLAGS)" \
      LDFLAGS="$(LDFLAGS)" \
      LIBS="$(LIBS)" \
      $(HTOOLS_PATH)/hgmake/hconf.sh
LD_E=$(LD) -o $@ $^ $(LDFLAGS) $(HCONF_LDFLAGS) $(LIBS) $(HCONF_LIBS)
MKDIR=test -d $(@D) || mkdir -p $(@D)
MV_D=file=$(patsubst %.c,%.d,$(<F)); if test -f $$file; then\
	echo -n $(@D)/ | cat - $$file > $(@:.o=.d);\
	rm -f $$file;\
fi

ifeq (1,$(V))
 AR_A_V=$(AR_A)
 CC_O_V=$(CC_O)
 HCONF_V=$(HCONF) -v
 LD_E_V=$(LD_E)
 MKDIR_V=$(MKDIR)
 MV_D_V=$(MV_D)
 QUIET_V=
else
 AR_A_V=@echo "AR    " $@ && $(AR_A)
 CC_O_V=@echo "CC    " $@ && $(CC_O)
 HCONF_V=@echo "HCONF " $@ && $(HCONF)
 LD_E_V=@echo "LD    " $@ && $(LD_E)
 MKDIR_V=@$(MKDIR)
 MV_D_V=@$(MV_D)
 QUIET_V=@
endif
