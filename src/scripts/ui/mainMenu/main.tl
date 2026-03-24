local aeMath = require("aeMath")
local Text = require("text")
local ButtonList = require("ui/btnList")

local target: aeMath.Vec2
local timer: number

local mainMenu: ButtonList
local exitMenu: ButtonList

global txt: Text

local execute: function(integer, string)

global function Init()
	aeWorld.load("mainMenu")

	txt = Text.new()
	txt:setString("Heroes of Envell")
	txt.ts.origin = aeMath.Vec3.new(txt.bounds.x * 0.5, txt.bounds.y * 0.5, 0)
	txt.ts.scale = aeMath.Vec3.new(2, 2, 0)

	target = aeMath.Vec2.new(0, 0)
	timer = -1

	mainMenu = ButtonList.new()
	mainMenu.anchor = aeMath.Vec2.new(0.5, 0.5)
	mainMenu:addButton("main_connect", "Подключиться", true)
	mainMenu:addButton("main_exit", "Выйти из игры", true)

	exitMenu = ButtonList.new()
	exitMenu.anchor = aeMath.Vec2.new(0.5, 0.5)
	exitMenu:addButton("exit_question", "Подтвердите выход", false)
	exitMenu:addButton("exit_discard", "Вернуться", true)
	exitMenu:addButton("exit_confirm", "Выйти", true)
end

global function OnResized()
	local winSize = aeWindow.size()
	local uiSize = aeWindow.uiSize()
	target.x = winSize.x * 0.5
	target.y = winSize.y * 0.1

	mainMenu.ts.pos.x = winSize.x * 0.2
	mainMenu.ts.pos.y = winSize.y * 0.5
	mainMenu:resized(aeMath.Vec2.new(winSize.x / uiSize.x, winSize.y / uiSize.y))
	exitMenu.ts.pos.x = winSize.x * 0.2
	exitMenu.ts.pos.y = winSize.y * 0.5
	exitMenu:resized(aeMath.Vec2.new(winSize.x / uiSize.x, winSize.y / uiSize.y))
end

global function Update()
	if aeWindow.keyJustPressed("F1") then
		print("===REQUESTED RELOAD===")
		aeCamera.clearCache()
		aeWindow.loadUI("mainMenu")
	end

	timer = timer + aeWindow.dt()
	if timer > 3 and timer < 4 then
		mainMenu:setActive(true)
	end
	txt.ts.pos = aeMath.Vec3.new(target.x,
		aeMath.lerp(
			-target.y, target.y, aeMath.easings.quadInOut(aeMath.clamp(timer, 0, 4) / 4)
		), 0
	)

	local b = mainMenu:update()
	if b ~= "" then
		local event = math.floor(tonumber(b:sub(1, 1))) ; local id = b:sub(2)
		execute(event, id)
	end
	b = exitMenu:update()
	if b ~= "" then
		local event = math.floor(tonumber(b:sub(1, 1))) ; local id = b:sub(2)
		execute(event, id)
	end
end

global function Draw()
	txt:draw()
	mainMenu:render()
	exitMenu:render()
end

function execute(event: integer, id: string)
	if id == "main_exit" and event == 2 then
		mainMenu:setActive(false)
		exitMenu:setActive(true)
		aeCamera.shaderUse("mesh")
		aeCamera.textureUse(1, "meshes/cheatCube_exit")
		aeCamera.shaderInt("tex2", 1)
		aeWorld.entExecute("cube", "SetState", { "Exit" })
		return
	end
	if id == "exit_discard" and event == 2 then
		exitMenu:setActive(false)
		mainMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Idle" })
		return
	end
	if id == "exit_confirm" and event == 2 then
		-- TODO animate
		aeWindow.close()
	end
end