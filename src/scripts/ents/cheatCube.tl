local aeMath = require("aeMath")

local ts: aeMath.Transform

global function Init()
	ts = aeMath.Transform.new()

	aeCamera.loadGLTF("cheatCube")
	aeEntity.loadMesh("cheatCube", "CubeV2")
	-- aeEntity.setAnimation("dissolving")
end

global function Update()
end

global function Draw()
	aeEntity.draw(ts)
	-- aeEntity.drawSkeleton(ts)
end