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
	aeCamera.shaderUse("mesh")
	aeCamera.shaderVec3("camera", aeMath.Vec3.new(0, 0, 0))
	aeEntity.draw(ts)
end