local aeMath = require("aeMath")
local Camera = require("camera")

local cam: Camera

global function Init()
	-- aeWindow.clearColor(aeMath.Vec3.new(0.1, 0.2, 0.3))
	-- aeWorld.spawn("test", "testEntity")
	aeWindow.clearColor(aeMath.Vec3.new(0.1, 0.1, 0.1))
	aeWorld.spawn("cube", "cheatCube")

	cam = Camera.new()
	cam.speed = 0.05
	cam.position.z = -2.5
	-- cam.position.y = 5
	-- cam.orientation.y = -30

	cam:apply()
end

global function Update()
	cam:noclip()
	aeCamera.shaderUse("mesh")
	-- aeCamera.shaderVec3("camera", aeMath.Vec3.new(0, -1, 0))
	aeCamera.shaderVec3("camera", cam.direction)
end