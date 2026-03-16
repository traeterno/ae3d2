local aeMath = require("aeMath")
local Camera = require("camera")

local cam: Camera
local ts: aeMath.Transform
local distance: number
local camHeight: number

local getInput: function(): aeMath.Vec3

global function Init()

	aeCamera.loadGLTF("heroV2")
	aeEntity.loadMesh("heroV2", "Body")
	aeCamera.unloadGLTF("heroV2")

	distance = 2.5
	camHeight = 1.25

	aeEntity.setAnimation("Summon")
	-- aeEntity.setAnimation("IdleUpper")
	aeEntity.setAnimation("IdleLower")

	cam = Camera.new()
	cam.distance = 2.5
	cam.orientation.y = -45
	cam.orientation.x = 90
	cam:apply()

	ts = aeMath.Transform.new()
	ts.angle.yaw = 90
	ts.pos.y = 0.25
	ts.pos.x = 0.55
	cam:follow(ts.pos, camHeight)
end

global function Update()
	local dt = aeWindow.dt()
	cam:rotate()

	distance = aeMath.clamp(distance + aeWindow.scroll().y * 0.5, 0.5, 5)
	cam.distance = aeMath.lerp(cam.distance, distance, dt * 10)

	local input = getInput()
	if input.x == 0 and input.z == 0 then return end
	local angle = 270 - (cam.orientation.x + math.deg(math.atan2(input.z, input.x)))
	ts.angle.yaw = aeMath.rotationLerp(ts.angle.yaw, angle, 10 * dt)

	ts.pos.x = ts.pos.x - math.sin(math.rad(angle)) * 0.05
	ts.pos.z = ts.pos.z - math.cos(math.rad(angle)) * 0.05
	cam:follow(ts.pos, camHeight)
end

global function Draw()
	-- aeCamera.shaderUse("mesh")
	-- aeCamera.shaderVec3("camera", cam.direction)
	aeEntity.draw(ts)
	-- aeEntity.drawSkeleton(ts)
end

function getInput(): aeMath.Vec3
	local forward = 0 ; local right = 0 ; local up = 0
	if aeWindow.keyPressed("W") then forward = forward + 1 end
	if aeWindow.keyPressed("S") then forward = forward - 1 end
	if aeWindow.keyPressed("D") then right = right + 1 end
	if aeWindow.keyPressed("A") then right = right - 1 end
	if aeWindow.keyPressed("Space") then up = up + 1 end
	if aeWindow.keyPressed("LShift") then up = up - 1 end
	return aeMath.Vec3.new(right, up, forward)
end