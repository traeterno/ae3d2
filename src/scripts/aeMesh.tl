local record GLTF
	pos: {number} | nil
	normals: {number} | nil
	uv: {number} | nil
	joints: {integer} | nil
	indices: {integer} | nil
	skeleton: integer
end

local ShapeMode: {string:integer} = {
	Points = 0,
	Lines = 1,
	LineLoop = 2,
	LineStrip = 3,
	Triangles = 4,
	TriangleStrip = 5,
	TriangleFan = 6
}

local record RenderTask
	vbo: integer
	ebo: integer
	shapeMode: integer
	attr: {integer}
	shader: string
	useRotation: boolean
	useTS: boolean
	indices: integer
	useSkeleton: boolean
	textures: { {integer, number} }
end

function RenderTask.default(): RenderTask
	local self: RenderTask = setmetatable({}, { __index = RenderTask })
	self.vbo = 0
	self.ebo = 0
	self.shapeMode = ShapeMode["Triangles"]
	self.attr = { 0, 0, 0, 0 }
	self.shader = ""
	self.useRotation = false
	self.useTS = true
	self.indices = 0
	self.useSkeleton = false
	self.textures = {}
	return self
end

local VertexType: {string:integer} = {
	Pos = 0,
	PosNormal = 1,
	PosUV = 2,
	PosJoint = 3,
	PosNormalUV = 4,
	PosNormalJoint = 5,
	PosUVJoint = 6,
	PosNormalUVJoint = 7
}

return {
	GLTF = GLTF,
	RenderTask = RenderTask,
	ShapeMode = ShapeMode,
	VertexType = VertexType,
}