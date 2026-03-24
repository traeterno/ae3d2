local aeMath = require("aeMath")
local Text = require("text")
local ButtonList = require("ui/btnList")

local target: aeMath.Vec2
local timer: number

local mainMenu: ButtonList
local connMenu: ButtonList
local settingsMenu: ButtonList
local exitMenu: ButtonList

global txt: Text

local execute: function(integer, string)

global function Init()
	aeWorld.load("mainMenu")

	txt = Text.new()
	txt:setString("Герои Энвелла")
	txt.ts.origin = aeMath.Vec3.new(txt.bounds.x * 0.5, txt.bounds.y * 0.5, 0)
	txt.ts.scale = aeMath.Vec3.new(2, 2, 0)

	target = aeMath.Vec2.new(0, 0)
	timer = -1

	mainMenu = ButtonList.new()
	mainMenu.anchor = aeMath.Vec2.new(0.5, 0.5)
	mainMenu:addButton("main_connect", "Подключиться", true)
	mainMenu:addButton("main_settings", "Настройки", true)
	mainMenu:addButton("main_exit", "Выйти из игры", true)

	connMenu = ButtonList.new()
	connMenu.anchor = aeMath.Vec2.new(0.5, 0.5)
	connMenu:addButton("conn_title", "Подключение к серверу", false)
	connMenu:addButton("conn_todo", "[В разработке]", false)
	connMenu:addButton("conn_exit", "В главное меню", true)

	settingsMenu = ButtonList.new()
	settingsMenu.anchor = aeMath.Vec2.new(0.5, 0.5)
	settingsMenu:addButton("settings_title", "Настройки", false)
	settingsMenu:addButton("settings_todo", "[В разработке]", false)
	settingsMenu:addButton("settings_exit", "В главное меню", true)

	exitMenu = ButtonList.new()
	exitMenu.anchor = aeMath.Vec2.new(0.5, 0.5)
	exitMenu:addButton("exit_title", "Подтвердите выход:", false)
	exitMenu:addButton("exit_discard", "Вернуться", true)
	exitMenu:addButton("exit_confirm", "Выйти", true)
end

global function OnResized()
	local winSize = aeWindow.size()
	target.x = winSize.x * 0.5
	target.y = winSize.y * 0.1

	mainMenu:resized(aeMath.Vec2.new(0.2, 0.5))
	connMenu:resized(aeMath.Vec2.new(0.2, 0.5))
	settingsMenu:resized(aeMath.Vec2.new(0.2, 0.5))
	exitMenu:resized(aeMath.Vec2.new(0.2, 0.5))
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
	b = connMenu:update()
	if b ~= "" then
		local event = math.floor(tonumber(b:sub(1, 1))) ; local id = b:sub(2)
		execute(event, id)
	end
	b = settingsMenu:update()
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
	connMenu:render()
	settingsMenu:render()
	exitMenu:render()
end

function execute(event: integer, id: string)
	if id == "main_connect" and event == 2 then
		mainMenu:setActive(false)
		connMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Connect" })
	elseif id == "main_exit" and event == 2 then
		mainMenu:setActive(false)
		exitMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Exit" })
	elseif id == "main_settings" and event == 2 then
		mainMenu:setActive(false)
		settingsMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Settings" })
	elseif id == "conn_exit" and event == 2 then
		connMenu:setActive(false)
		mainMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Idle" })
	elseif id == "settings_exit" and event == 2 then
		settingsMenu:setActive(false)
		mainMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Idle" })
	elseif id == "exit_discard" and event == 2 then
		exitMenu:setActive(false)
		mainMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Idle" })
	elseif id == "exit_confirm" and event == 2 then
		-- TODO animate
		aeWindow.close()
	end
end