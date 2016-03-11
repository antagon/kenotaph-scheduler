local kstimerange = {}

local KsTimeRange = {
	t_from = 0,
	t_to = 0
}

function KsTimeRange.strWeekTimeOffToInt (str)
	--local date_str = os.date ("%w %H:%M")
	local wday, hour, minute = string.match (str, "^(%d+) (%d+):(%d+)$")

	if wday == nil or hour == nil or minute == nil then
		return nil
	end

	return (wday * 86400 + hour * 3600 + minute * 60)
end

function KsTimeRange:isInRange (time)
	print ("Time: " .. time .. " From: " .. self.t_from .. " To: " .. self.t_to)
	return (time >= self.t_from and time < self.t_to)
end

function kstimerange.new (strtoff_beg, strtoff_end)
	local self = {}

	setmetatable (self, { __index = KsTimeRange })

	self.t_from = self.strWeekTimeOffToInt (strtoff_beg)
	self.t_to = self.strWeekTimeOffToInt (strtoff_end)

	return self
end

return kstimerange

