local KsAction = require ("ksaction")
local KsTimeRange = require ("kstimerange")

local ksdevice = {}

local KsDevice = {
	id = nil,
	action = {
		a_beg = {},
		a_end = {},
		a_err = {}
	}
}

function KsDevice:addAction (atype, name)
	print ("Adding an action '" .. name .. "' (" .. atype ..  ")")

	atype = string.lower (atype)

	-- Check whether atype is in defined range (i.e. beg/end/err)
	if self.action["a_" .. atype] == nil then
		return nil
	end

	self.action["a_" .. atype][name] = KsAction.new (name)
	return self.action["a_" .. atype][name]
end

function KsDevice:getAction (atype, name)
	atype = string.lower (atype)

	-- Check whether atype is in defined range (i.e. beg/end/err)
	if self.action["a_" .. atype] == nil then
		return nil
	end

	return self.action["a_" .. atype][name]
end

function KsDevice:getImmediateActions (atype)
	atype = string.lower (atype)

	-- Check whether atype is in defined range (i.e. beg/end/err)
	if self.action["a_" .. atype] == nil then
		return nil
	end

	local weekoff_now = KsTimeRange.strWeekTimeOffToInt (os.date ("%w %H:%M"))
	local actions = {}

	-- TODO: optimize algorithm by sorting the array by begin time offset
	for _, action in pairs (self.action["a_" .. atype]) do
		for _, time_range in ipairs (action:getTimeRanges ()) do
			if time_range:isInRange (weekoff_now) then
				table.insert (actions, action)
			end
		end
	end

	return actions
end

function ksdevice.new (id)
	local self = {}

	setmetatable (self, { __index = KsDevice })

	self.id = id

	return self
end

return ksdevice

