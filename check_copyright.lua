-- Copyright (c) 2018 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
--
-- Permission to use, copy, modify, and/or distribute this software for any
-- purpose with or without fee is hereby granted, provided that the above
-- copyright notice and this permission notice appear in all copies.
--
-- THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
-- WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
-- MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
-- ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
-- WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
-- ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
-- OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

-- Checks the copyright clause and years according to the repository.

function append(str, first, last)
	if str == nil then
		str = ''
	else
		str = str .. ', '
	end
	str = str .. first
	if first < last then
		str = str .. '-' .. last
	end
	return str
end

lookup = {}
f = io.popen('hg log -v')
for line in f:lines() do
	pre = string.sub(line, 1, 5)
	if pre == "date:" then
		local tokens = {}
		for token in string.gmatch(line, "%S+") do
			table.insert(tokens, token)
		end
		year = tokens[6]
	elseif pre == "files" then
		local files = {}
		for file in string.gmatch(line, "%S+") do
			table.insert(files, file)
		end
		table.remove(files, 1)
		for key,file in pairs(files) do
			if not lookup[file] then
				lookup[file] = {}
			end
			lookup[file][year] = 1
		end
	end
end
f:close()

fine = {}
missing = {}
invalid = {}
for file,years in pairs(lookup) do
	sorted = {}
	for year,yes in pairs(years) do
		table.insert(sorted, year)
	end
	table.sort(sorted)

	compact = nil
	first = sorted[1]
	last = first
	table.remove(sorted, 1)
	for i,year in ipairs(sorted) do
		if year + 0 == last + 1 then
			last = year
		elseif year + 0 > last + 1 then
			compact = append(compact, first, last)
			first = year
			last = year
		end
	end
	compact = append(compact, first, last)

	f = io.open(file)
	if nil ~= f then
		ok = false
		has_copyright = false
		line_no = 0
		for line in f:lines() do
			if line_no > 10 then
				break
			end
			line_no = line_no + 1
			if nil ~= line:find('Copyright') then
				has_copyright = true
			end
			if nil ~= line:find(compact, 1, true) then
				ok = true
				break
			end
		end
		f:close()
		if not has_copyright then
			missing[#missing + 1] = file
		elseif not ok then
			invalid[#invalid + 1] = file .. ' is not ' .. compact
		else
			fine[#fine + 1] = file
		end
	end
end

print('Invalid:')
for idx,file in pairs(invalid) do
	print(' ' .. file)
end
print('Missing:')
for idx,file in pairs(missing) do
	print(' ' .. file)
end
print('Fine:')
for idx,file in pairs(fine) do
	print(' ' .. file)
end
