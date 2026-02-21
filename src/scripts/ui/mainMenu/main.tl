local Sprite = require("ui/sprite")
local aeMath = require("aeMath")
local Text = require("ui/text")

global spr: Sprite
global txt: Text

global function Init()
	aeWindow.clearColor(aeMath.Vec3.new(0.1, 0.2, 0.3))
	spr = Sprite.new()
	spr:setTexture("global/icon")
	local size = spr:getTextureSize()
	spr:setOrigin(aeMath.Vec3.new(size.x * 0.5, size.y * 0.5, 0))

	txt = Text.new()
	txt:setString("Heroes of Envell")
end

global function OnResized()
	local winSize = aeWindow.size()
	spr:setPosition(aeMath.Vec3.new(winSize.x * 0.5, winSize.y * 0.5, 0))
end

global function Update()
	if aeWindow.keyJustPressed("F1") then
		aeWindow.loadUI("mainMenu")
		aeCamera.clearCache()
		print("===REQUESTED RELOAD===")
	end
	spr:rotate(aeMath.Vec3.new(0, 0, 90 * aeWindow.dt()))
end

global function Draw()
	spr:draw()
	txt:draw()
end