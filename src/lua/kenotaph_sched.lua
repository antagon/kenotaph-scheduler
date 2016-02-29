function NewKenotaphAction ()
	return {
		cmd = nil,
		toff = {
			t_begin = 0,
			t_end = 0
		},

		strToWeekTimeOffset = function (str)
			--local date_str = os.date ("%w %H:%M")
			local wday, hour, minute = string.match (str, "^(%d+) (%d+):(%d+)$")

			return (wday * 86400 + hour * 3600 + minute * 60)
		end,

		setTimeOffset = function (self, which, time_str)
			local time_off = self.strToWeekTimeOffset (time_str)

			if time_off == nil then
				return false
			end

			self.toff[which] = time_off

			return true
		end,

		setBeginTimeOffset = function (self, time_str)
			return self.setTimeOffset (self, "t_begin", time_str)
		end,

		setEndTimeOffset = function (self, time_str)
			return self.setTimeOffset (self, "t_end", time_str)
		end,

		setCmd = function (self, cmd)
			self.cmd = cmd
		end
	}
end

function NewKenotaphDevice ()
	return {
		action = {
			a_begin = nil,
			a_end = nil,
			a_error = nil
		},

		addAction = function (self, atype, name)
			print ("Adding an action " .. name .. "(" .. atype ..  ")")
			self.action[atype] = NewKenotaphAction ()
			return self.action[atype]
		end

		-- TODO...
		-- sortActions
	}
end

function NewKenotaphServer (server_name)
	return {
		name = server_name,
		device = {},

		addDevice = function (self, name)
			print ("Adding a device " .. name)
			self.device[name] = NewKenotaphDevice ()
			return self.device[name]
		end,

		getDevice = function (self, name)
			return self.device[name]
		end
	}
end

function NewKenotaphSched ()
	return {
		server = {},

		addServer = function (self, name)
			print ("Adding a server " .. name)
			self.server[name] = NewKenotaphServer (name)
			return self.server[name]
		end,

		getServer = function (self, name)
			return self.server[name]
		end
	}
end

--[[local Kenotaph = NewKenotaphSched ()

local keno_server = Kenotaph:addServer ("my_testserver")

local keno_dev = keno_server:addDevice ("my_testdevice")--]]

