local aeMath = require("aeMath")

local ts: aeMath.Transform

global function Init()
	aeCamera.loadGLTF("checkpoint")
	aeEntity.loadMesh("checkpoint", "Plane")
	aeCamera.unloadGLTF("checkpoint")

	ts = aeMath.Transform.new()

	aeWorld.spawn("checkpointMount", "checkpointMount")
end

global function Update()
end

global function Draw()
	aeEntity.draw(ts)
end