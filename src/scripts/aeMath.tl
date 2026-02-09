local record Vec2
	x: number
	y: number
end

function Vec2.new(x: number, y: number): Vec2
	local self: Vec2 = setmetatable({}, { __index = Vec2 })
	self.x = x
	self.y = y
	return self
end

local record Vec3
	x: number
	y: number
	z: number
end

function Vec3.new(x: number, y: number, z: number): Vec3
	local self: Vec3 = setmetatable({}, { __index = Vec3 })
	self.x = x
	self.y = y
	self.z = z
	return self
end

local record Vec4
	x: number
	y: number
	z: number
	w: number
end

function Vec4.new(x: number, y: number, z: number, w: number): Vec4
	local self: Vec4 = setmetatable({}, { __index = Vec4 })
	self.x = x
	self.y = y
	self.z = z
	self.w = w
	return self
end

local record Quat
	yaw: number
	pitch: number
	roll: number
	relative: boolean
end

function Quat.new(yaw: number, pitch: number, roll: number, relative: boolean): Quat
	local self: Quat = setmetatable({}, { __index = Quat })
	self.yaw = yaw
	self.pitch = pitch
	self.roll = roll
	self.relative = relative
	return self
end

local record Transform
	pos: Vec3
	angle: Quat
	scale: Vec3
	origin: Vec3
end

function Transform.new(): Transform
	local self: Transform = setmetatable({}, { __index = Transform })
	self.pos = Vec3.new(0, 0, 0)
	self.angle = Quat.new(0, 0, 0, false)
	self.scale = Vec3.new(1, 1, 1)
	self.origin = Vec3.new(0, 0, 0)
	return self
end

return {
	Vec2 = Vec2,
	Vec3 = Vec3,
	Vec4 = Vec4,
	Quat = Quat,
	Transform = Transform
}