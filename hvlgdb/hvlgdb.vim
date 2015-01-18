" Copyright (c) 2015 Hans Toshihide Törnqvist <hans.tornqvist@gmail.com>
"
" Permission to use, copy, modify, and distribute this software for any
" purpose with or without fee is hereby granted, provided that the above
" copyright notice and this permission notice appear in all copies.
"
" THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
" WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
" MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
" ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
" WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
" ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
" OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

lua << EOF
g_breakpoint_filename = ".hvlgdb.breakpoints"
g_save_filename = ".hvlgdb.save"
g_breakpoint_table = {}
vim.command('sign define breakpoint text=#')
EOF

" Clears breakpoint list (table).
function! HVLGDB_clear()
lua << EOF
	for i,v in ipairs(g_breakpoint_table) do
		vim.command('sign unplace '..v[3])
	end
	g_breakpoint_table = {}
	print('HVLGDB_clear(): All breakpoints cleared!')
EOF
endfunction

" Loads breakpoints from g_save_filename, whatever that has been set to.
function! HVLGDB_load()
lua << EOF
	local file = io.open(g_save_filename, 'r')
	if nil == file then
		print('HVLGDB_load(): Could not load from "'..g_save_filename..
		  '".')
		return
	end
	g_breakpoint_filename = file:read('*l')
	g_save_filename = file:read('*l')
	local size = tonumber(file:read('*l'))
	for i = 1,size do
		local v = {}
		v[1] = file:read('*l')
		v[2] = tonumber(file:read('*l'))
		v[3] = tonumber(file:read('*l'))
		vim.command('sign place '..v[3]..' line='..v[2]..
		  ' name=breakpoint file='..v[1])
		table.insert(g_breakpoint_table, i, v)
	end
	file:close()
EOF
endfunction

" Jumps to the next breakpoint.
function! HVLGDB_next_breakpoint()
lua << EOF
	local w = vim.window()
	local filename = w.buffer.fname
	local line_no_min = 1000000
	local line_no_next = 1000000
	for i,v in ipairs(g_breakpoint_table) do
		if v[1] == filename then
			if line_no_min > v[2] then
				line_no_min = v[2]
			end
			if w.line < v[2] and line_no_next > v[2] then
				line_no_next = v[2]
			end
		end
	end
	if line_no_next <= #w.buffer then
		w.line = line_no_next
	elseif line_no_min <= #w.buffer then
		w.line = line_no_min
	end
EOF
endfunction

" Saves breakpoints to g_save_filename for future use.
function! HVLGDB_save()
lua << EOF
	local file = io.open(g_save_filename, 'w')
	if nil == file then
		print('HVLGDB_save(): Could not save to "'..g_save_filename..
		  '".')
		return
	end
	file:write(g_breakpoint_filename..'\n')
	file:write(g_save_filename..'\n')
	file:write(#g_breakpoint_table..'\n')
	for i,v in ipairs(g_breakpoint_table) do
		file:write(v[1]..'\n')
		file:write(v[2]..'\n')
		file:write(v[3]..'\n')
	end
	file:close()
EOF
endfunction

" Asks for new filenames for the session.
function! HVLGDB_set_filenames()
lua << EOF
	vim.command('let l:filename = input("Script:", "'..
	  g_breakpoint_filename..'", "file")')
	g_breakpoint_filename = vim.eval('l:filename')
	vim.command('let l:filename = input("Script:", "'..g_save_filename..
	  '", "file")')
	g_save_filename = vim.eval('l:filename')
EOF
endfunction

" Toggles breakpoint on the current line.
function! HVLGDB_toggle_breakpoint()
lua << EOF
	local w = vim.window()
	local line_no = w.line
	local filename = w.buffer.fname
	for i,v in ipairs(g_breakpoint_table) do
		if v[1] == filename and v[2] == line_no then
			local id = v[3]
			table.remove(g_breakpoint_table, i)
			vim.command('sign unplace '..id..' file='..filename)
			return
		end
	end
	local id = 1
	local done
	repeat
		done = true
		for i,v in ipairs(g_breakpoint_table) do
			if v[3] == id then
				id = 1 + id
				done = false
			end
		end
	until done
	vim.command('sign place '..id..' line='..line_no..
	  ' name=breakpoint file='..filename)
	table.insert(g_breakpoint_table, {filename, line_no, id})
EOF
endfunction

" Writes breakpoints to g_breakpoint_filename for GDB.
function! HVLGDB_write_breakpoints()
lua << EOF
	local file = io.open(g_breakpoint_filename, 'w')
	if nil == file then
		print('HVLGDB_write_breakpoints(): Could not save to "'..
		  g_breakpoint_filename..'".')
		return
	end
	for i,v in ipairs(g_breakpoint_table) do
		file:write('break '..v[1]..':'..v[2]..'\n')
	end
	file:close()
	print('HVLDGB_write_breakpoints(): Breakpoints written!')
EOF
endfunction
