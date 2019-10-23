include("KeyConfig")
include("UiUtil")
include("math")
Main = {}



function tzw_on_game_ready()
	UiUtil.popText("基本版本 弹出框测试")
end

ImGuiCond_Always        = 1 << 0   -- Set the variable
ImGuiCond_Once          = 1 << 1   -- Set the variable once per runtime session (only the first call with succeed)
ImGuiCond_FirstUseEver  = 1 << 2   -- Set the variable if the object/window has no persistently saved data (no entry in .ini file)
ImGuiCond_Appearing     = 1 << 3    -- Set the variable if the object/window is appearing after being hidden/inactive (or the first time)



ImGuiWindowFlags_None                   = 0
ImGuiWindowFlags_NoTitleBar             = 1 << 0   -- Disable title-bar
ImGuiWindowFlags_NoResize               = 1 << 1   -- Disable user resizing with the lower-right grip
ImGuiWindowFlags_NoMove                 = 1 << 2   -- Disable user moving the window
ImGuiWindowFlags_NoScrollbar            = 1 << 3   -- Disable scrollbars (window can still scroll with mouse or programatically)
ImGuiWindowFlags_NoScrollWithMouse      = 1 << 4   -- Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
ImGuiWindowFlags_NoCollapse             = 1 << 5   -- Disable user collapsing window by double-clicking on it
ImGuiWindowFlags_AlwaysAutoResize       = 1 << 6   -- Resize every window to its content every frame
ImGuiWindowFlags_NoBackground           = 1 << 7  -- Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
ImGuiWindowFlags_NoSavedSettings        = 1 << 8   -- Never load/save settings in .ini file
ImGuiWindowFlags_NoMouseInputs          = 1 << 9   -- Disable catching mouse, hovering test with pass through.
ImGuiWindowFlags_MenuBar                = 1 << 10  -- Has a menu-bar
ImGuiWindowFlags_HorizontalScrollbar    = 1 << 11  -- Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
ImGuiWindowFlags_NoFocusOnAppearing     = 1 << 12  -- Disable taking focus when transitioning from hidden to visible state
ImGuiWindowFlags_NoBringToFrontOnFocus  = 1 << 13  -- Disable bringing window to front when taking focus (e.g. clicking on it or programatically giving it focus)
ImGuiWindowFlags_AlwaysVerticalScrollbar= 1 << 14  -- Always show vertical scrollbar (even if ContentSize.y < Size.y)
ImGuiWindowFlags_AlwaysHorizontalScrollbar=1<< 15  -- Always show horizontal scrollbar (even if ContentSize.x < Size.x)
ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 16  -- Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
ImGuiWindowFlags_NoNavInputs            = 1 << 18  -- No gamepad/keyboard navigation within the window
ImGuiWindowFlags_NoNavFocus             = 1 << 19  -- No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
ImGuiWindowFlags_NoNav                  = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus
ImGuiWindowFlags_NoDecoration           = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse
ImGuiWindowFlags_NoInputs               = ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus



EVENT_TYPE_K_RELEASE = 0
EVENT_TYPE_K_PRESS = 1
EVENT_TYPE_M_RELEASE = 2
EVENT_TYPE_M_PRESS = 3
EVENT_TYPE_M_MOVE = 4
EVENT_TYPE_K_CHAR_INPUT = 5

GAME_PART_BLOCK = 0
GAME_PART_CYLINDER = 1
GAME_PART_LIFT = 2
GAME_PART_CONTROL = 3
GAME_PART_THRUSTER = 4
GAME_PART_CANNON = 5
GAME_PART_BEARING = 6
GAME_PART_SPRING = 7
GAME_PART_NOT_VALID = 9999

local m_currIndex = 1
local lift_state = 0
local isOpenTestWindow = true
local testIcon = nil

local m_isDragingInventory = false
--init
function tzw_engine_init()
	print("lua init")
	testIcon = TextureMgr.shared():getByPathSimple("./Texture/NodeEditor/ic_restore_white_24dp.png")
end

