import CubeEngine
import Game
import KeyConfig
import ImGui
import math
from enum import Enum
ImGuiCond_Always        = 1 << 0   # Set the variable
ImGuiCond_Once          = 1 << 1   # Set the variable once per runtime session (only the first call with succeed)
ImGuiCond_FirstUseEver  = 1 << 2   # Set the variable if the object/window has no persistently saved data (no entry in .ini file)
ImGuiCond_Appearing     = 1 << 3    # Set the variable if the object/window is appearing after being hidden/inactive (or the first time)



ImGuiWindowFlags_None                   = 0
ImGuiWindowFlags_NoTitleBar             = 1 << 0   # Disable title-bar
ImGuiWindowFlags_NoResize               = 1 << 1   # Disable user resizing with the lower-right grip
ImGuiWindowFlags_NoMove                 = 1 << 2   # Disable user moving the window
ImGuiWindowFlags_NoScrollbar            = 1 << 3   # Disable scrollbars (window can still scroll with mouse or programatically)
ImGuiWindowFlags_NoScrollWithMouse      = 1 << 4   # Disable user vertically scrolling with mouse wheel. On child window, mouse wheel will be forwarded to the parent unless NoScrollbar is also set.
ImGuiWindowFlags_NoCollapse             = 1 << 5   # Disable user collapsing window by double-clicking on it
ImGuiWindowFlags_AlwaysAutoResize       = 1 << 6   # Resize every window to its content every frame
ImGuiWindowFlags_NoBackground           = 1 << 7  # Disable drawing background color (WindowBg, etc.) and outside border. Similar as using SetNextWindowBgAlpha(0.0f).
ImGuiWindowFlags_NoSavedSettings        = 1 << 8   # Never load/save settings in .ini file
ImGuiWindowFlags_NoMouseInputs          = 1 << 9   # Disable catching mouse, hovering test with pass through.
ImGuiWindowFlags_MenuBar                = 1 << 10  # Has a menu-bar
ImGuiWindowFlags_HorizontalScrollbar    = 1 << 11  # Allow horizontal scrollbar to appear (off by default). You may use SetNextWindowContentSize(ImVec2(width,0.0f)); prior to calling Begin() to specify width. Read code in imgui_demo in the "Horizontal Scrolling" section.
ImGuiWindowFlags_NoFocusOnAppearing     = 1 << 12  # Disable taking focus when transitioning from hidden to visible state
ImGuiWindowFlags_NoBringToFrontOnFocus  = 1 << 13  # Disable bringing window to front when taking focus (e.g. clicking on it or programatically giving it focus)
ImGuiWindowFlags_AlwaysVerticalScrollbar= 1 << 14  # Always show vertical scrollbar (even if ContentSize.y < Size.y)
ImGuiWindowFlags_AlwaysHorizontalScrollbar=1<< 15  # Always show horizontal scrollbar (even if ContentSize.x < Size.x)
ImGuiWindowFlags_AlwaysUseWindowPadding = 1 << 16  # Ensure child windows without border uses style.WindowPadding (ignored by default for non-bordered child windows, because more convenient)
ImGuiWindowFlags_NoNavInputs            = 1 << 18  # No gamepad/keyboard navigation within the window
ImGuiWindowFlags_NoNavFocus             = 1 << 19  # No focusing toward this window with gamepad/keyboard navigation (e.g. skipped by CTRL+TAB)
ImGuiWindowFlags_NoNav                  = ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus
ImGuiWindowFlags_NoDecoration           = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse
ImGuiWindowFlags_NoInputs               = ImGuiWindowFlags_NoMouseInputs | ImGuiWindowFlags_NoNavInputs | ImGuiWindowFlags_NoNavFocus



