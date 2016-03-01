KenotaphTimeRange = {
	t_from = 0,
	t_to = 0,

	strWeekTimeOffToInt = function (str)
		--local date_str = os.date ("%w %H:%M")
		local wday, hour, minute = string.match (str, "^(%d+) (%d+):(%d+)$")

		if wday == nil or hour == nil or minute == nil then
			return nil
		end

		return (wday * 86400 + hour * 3600 + minute * 60)
	end,

	isInRange = function (self, time)
		print ("Time: " .. time .. " From: " .. self.t_from .. " To: " .. self.t_to)
		return (time >= self.t_from and time < self.t_to)
	end
}

KenotaphAction = {
	name = action_name,
	cmd = nil,
	wtoff = {},

	addTimeRange = function (self, strtoff_beg, strtoff_end)
		print ("Adding a time range (" .. strtoff_beg .. ", " .. strtoff_end .. ") for '" .. self.name .. "'")
		local time_range = KenotaphTimeRange:new (strtoff_beg, strtoff_end)

		table.insert (self.wtoff, time_range)

		return time_range
	end,

	getTimeRanges = function (self)
		return self.wtoff
	end
}

KenotaphDevice = {
	id = device_id,
	action = {
		a_beg = {},
		a_end = {},
		a_err = {}
	},

	addAction = function (self, atype, name)
		print ("Adding an action '" .. name .. "' (" .. atype ..  ")")

		atype = string.lower (atype)

		-- Check whether atype is in defined range (i.e. beg/end/err)
		if self.action["a_" .. atype] == nil then
			return nil
		end

		self.action["a_" .. atype][name] = KenotaphAction:new (name)
		return self.action["a_" .. atype][name]
	end,

	getAction = function (self, atype, name)
		atype = string.lower (atype)

		-- Check whether atype is in defined range (i.e. beg/end/err)
		if self.action["a_" .. atype] == nil then
			return nil
		end

		return self.action["a_" .. atype][name]
	end,

	getImmediateAction = function (self, atype)
		atype = string.lower (atype)

		-- Check whether atype is in defined range (i.e. beg/end/err)
		if self.action["a_" .. atype] == nil then
			return nil
		end

		local weekoff_now = KenotaphTimeRange.strWeekTimeOffToInt (os.date ("%w %H:%M"))

		-- TODO: optimize algorithm by sorting the array by begin time offset
		for action_idx, action in pairs (self.action["a_" .. atype]) do
			for timerange_idx, time_range in ipairs (action:getTimeRanges ()) do
				if time_range:isInRange (weekoff_now) then
					return action
				end
			end
		end

		return nil
	end
}

KenotaphServer = {
	name = server_name,
	device = {},

	addDevice = function (self, id)
		print ("Adding a device '" .. id .. "'")
		self.device[id] = KenotaphDevice:new (id)
		return self.device[id]
	end,

	getDevice = function (self, name)
		return self.device[name]
	end
}

KenotaphSched = {
	server = {},

	addServer = function (self, name)
		print ("Adding a server '" .. name .. "'")
		self.server[name] = KenotaphServer:new (name)
		return self.server[name]
	end,

	getServer = function (self, name)
		return self.server[name]
	end
}

function KenotaphTimeRange:new (strtoff_beg, strtoff_end)
	local time_range = {}

	setmetatable (time_range, KenotaphTimeRange)

	self.__index = self

	time_range.t_from = self.strWeekTimeOffToInt (strtoff_beg)
	time_range.t_to = self.strWeekTimeOffToInt (strtoff_end)

	return time_range
end

function KenotaphAction:new (name)
	local action = {}

	setmetatable (action, KenotaphAction)

	self.__index = self

	action.name = name

	return action
end

function KenotaphDevice:new (id)
	local device = {}

	setmetatable (device, KenotaphDevice)

	self.__index = self

	device.id = id

	return device
end

function KenotaphServer:new (name)
	local server = {}

	setmetatable (server, KenotaphServer)

	self.__index = self

	server.name = name

	return server
end

function KenotaphSched:new ()
	local resource = {}

	setmetatable (resource, KenotaphSched)

	self.__index = self

	return resource
end

