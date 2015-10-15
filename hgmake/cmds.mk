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

AR_A=$(AR) rcs $@ $^
CC_O=$(CONFIG_CC) -c -o $@ $< -MMD $(CPPFLAGS) $(CONFIG_CPPFLAGS) $(CFLAGS) $(CONFIG_CFLAGS)
LD_E=$(LD) -o $@ $^ $(LDFLAGS)
MKDIR=test -d $(@D) || mkdir -p $(@D)
MV_D=file=$(patsubst %.c,%.d,$(<F)); if test -f $$file; then\
	echo -n $(@D)/ | cat - $$file > $(@:.o=.d);\
	rm -f $$file;\
fi
RM_RF=rm -rf

ifeq (1,$(V))
 AR_A_V=$(AR_A)
 CC_O_V=$(CC_O)
 CONF_V=
 HCONF_V=$(HCONF) -v
 LD_E_V=$(LD_E)
 MKDIR_V=$(MKDIR)
 MV_D_V=$(MV_D)
 RM_V=$(RM_RF)
 QUIET_V=
else
 AR_A_V=@echo "AR    " $@ && $(AR_A)
 CC_O_V=@echo "CC    " $@ && $(CC_O)
 CONF_V=@echo "CONF  " $@
 HCONF_V=@echo "HCONF " $@ && $(HCONF)
 LD_E_V=@echo "LD    " $@ && $(LD_E)
 MKDIR_V=@$(MKDIR)
 MV_D_V=@$(MV_D)
 RM_V=@echo "RM    " $@ && $(RM_RF)
 QUIET_V=@
endif
