global ts: Transform
global texRect: Vec4
global texSize: Vec2

global function Init()
	aeWindow.clearColor(aeMath.vec3(0.1, 0.2, 0.3))
	texSize = aeCamera.textureSize("global/icon")
	ts = {
		pos = aeMath.vec3(0, 0, 256),
		origin = aeMath.vec3(texSize.x / 2, texSize.y / 2, 0),
		angle = aeMath.quat(0, 0, 0, false)
	}
	texRect = aeMath.vec4(0, 0, texSize.x, texSize.y)
end

global function OnResized()
end

global function Update()
	if aeWindow.keyJustPressed("F1") then
		aeWindow.loadUI("mainMenu")
		aeCamera.clearCache()
	end
	ts.angle.yaw = ts.angle.yaw + 3
end

global function Draw()
	aeCamera.shaderUse("sprite")
	aeCamera.textureUse("global/icon")
	aeCamera.shaderInt("tex", 0)
	aeCamera.shaderVec4("rect", texRect)
	aeCamera.shaderVec2("texSize", texSize)
	aeCamera.setModelMatrix(ts)
	aeCamera.drawSprite()
end