EVENT_TYPE_K_RELEASE = 0
EVENT_TYPE_K_PRESS = 1
EVENT_TYPE_M_RELEASE = 2
EVENT_TYPE_M_PRESS = 3
EVENT_TYPE_M_MOVE = 4
EVENT_TYPE_K_CHAR_INPUT = 5
EVENT_TYPE_M_SCROLL = 6

GAME_PART_BLOCK = 0
GAME_PART_CYLINDER = 1
GAME_PART_LIFT = 2
GAME_PART_CONTROL = 3
GAME_PART_THRUSTER = 4
GAME_PART_CANNON = 5
GAME_PART_BEARING = 6
GAME_PART_SPRING = 7
SPECIAL_PART_PAINTER = 8
SPECIAL_PART_DIGGER = 9
GAME_PART_BUTTON = 10
GAME_PART_SWITCH = 11
GAME_PART_NOT_VALID = 9999





class WindowType(Enum):
	INVENTORY = 0,
	NODE_EDITOR = 1,
	VEHICLE_FILE_BROWSER = 2,
	RESUME_MENU = 3,
	HELP_PAGE =4 ,
	ATTRIBUTE_WINDOW =5 ,
	PAINTER = 6,

class GameState():
	m_currIndex = 1
	lift_state = 0
	isOpenTestWindow = True
	testIcon = None
	g_blockRotate = 0
	g_blockAttachIndex = 0
	m_isDragingInventory = False
	m_inventory = []
	m_itemSlots = {}
	currentSelectedItem = None
	m_isControlKeyPress = False
	m_isHoldButton = None
	oldPlayerPos = None

#init
def onEngineInit():
	print("Python init")
	GameState.testIcon = CubeEngine.TextureMgr.shared().getByPathSimple("UITexture/Icon/icons8-border-none-96.png")

test_var = 21
localTestVar = 4
#ui update
def handleUIUpdate(dt):
	if Game.GameWorld.shared().getCurrentState() == Game.GAME_STATE_RUNNING:
		if not Game.GameUISystem.shared().isVisible():
			#除了物品栏之外，没有其他界面打开的时候，才画底部的Hud
			# if not Game.GameUISystem.shared():isAnyShow() or Game.GameUISystem.shared():isOpenAssetEditor() :
				
			# 
			drawHud()
			updateLifting(dt)
		# elif GameWorld.shared():getCurrentState() == CPP_GAME.GAME_STATE_MAIN_MENU :
		# 	drawEntryInterface()



# inventory

class SlotObj():
	def __init__(self, target):
		self.target = target

for i in range(0, 7):
	GameState.m_itemSlots[i] = SlotObj(None)

GameState.m_itemSlots[0].target = "Lift"
GameState.m_itemSlots[1].target = "Block"
GameState.m_itemSlots[2].target = "Seat"
GameState.m_itemSlots[3].target = "Bearing"
GameState.m_itemSlots[4].target = "WheelOffRoad-Medium"

def updateLifting(dt):
	if GameState.lift_state != 0:
		lift = Game.BuildingSystem.shared().getLift()
		if lift != None:
			lift.liftUp(GameState.lift_state * dt * 2.0)

def InitInventory():
	inventoryAmount = Game.ItemMgr.shared().getItemAmount()
	print("inventoryAmount", inventoryAmount)
	for i in range(0, inventoryAmount):
		item = Game.ItemMgr.shared().getItemByIndex(i)
		print("the Item Name",item.m_name,"the item type",item.getTypeInInt())
		ItemClassData = "PlaceableBlock"
		if item.isSpecialFunctionItem():
			ItemClassData = "SpeicalBlock"
		itemTable = {"name" : item.m_name, 
					"ItemClass" : ItemClassData, 
					"ItemType" : item.getTypeInInt(), 
					"desc" : item.m_desc, 
					"ThumbNail" : item.getThumbNailTextureId()
					}
		GameState.m_inventory.append(itemTable)

