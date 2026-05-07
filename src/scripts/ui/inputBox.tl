local Text = require("text")
local aeMath = require("aeMath")

local record InputBox
	str: {integer}
	txt: Text
	vbo: integer
	ts: aeMath.Transform
	cursor: integer
	active: boolean
	timer: number
	repeatTO: number
end

function InputBox.new(): InputBox
	local self: InputBox = setmetatable({}, { __index = InputBox })
	self.str = {}
	self.txt = Text.new()
	self.vbo = aeCamera.createBuffer()
	self.ts = aeMath.Transform.new()
	self.cursor = 1
	self.active = false
	self.timer = 0
	self.repeatTO = 0.125
	return self
end

function InputBox:reset()
	self.str = {}
	self.txt:setString("")
	self.cursor = 1
	self.timer = 0
	self.repeatTO = 0.125
end

function InputBox:getString(): string
	local out = ""
	for _, c in ipairs(self.str) do out = out..utf8.char(c) end
	return out
end

function InputBox:update(): boolean
	if not self.active then
		self.txt:setString(self:getString())
		return false
	end

	if aeWindow.keyJustPressed("Escape") then self.active = false end

	self.timer = (self.timer + aeWindow.dt()) % 1
	self.repeatTO = self.repeatTO + aeWindow.dt()

	if self.repeatTO >= 0.125 then
		if aeWindow.keyPressed("Left") then
			self.timer = 0
			self.repeatTO = 0
			self.cursor = math.floor(aeMath.clamp(self.cursor - 1, 1, #self.str + 1))
		end
		
		if aeWindow.keyPressed("Right") then
			self.timer = 0
			self.repeatTO = 0
			self.cursor = math.floor(aeMath.clamp(self.cursor + 1, 1, #self.str + 1))
		end
	
		if aeWindow.keyPressed("Backspace") and #self.str ~= 0 then
			self.timer = 0
			self.repeatTO = 0
			table.remove(self.str, self.cursor - 1)
			self.cursor = self.cursor - 1
		end
	end

	local inp = aeWindow.textInput()
	if inp ~= 0 then
		if self.cursor == #self.str then
			self.str[self.cursor] = inp
		else
			table.insert(self.str, self.cursor, inp)
		end
		self.cursor = self.cursor + 1
	end

	local s = ""
	if #self.str == 0 then s = (self.timer < 0.5 and "|" or "") end
	for i, c in ipairs(self.str) do
		s = s..utf8.char(c)
		if i + 1 == self.cursor then s = s..(self.timer < 0.5 and "|" or "") end
	end
	self.txt:setString(s)
	return aeWindow.keyJustPressed("Enter") and #self.str ~= 0
end

function InputBox:render()
	-- aeCamera.drawShape(self.vbo, 4, aeMath.shapes["TriangleStrip"], self.ts)
	self.txt:draw()
end

return InputBox