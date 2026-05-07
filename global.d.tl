local aeMath = require("aeMath")
local aeMesh = require("aeMesh")

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
	mousePos: function(): aeMath.Vec2
	mousePressed: function(string): boolean
	mouseJustPressed: function(string): boolean
	textInput: function(): integer
end

global record aeCamera
	textureUse: function(integer, string)
	textureSize: function(string): aeMath.Vec2
	shaderUse: function(string)
	shaderInt: function(string, integer)
	shaderFloat: function(string, number)
	shaderVec2: function(string, aeMath.Vec2)
	shaderVec3: function(string, aeMath.Vec3)
	shaderVec4: function(string, aeMath.Vec4)
	drawSprite: function(aeMath.Transform)
	clearCache: function()
	createBuffer: function(): integer
	removeBuffer: function(integer)
	buildText: function(string, integer): integer, aeMath.Vec2
	drawText: function(integer, integer, aeMath.Transform)
	drawShape: function(integer, integer, integer, aeMath.Transform)
	mouseDelta: function(): aeMath.Vec2
	buildView: function(table)
	lookAt: function(table)
	loadGLTF: function(string)
	unloadGLTF: function(string)
	genVBO: function(integer, { number })
	genEBO: function(integer, { integer })
	applyTransform: function(aeMath.Vec4, aeMath.Transform): aeMath.Vec4

	render: function(aeMesh.RenderTask, aeMath.Transform)
	getGLTF: function(string, string): aeMesh.GLTF
	getTexture: function(string): integer
end


global record aeWorld
	load: function(string)
	spawn: function(string, string)
	execute: function(string, table)
	entExecute: function(string, string, table)
end

global record aeEntity
	loadMesh: function(string, string)
	draw: function(aeMath.Transform)
	drawSkeleton: function(aeMath.Transform)
	startAnimation: function(string)
	stopAnimation: function(string)
	getBone: function(string): integer
	useBoneTS: function(integer, aeMath.Transform, aeMath.Transform)

	loadSkeleton: function(string, integer)
	startAnim: function(string)
	stopAnim: function(string)
end

global record aeNetwork
	connect: function(string, integer)
	disconnect: function()
	reconnect: function()
	isReady: function(): boolean
	search: function()
	getServerIP: function(): string, integer
	stopSearch: function()
	status: function(): integer
end

global _winptr: integer
global _executor: string