def on_game_start():
	InitInventory()
	player = Game.GameWorld.shared().getPlayer()
	# choose a empty slot as init selected
	for k, v in GameState.m_itemSlots.items():
		if v.target == None:
			GameState.m_currIndex = k
			break
	player.setCurrSelected("")



def formatV3(v):
	return "%.2f*%.2f*%.2f"%(v.x, v.y, v.z)

def onDrawWindow(windowType):
	isOpen = ImGui.Begin(CubeEngine.TR("资产浏览器"), 0)
	i = 0
	itemSize = 80
	realItemSize = itemSize
	GameState.m_isDragingInventory = False

	window_visible_x2 = ImGui.GetWindowPos().x + ImGui.GetWindowContentRegionMax().x
	last_button_x2 = 0

	inventoryAmount = Game.ItemMgr.shared().getItemAmount()
	s = ImGui.GetStyle();
	spaceX = s.ItemSpacing.x;
	padding = s.FramePadding.x;
	rightPanelWidth = 150
	size = ImGui.GetWindowWidth() - s.IndentSpacing - rightPanelWidth
	ImGui.BeginChild("InventoryViewer",int(size), 0, 0);
	
	ImGui.Columns(math.floor(size / (itemSize + (2 * padding))),"InventoryCol",False)
	# for k, v in pairs(GameState.m_inventory) :
	for i in range(0, inventoryAmount):
		# coloum = math.floor(size / (itemSize + spaceX * 2 + padding * 2)) + 1
		# if ((i % coloum) != 0) :
		# 	ImGui.SameLine(0, -1);
		# 
		ImGui.PushID_str("inventory" + str(i))
		ImGui.BeginGroup()
		iconTexture = GameState.testIcon.handle()
		item = Game.ItemMgr.shared().getItemByIndex(i)
		if item.getThumbNailTextureId() != 0 :
			iconTexture = item.getThumbNailTextureId()
		

		# # print(item)

		if(ImGui.ImageButton(iconTexture, ImGui.ImVec2(itemSize, itemSize))) :
			GameState.currentSelectedItem  = item
		

		# print("aaaaaaaaaaaa"..GameState.currentSelectedItem.m_desc)
		last_button_x2 = ImGui.GetItemRectMax().x - ImGui.GetItemRectMin().x
		next_button_x2 = last_button_x2 + s.ItemSpacing.x + itemSize; # Expected position if next button was on same line

		# Our buttons are both drag sources and drag targets here!
		if (ImGui.BeginDragDropSource()) :
			ImGui.SetDragDropPayload("DND_DEMO_CELL", i);    # Set payload to carry the index of our item (could be anything)
			ImGui.Text(CubeEngine.TR("拖拽") + CubeEngine.TR(item.m_desc));   # Display preview (could be anything, e.g. when dragging an image we could decide to display the filename and a small preview of the image, etc.)
			ImGui.EndDragDropSource()
			GameState.m_isDragingInventory = True
		
		if GameState.currentSelectedItem and GameState.currentSelectedItem.m_name == item.m_name :
			sizeMin = ImGui.GetItemRectMin()
			sizeMax = ImGui.GetItemRectMax()
			ImGui.DrawFrame(sizeMin, sizeMax, 2, ImGui.ImVec4(0, 0, 0, 1))
		
		ImGui.Text(CubeEngine.TR(item.m_desc))

		ImGui.EndGroup()
		ImGui.PopID()

		ImGui.NextColumn()
	
	ImGui.Columns(1,"InventoryCol",False)
	ImGui.EndChild()
	ImGui.SameLine(0, -1)
	ImGui.BeginChild("DetailInfo",0, 0, 0)
	if GameState.currentSelectedItem  != None :
		ImGui.Text(CubeEngine.TR(GameState.currentSelectedItem .m_desc));
		ImGui.TextWrapped(CubeEngine.TR(GameState.currentSelectedItem.m_description));
		ImGui.TextWrapped(CubeEngine.TR("Mass: ")+ str(GameState.currentSelectedItem.m_physicsInfo.mass));
		size = GameState.currentSelectedItem.m_physicsInfo.size;
		ImGui.TextWrapped(CubeEngine.TR("Size: ") + str(formatV3(size)));
	
	ImGui.EndChild()
	ImGui.End()
	# os.exit()
	return isOpen


