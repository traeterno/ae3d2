local aeMath = require("aeMath")
local aeMesh = require("aeMesh")

local transform: aeMath.Transform
local render: aeMesh.RenderTask

local activeInput: boolean

local getInput: function(): aeMath.Vec3

global function Init()
	transform = aeMath.Transform.new()

	render = {}
	render.vbo = aeCamera.createBuffer()
	render.ebo = aeCamera.createBuffer()
	render.shapeMode = aeMesh.ShapeMode["Triangles"]

	local data = aeCamera.getGLTF("heroV2", "Body")
end

global function Update()
end

global function Draw()
	aeCamera.render(render, transform)
end