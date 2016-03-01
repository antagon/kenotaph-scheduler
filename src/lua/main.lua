require "kenotaph_sched"

local KenotaphSched = KenotaphSched:new ()

KenotaphSched:addServer ("test_server1")

KenotaphSched:getServer ("test_server1"):addDevice ("bb_phone")

KenotaphSched:getServer ("test_server1"):getDevice ("bb_phone"):addAction ("beg", "test_action1_beg")
KenotaphSched:getServer ("test_server1"):getDevice ("bb_phone"):addAction ("end", "test_action1_end")

KenotaphSched:getServer ("test_server1"):getDevice ("bb_phone"):getAction ("beg", "test_action1_beg"):addTimeRange ("2 02:20", "2 03:29")
KenotaphSched:getServer ("test_server1"):getDevice ("bb_phone"):getAction ("end", "test_action1_end"):addTimeRange ("2 02:20", "2 03:29")

--
-- Test input data...
--
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

function main (argv)
	if argv == nil then
		return 1
	end

	local arg = argv

	while arg do
		local server = KenotaphSched:getServer (arg.server)

		if server ~= nil then
			io.write ("== GOT message " .. arg.msg)

			local evt_type, id, iface = string.match (arg.msg, "^([%a%p]+) ([%w%p]+) ([%w%p]+)\n$")

			local device = server:getDevice (id)

			if device ~= nil then
				io.write ("Device " .. device.id .. "\n")

				local action = device:getImmediateAction (evt_type)

				if action ~= nil then
					io.write ("Now running by the action " .. action.name .. "\n")
				else
					io.write ("No scheduled action for this event\n")
				end
			else
				io.write ("Unknown device " .. id .. "\n")
			end
		else
			io.write ("server " .. lserver .. " not found...\n")
		end

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

