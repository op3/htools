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

if (2 -gt $args.length) {
	write-host Usage: $myInvocation.myCommand \"$(IntDir) \" out-file files...
	exit 1
}
$outdir=$($args[0]).trim() + "\htest"
$outfile="$outdir\$($args[1])"
$outtmp="$outfile.tmp"
if (!(test-path $outdir)) {
	ni $outdir -type directory | out-null
}
$do_rebuild=test-path $outfile
$list=@()
for ($i = 2; $i -lt $args.length; $i++) {
	ls $($args[$i]) | %{
		$name=$_.name
		$suite="$outdir\$name" -replace '(.*)\.c', '$1.suite'
		if (!(test-path $suite) -or $suite.lastWriteTime -lt $_.lastWriteTime) {
			cat $_ | sls "HTEST_SUITE\((.*)\)" | %{$_.matches.groups[1].value} | sc $suite
			$do_rebuild=$true
		}
		cat $suite | %{$list+=$_}
	}
}
if ($do_rebuild) {
	sc $outtmp "#include <htest/htest.h>"
	foreach ($name in $list) {
		ac $outtmp "HTEST_SUITE_PROTO($name);"
	}
	ac $outtmp "HTEST_SUITE_LIST_BEGIN"
	foreach ($name in $list) {
		ac $outtmp "HTEST_SUITE_LIST_ADD($name)"
	}
	ac $outtmp "HTEST_SUITE_LIST_END"
	mv -force $outtmp $outfile
}
