local aeMath = require("aeMath")

local ts: aeMath.Transform

global function Init()
	aeCamera.loadGLTF("checkpointMount")
	aeEntity.loadMesh("checkpointMount", "Plane")
	aeCamera.unloadGLTF("checkpointMount")

	ts = aeMath.Transform.new()
	ts.pos.y = 0.25
end

global function Update()
end

global function Draw()
	aeEntity.draw(ts)
end