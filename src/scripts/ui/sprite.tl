local aeMath = require("aeMath")
local Vec2 = aeMath.Vec2
local Vec3 = aeMath.Vec3
local Vec4 = aeMath.Vec4
local Quat = aeMath.Quat
local Transform = aeMath.Transform

local record Sprite
	ts: Transform
	texture: string
	texRect: Vec4
	texSize: Vec2
end

function Sprite.new(): Sprite
	local self: Sprite = setmetatable({}, { __index = Sprite })
	self.ts = {
		pos = Vec3.new(0, 0, 0),
		angle = Quat.new(0, 0, 0, true),
		origin = Vec3.new(0, 0, 0),
		scale = Vec3.new(1, 1, 1)
	}
	self.texture = ""
	self.texRect = Vec4.new(0, 0, 0, 0)
	self.texSize = Vec2.new(0, 0)
	return self
end

function Sprite:setTexture(id: string)
	self.texture = id
	self.texSize = aeCamera.textureSize(self.texture)
	self.texRect = Vec4.new(0, 0, self.texSize.x, self.texSize.y)
end

function Sprite:getTextureSize(): Vec2
	return self.texSize
end

function Sprite:setTextureRect(rect: Vec4)
	self.texRect = rect
end

function Sprite:setPosition(pos: Vec3)
	self.ts.pos = pos
end

function Sprite:translate(pos: Vec3)
	self.ts.pos.x = self.ts.pos.x + pos.x
	self.ts.pos.y = self.ts.pos.y + pos.y
	self.ts.pos.z = self.ts.pos.z + pos.z
end

function Sprite:setScale(scale: Vec3)
	self.ts.scale = scale
end

function Sprite:scale(scale: Vec3)
	self.ts.scale.x = self.ts.scale.x * scale.x
	self.ts.scale.y = self.ts.scale.y * scale.y
	self.ts.scale.z = self.ts.scale.z * scale.z
end

function Sprite:setOrigin(origin: Vec3)
	self.ts.origin = origin
end

function Sprite:setRotation(angle: Quat)
	self.ts.angle = angle
end

function Sprite:rotate(angle: Vec3)
	self.ts.angle.yaw = self.ts.angle.yaw + angle.x
	self.ts.angle.pitch = self.ts.angle.pitch + angle.y
	self.ts.angle.roll = self.ts.angle.roll + angle.z
end

function Sprite:draw()
	aeCamera.shaderUse("sprite")
	aeCamera.textureUse(self.texture)
	aeCamera.shaderInt("tex", 0)
	aeCamera.shaderVec4("rect", self.texRect)
	aeCamera.shaderVec2("texSize", self.texSize)
	aeCamera.setModelMatrix(self.ts)
	aeCamera.drawSprite()
end

return Sprite