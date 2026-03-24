local aeMath = require("aeMath")

local transform: aeMath.Transform
local state: string
local transition: number

local idle: thread ; local idleFN: function()
local conn: thread ; local connFN: function()
local sets: thread ; local setsFN: function()
local exit: thread ; local exitFN: function()

global function Init()
	aeCamera.loadGLTF("cheatCube")
	aeEntity.loadMesh("cheatCube", "CubeV2")

	transform = aeMath.Transform.new()
	state = "Idle"
	transition = 1.0
	
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
	aeCamera.shaderUse("mesh")
	if state == "Exit" then aeCamera.textureUse(1, "meshes/cheatCube/exit")
	elseif state == "Settings" then aeCamera.textureUse(1, "meshes/cheatCube/settings")
	elseif state == "Connect" then aeCamera.textureUse(1, "meshes/cheatCube/connect") end
	aeCamera.shaderInt("tex2", 1)
	aeEntity.draw(transform)
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

			aeCamera.shaderUse("mesh")
			aeCamera.shaderFloat("texGrade", 1 - t)
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

			aeCamera.shaderUse("mesh")
			aeCamera.shaderFloat("texGrade", t)
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

			aeCamera.shaderUse("mesh")
			aeCamera.shaderFloat("texGrade", t)
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

			aeCamera.shaderUse("mesh")
			aeCamera.shaderFloat("texGrade", t)
			targetTS.angle.yaw = aeMath.lerp(transitionTS.angle.yaw, targetAngle.yaw, t)
			targetTS.angle.pitch = aeMath.lerp(transitionTS.angle.pitch, targetAngle.pitch, t)
			targetTS.angle.roll = aeMath.lerp(transitionTS.angle.roll, targetAngle.roll, t)
		end
		coroutine.yield(targetTS)
	end
end