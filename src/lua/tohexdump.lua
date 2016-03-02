if #arg < 3 then
	io.write (string.format ("Usage: %s <input> <output> <var-name>\n", arg[0]))
	os.exit (1)
end

local infile = arg[1]
local outfile = arg[2]
local varname = arg[3]

local exitval = 0
local fdin = nil
local fdout = nil
local errmsg = ""
local hexstr_len = 0
local hexstr = ""

fdin, errmsg = io.open (infile, "rb")

if fdin == nil then
	io.write (string.format ("cannot open file %s\n", errmsg))
	exitval = 1
	goto exit
end

while true do
	local b = fdin:read (1)

	if b == nil then
		break
	end

	hexstr = hexstr .. string.format ("\\x%02x", string.byte (b))
	hexstr_len = hexstr_len + 1
end

fdout, errmsg = io.open (outfile, "wb")

if fdout == nil then
	io.write (string.format ("cannot open file %s\n", errmsg))
	exitval = 1
	goto exit
end

fdout:write (string.format ("const char %s[] = \"%s\";\nunsigned long int %s_len = %d;\n", varname, hexstr, varname, hexstr_len))

::exit::
if fdin ~= nil then
	io.close (fdin)
end

if fdout ~= nil then
	io.close (fdout)
end

os.exit (exitval)