--ui update
function tzw_engine_ui_update(dt)
	if isShowHelpPage then
		drawHelpPage()
	end
	drawHud()
	if MainMenu.shared():isVisible() then
		drawInventory()
	else
		if GameWorld.shared():getCurrentState() == CPP_GAME.GAME_STATE_RUNNING then
			updateLifting(dt)
		elseif GameWorld.shared():getCurrentState() == CPP_GAME.GAME_STATE_MAIN_MENU then
			drawEntryInterface()
		end
	end
	UiUtil.handlePopText()
end

local m_itemSlots = {}

for i = 1, 5 do
	table.insert(m_itemSlots, {target = nil})
end



-- inventory
local m_inventory = {}
table.insert(m_inventory, {name = "Lift", ItemClass = "Lift", ItemType = GAME_PART_LIFT, desc = "升降台"})
table.insert(m_inventory, {name = "Block", ItemClass = "PlaceableBlock", ItemType = GAME_PART_BLOCK, desc = "普通方块"})
table.insert(m_inventory, {name = "Cylinder", ItemClass = "PlaceableBlock", ItemType = GAME_PART_CYLINDER, desc = "轮子"})
table.insert(m_inventory, {name = "Cannon", ItemClass = "PlaceableBlock", ItemType = GAME_PART_CANNON, desc = "炮筒"})
table.insert(m_inventory, {name = "Spring", ItemClass = "PlaceableBlock", ItemType = GAME_PART_THRUSTER, desc = "喷射器"})
table.insert(m_inventory, {name = "Bearing", ItemClass = "PlaceableBlock", ItemType = GAME_PART_BEARING, desc = "轴承"})
table.insert(m_inventory, {name = "Spring", ItemClass = "PlaceableBlock", ItemType = GAME_PART_SPRING, desc = "弹簧"})
table.insert(m_inventory, {name = "ControlPart", ItemClass = "PlaceableBlock", ItemType = GAME_PART_CONTROL, desc = "控制方块"})
table.insert(m_inventory, {name = "TerrainTool", ItemClass = "TerrainTool", ItemType = 0, desc = "地形工具"})

function updateLifting(dt)
	if lift_state ~= 0 then
		local lift = BuildingSystem.shared():getLift()
		if lift ~= nil then
			lift:liftUp(lift_state * dt * 2.0);
		end
	end
end

function drawHelpPage()
	isShowHelpPage = ImGui.Begin("帮助页面", ImGuiWindowFlags_NoResize)

	if ImGui.CollapsingHeader("基础操作", 0) then
		ImGui.TextWrapped("WASD 来控制相机四方向移动，鼠标移动控制视线方向，空格键跳跃")
		ImGui.TextWrapped("'~'键切换显示菜单")
	end
	if ImGui.CollapsingHeader("升降工作台", 0) then
		ImGui.TextWrapped("玩家可以在地面上放置升降控制台，并可以将物块放在上面，在升降控制台上的物体不开启物理效果，")
		ImGui.TextWrapped("可以更方面的建造，同时小键盘的上下方向键可以竖直平移工作台")
		ImGui.TextWrapped("删除升降台后将开启载具的物理，相反的，建造好升降台后，准心对准活动的方块按J键可以将其放回工作台")
	end
	if ImGui.CollapsingHeader("约束", 0) then
		ImGui.TextWrapped("目前玩家可放置的有两种约束，弹簧和轴承，放置普通方块时，物体会自动连接成一个刚体，如果放置一个约束")
		ImGui.TextWrapped("则刚体之前可以活动，可以使用这个来建造小车")
	end
	if ImGui.CollapsingHeader("关于", 0) then
		ImGui.TextWrapped("Cube Engine By tzw")
		ImGui.TextWrapped("任何疑问皆可发送至 tzwtangziwen@163.com")
	end
	ImGui.End()
end

function showHelpPage()
	isShowHelpPage = true
end

