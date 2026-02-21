local aeMath = require("aeMath")

global function Init()
	aeWindow.clearColor(aeMath.Vec3.new(0.1, 0.2, 0.3))
	aeWorld.spawn("test", "testEntity")
end

global function Update()
	-- 
end