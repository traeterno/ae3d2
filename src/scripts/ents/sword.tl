local aeMath = require("aeMath")

local bone: integer
local ts: aeMath.Transform
local bearerTS: aeMath.Transform

global function Init()
	aeEntity.loadMesh("sword", "Plane")
	aeCamera.unloadGLTF("sword")

	ts = aeMath.Transform.new()
	-- ts.pos.z = 0.1
	-- ts.pos.y = 2
	-- ts.angle.pitch = 190

	ts.angle.pitch = -90
	ts.angle.roll = -15
	ts.pos.z = 0.1

	bearerTS = aeMath.Transform.new()

	bone = 0
end

global function Update() end

global function Draw()
	aeEntity.useBoneTS(bone, bearerTS, ts)
	aeEntity.draw(nil)
end

global function SetBone(data: table)
	bone = data[1] as integer
end

global function SetBearer(data: table)
	bearerTS = data[1] as aeMath.Transform
end