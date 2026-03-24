local aeMath = require("aeMath")
local Transform = aeMath.Transform
local Vec2 = aeMath.Vec2
local Vec4 = aeMath.Vec4

local record Text
	id: integer
	len: integer
	str: string
	ts: Transform
	bounds: Vec2
	color: Vec4
end

function Text.new(): Text
	local self: Text = setmetatable({}, { __index = Text })
	self.id = aeCamera.createVBO()
	self.len = 0
	self.str = ""
	self.ts = Transform.new()
	self.bounds = Vec2.new(0, 0)
	self.color = Vec4.new(1, 1, 1, 1)
	return self
end

function Text:setString(s: string)
	self.str = s
	self.len, self.bounds = aeCamera.buildText(self.str, self.id)
end

function Text:getString(): string
	return self.str
end

function Text:draw()
	aeCamera.shaderUse("text")
	aeCamera.shaderVec4("clr", self.color)
	aeCamera.drawText(self.id, self.len, self.ts)
end

function Text:getGlobalBounds(): aeMath.Vec4
	local w = self.bounds.x ; local h = self.bounds.y
	local a = aeCamera.applyTransform(aeMath.Vec4.new(0, 0, 0, 1), self.ts)
	local b = aeCamera.applyTransform(aeMath.Vec4.new(w, 0, 0, 1), self.ts)
	local c = aeCamera.applyTransform(aeMath.Vec4.new(w, h, 0, 1), self.ts)
	local d = aeCamera.applyTransform(aeMath.Vec4.new(0, h, 0, 1), self.ts)
	local minX = math.min(a.x, b.x, c.x, d.x)
	local minY = math.min(a.y, b.y, c.y, d.y)
	local maxX = math.max(a.x, b.x, c.x, d.x)
	local maxY = math.max(a.y, b.y, c.y, d.y)
	return aeMath.Vec4.new(minX, minY, maxX - minX, maxY - minY)
end

return Text