function drawInventory()
	ImGui.Begin("资产浏览器", 0)
	local i = 0
	local itemSize = 80
	m_isDragingInventory = false
	for k, v in pairs(m_inventory) do
		local s = ImGui.GetStyle();
		local spaceX = s.ItemSpacing.x;
		local padding = s.FramePadding.x;
		
		local size = ImGui.GetWindowWidth() - s.IndentSpacing;
		local coloum = math.floor(size / (itemSize + spaceX * 2 + padding * 2)) + 1
		if ((i % coloum) ~= 0) then
			ImGui.SameLine(0, -1);
		end
		ImGui.PushID_str("inventory" .. i);
		ImGui.BeginGroup();
		ImGui.ImageButton(testIcon:handle(), ImGui.ImVec2(itemSize, itemSize));
		-- Our buttons are both drag sources and drag targets here!
		if (ImGui.BeginDragDropSource()) then
			ImGui.SetDragDropPayload("DND_DEMO_CELL", k);    -- Set payload to carry the index of our item (could be anything)
			ImGui.Text("拖拽" .. v.desc);   -- Display preview (could be anything, e.g. when dragging an image we could decide to display the filename and a small preview of the image, etc.)
			ImGui.EndDragDropSource();
			m_isDragingInventory = true
		end
		ImGui.Text(v.desc);
		ImGui.EndGroup();
		ImGui.PopID();
		i = i + 1
	end
	ImGui.End();
end

