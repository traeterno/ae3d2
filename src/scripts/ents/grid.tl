local aeMath = require("aeMath")

local vbo: integer
local ts: aeMath.Transform
local count: integer

local timer: number

local addPoint: function({ number }, aeMath.Vec3, aeMath.Vec4)
local animate: function()

global Update: function()

global function Init()
	ts = aeMath.Transform.new()
	vbo = aeCamera.createVBO()

	Update = animate
	timer = 0
end

global function Draw()
	aeCamera.drawShape(vbo, count, aeMath.shapes["Lines"], ts)
end

function addPoint(data: { number }, pos: aeMath.Vec3, clr: aeMath.Vec4)
	table.insert(data, pos.x)
	table.insert(data, pos.y)
	table.insert(data, pos.z)
	table.insert(data, clr.x)
	table.insert(data, clr.y)
	table.insert(data, clr.z)
	table.insert(data, clr.w)
end

local function buildGrid(clr: number)
	local data: { number } = {}
	
	local length = 1
	local step = 0.1

	local c = 2 * (clr < 0.5 and clr or (1 - clr))

	count = 0
	for x = 0, length, step do
		addPoint(data, aeMath.Vec3.new(x, 0, -length), aeMath.Vec4.new(clr * 2, c, c, 1))
		addPoint(data, aeMath.Vec3.new(x, 0, length), aeMath.Vec4.new(clr * 2, c, c, 1))
		count = count + 2
		if x ~= 0 then
			addPoint(data, aeMath.Vec3.new(-x, 0, -length), aeMath.Vec4.new(clr * 2, c, c, 1))
			addPoint(data, aeMath.Vec3.new(-x, 0, length), aeMath.Vec4.new(clr * 2, c, c, 1))
			count = count + 2
		end
	end
	for z = 0, length, step do
		addPoint(data, aeMath.Vec3.new(-length, 0, z), aeMath.Vec4.new(c, c, clr * 2, 1))
		addPoint(data, aeMath.Vec3.new(length, 0, z), aeMath.Vec4.new(c, c, clr * 2, 1))
		count = count + 2
		if z ~= 0 then
			addPoint(data, aeMath.Vec3.new(-length, 0, -z), aeMath.Vec4.new(c, c, clr * 2, 1))
			addPoint(data, aeMath.Vec3.new(length, 0, -z), aeMath.Vec4.new(c, c, clr * 2, 1))
			count = count + 2
		end
	end
	aeCamera.genVBO(vbo, data)
end

function animate()
	if timer < 4 then
		timer = timer + aeWindow.dt()
		buildGrid(aeMath.easings.sineInOut(aeMath.clamp(timer, 0, 4) / 4))
	end
end