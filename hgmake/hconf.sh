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
	cc=`cat $target_mk | sed -n 1p`
	cmd="$cc $build_cppflags $build_cflags $build_ldflags -o $build_output $build_input $build_libs"
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
	try_option="$args0"
	try_cppflags=`sed -n 2p $target_mk`
	try_cflags=`sed -n 3p $target_mk`
	try_ldflags=`sed -n 4p $target_mk`
	try_libs=`sed -n 5p $target_mk`
	echo -n " $file: Testing option '$try_option'... " | tee -a $log
	if [ $is_source ]; then
		try_compile_input=$file_c
		try_compile_output=$file_o
		try_link_input="$file_o $main_c"
	else
		try_compile_input=$main_c
		try_compile_output=$main_o
		try_link_input=$main_o
	fi
	build "$try_compile_output" "$try_compile_input" "$try_cppflags -I. -I$build_dir" "$try_cflags -c" "" ""
	compile_result=$build_result
	if [ 0 -eq $compile_result ]; then
		if [ "$no_option" = "$try_option" ]; then
			clean_hconf
			echo Compilation must fail if file not hconf:ed! | tee -a $log
			exit 1
		fi
		build /dev/null "$try_link_input" "" "" "$try_ldflags" "$try_libs"
		try_link_result=$build_result
		if [ 0 -eq $try_link_result ]; then
			echo Perfect. | tee -a $log
			exit 0
		fi
		if [ $try_link_result -lt $best_link_result ]; then
			best_args0=$try_option
			best_args1=$try_cppflags
			best_args2=$try_cflags
			best_args3=$try_ldflags
			best_args4=$try_libs
			best_link_result=$try_link_result
			echo Keeping. | tee -a $log
		fi
	else
		if [ "$no_option" = "$try_option" ]; then
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
	echo > $target_mk.tmp
	for i in 1 2 3 4; do
		eval echo \$args$i >> $target_mk.tmp
	done
	$HTOOLS_PATH/hgmake/hconf_merge.sh $base_mk $target_mk.tmp > $target_mk
	rm -f $target_mk.tmp
}

verbose=0
if [ "x-v" = "x$1" ]; then
	verbose=1
	shift
fi
if [ 3 -ne $# ]; then
	echo "Usage: $0 [-v] hconf-cache build-dir file" 1>&2
	echo " hconf-cache  Hconf cache data to be used." 1>&2
	echo " build-dir    Generated files will be put here." 1>&2
	echo " file         File to undergo hconf:ing." 1>&2
	echo " -v           Verbose output." 1>&2
	exit 1
fi
base_mk=$1
build_dir=$2
file=$3

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
[ -d $target_dir ] || mkdir -p $target_dir
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
	exit 0
fi
clean_hconf
echo " $file: No usable HCONF_* option found." 1>&2
exit 1