function drawHud()
	local screenSize = Engine:shared():winSize()
	local yOffset = 15.0;
	local window_pos = ImGui.ImVec2(screenSize.x / 2.0, screenSize.y - yOffset);
	local window_pos_pivot = ImGui.ImVec2(0.5, 1.0);
	ImGui.SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	local itemSize = 80
	ImGui.Begin("Profiler", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	for k, v in pairs(m_itemSlots) do
		-- ImGui.RadioButton(v.desc, m_currIndex == k)
		ImGui.BeginGroup();
		local needPop = false
		if v["target"] == nil then
			ImGui.ImageButton(testIcon:handle(), ImGui.ImVec2(itemSize, itemSize));
			if m_currIndex == k then
				ImGui.PushStyleColor(0, ImGui.ImVec4(1, 1, 0, 1));
				needPop = true
			end
			ImGui.Text("Empty");
		else
			ImGui.ImageButton(testIcon:handle(), ImGui.ImVec2(itemSize, itemSize));
			if m_currIndex == k then
				ImGui.PushStyleColor(0, ImGui.ImVec4(1, 1, 0, 1));
				needPop = true
			end
			ImGui.Text(m_inventory[v["target"]].desc);
		end
		if needPop then
			ImGui.PopStyleColor()
		end
		ImGui.EndGroup();
		if ImGui.BeginDragDropTargetAnyWindow() then
			local payLoad = ImGui.AcceptDragDropPayload("DND_DEMO_CELL", 0)
			-- print("payLoad", payLoad)
			if payLoad ~= nil then
				local payLoadIdx = ImGui.GetPayLoadData2Int(payLoad)
				m_itemSlots[k]["target"] = payLoadIdx
			end
			ImGui.EndDragDropTarget()
		end
		ImGui.SameLine(0, -1.0)
	end
	ImGui.End()
end

function drawEntryInterface()
	local screenSize = Engine:shared():winSize()
	ImGui.SetNextWindowPos(ImGui.ImVec2(screenSize.x / 2.0, screenSize.y / 2.0), ImGuiCond_Always, ImGui.ImVec2(0.5, 0.5));
	if ImGui.BeginNoClose("CubeEngine",ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse) then

		if(ImGui.Button("开始游戏", ImGui.ImVec2(160, 35))) then
			GameWorld.shared():startGame()
			Engine:shared():setUnlimitedCursor(true)
		end
		ImGui.Spacing()
		if(ImGui.Button("帮助", ImGui.ImVec2(160, 35))) then
			print "help btn clicked"
		end
		ImGui.Spacing()
		if(ImGui.Button("退出", ImGui.ImVec2(160, 35))) then
			print "on Exit"
		end
		ImGui.Spacing()
		ImGui.End()
	end
end

function onKeyPress(input_event)
	if input_event.keycode == TZW_KEY_UP then
		lift_state = 1
	elseif input_event.keycode == TZW_KEY_DOWN then
		lift_state = -1
	end
end

oldPlayerPos = nil
function onKeyRelease(input_event)
	local player = GameWorld.shared():getPlayer()
	if input_event.keycode == TZW_KEY_1 then
		m_currIndex = 1
	elseif input_event.keycode == TZW_KEY_2 then
		m_currIndex = 2
	elseif input_event.keycode == TZW_KEY_3 then
		m_currIndex = 3
	elseif input_event.keycode == TZW_KEY_4 then
		m_currIndex = 4
	elseif input_event.keycode == TZW_KEY_5 then
		m_currIndex = 5
	elseif input_event.keycode == TZW_KEY_6 then
		m_currIndex = 6
	elseif input_event.keycode == TZW_KEY_7 then
		m_currIndex = 7
	elseif input_event.keycode == TZW_KEY_UP then
		lift_state = lift_state - 1
	elseif input_event.keycode == TZW_KEY_DOWN then
		lift_state = lift_state + 1
	elseif input_event.keycode == TZW_KEY_E then
		if BuildingSystem.shared():getCurrentControlPart() == nil then
			local result = BuildingSystem.shared():rayTest(player:getPos(), player:getForward(), 10)
			if result and result.m_parent:getType() == 3 then
				oldPlayerPos = player:getPos()
				BuildingSystem.shared():setCurrentControlPart(result.m_parent)
			end
		else
			player:setPos(oldPlayerPos)
			BuildingSystem.shared():setCurrentControlPart(nil)
		end
	end
end

function placeItem(item)
	local player = GameWorld.shared():getPlayer()
	local result = BuildingSystem.shared():rayTest(player:getPos(), player:getForward(), 10)
	if item.ItemType >= 0 then
		local aBlock = BuildingSystem.shared():createPart(item.ItemType)
		if result == nil then
			BuildingSystem.shared():placeGamePart(aBlock, GameWorld.shared():getPlayer():getPos() + player:getForward():scale(10))
		else
			BuildingSystem.shared():attachGamePartNormal(aBlock, result)
		end
	else
		if item.ItemType == GAME_PART_BEARING then
			BuildingSystem.shared():placeBearingToAttach(result)
		elseif item.ItemType == GAME_PART_SPRING then
			BuildingSystem.shared():placeSpringToAttach(result)
		end
	end
end

function handleItemPrimaryUse(item)
	local player = GameWorld.shared():getPlayer()
	if (item.ItemClass == "PlaceableBlock") then
		placeItem(item)
	elseif (item.ItemClass == "TerrainTool") then --fill the terrain
		BuildingSystem.shared():terrainForm(player:getPos(), player:getForward(), 10, 0.3, 3.0)
	elseif (item.ItemClass == "Lift") then
		local resultPos = BuildingSystem.shared():hitTerrain(player:getPos(), player:getForward(), 10)
		BuildingSystem.shared():placeLiftPart(resultPos)
		print ("the Hit terrain Pos is", resultPos.x, resultPos.y, resultPos.z)
	end
end

function handleItemSecondaryUse(item)
	local player = GameWorld.shared():getPlayer()
	if (item.ItemClass == "PlaceableBlock" or item.ItemClass == "Lift") then
		local result = BuildingSystem.shared():rayTest(player:getPos(), player:getForward(), 10)
		if result then
			player:removePartByAttach(result)
		end
	elseif (item.ItemClass == "TerrainTool") then --dig the terrain
		BuildingSystem.shared():terrainForm(player:getPos(), player:getForward(), 10, -0.3, 3.0)
	end
end

function getItemFromSlotIndex()
	return m_inventory[m_itemSlots[m_currIndex]["target"]]
end

function onMouseRelease(input_event)
	if input_event.arg == 0 then --left mouse
		handleItemPrimaryUse(getItemFromSlotIndex())
	elseif input_event.arg == 1 then --right mouse
		handleItemSecondaryUse(getItemFromSlotIndex())
	end
end

--input event
function tzw_engine_input_event(input_event)
	if MainMenu.shared():isVisible() then
		return
	else
		if GameWorld.shared():getCurrentState() == CPP_GAME.GAME_STATE_RUNNING then
			if input_event.type == EVENT_TYPE_K_RELEASE then 
				onKeyRelease(input_event)
			elseif input_event.type == EVENT_TYPE_K_PRESS then 
				onKeyPress(input_event)
			elseif input_event.type == EVENT_TYPE_M_RELEASE then 
				onMouseRelease(input_event)
			end
		end
	end
end


return Main