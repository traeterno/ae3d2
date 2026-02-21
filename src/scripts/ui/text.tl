local aeMath = require("aeMath")
local Transform = aeMath.Transform

local record Text
	id: integer
	len: integer
	str: string
	ts: Transform
end

function Text.new(): Text
	local self: Text = setmetatable({}, { __index = Text })
	self.id = aeCamera.createVBO()
	self.len = 0
	self.str = ""
	self.ts = Transform.new()
	return self
end

function Text:setString(s: string)
	self.str = s
	self.len = aeCamera.buildText(self.str, self.id)
end

function Text:getString(): string
	return self.str
end

function Text:draw()
	aeCamera.shaderUse("text")
	-- aeCamera.setModelMatrix(self.ts)
	aeCamera.drawText(self.id, self.len)
end

return Text