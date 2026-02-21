local Sprite = require("sprite")
local aeMath = require("aeMath")
local Camera = require("camera")

global spr: Sprite
global cam: Camera

global function Init()
	spr = Sprite.new()
	spr:setTexture("global/icon")
	local size = spr:getTextureSize()
	spr:setOrigin(aeMath.Vec3.new(size.x * 0.5, size.y * 0.5, 0))
	spr:setPosition(aeMath.Vec3.new(0, -10, 0))
	spr:setRotation(aeMath.Quat.new(0, 90, 0, true))

	cam = Camera.new()
end

global function Update()
	cam:noclip()
	-- cam:apply()
end

global function Draw()
	spr:draw()
end