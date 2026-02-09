global record Vec2
	x: number
	y: number
end

global record Vec3
	x: number
	y: number
	z: number
end

global record Vec4
	x: number
	y: number
	z: number
	w: number
end

global record Quaternion
	yaw: number
	pitch: number
	roll: number
	global: boolean
end

global record Transform
	pos: Vec3
	origin: Vec3
	angle: Quaternion
end

global record aeWindow
	close: function()
	clearColor: function(Vec3)
	keyPressed: function(string): boolean
	keyJustPressed: function(string): boolean
	loadUI: function(string)
	size: function(): Vec2
	uiSize: function(): Vec2
end

global record aeMath
	vec2: function(number, number): Vec2
	vec3: function(number, number, number): Vec3
	vec4: function(number, number, number, number): Vec4
	quat: function(number, number, number, boolean)
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
end