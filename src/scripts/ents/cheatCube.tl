local aeMath = require("aeMath")

local ts: aeMath.Transform

global function Init()
	-- aeCamera.loadGLTF("cheatCube")
	-- aeEntity.loadMesh("cheatCube", "CubeV2")
	-- aeCamera.unloadGLTF("cheatCube")

	-- aeEntity.setAnimation("dissolving")

	aeCamera.loadGLTF("test")
	aeEntity.loadMesh("test", "Cube")
	aeEntity.setAnimation("Action")

	ts = aeMath.Transform.new()
	ts.angle.relative = true
	-- ts.scale = aeMath.Vec3.new(10, 10, 10)
end

global function Update()
	-- ts.angle.pitch = ts.angle.pitch + 45 * aeWindow.dt()
end

global function Draw()
	aeEntity.draw(ts)
	aeEntity.drawSkeleton(ts)
end