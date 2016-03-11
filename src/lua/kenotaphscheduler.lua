local KsServer = require ("ksserver")

local kenotaphscheduler = {}

local KenotaphScheduler = {
	server = {}
}

function KenotaphScheduler:addServer (name)
	print ("Adding a server '" .. name .. "'")
	self.server[name] = KsServer:new (name)
	return self.server[name]
end

function KenotaphScheduler:getServer (name)
	return self.server[name]
end

function kenotaphscheduler.new ()
	local self = {}

	setmetatable (self, { __index = KenotaphScheduler })

	return self
end

return kenotaphscheduler

