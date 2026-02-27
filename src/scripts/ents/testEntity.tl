local Sprite = require("sprite")
local aeMath = require("aeMath")
local Camera = require("camera")

global spr: Sprite
global cam: Camera
global ts: aeMath.Transform

global function Init()
	spr = Sprite.new()
	spr:setTexture("global/icon")
	local size = spr:getTextureSize()
	spr:setOrigin(aeMath.Vec3.new(size.x * 0.5, size.y * 0.5, 0))
	spr:setPosition(aeMath.Vec3.new(0, -10, 0))
	spr:setRotation(aeMath.Quat.new(0, 90, 0, true))

	aeCamera.loadGLTF("cheatCube")
	aeEntity.loadMesh("cheatCube", "CubeV2")
	aeEntity.loadSkeleton("cheatCube", "Armature")
	aeCamera.unloadGLTF("cheatCube")

	cam = Camera.new()
	cam.distance = 50
	ts = aeMath.Transform.new()
	ts.scale = aeMath.Vec3.new(100, 100, 100)
end

global function Update()
	cam:noclip()
	ts.pos = cam.position
	ts.angle.yaw = -cam.orientation.x
	-- cam:apply()
end

global function Draw()
	aeCamera.shaderUse("mesh")
	aeCamera.setModelMatrix(ts)
	aeCamera.shaderVec3("camera", cam.direction)
	aeEntity.drawMesh()
	spr:draw()
end