require "kenotaph_sched"

local argv = {
	server = "test_server1",
	msg = "BEG bb_phone wlan0:1\n",
	msg_len = string.len ("BEG bb_phone wlan0\n"),
	next = {
		server = "test_server1",
		msg = "BEG android_phone wlan0\n",
		msg_len = string.len ("BEG android_phone wlan0\n"),
		next = nil
	}
}

local Kenotaph = NewKenotaphSched ()

Kenotaph:addServer ("test_server1")

function main (argv)
	if argv == nil then
		return 1
	end

	local arg = argv

::_continue::
	while arg do
		local lserver = Kenotaph:getServer (arg.server)

		if lserver == nil then
			goto _continue
		end

		print ("Server " .. lserver.name .. " found...")

		local evt_type, id, iface = string.match (arg.msg, "^([%a%p]+) ([%w%p]+) ([%w%p]+)\n$")

		print (evt_type, id, iface)

		arg = arg.next
	end

	return 0
end

--[[local argv = {
	{
		msg = "BEG danielbeseda wlan0",
		len = 23,
		prev = nil,
		next = nil
	}
}]]--

os.exit (main (argv))

