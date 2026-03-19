local aeMath = require("aeMath")

global function Init()
	aeWindow.clearColor(aeMath.Vec3.new(0.1, 0.2, 0.3))
	aeWorld.spawn("test", "testEntity")
	aeWorld.spawn("checkpoint", "checkpoint")

	aeCamera.shaderUse("mesh")
	aeCamera.shaderInt("lightsCount", 1)
	aeCamera.shaderVec3("lightDir[0]", aeMath.Vec3.new(0, -1, 0))
	aeCamera.shaderVec3("lightClr[0]", aeMath.Vec3.new(1, 1, 1))
end

global function Update()
end