local aeMath = require("aeMath")
local Camera = require("camera")

local cam: Camera
local ts: aeMath.Transform
local distance: number
local camHeight: number
local active: boolean

local handBone: integer

local getInput: function(): aeMath.Vec3

global function Init()
	aeCamera.loadGLTF("heroV2")
	aeEntity.loadMesh("heroV2", "Body")
	aeCamera.unloadGLTF("heroV2")

	distance = 2.5
	camHeight = 1.25
	active = false

	aeEntity.startAnimation("IdleUpper")
	aeEntity.startAnimation("IdleLower")

	cam = Camera.new()
	cam.distance = 2.5
	cam.orientation.y = -30
	cam.orientation.x = 180
	cam:apply()

	ts = aeMath.Transform.new()
	ts.pos.y = 0.25
	ts.pos.z = 0.55
	cam:follow(ts.pos, camHeight)
	aeCamera.mouseDelta()

	handBone = 0
end

global function Update()
	if handBone == 0 then
		handBone = aeEntity.getBone(
			"Root/Handler/Torso/Chest/Shoulder.R/Arm.R/Forearm.R/Hand.R/Fingers.R"
		)
		aeWorld.entExecute("sword", "SetBone", {handBone})
	end

	if aeWindow.keyJustPressed("Tab") then
		active = not active
		if active then aeCamera.mouseDelta() end
	end
	if not active then
		aeWorld.entExecute("sword", "SetBearer", {ts})
		return
	end

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
	aeWorld.entExecute("sword", "SetBearer", {ts})
end

global function Draw()
	aeEntity.draw(ts)
	-- aeEntity.drawSkeleton(ts)
end

function getInput(): aeMath.Vec3
	local forward = 0 ; local right = 0 ; local up = 0
	if aeWindow.keyPressed("W") then forward = forward + 1 end
	if aeWindow.keyPressed("S") then forward = forward - 1 end
	if aeWindow.keyPressed("A") then right = right + 1 end
	if aeWindow.keyPressed("D") then right = right - 1 end
	if aeWindow.keyPressed("Space") then up = up + 1 end
	if aeWindow.keyPressed("LShift") then up = up - 1 end
	return aeMath.Vec3.new(right, up, forward)
end
