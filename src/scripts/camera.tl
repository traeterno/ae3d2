local aeMath = require("aeMath")

local record Camera
	position: aeMath.Vec3
	orientation: aeMath.Vec3
	distance: number
	sensitivity: number
	cursorLock: boolean
	direction: aeMath.Vec3
	front: aeMath.Vec2
	speed: number
end

function Camera.new(): Camera
	local self: Camera = setmetatable({}, { __index = Camera })
	self.position = aeMath.Vec3.new(0, 0, 0)
	self.orientation = aeMath.Vec3.new(0, 0, 0)
	self.distance = 0
	self.sensitivity = 0.25
	self.cursorLock = false
	self.direction = aeMath.Vec3.new(0, 0, 1)
	self.front = aeMath.Vec2.new(0, 1)
	self.speed = 1
	aeCamera.lookAt({
		position = self.position,
		target = self.direction
	})
	return self
end

function Camera:buildAxes()
	local yaw = math.rad(-self.orientation.x)
	local pitch = math.rad(self.orientation.y)
	self.front.x = math.sin(yaw)
	self.front.y = math.cos(yaw)
	self.direction.z = self.front.y * math.cos(pitch)
	self.direction.x = self.front.x * math.cos(pitch)
	self.direction.y = math.sin(pitch)
end

function Camera:noclip()
	if not aeWindow.isFocused() then
		self.cursorLock = false
		return
	end
	if aeWindow.keyJustPressed("Tab") then
		self.cursorLock = not self.cursorLock
		aeCamera.mouseDelta()
	end
	if not self.cursorLock then return end
	local deltaAngle = aeCamera.mouseDelta()
	self.orientation.x = (self.orientation.x + deltaAngle.x * self.sensitivity) % 360.0
	self.orientation.y = aeMath.clamp(
		self.orientation.y - deltaAngle.y * self.sensitivity, -80, 80
	)

	self:buildAxes()

	local input = aeMath.Vec3.new(0, 0, 0)
	if aeWindow.keyPressed("W") then input.z = input.z + 1 end
	if aeWindow.keyPressed("S") then input.z = input.z - 1 end
	if aeWindow.keyPressed("A") then input.x = input.x - 1 end
	if aeWindow.keyPressed("D") then input.x = input.x + 1 end
	if aeWindow.keyPressed("Space") then input.y = input.y + 1 end
	if aeWindow.keyPressed("LShift") then input.y = input.y - 1 end

	local movement = aeMath.Vec3.new(
		self.front.x * input.z - self.front.y * input.x,
		input.y,
		self.front.y * input.z + self.front.x * input.x
	)

	self.position.x = self.position.x + movement.x * self.speed
	self.position.y = self.position.y + movement.y * self.speed
	self.position.z = self.position.z + movement.z * self.speed

	aeCamera.buildView({
		pos = self.position,
		orientation = self.orientation,
		distance = self.distance
	})
end

function Camera:rotate()
	if not aeWindow.isFocused() then return end
	self.cursorLock = false
	
	local deltaAngle = aeCamera.mouseDelta()
	self.orientation.x = (self.orientation.x + deltaAngle.x * self.sensitivity) % 360.0
	self.orientation.y = aeMath.clamp(
		self.orientation.y - deltaAngle.y * self.sensitivity, -80, 80
	)

	self:buildAxes()
	aeCamera.buildView({
		pos = self.position,
		orientation = self.orientation,
		distance = self.distance
	})
end

function Camera:follow(point: aeMath.Vec3, height: number)
	self.position = aeMath.Vec3.new(point.x, point.y + height, point.z)

	aeCamera.buildView({
		pos = self.position,
		orientation = self.orientation,
		distance = self.distance
	})
end

function Camera:apply()
	self.orientation.x = self.orientation.x % 360.0
	self.orientation.z = self.orientation.z % 360.0
	self.orientation.y = aeMath.clamp(self.orientation.y, -80, 80)
	self:buildAxes()
	aeCamera.buildView({
		pos = self.position,
		orientation = self.orientation,
		distance = self.distance
	})
end

return Camera