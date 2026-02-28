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
	isFocused: function(): boolean
end

global record aeCamera
	textureUse: function(string)
	textureSize: function(string): Vec2
	shaderUse: function(string)
	shaderInt: function(string, integer)
	shaderVec2: function(string, Vec2)
	shaderVec3: function(string, Vec3)
	shaderVec4: function(string, Vec4)
	setModelMatrix: function(Transform)
	drawSprite: function()
	clearCache: function()
	createVBO: function(): integer
	removeVBO: function(integer)
	buildText: function(string, integer): integer, Vec2
	drawText: function(integer, integer)
	mouseDelta: function(): Vec2
	buildView: function(table)
	lookAt: function(table)
	loadGLTF: function(string)
	unloadGLTF: function(string)
end

global record aeWorld
	load: function(string)
	spawn: function(string, string)
end

global record aeEntity
	loadMesh: function(string, string)
	loadSkeleton: function(string, integer)
	drawMesh: function()
end

global _winptr: integer
global _executor: string