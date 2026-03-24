local aeMath = require("aeMath")
local Camera = require("camera")

local cam: Camera
local timer: number

global function Init()
	aeWorld.spawn("cube", "cheatCube")
	aeWorld.spawn("coordinates", "grid")

	aeCamera.shaderUse("mesh")
	aeCamera.shaderInt("lightsCount", 1)
	aeCamera.shaderVec3("lightDir[0]", aeMath.Vec3.new(1, -1, 1):normalize())
	aeCamera.shaderVec3("lightClr[0]", aeMath.Vec3.new(1, 1, 1))

	cam = Camera.new()
	cam.speed = 0.01
	cam.orientation.x = 45
	cam.orientation.y = -25
	cam.distance = 0.4
	cam:apply()

	timer = -0.5
end

global function Update()
	timer = timer + aeWindow.dt()
	cam.position.y = aeMath.lerp(
		1, 0.1,
		aeMath.easings.quadInOut(aeMath.clamp(timer, 0, 4) / 4)
	)
	cam:apply()
end