local aeMath = require("aeMath")
local aeMesh = require("aeMesh")

local render: aeMesh.RenderTask
local transform: aeMath.Transform

local addPoint: function({number}, aeMath.Vec3, aeMath.Vec4)

global function Init()
	transform = aeMath.Transform.new()

	render = aeMesh.RenderTask.default()
	render.vbo = aeCamera.createBuffer()
	render.ebo = aeCamera.createBuffer()
	render.shapeMode = aeMesh.ShapeMode["Lines"]
	render.attr = { 3, 4, 0, 0 }
	render.shader = "shape"
	
	local data = {}
	local index = {}

	local length = 1
	local step = 0.1

	for i = 0, length, step do
		local x = i * 2 - 1
		addPoint(data, aeMath.Vec3.new(x, 0, -length), aeMath.Vec4.new(1,0,0,1))
		addPoint(data, aeMath.Vec3.new(x, 0, length), aeMath.Vec4.new(1,0,0,1))
		addPoint(data, aeMath.Vec3.new(-length, 0, x), aeMath.Vec4.new(0,0,1,1))
		addPoint(data, aeMath.Vec3.new(length, 0, x), aeMath.Vec4.new(0,0,1,1))
	end
	for i = 0, math.floor(#data / 7) - 1 do index[i + 1] = i end
	render.indices = #index

	aeCamera.genVBO(render.vbo, data)
	aeCamera.genEBO(render.ebo, index)
end

global function Update()
end

global function Draw()
	aeCamera.render(render, transform)
end

function addPoint(data: {number}, pos: aeMath.Vec3, clr: aeMath.Vec4)
	table.insert(data, pos.x)
	table.insert(data, pos.y)
	table.insert(data, pos.z)
	table.insert(data, clr.x)
	table.insert(data, clr.y)
	table.insert(data, clr.z)
	table.insert(data, clr.w)
end