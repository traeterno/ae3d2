local aeMath = require("aeMath")
local Text = require("text")

global txt: Text

global function Init()
	txt = Text.new()
	txt:setString("Heroes of Envell")
	txt.ts.origin = aeMath.Vec3.new(txt.bounds.x * 0.5, 0, 0)

	aeWorld.load("testingGrounds")
end

global function OnResized()
	local winSize = aeWindow.size()
	txt.ts.pos = aeMath.Vec3.new(winSize.x * 0.5, 0, 0)
end

global function Update()
	if aeWindow.keyJustPressed("F1") then
		aeWindow.loadUI("mainMenu")
	end
	txt.ts.angle.roll = txt.ts.angle.roll + 90 * aeWindow.dt()
end

global function Draw()
	-- txt:draw()
end