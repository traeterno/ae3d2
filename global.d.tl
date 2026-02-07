global record vec2
	x: number
	y: number
end

global record vec3
	x: number
	y: number
	z: number
end

global record window
	close: function()
	clearColor: function(vec3)
	keyPressed: function(string): boolean
	keyJustPressed: function(string): boolean
	loadUI: function(string)
	size: function(): vec2
	uiSize: function(): vec2
end

global record aeMath
	vec2: function(x: number, y: number): vec2
	vec3: function(x: number, y: number, z: number): vec3
end