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

no_option=no_option

build_result=
build()
{
	build_output=$1
	build_input=$2
	build_cppflags=$3
	build_cflags=$4
	build_ldflags=$5
	build_libs=$6
	cmd="$CC $build_cppflags -I. -I$build_dir $build_cflags $build_ldflags -o $build_output $build_input $build_libs"
	out="`$cmd 2>&1`"
	build_result=$?
	if [ 0 -ne $verbose ]; then
		echo "$cmd:" | tee -a $log
		echo "$out" | tee -a $log
	else
		echo "$cmd:" >> $log
		echo "$out" >> $log
	fi
	if [ 0 -ne $build_result ]; then
		build_result=`echo $out | wc -l`
	fi
}

clean_hconf()
{
	rm -f $target_h $target_mk
}

for i in 0 1 2 3 4; do
	eval best_args$i=
done
best_link_result=1000
try()
{
	write_hconf
	try_option=$args0
	try_cppflags=$args1
	try_cflags=$args2
	try_ldflags=$args3
	try_libs=$args4
	echo -n " $file: Testing option '$option'... " | tee -a $log
	if [ $is_source ]; then
		try_compile_input=$file_c
		try_compile_output=$file_o
		try_link_input="$file_o $main_c"
	else
		try_compile_input=$main_c
		try_compile_output=$main_o
		try_link_input=$main_o
	fi
	build "$try_compile_output" "$try_compile_input" "$CPPFLAGS $try_cppflags" "$CFLAGS $try_cflags -c" "" ""
	compile_result=$build_result
	if [ 0 -eq $compile_result ]; then
		if [ "$no_option" = "$option" ]; then
			clean_hconf
			echo Compilation must fail if file not hconf:ed! | tee -a $log
			exit 1
		fi
		build /dev/null "$try_link_input" "" "" "$LDFLAGS $try_ldflags" "$LIBS $try_libs"
		try_link_result=$build_result
		if [ 0 -eq try_link_result ]; then
			echo Perfect. | tee -a $log
			exit 0
		fi
		if [ $try_link_result -lt $best_link_result ]; then
			for i in 0 1 2 3 4; do
				eval best_args$i=\$args$i
			done
			best_link_result=$try_link_result
			echo Keeping. | tee -a $log
		fi
	else
		if [ "$no_option" = "$option" ]; then
			echo Ok, hconfing.
		else
			echo Failed.
		fi | tee -a $log
	fi
}

write_hconf()
{
	uppered=HCONF_`echo $file_h | sed 's/[\/\.]/_/g' | tr 'a-z' 'A-Z'`
	cat << END > $target_h
#ifndef $uppered
#define $uppered
#define $args0
#undef HCONF_TEST
#endif
END
	echo > $target_mk
	for i in 1 2 3 4; do
		eval echo \$args$i >> $target_mk
	done
}

verbose=0
if [ "x-v" = "x$1" ]; then
	verbose=1
	shift
fi
if [ 2 -ne $# ]; then
	echo "Usage: $0 [-v] build-dir file" 1>&2
	echo " build_dir  Generated files will be put here." 1>&2
	echo " file       File to undergo hconf:ing." 1>&2
	echo " -v         Verbose output." 1>&2
	exit 1
fi
build_dir=$1
file=$2

log=$build_dir/hconf/${file}.log
rm -f $log
main_c=$build_dir/hconf/${file}.main.c
rm -f $main_c

is_source=
if [ ".c" = `echo $file | sed 's/.*\(..\)/\1/'` ]; then
	is_source=0
fi
if [ $is_source ]; then
	file_c=$file
	file_o=$build_dir/hconf/`echo $file | sed 's/\.c$/.o/'`
else
	main_o=`echo $main_c | sed 's/\.c$/.o/'`
fi
file_h=`echo $file | sed 's/\.c$/.h/'`
file_mk=`echo $file_h | sed 's/\.h$/.mk/'`
target_h=$build_dir/hconf/$file_h
target_mk=$build_dir/hconf/$file_mk
target_dir=`dirname $target_h`
test -d $target_dir || mkdir -p $target_dir
if [ ! $is_source ]; then
	cat << END > $main_c
#include <$file>
#ifndef HCONF_TEST
# define HCONF_TEST return 0
#endif
int main(void) {
	HCONF_TEST;
	return 0;
}
END
else
	cat << END > $main_c
int main(void) {
	return 0;
}
END
fi

args0=$no_option
for i in 1 2 3 4; do
	eval args$i=
done
try
list_line_no=`grep -n "if defined(HCONF_" $file | cut -f1 -d:`
for line_no in $list_line_no; do
	args0=`sed -n "${line_no}s/.*if defined(\(HCONF_.*\)).*/\1/p" $file`
	while true; do
		line_no=`expr $line_no + 1`
		line="`sed -n "${line_no}p" $file`"

		var=`echo "$line" | sed -n 's/\/\* *\([_0-9A-Za-z]*\)=.*/\1/p'`
		if [ ! $var ]; then
			break
		fi
		expr=`echo "$line" | sed -n 's/\/\* *\(.*\) *\*\/.*/\1/p'`
		if [ "CPPFLAGS" = $var ] ||
			[ "CFLAGS" = $var ] ||
			[ "LDFLAGS" = $var ] ||
			[ "LIBS" = $var ]; then
			eval OPT_$expr
		else
			eval $expr
		fi
	done
	args1="$OPT_CPPFLAGS"
	args2="$OPT_CFLAGS"
	args3="$OPT_LDFLAGS"
	args4="$OPT_LIBS"
	try
done
if [ $best_args0 ]; then
	for i in 0 1 2 3 4; do
		eval args$i=\$best_args$i
	done
	write_hconf
	echo Will use non-optimal option $args0. | tee -a $log
	exit 0
fi
clean_hconf
echo " $file: No usable HCONF_* option found." 1>&2
exit 1
