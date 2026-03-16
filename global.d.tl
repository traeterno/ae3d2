local aeMath = require("aeMath")

global record aeWindow
	close: function()
	clearColor: function(aeMath.Vec3)
	keyPressed: function(string): boolean
	keyJustPressed: function(string): boolean
	loadUI: function(string)
	size: function(): aeMath.Vec2
	uiSize: function(): aeMath.Vec2
	dt: function(): number
	isFocused: function(): boolean
	scroll: function(): aeMath.Vec2
end

global record aeCamera
	textureUse: function(string)
	textureSize: function(string): aeMath.Vec2
	shaderUse: function(string)
	shaderInt: function(string, integer)
	shaderVec2: function(string, aeMath.Vec2)
	shaderVec3: function(string, aeMath.Vec3)
	shaderVec4: function(string, aeMath.Vec4)
	drawSprite: function()
	clearCache: function()
	createVBO: function(): integer
	removeVBO: function(integer)
	buildText: function(string, integer): integer, aeMath.Vec2
	drawText: function(integer, integer)
	mouseDelta: function(): aeMath.Vec2
	buildView: function(table)
	lookAt: function(table)
	loadGLTF: function(string)
	unloadGLTF: function(string)
end

-- TODO remove 'setModelMatrix'; pass transform as 'drawX' argument

global record aeWorld
	load: function(string)
	spawn: function(string, string)
end

global record aeEntity
	loadMesh: function(string, string)
	draw: function(aeMath.Transform)
	drawSkeleton: function(aeMath.Transform)
	setAnimation: function(string)
	stopAnimation: function(string)
end

global _winptr: integer
global _executor: string