#!/bin/sh

# Copyright (c) 2016-2017 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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

# Checks the copyright clause and years according to the repository.

missing=
invalid=
skipped=
pass=
inputs=`hg status -acdmr | grep -v -e strlc -e queue.h -e tree.h | sed 's, ,:,'`
input_num=`echo $inputs | wc -w | sed 's, *\(.*\) *,\1,'`
input_i=0
for input in $inputs
do
	path=`echo $input | cut -d':' -f2`
	input_i=$(($input_i + 1))
	progress=`echo "(100 * $input_i) / ($input_num - 1)" | bc`
	echo -n "\rProgress: $progress% ($input_i/$input_num)"
	res=`head -n10 $path | grep Copyright`
	if [ "x" = "x$res" ]
	then
		missing="$missing
$path"
		continue
	fi
	years="`hg log $path | grep ^date: | sed 's,.* \(20..\) .*,\1,'`"
	status=`echo $input | cut -d':' -f1`
	[ "$status" = "M" -o "$status" = "A" ] && years="$years `date +%Y`"
	if [ "x" = "x$years" ]
	then
		skipped="$skipped
$path"
		continue
	fi
	years="$years 10000"
	start_year=`echo $years | cut -d' ' -f1`
	end_year=$start_year
	years=`echo $years | cut -d' ' -f2-`
	compact=`first=1
	for year in $years
	do
		[ $year = $end_year ] && continue
		if [ $year -eq $(($end_year + 1)) ]
		then
			end_year=$year
			continue
		fi
		[ "$first" ] || echo ", "
		first=
		if [ $start_year = $end_year ]
		then
			echo -n $start_year
		else
			echo -n $start_year-$end_year
		fi
		[ $year = 10000 ] && break
		start_year=$year
		end_year=$year
	done`
	match=`echo $res | grep "$compact"`
	if [ "x" = "x$match" ]
	then
		invalid="$invalid
$path: Should be \"$compact\""
		continue
	fi
	pass="$pass $path"
done
echo
echo "Invalid:$invalid
Missing:$missing
Skipped:$skipped
Passed:$pass" | less -F
