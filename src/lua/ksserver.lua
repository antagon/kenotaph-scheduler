local KsDevice = require ("ksdevice")

local ksserver = {}

local KsServer = {
	name = nil,
	device = {}
}

function KsServer:addDevice (id)
	print ("Adding a device '" .. id .. "'")
	self.device[id] = KsDevice:new (id)
	return self.device[id]
end

function KsServer:getDevice (name)
	return self.device[name]
end

function ksserver.new (name)
	local self = {}

	setmetatable (self, { __index = KsServer })

	self.name = name

	return self
end

return ksserver

