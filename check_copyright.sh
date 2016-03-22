#!/bin/sh
# Copyright (c) 2016 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
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
pass=
for file in `find . -type f | grep -v \.hg | grep -v data | grep -v build_`
do
	res=`head -n2 $file | grep Copyright | grep Toshihide`
	if [ "x" = "x$res" ]
	then
		missing="$missing\n$file"
		continue
	fi
	years="`hg log $file | grep ^date: | sed 's,.* \(20..\) .*,\1,'`"
	years="`echo $years | awk '{do printf"%s"(NF>1?FS:RS),$NF;while(--NF)}'`"
	set -A array
	for year in $years
	do
		array[${#array[@]}]=$year
	done
	start_value=${array[0]}
	end_value=$start_value
	i=0
	compact=`first=1
	while true
	do
		i=$(($i + 1))
		if [ $i -ge ${#array[@]} ]
		then
			value=10000
		else
			value=${array[$i]}
		fi
		[ $value = $end_value ] && continue
		if [ $value = $(($end_value + 1)) ]
		then
			end_value=$value
		else
			[ 1 -ne $first ] && echo ", "
			first=0
			if [ $start_value -eq $end_value ]
			then
				echo -n $start_value
			else
				echo -n $start_value-$end_value
			fi
			[ 10000 -eq $value ] && break
			start_value=$value
			end_value=$start_value
		fi
	done`
	match=`echo $res | grep "$compact"`
	if [ "x" = "x$match" ]
	then
		invalid="$invalid\n$file:\"$compact\""
		continue
	fi
	pass="$pass\n$file"
done
echo Missing:$missing\\nInvalid:$invalid\\nPass:$pass | less
