local Text = require("text")
local aeMath = require("aeMath")

local record Button
	id: string
	txt: Text
	clickable: boolean
end

local record ButtonList
	type Button = Button
	btns: { Button }
	ts: aeMath.Transform
	bounds: aeMath.Vec2
	anchor: aeMath.Vec2
	lineHeight: number
	targetScale: number
	active: boolean
	timer: number
end

function ButtonList.new(): ButtonList
	local self: ButtonList = setmetatable({}, { __index = ButtonList })
	self.btns = {}
	self.ts = aeMath.Transform.new()
	self.bounds = aeMath.Vec2.new(0, 0)
	self.anchor = aeMath.Vec2.new(0, 0)
	self.lineHeight = 32 * 1.5
	self.targetScale = 1
	self.active = false
	self.timer = 1
	return self
end

function ButtonList:setButton(index: integer, id: string, txt: string, clickable: boolean)
	if id == "" then table.remove(self.btns, index) end
	local btn: Button = {}
	btn.id = id
	btn.txt = Text.new()
	btn.txt:setString(txt)
	btn.txt.ts.origin = aeMath.Vec3.new(
		btn.txt.bounds.x * self.anchor.x,
		btn.txt.bounds.y * self.anchor.y,
		0
	)
	btn.clickable = clickable
	self.btns[index] = btn
	self.bounds = aeMath.Vec2.new(0, #self.btns * self.lineHeight)
	for _, b in ipairs(self.btns) do
		self.bounds.x = math.max(self.bounds.x, b.txt.bounds.x)
	end
end

function ButtonList:addButton(id: string, txt: string, clickable: boolean)
	self:setButton(#self.btns + 1, id, txt, clickable)
end

function ButtonList:update(): string
	self.timer = self.timer + aeWindow.dt()
	if not aeWindow.isFocused() or not self.active then return "" end

	local out = ""

	local mpos = aeWindow.mousePos()
	local offset = (self.lineHeight - self.bounds.y) * self.anchor.y
	for i, btn in ipairs(self.btns) do
		btn.txt.ts.pos = aeMath.Vec3.new(
			self.ts.pos.x,
			self.ts.pos.y + (i - 1) * self.lineHeight + offset,
			0
		)
		if btn.clickable then
			local bounds = btn.txt:getGlobalBounds()
			local scale = btn.txt.ts.scale.x / self.targetScale
			local status = 0
			if aeMath.contains(bounds, mpos) then
				scale = aeMath.lerp(scale, 1, aeWindow.dt() * 10)
				status = 1
				if aeWindow.mouseJustPressed("Left") then
					status = 2
				end
			else
				scale = aeMath.lerp(scale, 0.8, aeWindow.dt() * 10)
			end
			btn.txt.ts.scale = aeMath.Vec3.new(scale * self.targetScale, scale * self.targetScale, 0)
			if status ~= 0 and self.timer > 1 then
				out = tostring(status)..btn.id
			end
		else
			btn.txt.ts.scale = aeMath.Vec3.new(0.8 * self.targetScale, 0.8 * self.targetScale, 0)
		end
	end
	return out
end

function ButtonList:render()
	if not self.active and self.timer > 1 then return end
	for _, btn in ipairs(self.btns) do
		btn.txt.color.w = aeMath.easings.cubicInOut(aeMath.clamp(
			self.active and self.timer or 1 - self.timer,
			0, 1
		))
		btn.txt:draw()
	end
end

function ButtonList:resized(scale: aeMath.Vec2)
	self.targetScale = (scale.x + scale.y) * 0.5
end

function ButtonList:setActive(active: boolean)
	if self.active == active then return end
	self.active = active
	self.timer = 0
end

return ButtonList