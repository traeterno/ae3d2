local aeMath = require("aeMath")

--[[
	TODO

	0.
	rendering update:
	move all the drawing to new model

	1.
	add proper lighting system:
	in this script add function {Enable/Disable}Lighting(shaderName),
	which will register the shader for updating the uniforms
	when lights have been updated.

	2.
	custom shaders for heroes:
	when using textures, points with color (0xFFFFFF) will be filled with accent color
	(can be set up)
]]

global function Init()
	aeWindow.clearColor(aeMath.Vec3.new(0.1, 0.2, 0.3))
	aeWorld.spawn("sky", "sky")
	aeWorld.spawn("test", "testEntity")
	aeWorld.spawn("checkpoint", "checkpoint")
	aeWorld.spawn("sword", "sword")
	aeWorld.spawn("p1", "hero")

	aeCamera.shaderUse("mesh")
	aeCamera.shaderInt("lightsCount", 4)
	aeCamera.shaderFloat("texGrade", 0)

	aeCamera.shaderVec3("lightDir[0]", aeMath.Vec3.new(-1, -1, -1))
	aeCamera.shaderVec3("lightClr[0]", aeMath.Vec3.new(0.25, 0.25, 0.25))
	
	aeCamera.shaderVec3("lightDir[1]", aeMath.Vec3.new(1, -1, -1))
	aeCamera.shaderVec3("lightClr[1]", aeMath.Vec3.new(0.25, 0.25, 0.25))
	
	aeCamera.shaderVec3("lightDir[2]", aeMath.Vec3.new(1, -1, 1))
	aeCamera.shaderVec3("lightClr[2]", aeMath.Vec3.new(0.25, 0.25, 0.25))
	
	aeCamera.shaderVec3("lightDir[3]", aeMath.Vec3.new(-1, -1, 1))
	aeCamera.shaderVec3("lightClr[3]", aeMath.Vec3.new(0.25, 0.25, 0.25))
end

global function Update()
	if aeWindow.keyJustPressed("F1") then
		aeCamera.clearCache()
		aeWindow.loadUI("hud")
		aeWorld.load("testingGrounds")
	end
end