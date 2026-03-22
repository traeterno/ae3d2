local aeMath = require("aeMath")

global function Init()
	aeWindow.clearColor(aeMath.Vec3.new(0.1, 0.2, 0.3))
	aeWorld.spawn("test", "testEntity")
	aeWorld.spawn("checkpoint", "checkpoint")

	aeCamera.shaderUse("mesh")
	aeCamera.shaderInt("lightsCount", 4)

	aeCamera.shaderVec3("lightDir[0]", aeMath.Vec3.new(-1, -1, -1))
	aeCamera.shaderVec3("lightClr[0]", aeMath.Vec3.new(0.25, 0.25, 0.25))
	
	aeCamera.shaderVec3("lightDir[1]", aeMath.Vec3.new(1, -1, -1))
	aeCamera.shaderVec3("lightClr[1]", aeMath.Vec3.new(0.25, 0.25, 0.25))
	
	aeCamera.shaderVec3("lightDir[2]", aeMath.Vec3.new(1, -1, 1))
	aeCamera.shaderVec3("lightClr[2]", aeMath.Vec3.new(0.25, 0.25, 0.25))
	
	aeCamera.shaderVec3("lightDir[3]", aeMath.Vec3.new(-1, -1, 1))
	aeCamera.shaderVec3("lightClr[3]", aeMath.Vec3.new(0.25, 0.25, 0.25))

	aeNetwork.connect("127.0.0.1", 26225)
end

global function Update()
end