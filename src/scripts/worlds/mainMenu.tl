local aeMath = require("aeMath")
local Camera = require("camera")

local cam: Camera

global function Init()
	aeWorld.spawn("cube", "cheatCube")

	aeCamera.shaderUse("mesh")
	aeCamera.shaderInt("lightsCount", 1)
	aeCamera.shaderVec3("lightDir[0]", aeMath.Vec3.new(-1, -1, 1):normalize())
	aeCamera.shaderVec3("lightClr[0]", aeMath.Vec3.new(1, 1, 1))

	cam = Camera.new()
	cam.orientation.x = 45
	cam.orientation.y = -30
	cam.distance = 0.5
	cam:apply()
end

global function Update()
	if aeWindow.keyJustPressed("F1") then
		aeCamera.clearCache()
		aeWindow.loadUI("mainMenu")
	end
end