def drawHud():
	screenSize = CubeEngine.Engine.shared().winSize()
	yOffset = 15.0;
	window_pos = ImGui.ImVec2(screenSize.x / 2.0, screenSize.y - yOffset);
	window_pos_pivot = ImGui.ImVec2(0.5, 1.0);

	window_pos_pivot_bottom_right = ImGui.ImVec2(1.0, 1.0);
	ImGui.SetNextWindowPos(ImGui.ImVec2(screenSize.x - 50.0, screenSize.y - yOffset), ImGuiCond_Always, window_pos_pivot_bottom_right);
	ImGui.Begin("Rotate Tips", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoBringToFrontOnFocus);
	if Game.BuildingSystem.shared().isIsInXRayMode() :
		ImGui.Text(CubeEngine.TR("X光模式"));
	else:
		ImGui.Text(CubeEngine.TR("当前旋转角度")+ str(GameState.g_blockRotate));
	
	ImGui.End()


	ImGui.SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
	itemSize = 64.0
	ImGui.Begin("Hud", ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove);
	for k, v in GameState.m_itemSlots.items():
		# ImGui.RadioButton(v.desc, GameState.m_currIndex == k)
		ImGui.BeginGroup();
		needPop = False
		if v.target == None :
			ImGui.Image(GameState.testIcon.handle(), ImGui.ImVec2(itemSize, itemSize));
			if GameState.m_currIndex == k :
				ImGui.PushStyleColor(0, ImGui.ImVec4(0.2, 0.2, 0.2, 1));
				sizeMin = ImGui.GetItemRectMin()
				sizeMax = ImGui.GetItemRectMax()
				ImGui.DrawFrame(sizeMin, sizeMax, 3.0, ImGui.ImVec4(1, 0, 1, 1))
				needPop = True
			
		else:
			iconTexture = GameState.testIcon.handle()
			item = getItemFromSpecifiedSlotIndex(k);
			if item.getThumbNailTextureId() != 0 :
				iconTexture = item.getThumbNailTextureId()
			
			ImGui.Image(iconTexture, ImGui.ImVec2(itemSize, itemSize));
			if GameState.m_currIndex == k :
				ImGui.PushStyleColor(0, ImGui.ImVec4(0.2, 0.2, 0.2, 1));
				needPop = True
				sizeMin = ImGui.GetItemRectMin()
				sizeMax = ImGui.GetItemRectMax()
				ImGui.DrawFrame(sizeMin, sizeMax, 3.0, ImGui.ImVec4(1, 0, 1, 1))
			
		
		if needPop :
			ImGui.PopStyleColor()
		
		ImGui.EndGroup()
		if ImGui.BeginDragDropTargetAnyWindow() :
			payLoad = ImGui.AcceptDragDropPayload("DND_DEMO_CELL", 0)
			# print("payLoad", payLoad)
			if payLoad != None :
				payLoadIdx = ImGui.GetPayLoadData2Int(payLoad)
				payLoadItem = Game.ItemMgr.shared().getItemByIndex(payLoadIdx)
				print ("Play load Here   "+ (payLoadItem.m_name))
				GameState.m_itemSlots[k].target = payLoadItem.m_name
				player = Game.GameWorld.shared().getPlayer()
				if k == GameState.m_currIndex :
					if GameState.m_itemSlots[GameState.m_currIndex].target:
						player.setCurrSelected(GameState.m_itemSlots[GameState.m_currIndex].target)
					else:
						player.setCurrSelected(None)
				
			
			ImGui.EndDragDropTarget()
		
		ImGui.SameLine(0, -1.0)
	
	ImGui.End()


