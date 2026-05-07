local pow = math.pow
local cos = math.cos
local sin = math.sin

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

function Vec3:lengthSquared(): number
	return pow(self.x, 2) + pow(self.y, 2) + pow(self.z, 2)
end

function Vec3:length(): number
	return math.sqrt(self:lengthSquared())
end

function Vec3:normalize(): Vec3
	local l = 1 / self:length()
	return Vec3.new(self.x * l, self.y * l, self.z * l)
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
end

function Quat.new(yaw: number, pitch: number, roll: number): Quat
	local self: Quat = setmetatable({}, { __index = Quat })
	self.yaw = yaw
	self.pitch = pitch
	self.roll = roll
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
	self.angle = Quat.new(0, 0, 0)
	self.scale = Vec3.new(1, 1, 1)
	self.origin = Vec3.new(0, 0, 0)
	return self
end

local function Clamp(x: number, min: number, max: number): number
	return math.min(math.max(x, min), max)
end

local function Cycle(x: number, cycle: number): number
	while x < 0 do x = x + cycle end
	return x % cycle
end

local function RotationLerp(a: number, b: number, t: number): number
	local angle = ((((b - a) % 360) + 540) % 360) - 180
	return a + angle * t
end

local function HSV2RGB(h: number, s: number, v: number): Vec3
	local c = s * v
	local x = c * (1 - math.abs((h / 60) % 2 - 1))
	local m = v - c

	local clr = Vec3.new

	if h < 60 then return clr(c + m, x + m, m)
	elseif h < 120 then return clr(x + m, c + m, m)
	elseif h < 180 then return clr(m, c + m, x + m)
	elseif h < 240 then return clr(m, x + m, c + m)
	elseif h < 300 then return clr(x + m, m, c + m)
	else return clr(c + m, m, x + m) end
end

local function Lerp(a: number, b: number, t: number): number
	return a * (1 - t) + b * t
end

local function SineIn(t: number): number
	return 1 - cos(t * math.pi * 0.5)
end

local function SineOut(t: number): number
	return sin(t * math.pi * 0.5)
end

local function SineInOut(t: number): number
	return -(cos(math.pi * t) - 1) * 0.5
end

local function QuadIn(t: number): number
	return pow(t, 2)
end

local function QuadOut(t: number): number
	return 1 - pow(1 - t, 2)
end

local function QuadInOut(t: number): number
	if t < 0.5 then return 2 * pow(t, 2)
	else return 1 - pow(-2 * t + 2, 2) * 0.5 end
end

local function CubicIn(t: number): number
	return pow(t, 3)
end

local function CubicOut(t: number): number
	return 1 - pow(1 - t, 3)
end

local function CubicInOut(t: number): number
	if t < 0.5 then return 4 * pow(t, 3)
	else return 1 - pow(-2 * t + 2, 3) * 0.5 end
end

local function Contains(bounds: Vec4, point: Vec2): boolean
	return (
		point.x >= bounds.x and point.x <= bounds.x + bounds.z and
		point.y >= bounds.y and point.y <= bounds.y + bounds.w
	)
end

local NetworkStatus: { string : number} = {
	Idle = 0,
	Disconnected = 1,
	Connected = 2,
	ConnectionFailed = 3,
	Connecting = 4,
	Searching = 5,
	ServerFound = 6,
	ServerNotFound = 7
}

return {
	Vec2 = Vec2,
	Vec3 = Vec3,
	Vec4 = Vec4,
	Quat = Quat,
	Transform = Transform,
	clamp = Clamp,
	cycle = Cycle,
	rotationLerp = RotationLerp,
	hsv2rgb = HSV2RGB,
	lerp = Lerp,
	contains = Contains,
	easings = {
		sineIn = SineIn,
		sineOut = SineOut,
		sineInOut = SineInOut,
		quadIn = QuadIn,
		quadOut = QuadOut,
		quadInOut = QuadInOut,
		cubicIn = CubicIn,
		cubicOut = CubicOut,
		cubicInOut = CubicInOut
	},
	networkStatus = NetworkStatus
}