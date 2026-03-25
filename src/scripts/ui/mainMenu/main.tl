local aeMath = require("aeMath")
local Text = require("text")
local ButtonList = require("ui/btnList")
local InputBox = require("ui/inputBox")

local target: aeMath.Vec2
local server: { string, integer }

local mainMenu: ButtonList
local connMenu: ButtonList
local settingsMenu: ButtonList
local exitMenu: ButtonList

local input: { InputBox, number }

global txt: Text

local execute: function(string)

local init: thread ; local initFN: function()

global function Init()
	aeNetwork.disconnect()
	aeNetwork.stopSearch()

	aeWorld.load("mainMenu")

	txt = Text.new()
	txt:setString("Герои Энвелла")
	txt.ts.origin = aeMath.Vec3.new(txt.bounds.x * 0.5, txt.bounds.y * 0.5, 0)
	txt.ts.scale = aeMath.Vec3.new(2, 2, 0)

	target = aeMath.Vec2.new(0, 0)
	server = { "", 0 }

	input = { InputBox.new(), 0 }

	mainMenu = ButtonList.new()
	mainMenu.anchor = aeMath.Vec2.new(0.5, 0.5)
	mainMenu:addButton("main_connect", "Подключиться", true)
	mainMenu:addButton("main_settings", "Настройки", true)
	mainMenu:addButton("main_exit", "Выйти из игры", true)

	connMenu = ButtonList.new()
	connMenu.anchor = aeMath.Vec2.new(0.5, 0.5)
	connMenu:addButton("conn_title", "Подключение к серверу", false)
	connMenu:addButton("conn_enter", "[Нет адреса]", false)
	connMenu:addButton("conn_custom", "Ввести адрес", true)
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

	init = coroutine.create(initFN)
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

	if coroutine.status(init) ~= "dead" then coroutine.resume(init) end

	local status = aeNetwork.status()
	if status == aeMath.networkStatus["Idle"] then
	elseif status == aeMath.networkStatus["Searching"] then
		local _, port = aeNetwork.getServerIP()
		connMenu:setButton(1, "conn_title", "Поиск сервера"..string.rep(".", port), false)
		connMenu:setButton(2, "conn_enter", "[Нет адреса]", false)
		connMenu:setButton(3, "conn_custom", "Ввести адрес", true)
	elseif status == aeMath.networkStatus["ServerNotFound"] then
		connMenu:setButton(1, "conn_title", "Сервер не найден.", false)
	elseif status == aeMath.networkStatus["ServerFound"] then
		local ip, port = aeNetwork.getServerIP()
		if server[2] ~= port then
			server = { ip, port }
			connMenu:setButton(1, "conn_title",
				string.format("Найден сервер: %s:%i", ip, port), false
			)
			connMenu:setButton(2, "conn_enter", "Подключиться", true)
			connMenu:setButton(3, "conn_custom", "Ввести адрес", true)
		end
	elseif status == aeMath.networkStatus["Connecting"] then
		connMenu:setButton(1, "conn_title", "Подключение...", false)
		connMenu:setButton(2, "conn_enter",
			string.format("%s:%i", server[1], server[2]), false
		)
	elseif status == aeMath.networkStatus["ConnectionFailed"] then
		connMenu:setButton(1, "conn_title", "Не удалось подключиться", false)
		connMenu:setButton(2, "conn_enter", "[Нет адреса]", false)
		connMenu:setButton(3, "conn_custom", "Ввести адрес", true)
		server = { "", 0 }
		aeNetwork.disconnect()
	elseif status == aeMath.networkStatus["Connected"] then
		if server[2] ~= 0 then
			server = { "", 0 }
			local ip, port = aeNetwork.getServerIP()
			connMenu:setButton(1, "conn_title", "Подключено.", false)
			connMenu:setButton(2, "conn_enter",
				string.format("%s:%i", ip, port), false
			)
			connMenu:setButton(3, "conn_custom", "Войти в мир", true)
		end
	else print("Unknown network status", status)
	end

	local i = input[1]

	if i:update() then
		local addr = i:getString()
		local pos, _ = string.find(addr, ":", 1, true)
		if pos ~= nil then
			local ip = string.sub(addr, 1, pos - 1)
			local port = tonumber(string.sub(addr, pos + 1))
			if port ~= nil then
				server = { ip, math.floor(port) }
				aeNetwork.connect(ip, math.floor(port))
				i:reset()
				i.active = false
			end
		end
	end

	execute(mainMenu:update())
	execute(connMenu:update())
	execute(settingsMenu:update())
	execute(exitMenu:update())
end

global function Draw()
	txt:draw()
	mainMenu:render()
	connMenu:render()
	settingsMenu:render()
	exitMenu:render()
	input[1]:render()
end

function execute(b: string)
	if b == "" then return end
	local event = math.floor(tonumber(b:sub(1, 1))) ; local id = b:sub(2)
	if id == "main_connect" and event == 2 then
		mainMenu:setActive(false)
		connMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Connect" })
		aeNetwork.search()
	elseif id == "main_exit" and event == 2 then
		mainMenu:setActive(false)
		exitMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Exit" })
	elseif id == "main_settings" and event == 2 then
		mainMenu:setActive(false)
		settingsMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Settings" })
	end

	if id == "conn_exit" and event == 2 then
		connMenu:setActive(false)
		mainMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Idle" })
		aeNetwork.stopSearch()
		aeNetwork.disconnect()
		server = { "", 0 }
		input[1].active = false
		input[2] = 0
	elseif id == "conn_enter" and event == 2 then
		if server[2] ~= 0 then
			aeNetwork.connect(server[1], server[2])
		end
	elseif id == "conn_custom" and event == 2 then
		local status = aeNetwork.status()
		if status == aeMath.networkStatus["Connected"] then
			-- TODO
		else
			input[1].active = true
			input[1]:reset()
			input[2] = 0
			aeNetwork.stopSearch()
		end
	end
	
	if id == "settings_exit" and event == 2 then
		settingsMenu:setActive(false)
		mainMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Idle" })
	end
	
	if id == "exit_discard" and event == 2 then
		exitMenu:setActive(false)
		mainMenu:setActive(true)
		aeWorld.entExecute("cube", "SetState", { "Idle" })
	elseif id == "exit_confirm" and event == 2 then
		-- TODO animate
		aeWindow.close()
	end
end

function initFN()
	local timer = -1.0
	local ready = false

	while true do
		timer = aeMath.clamp(timer + aeWindow.dt(), -1, 4)
		
		if not ready and timer > 3 then
			mainMenu:setActive(true)
			ready = true
		end

		txt.ts.pos = aeMath.Vec3.new(
			target.x,
			aeMath.lerp(-target.y, target.y, aeMath.easings.quadInOut(timer / 4)),
			0
		)

		if timer == 4 then break end
		coroutine.yield()
	end
end