def findItemNameByIdx(id):
	return Game.ItemMgr.shared().getItemByIndex(id)


def findItemByName(name):
	return Game.ItemMgr.shared().getItem(name)




def onKeyPress(input_event):
	player = Game.GameWorld.shared().getPlayer()
	if input_event.keycode == KeyConfig.TZW_KEY_UP :
		GameState.lift_state = 1
	elif input_event.keycode == KeyConfig.TZW_KEY_DOWN :
		GameState.lift_state = -1
	elif input_event.keycode == KeyConfig.TZW_KEY_E :
		if Game.BuildingSystem.shared().getCurrentControlPart() == None :
			result = Game.BuildingSystem.shared().rayTestPart(player.getPos(), player.getForward(), 10)
			if result and Game.BuildingSystem.shared().getGamePartTypeInt(result) == GAME_PART_BUTTON :
				player.pressButton(result)
				GameState.m_isHoldButton = result
			elif result and Game.BuildingSystem.shared().getGamePartTypeInt(result) == GAME_PART_SWITCH :
				# player:pressButton(result)
				GameState.m_isHoldButton = result
			
		
	elif input_event.keycode == KeyConfig.TZW_KEY_LEFT_CONTROL :
		GameState.m_isControlKeyPress = True
	
def onKeyRelease(input_event):
	player = Game.GameWorld.shared().getPlayer()
	isKeyPress = False
	if CubeEngine.GUISystem.shared().isUiCapturingInput() :
		return
	if input_event.keycode == KeyConfig.TZW_KEY_1 :
		GameState.m_currIndex = 0
		isKeyPress = True
	elif input_event.keycode == KeyConfig.TZW_KEY_2 :
		GameState.m_currIndex = 1
		isKeyPress = True
	elif input_event.keycode == KeyConfig.TZW_KEY_3 :
		GameState.m_currIndex = 2
		isKeyPress = True
	elif input_event.keycode == KeyConfig.TZW_KEY_4 :
		GameState.m_currIndex = 3
		isKeyPress = True
	elif input_event.keycode == KeyConfig.TZW_KEY_5 :
		GameState.m_currIndex = 4
		isKeyPress = True
	elif input_event.keycode == KeyConfig.TZW_KEY_6 :
		GameState.m_currIndex = 5
		isKeyPress = True
	elif input_event.keycode == KeyConfig.TZW_KEY_7 :
		GameState.m_currIndex = 6
		isKeyPress = True
	elif input_event.keycode == KeyConfig.TZW_KEY_8 :
		GameState.m_currIndex = 7
		isKeyPress = True
	elif input_event.keycode == KeyConfig.TZW_KEY_LEFT_CONTROL :
		GameState.m_isControlKeyPress = False
	elif input_event.keycode == KeyConfig.TZW_KEY_UP :
		GameState.lift_state = GameState.lift_state - 1
	elif input_event.keycode == KeyConfig.TZW_KEY_DOWN :
		GameState.lift_state = GameState.lift_state + 1
	elif input_event.keycode == KeyConfig.TZW_KEY_Q :
		player:getPreviewItem().switchAttachment()
	elif input_event.keycode == KeyConfig.TZW_KEY_R :
		if GameState.m_isControlKeyPress :
			GameState.g_blockRotate = GameState.g_blockRotate + 45
		else:
			GameState.g_blockRotate = GameState.g_blockRotate - 45
		
		if GameState.g_blockRotate >= 360 :
			GameState.g_blockRotate = 0
		elif GameState.g_blockRotate < 0 :
			GameState.g_blockRotate = GameState.g_blockRotate + 360
		
		player.setPreviewAngle(GameState.g_blockRotate)
	elif input_event.keycode == KeyConfig.TZW_KEY_I :
		Game.GameUISystem.shared().setIsShowAssetEditor(True)
	elif input_event.keycode == KeyConfig.TZW_KEY_F :
		if Game.BuildingSystem.shared().getCurrentControlPart() == None :
			result = Game.BuildingSystem.shared().rayTestPartAny(player.getPos(), player.getForward(), 10)
			#轴承翻转
			if result and Game.BuildingSystem.shared().getGamePartTypeInt(result) == GAME_PART_BEARING :
				Game.BuildingSystem.shared().flipBearing(result);
			#打开节点编辑器
			elif result and Game.BuildingSystem.shared().getGamePartTypeInt(result) == GAME_PART_CONTROL :
				Game.GameUISystem.shared().setIsShowNodeEditor(True);
			
		
	elif input_event.keycode == KeyConfig.TZW_KEY_E :
		if Game.BuildingSystem.shared().getCurrentControlPart() == None :
			if GameState.m_isHoldButton != None :
				if Game.BuildingSystem.shared().getGamePartTypeInt(GameState.m_isHoldButton) == GAME_PART_BUTTON :
					player.releaseButton(GameState.m_isHoldButton)
				elif Game.BuildingSystem.shared().getGamePartTypeInt(GameState.m_isHoldButton) == GAME_PART_SWITCH :
					player.releaseSwitch(GameState.m_isHoldButton)
				
				GameState.m_isHoldButton = None
			else:
				result = Game.BuildingSystem.shared().rayTestPart(player.getPos(), player.getForward(), 10)
				if result and Game.BuildingSystem.shared().getGamePartTypeInt(result) == GAME_PART_CONTROL :
					GameState.oldPlayerPos = player.getPos()
					Game.BuildingSystem.shared().setCurrentControlPart(result)
				elif result and Game.BuildingSystem.shared().getGamePartTypeInt(result) == GAME_PART_LIFT :
					Game.GameUISystem.shared().setIsFileBroswerOpen(True)
				else:
					player.openCurrentPartInspectMenu();
				
			
		else:
			player.setPos(GameState.oldPlayerPos)
			Game.BuildingSystem.shared().setCurrentControlPart(None)
		
	
	if isKeyPress :
		notifySelectItem()

