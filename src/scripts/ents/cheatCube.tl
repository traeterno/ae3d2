local aeMath = require("aeMath")

local idleTS: aeMath.Transform
local ts: aeMath.Transform
local idleTimer: number
local timer: number

local exitTS: aeMath.Transform

local state: string

global function Init()
	idleTS = aeMath.Transform.new()
	idleTS.pos.y = 0.1

	ts = idleTS

	exitTS = aeMath.Transform.new()
	exitTS.pos.y = 0.1
	exitTS.angle.yaw = -135

	aeCamera.loadGLTF("cheatCube")
	aeEntity.loadMesh("cheatCube", "CubeV2")
	-- aeEntity.startAnimation("dissolving")

	idleTimer = 0
	timer = 0

	state = "Idle"
end

global function Update()
	idleTimer = idleTimer + aeWindow.dt()
	timer = timer + aeWindow.dt()

	local angle = idleTimer * math.pi * 0.25
	local amp = 15

	idleTS.angle = aeMath.Quat.new(
		amp * 2 * math.cos(angle * 0.5),
		amp * math.sin(angle),
		amp * math.sin(angle * 0.5),
		true
	)

	if state == "Idle" then
		local t = aeMath.easings.cubicInOut(aeMath.clamp(1 - timer, 0, 1))
		ts.angle.yaw = aeMath.lerp(idleTS.angle.yaw, exitTS.angle.yaw, t)
		ts.angle.pitch = aeMath.lerp(idleTS.angle.pitch, exitTS.angle.pitch, t)
		ts.angle.roll = aeMath.lerp(idleTS.angle.roll, exitTS.angle.roll, t)
		aeCamera.shaderUse("mesh")
		aeCamera.shaderFloat("texGrade", t)
	elseif state == "Exit" then
		local t = aeMath.easings.cubicInOut(aeMath.clamp(timer, 0, 1))
		ts.angle.yaw = aeMath.lerp(idleTS.angle.yaw, exitTS.angle.yaw, t)
		ts.angle.pitch = aeMath.lerp(idleTS.angle.pitch, exitTS.angle.pitch, t)
		ts.angle.roll = aeMath.lerp(idleTS.angle.roll, exitTS.angle.roll, t)
		aeCamera.shaderUse("mesh")
		aeCamera.shaderFloat("texGrade", t)
	end
end

global function Draw()
	aeEntity.draw(ts)
	-- aeEntity.drawSkeleton(ts)
end

global function SetState(data: table)
	if state == data[1] as string then return end
	state = data[1] as string
	timer = 0
	if state == "Idle" then
		aeCamera.shaderUse("mesh")
	elseif state == "Exit" then
		aeCamera.shaderUse("mesh")
		aeCamera.textureUse(1, "meshes/cheatCube_exit")
	end
end