#!/bin/sh

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

write_hconf() {
	local target_dir=`dirname $target`
	test -d $target_dir || mkdir -p $target_dir
	local uppered=HCONF_`echo $file_h | sed 's/[\/\.]/_/g' | tr 'a-z' 'A-Z'`
	if [ 0 -eq $is_source ]; then
		header="#include <$file_h>"
	else
		header=
	fi
	cat << END > $target
#ifndef $uppered
#define $uppered
#define $opt
$header
#endif
END
}

verbose=0
if [ "x-v" = "x$1" ]; then
	verbose=1
	shift
fi
if [ 2 -ne $# ]; then
	echo "Usage: $0 [-v] build-dir file" 1>&2
	echo " -v  Verbose output." 1>&2
	exit 1
fi
build_dir=$1
file=$2

file_h=`echo $file | sed 's/\.c/.h/'`
file_c_h=`echo $file | sed 's/\.c/.c.h/'`
target=`echo $build_dir/hconf/$file_c_h`

test `echo $file | sed 's/.*\.\(h\)$/\1/'` = "h"
is_source=$?
log=$build_dir/hconf/${file}_log
main=$build_dir/hconf/${file}_main.c
main_dir=`dirname $main`
test -d $main_dir || mkdir -p $main_dir

option_list="start `grep "defined(HCONF_" $file | sed 's/.*defined(//;s/).*$//'` end"
if [ 0 -eq $is_source ]; then
	cat << END > $main
#include <$file>
#ifndef HCONF_TEST
# define HCONF_TEST do {} while (0)
#endif
int main(void) {
	HCONF_TEST;
	return 0;
}
END
fi
rm -f $log
for opt in $option_list; do
	if [ "end" = "$opt" ]; then
		rm -f $target
		echo " $file: No usable HCONF_* option found." 1>&2
		exit 1
	fi
	echo -n " $file: Testing option '$opt'... " | tee -a $log
	if [ 0 -eq $is_source ]; then
		cmd="$CC -D$opt $CPPFLAGS $CFLAGS -o /dev/null $main"
	else
		write_hconf
		cmd="$CC $CPPFLAGS $CFLAGS -c -o /dev/null $file"
	fi
	out="`$cmd 2>&1`"
	result=$?
	if [ 0 -ne $verbose ]; then
		echo "$cmd:" | tee -a $log
		echo "$out" | tee -a $log
	else
		echo "$cmd:" >> $log
		echo "$out" >> $log
	fi
	if [ $result -eq 0 ]; then
		if [ "start" = "$opt" ]; then
			rm -f $target
			echo Malformed hconf-file, must throw error if not configured! | tee -a $log
			exit 1
		fi
		break
	fi
	if [ "start" = "$opt" ]; then
		echo Ok, hconfing.
	else
		echo Failed.
	fi | tee -a $log
done
write_hconf
echo Passed. | tee -a $log