def notifySelectItem():
	player = Game.GameWorld.shared().getPlayer()
	if GameState.m_itemSlots[GameState.m_currIndex].target:
		player.setCurrSelected(GameState.m_itemSlots[GameState.m_currIndex].target)
	else:
		player.setCurrSelected("")

def checkIsNormalPart(itemType):
	constrainType = {GAME_PART_BEARING, GAME_PART_SPRING}
	isNormal = True
	for v in constrainType:
		if v == itemType:
			isNormal = False
			break
	  
	
	return isNormal


def placeItem(item):
	print ("placeItem"+ str(item.getTypeInInt()))
	player = Game.GameWorld.shared().getPlayer()
	result = Game.BuildingSystem.shared().rayTest(player.getPos(), player.getForward(), 10)
	if checkIsNormalPart(item.getTypeInInt()) :
		aBlock = Game.BuildingSystem.shared().createPart(item.getTypeInInt(), item.m_name)
		
		if result == None :
			resultPos = Game.BuildingSystem.shared().hitTerrain(player.getPos(), player.getForward(), 10)
			if resultPos.y > -99999 :
				print("place and create static")
				Game.BuildingSystem.shared().placeGamePartStatic(aBlock, resultPos)
			
		else:
			print("degree "+  str(GameState.g_blockRotate))
			Game.BuildingSystem.shared().attachGamePart(aBlock, result, GameState.g_blockRotate, aBlock.getPrettyAttach(result, player.getPreviewItem().getCurrAttachment()))
			GameState.g_blockRotate = 0 #reset
			player.setPreviewAngle(GameState.g_blockRotate)
		
	else:
		if result == None :
			print(": nothing")
		else:
			if item.getTypeInInt() == GAME_PART_BEARING :
				Game.BuildingSystem.shared().placeBearingToAttach(result, item.m_name)
			elif item.getTypeInInt() == GAME_PART_SPRING :
				Game.BuildingSystem.shared().placeSpringToAttach(result, item.m_name)
			
		
	


