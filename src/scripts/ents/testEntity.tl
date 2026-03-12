local Sprite = require("sprite")
local aeMath = require("aeMath")
local Camera = require("camera")

global spr: Sprite
global cam: Camera
global ts: aeMath.Transform

local getInput: function(): aeMath.Vec3

global function Init()
	spr = Sprite.new()
	spr:setTexture("global/icon")
	local size = spr:getTextureSize()
	spr:setOrigin(aeMath.Vec3.new(size.x * 0.5, size.y * 0.5, 0))
	spr:setRotation(aeMath.Quat.new(0, 90, 0, true))
	spr:setScale(aeMath.Vec3.new(0.05, 0.05, 0.05))

	aeCamera.loadGLTF("heroV2")
	aeEntity.loadMesh("heroV2", "Body")
	aeCamera.unloadGLTF("heroV2")

	cam = Camera.new()
	cam.distance = 2.5
	cam.orientation.y = -45
	cam:apply()
	ts = aeMath.Transform.new()
	ts.angle.yaw = 180
	cam:follow(ts.pos, 1.5)

	aeEntity.setAnimation("anim")
end

global function Update()
	cam:rotate()

	local input = getInput()
	if input.x == 0 and input.z == 0 then return end
	local angle = 270 - (cam.orientation.x + math.deg(math.atan2(input.z, input.x)))
	ts.angle.yaw = aeMath.rotationLerp(ts.angle.yaw, angle, 10 * aeWindow.dt())

	ts.pos.x = ts.pos.x - math.sin(math.rad(angle)) * 0.05
	ts.pos.z = ts.pos.z - math.cos(math.rad(angle)) * 0.05
	cam:follow(ts.pos, 1.5)
end

global function Draw()
	aeCamera.shaderUse("mesh")
	aeCamera.shaderVec3("camera", cam.direction)
	aeEntity.draw(ts)
	aeEntity.drawSkeleton(ts)
	spr:draw()
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