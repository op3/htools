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

if (1 -gt $args.length) {
	write-host Usage: $myInvocation.myCommand \"$(IntDir) \" files...
	exit 1
}
$intdir=$($args[0]).trim()
for ($i = 1; $i -lt $args.length; $i++) {
	$p="$intdir\hconf\$($args[$i])"
	$dirname=split-path -parent $p
	if (!(test-path $dirname)) {
		ni $dirname -type directory | out-null
	}
	if (!(test-path $p)) {
		ni $p -type file | out-null
	}
}
