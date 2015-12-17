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

if [ 0 -eq $# ]; then
	echo Usage: $0 hconf-cache-files
	echo " Does the following for each line of the given hconf cache files:"
	echo " 1) Takes the line from the first file."
	echo " 2) Removes duplicates and filters away -D* with lesser values, e.g.:"
	echo "  ./$0 -D_BSD_SOURCE -D_POSIX_C_SOURCE=1 -D_POSIX_C_SOURCE=200809 -I."
	echo "  -D_BSD_SOURCE -D_POSIX_C_SOURCE=200809 -I."
	echo " 3..4) Removes duplicates."
	echo " 5) Removes consecutive duplicates."
	echo " The result is dumped to stdout."
	exit 1
fi

pasted=`paste -d' ' $*`

# Line 1.
head -1 $1
# Line 2.
src=`echo "$pasted" | sed -n 2p`
# TODO: Separate -D and the rest, and filter only the -D options.
src=`echo "$src" | tr ' ' '\n' | awk '!a[$0]++'`
srcn=`echo "$src" | wc -l`
i=0
while true; do
	i=`expr $i + 1`
	[ $srcn -lt $i ] && break
	arg_i=`echo "$src" | sed -n ${i}p`
	[ "x-D" != "x`echo $arg_i | cut -c 1-2`" ] && continue
	dname_i=`echo "$arg_i" | sed -n 's/-D\([^=]*\)=.*/\1/p'`
	dvalue_i=`echo "$arg_i" | sed -n 's/-D[^=]*=\(.*\)/\1/p'`
	j=$i
	while true; do
		j=`expr $j + 1`
		[ $srcn -lt $j ] && break
		arg_j=`echo "$src" | sed -n ${j}p`
		[ "x-D" != "x`echo $arg_j | cut -c 1-2`" ] && continue
		dname_j=`echo "$arg_j" | sed -n 's/-D\([^=]*\)=.*/\1/p'`
		dvalue_j=`echo "$arg_j" | sed -n 's/-D[^=]*=\(.*\)/\1/p'`
		if [ "$dname_i" -a "x$dname_j" = "x$dname_i" ]; then
			do_break=
			if [ 1 -eq `expr $dvalue_j '>' $dvalue_i` ]; then
				del_i=$i
				i=`expr $i - 1`
				do_break=1
			else
				del_i=$j
				j=`expr $j - 1`
			fi
			src=`echo "$src" | sed ${del_i}d`
			srcn=`expr $srcn - 1`
			[ "$do_break" ] && break
		fi
	done
done
echo $src
# Line 3,4.
for line_i in 3 4; do
	echo "$pasted" | sed -n ${line_i}p | tr ' ' '\n' | sort -u | tr '\n' ' '
	echo
done
# Line 5.
echo "$pasted" | sed -n 5p | tr ' ' '\n' | awk '!a[$0]++' | tr '\n' ' '
echo
