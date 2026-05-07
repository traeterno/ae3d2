local aeMath = require("aeMath")
local aeMesh = require("aeMesh")

local render: aeMesh.RenderTask
local transform: aeMath.Transform
local state: string
local transition: number

local idle: thread ; local idleFN: function()
local conn: thread ; local connFN: function()
local sets: thread ; local setsFN: function()
local exit: thread ; local exitFN: function()

global function Init()
	local data = aeCamera.getGLTF("cheatCube", "CubeV2")
	render = aeMesh.RenderTask.default()
	render.vbo = aeCamera.createBuffer()
	render.ebo = aeCamera.createBuffer()
	render.attr = { 4, 3, 2, 0 }
	render.shader = "mesh2"
	render.useSkeleton = true
	-- render.textures = { aeCamera.getTexture("meshes/cheatCube") }
	render.textures = {
		{ aeCamera.getTexture("meshes/cheatCube"), 1 }
	}
	render.indices = #data.indices

	local vbo: {number} = {}
	local p = data.pos as {number} or {}
	local n = data.normals as {number} or {}
	local t = data.uv as {number} or {}
	local j = data.joints as {integer} or {}

	for i = 0, math.floor(#p / 3) do
		vbo[i*9+1] = p[i*3+1]
		vbo[i*9+2] = p[i*3+2]
		vbo[i*9+3] = p[i*3+3]
		vbo[i*9+4] = j[i+1]
		vbo[i*9+5] = n[i*3+1]
		vbo[i*9+6] = n[i*3+2]
		vbo[i*9+7] = n[i*3+3]
		vbo[i*9+8] = t[i*2+1]
		vbo[i*9+9] = t[i*2+2]
	end

	aeCamera.genVBO(render.vbo, vbo)
	aeCamera.genEBO(render.ebo, data.indices)

	aeEntity.loadSkeleton("cheatCube", data.skeleton)

	-- aeCamera.loadGLTF("cheatCube")
	-- aeEntity.loadMesh("cheatCube", "CubeV2")

	transform = aeMath.Transform.new()
	state = "Idle"
	transition = 0.0
	
	idle = coroutine.create(idleFN)
	conn = coroutine.create(connFN)
	sets = coroutine.create(setsFN)
	exit = coroutine.create(exitFN)
end

global function Update()
	local _: boolean
	if state == "Idle" then
		_, transform = coroutine.resume(idle) as (boolean, aeMath.Transform)
	elseif state == "Exit" then
		_, transform = coroutine.resume(exit) as (boolean, aeMath.Transform)
	elseif state == "Settings" then
		_, transform = coroutine.resume(sets) as (boolean, aeMath.Transform)
	elseif state == "Connect" then
		_, transform = coroutine.resume(conn) as (boolean, aeMath.Transform)
	end
end

global function Draw()
	aeCamera.render(render, transform)

	-- aeCamera.shaderUse("mesh")
	-- if state == "Exit" then aeCamera.textureUse(1, "meshes/cheatCube/exit")
	-- elseif state == "Settings" then aeCamera.textureUse(1, "meshes/cheatCube/settings")
	-- elseif state == "Connect" then aeCamera.textureUse(1, "meshes/cheatCube/connect") end
	-- aeCamera.shaderInt("tex2", 1)
	-- aeEntity.draw(transform)
end

global function SetState(data: table)
	local newState = data[1] as string
	if state == newState then return end
	state = newState
	transition = 0
end

function idleFN()
	local transitionTS: aeMath.Transform
	local targetAngle: aeMath.Quat
	local targetTS = aeMath.Transform.new()
	targetTS.pos.y = 0.1
	local timer = 0.0
	local amp = 15

	local pi = math.pi
	local sin = math.sin
	local cos = math.cos

	while true do
		timer = timer + aeWindow.dt()
		local angle = timer * pi * 0.25
		targetAngle = aeMath.Quat.new(
			amp * 2 * cos(angle * 0.5),
			amp * sin(angle),
			amp * sin(angle * 0.5)
		)

		if transition < 1 then
			if transition == 0 then transitionTS = transform end

			transition = aeMath.clamp(transition + aeWindow.dt(), 0, 1)
			local t = aeMath.easings.cubicInOut(transition)

			-- aeCamera.shaderUse("mesh")
			-- aeCamera.shaderFloat("texGrade", 1 - t)
			targetTS.angle.yaw = aeMath.lerp(transitionTS.angle.yaw, targetAngle.yaw, t)
			targetTS.angle.pitch = aeMath.lerp(transitionTS.angle.pitch, targetAngle.pitch, t)
			targetTS.angle.roll = aeMath.lerp(transitionTS.angle.roll, targetAngle.roll, t)
		else targetTS.angle = targetAngle end
		coroutine.yield(targetTS)
	end
end

function setsFN()
	local transitionTS: aeMath.Transform
	local targetAngle = aeMath.Quat.new(-45, 115, 0)
	local targetTS = aeMath.Transform.new()
	targetTS.pos.y = 0.1

	while true do
		if transition < 1 then
			if transition == 0 then transitionTS = transform end

			transition = aeMath.clamp(transition + aeWindow.dt(), 0, 1)
			local t = aeMath.easings.cubicInOut(transition)

			-- aeCamera.shaderUse("mesh")
			-- aeCamera.shaderFloat("texGrade", t)
			targetTS.angle.yaw = aeMath.lerp(transitionTS.angle.yaw, targetAngle.yaw, t)
			targetTS.angle.pitch = aeMath.lerp(transitionTS.angle.pitch, targetAngle.pitch, t)
			targetTS.angle.roll = aeMath.lerp(transitionTS.angle.roll, targetAngle.roll, t)
		end
		coroutine.yield(targetTS)
	end
end

function exitFN()
	local transitionTS: aeMath.Transform
	local targetAngle = aeMath.Quat.new(-135, 0, -25)
	local targetTS = aeMath.Transform.new()
	targetTS.pos.y = 0.1

	while true do
		if transition < 1 then
			if transition == 0 then transitionTS = transform end
			
			transition = aeMath.clamp(transition + aeWindow.dt(), 0, 1)
			local t = aeMath.easings.cubicInOut(transition)

			-- aeCamera.shaderUse("mesh")
			-- aeCamera.shaderFloat("texGrade", t)
			targetTS.angle.yaw = aeMath.lerp(transitionTS.angle.yaw, targetAngle.yaw, t)
			targetTS.angle.pitch = aeMath.lerp(transitionTS.angle.pitch, targetAngle.pitch, t)
			targetTS.angle.roll = aeMath.lerp(transitionTS.angle.roll, targetAngle.roll, t)
		end
		coroutine.yield(targetTS)
	end
end

function connFN()
	local transitionTS: aeMath.Transform
	local targetAngle = aeMath.Quat.new(135, -25, 0)
	local targetTS = aeMath.Transform.new()
	targetTS.pos.y = 0.1

	while true do
		if transition < 1 then
			if transition == 0 then transitionTS = transform end

			transition = aeMath.clamp(transition + aeWindow.dt(), 0, 1)
			local t = aeMath.easings.cubicInOut(transition)

			-- aeCamera.shaderUse("mesh")
			-- aeCamera.shaderFloat("texGrade", t)
			targetTS.angle.yaw = aeMath.lerp(transitionTS.angle.yaw, targetAngle.yaw, t)
			targetTS.angle.pitch = aeMath.lerp(transitionTS.angle.pitch, targetAngle.pitch, t)
			targetTS.angle.roll = aeMath.lerp(transitionTS.angle.roll, targetAngle.roll, t)
		end
		coroutine.yield(targetTS)
	end
end