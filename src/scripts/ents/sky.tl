local aeMath = require("aeMath")
local aeMesh = require("aeMesh")

local ts: aeMath.Transform
local rt: aeMesh.RenderTask

-- TODO get back the actual sky

global function Init()
	ts = aeMath.Transform.new()

	rt = {}
	rt.vbo = aeCamera.createBuffer()
	rt.ebo = aeCamera.createBuffer()
	rt.shapeMode = aeMesh.ShapeMode["Triangles"]

	local data = aeCamera.getGLTF("test", "Cube")

	aeEntity.loadSkeleton("test", data.skeleton)
	aeEntity.startAnim("Action")

	-- rt.attr = { 3, 4, 2, 0 }
	rt.attr = { 3, 4, 0, 0 }
	rt.shader = "test"
	rt.useTS = true
	rt.useRotation = true
	rt.useSkeleton = true
	rt.textures = {0}

	local buf = {}
	local p = data.pos as {number}
	local n = data.normals as {number}
	-- local t = data.uv as {number}
	local w = data.joints as {integer}
	for i = 0, #p / 3 do
		local j = math.floor(i)
		-- local o = j * 9
		local o = j * 7
		buf[o+1] = p[j*3+1]
		buf[o+2] = p[j*3+2]
		buf[o+3] = p[j*3+3]
		buf[o+4] = n[j*3+1]
		buf[o+5] = n[j*3+2]
		buf[o+6] = n[j*3+3]
		buf[o+7] = w[j+1]
		-- buf[o+8] = t[j*2+1]
		-- buf[o+9] = t[j*2+2]
		-- buf[o+7] = 255
	end

	aeCamera.genVBO(rt.vbo, buf)
	aeCamera.genEBO(rt.ebo, data.indices)
	rt.indices = #data.indices

	-- rt.attr = { 3, 0, 0, 0 }
	-- rt.shader = "sky"
	-- rt.useRotation = false
	-- rt.useTS = true

	-- local s = 10
	-- aeCamera.genVBO(rt.vbo, {
	-- 	-s,-s,-s,
	-- 	 s,-s,-s,
	-- 	 s, s,-s,
	-- 	-s, s,-s
	-- })

	-- local indices = { 0, 1, 2, 2, 3, 0 }
	-- aeCamera.genEBO(rt.ebo, indices)
	-- rt.indices = #indices
end

global function Update()
end

global function Draw()
	aeCamera.render(rt, ts)
end