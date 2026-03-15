local aeMath = require("aeMath")

local ts: aeMath.Transform
local debugger: boolean

global function Init()
	ts = aeMath.Transform.new()

	-- aeCamera.loadGLTF("cheatCube")
	-- aeEntity.loadMesh("cheatCube", "CubeV2")
	-- aeEntity.setAnimation("dissolving")
	-- ts.scale = aeMath.Vec3.new(10, 10, 10)

	aeCamera.loadGLTF("heroV2")
	aeEntity.loadMesh("heroV2", "Body")
	aeEntity.setAnimation("rest")
	ts.scale = aeMath.Vec3.new(2, 2, 2)

	-- aeCamera.loadGLTF("test")
	-- aeEntity.loadMesh("test", "Cube")
	-- aeEntity.setAnimation("Action")
	debugger = true
end

global function Update()
	-- ts.angle.yaw = ts.angle.yaw + 45 * aeWindow.dt()
	if aeWindow.keyJustPressed("E") then
		aeCamera.shaderUse("mesh")
		debugger = not debugger
		aeCamera.shaderInt("debug", debugger and 0 or 1)
	end
end

global function Draw()
	aeEntity.draw(ts)
	-- aeEntity.drawSkeleton(ts)
end