def handleItemPrimaryUse(item):
	player = Game.GameWorld.shared().getPlayer()
	if (not item.isSpecialFunctionItem()) :
		placeItem(item)
	elif item.getTypeInInt() == GAME_PART_LIFT :
		result = Game.BuildingSystem.shared().rayTestPart(player.getPos(), player.getForward(), 10)
		if result != None :
			#先收纳 再搞事
			Game.BuildingSystem.shared().liftStore(result)
		else:
			resultPos = Game.BuildingSystem.shared().hitTerrain(player.getPos(), player.getForward(), 10)
			if resultPos.y > -99999 :
				Game.BuildingSystem.shared().placeLiftPart(resultPos)
			
		
	elif (item.getTypeInInt() == SPECIAL_PART_PAINTER) : #paint the object
		player.paint();
	elif (item.getTypeInInt() == SPECIAL_PART_DIGGER) : #fill the terrain
		Game.BuildingSystem.shared().terrainForm(player.getPos(), player.getForward(), 10, 0.8, 3.0)
	
	player.updateCrossHairTipsInfo()


def handleItemSecondaryUse(item):
	player = Game.GameWorld.shared().getPlayer()
	if (not item.isSpecialFunctionItem() or item.getTypeInInt() == GAME_PART_LIFT) :
		result = Game.BuildingSystem.shared().rayTestPart(player.getPos(), player.getForward(), 10)
		if result :
			player.removePart(result)
		
	elif (item.getTypeInInt() == SPECIAL_PART_PAINTER) : #paint the object
		Game.GameUISystem.shared().setPainterShow(True)
	elif (item.getTypeInInt() == SPECIAL_PART_DIGGER) : #dig the terrain
		Game.BuildingSystem.shared().terrainForm(player.getPos(), player.getForward(), 10, -0.8, 3.0)
	
	player.updateCrossHairTipsInfo()


def getItemFromSlotIndex():
	return findItemByName(GameState.m_itemSlots[GameState.m_currIndex].target)


def getItemFromSpecifiedSlotIndex(index):
	return findItemByName(GameState.m_itemSlots[index].target)


def onMouseRelease(input_event):
	if not Game.BuildingSystem.shared().isIsInXRayMode() :
		item = getItemFromSlotIndex()
		if item != None :
			if input_event.arg == 0 : #left mouse
				handleItemPrimaryUse(item)
			elif input_event.arg == 1 : #right mouse
				handleItemSecondaryUse(item)
	else:
		print("xray mode can not place")
	

def onMouseScroll(input_event):
	print("scroll");
	slotSize = len(GameState.m_itemSlots)
	if(int(input_event.offset.y) == 1):
		GameState.m_currIndex += 1;
		GameState.m_currIndex %= slotSize
	elif(int(input_event.offset.y) == -1):
		GameState.m_currIndex -= 1;
		GameState.m_currIndex += slotSize
		GameState.m_currIndex %= slotSize
	notifySelectItem()

#input event
def onEngineInputEvent(input_event):
	if Game.GameUISystem.shared().isAnyShow() :
		return
	else:
		if Game.GameWorld.shared().getCurrentState() == Game.GAME_STATE_RUNNING :
			if input_event.type == EVENT_TYPE_K_RELEASE : 
				onKeyRelease(input_event)
			elif input_event.type == EVENT_TYPE_K_PRESS : 
				onKeyPress(input_event)
			elif input_event.type == EVENT_TYPE_M_RELEASE : 
				onMouseRelease(input_event)
			elif input_event.type == EVENT_TYPE_M_SCROLL :
				onMouseScroll(input_event)