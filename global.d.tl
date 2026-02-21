local aeMath = require("aeMath")
local Vec2 = aeMath.Vec2
local Vec3 = aeMath.Vec3
local Vec4 = aeMath.Vec4
local Transform = aeMath.Transform

global record aeWindow
	close: function()
	clearColor: function(Vec3)
	keyPressed: function(string): boolean
	keyJustPressed: function(string): boolean
	loadUI: function(string)
	size: function(): Vec2
	uiSize: function(): Vec2
	dt: function(): number
end

global record aeCamera
	textureUse: function(string)
	textureSize: function(string): Vec2
	shaderUse: function(string)
	shaderInt: function(string, integer)
	shaderVec2: function(string, Vec2)
	shaderVec4: function(string, Vec4)
	setModelMatrix: function(Transform)
	drawSprite: function()
	clearCache: function()
	createVBO: function(): integer
	removeVBO: function(integer)
	buildText: function(string, integer): integer, Vec2
	drawText: function(integer, integer)
end