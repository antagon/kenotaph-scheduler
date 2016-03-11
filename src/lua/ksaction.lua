local KsTimeRange = require ("kstimerange")

local ksaction = {}

local KsAction = {
	name = nil,
	cmd = nil,
	wtoff = {}
}

function KsAction:addTimeRange (strtoff_beg, strtoff_end)
	print ("Adding a time range (" .. strtoff_beg .. ", " .. strtoff_end .. ") for '" .. self.name .. "'")

	local time_range = KsTimeRange.new (strtoff_beg, strtoff_end)

	table.insert (self.wtoff, time_range)

	return time_range
end

function KsAction:setCmd (cmd)
	self.cmd = cmd
end

function KsAction:getCmd ()
	return self.cmd
end

function KsAction:getTimeRanges ()
	return self.wtoff
end

function ksaction.new (name)
	local self = {}

	setmetatable (self, { __index = KsAction })

	self.name = name

	return self
end

return ksaction

