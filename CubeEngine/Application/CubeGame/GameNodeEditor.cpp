#include "GameNodeEditor.h"
#include "2D/imnodes.h"
#include "NodeEditorNodes/ControlPartNode.h"
#include "NodeEditorNodes/BearingPartNode.h"
//#include <minwindef.h>
#include "Base/GuidMgr.h"
#include "ControlPart.h"
#include "SpringPart.h"
#include "Engine/Engine.h"
#include "NodeEditor/Include/imgui_node_editor.h"
#include "NodeEditor/Source/imgui_node_editor_internal.h"
#include "NodeEditor/BlueprintUtilities/Include/ax/Drawing.h"
#include "NodeEditor/BlueprintUtilities/Include/ax/Widgets.h"
#include "NodeEditor/BlueprintUtilities/Include/ax/Builders.h"
#include "Texture/TextureMgr.h"
#include "BehaviorNode.h"
#include "TriggerNode.h"
#include "NodeEditorNodes/SpinNode.h"
#include "NodeEditorNodes/KeyTriggerNode.h"
#include "Event/EventMgr.h"
#include "BuildingSystem.h"
#include "NodeEditorNodes/DebugBehaviorNode.h"
#include "NodeEditorNodes/VectorNode.h"
#include "NodeEditorNodes/UseNode.h"
#include "NodeEditorNodes/ConstantIntNode.h"
#include "NodeEditorNodes/KeyAnyTriggerNode.h"
#include "NodeEditorNodes/ToggleNode.h"
#include "ThrusterPart.h"

namespace ed = ax::NodeEditor;
namespace util = ax::NodeEditor::Utilities;
// Struct to hold basic information about connection between
// pins. Note that connection (aka. link) has its own ID.
// This is useful later with dealing with selections, deletion
// or other operations.


namespace tzw
{
enum class PinType
{
    Flow,
    Bool,
    Int,
    Float,
    String,
    Object,
    Function,
    Delegate,
};
	static int newLinkPin  = 0;
	using IconType = ax::Drawing::IconType;
	int g_uniqueLinkIndex;
	static ed::EditorContext* g_Context = nullptr;
	static bool * g_isOpen = nullptr;
	static Texture* s_SaveIcon;
	static Texture * s_HeaderBackground;
	static Texture * s_RestoreIcon;
	GameNodeEditor::GameNodeEditor()
	{
		g_Context = ed::CreateEditor();
	    s_HeaderBackground = TextureMgr::shared()->getByPath("./Texture/NodeEditor/BlueprintBackground.png");
	    s_SaveIcon         = TextureMgr::shared()->getByPath("./Texture/NodeEditor/ic_save_white_24dp.png");
	    s_RestoreIcon      = TextureMgr::shared()->getByPath("./Texture/NodeEditor/ic_restore_white_24dp.png");
		EventMgr::shared()->addFixedPiorityListener(this);
	}

	void GameNodeEditor::drawIMGUI(bool * isOpen)
	{
		newNodeEditorDraw(isOpen);
	}

	void GameNodeEditor::addNode(GameNodeEditorNode* newNode)
	{
		m_gameNodes.push_back(newNode);
		if(newNode->getType() == Node_TYPE_TRIGGER)
		{
			m_triggerList.push_back(static_cast<KeyTriggerNode *> (newNode));
		}
		//m_nodeGlobalCount +=1;
		//newNode->m_nodeID = m_nodeGlobalCount;
	}

	void GameNodeEditor::removeNode(GameNodeEditorNode* node)
	{
		auto result = std::find(m_gameNodes.begin(), m_gameNodes.end(), node);
		if(result != m_gameNodes.end())
		{
			m_gameNodes.erase(result);
			removeAllLink(node);
		}
		
	}

	void GameNodeEditor::removeAllLink(GameNodeEditorNode* node)
	{
		for (auto i = m_links.begin(); i != m_links.end();)
		{

			if(node->checkInNodeAttr(i->InputId) || node->checkOutNodeAttr(i->OutputId))
			{
				i = m_links.erase(i);
			}
			else
			{
				++i;
			}
		}
	}

	void GameNodeEditor::raiseEventToNode(int startAttr, int endAttr)
	{
		GameNodeEditorNode * startNode = nullptr;
		GameNodeEditorNode * endNode = nullptr;
		for (auto node : m_gameNodes)
		{
			if (!startNode) 
			{     
				if(node->checkOutNodeAttr(startAttr)) 
				{
					startNode = node;
				}
            }
			if (!endNode) 
			{     
				if(node->checkInNodeAttr(endAttr)) 
				{
					endNode = node;
				}
            }
			if (endNode && startNode) 
			{     
				break;
            }
		}

		if(startNode && endNode) 
		{
			startNode->onLinkOut(startAttr, endAttr, endNode);
			endNode->onLinkIn(startAttr, endAttr, startNode);
		}

	}

	void GameNodeEditor::dump(rapidjson::Value& partDocObj, rapidjson::Document::AllocatorType& allocator)
	{
		ed::SetCurrentEditor(g_Context);
		rapidjson::Value NodeGraphObj(rapidjson::kObjectType);

		rapidjson::Value NodeListObj(rapidjson::kArrayType);
		for (auto node : m_gameNodes)
		{
			rapidjson::Value NodeObj(rapidjson::kObjectType);
			auto origin = ed::GetNodePosition(node->m_nodeID);
			NodeObj.AddMember("orgin_x", origin.x, allocator);
			NodeObj.AddMember("orgin_y", origin.y, allocator);
			
			node->dump(NodeObj, allocator);
			NodeListObj.PushBack(NodeObj, allocator);
		}
		NodeGraphObj.AddMember("NodeList", NodeListObj, allocator);

		rapidjson::Value linkListDoc(rapidjson::kArrayType);
		for (auto& m_link : m_links)
		{

			auto startAttr = m_link.InputId;
			auto endAttr = m_link.OutputId;
			GameNodeEditorNode * startNode = nullptr;
			GameNodeEditorNode * endNode = nullptr;
			for (auto node : m_gameNodes)
			{
				if (!startNode) 
				{     
					if(node->checkOutNodeAttr(startAttr)) 
					{
						startNode = node;
					}
	            }
				if (!endNode) 
				{     
					if(node->checkInNodeAttr(endAttr)) 
					{
						endNode = node;
					}
	            }
				if (endNode && startNode) 
				{     
					break;
	            }
			}

			if(startNode && endNode)
			{
				rapidjson::Value linkObj(rapidjson::kObjectType);
				linkObj.AddMember("from", std::string(startNode->getGUID()), allocator);
				linkObj.AddMember("to", std::string(endNode->getGUID()), allocator);
				linkObj.AddMember("fromOutputID", startNode->getOutputAttrLocalIndexByGid(startAttr), allocator);
				linkObj.AddMember("toInputID", endNode->getInputAttrLocalIndexByGid(endAttr), allocator);
				linkListDoc.PushBack(linkObj, allocator);
			}
		}
		NodeGraphObj.AddMember("NodeLinkList", linkListDoc, allocator);
		partDocObj.AddMember("NodeGraph", NodeGraphObj, allocator);
	}

	void GameNodeEditor::load(rapidjson::Value& NodeGraphObj)
	{
		//read node
		auto& NodeList = NodeGraphObj["NodeList"];
		for(unsigned int i = 0; i < NodeList.Size(); i++)
		{
			auto& node = NodeList[i];
			GameNodeEditorNode * newNode = nullptr;
			//we skip create resource Node, but find exist resource node, and set properly UID
			if(node["NodeType"].GetInt() == Node_TYPE_RES)
			{
				auto resUID = node["ResUID"].GetString();
				auto resType = node["ResType"].GetString();
				if(strcmp(resType, "ControlPart") == 0)//ControlPart
				{
					auto controlPart = reinterpret_cast<ControlPart*>(GUIDMgr::shared()->get(resUID));
					newNode = controlPart->getGraphNode();
					
				}
				else if(strcmp(resType, "BearPart") == 0)//BearPart
				{
					auto bearPart = reinterpret_cast<BearPart*>(GUIDMgr::shared()->get(resUID));
					newNode = bearPart->getGraphNode();
				}
				else if(strcmp(resType, "SpringPart") == 0)//SpringPart
				{
					auto bearPart = reinterpret_cast<SpringPart*>(GUIDMgr::shared()->get(resUID));
					newNode = bearPart->getGraphNode();
				}
				else if(strcmp(resType, "Thruster") == 0)//Thruster
				{
					auto bearPart = reinterpret_cast<ThrusterPart*>(GUIDMgr::shared()->get(resUID));
					newNode = bearPart->getGraphNode();
				}
				if(newNode)
				{
					newNode->setGUID(node["UID"].GetString());
				}
			}
			else //normal logic & function node, we need create here
			{
				auto nodeClass = node["NodeClass"].GetInt();
				switch(nodeClass)
				{
                case Node_CLASS_KEY_TRIGGER:
					newNode = new KeyTriggerNode();
					break;
                case Node_CLASS_KEY_ANY_TRIGGER:
					newNode = new KeyAnyTriggerNode();
					break;
                case Node_CLASS_SPIN:
					newNode = new SpinNode();
					break;
                case Node_CLASS_VECTOR:
					newNode = new VectorNode();
					break;
                case Node_CLASS_USE:
					newNode = new UseNode();
					break;
                case Node_CLASS_CONSTANT_INT:
					newNode = new ConstantIntNode();
					break;
                case Node_CLASS_TOGGLE:
					newNode = new ToggleNode();
					break;
				}
				
				addNode(newNode);
			}
			newNode->load(node);
		}
		//read link
		auto& linkList = NodeGraphObj["NodeLinkList"];
		for(unsigned int i = 0; i < linkList.Size(); i++) 
		{
			auto& linkObj = linkList[i];
			GameNodeEditorNode * nodeA = reinterpret_cast<GameNodeEditorNode*>(GUIDMgr::shared()->get(linkObj["from"].GetString()));
			GameNodeEditorNode * nodeB = reinterpret_cast<GameNodeEditorNode*>(GUIDMgr::shared()->get(linkObj["to"].GetString()));
			int fromOutputID = linkObj["fromOutputID"].GetInt();
			int toInputID = linkObj["toInputID"].GetInt();
			makeLinkByNode(nodeA, nodeB, fromOutputID, toInputID);
		}
	}




	void GameNodeEditor::makeLinkByNode(GameNodeEditorNode* NodeA, GameNodeEditorNode* NodeB, int indexOfA, int indeOfB)
	{
		int start_attr = NodeA->getOutByIndex(indexOfA)->gID;
		int end_attr = NodeB->getInByIndex(indeOfB)->gID;
		LinkInfo info;
		info.Id = ++g_uniqueLinkIndex;
		info.InputId = start_attr;
		info.OutputId = end_attr;
		m_links.push_back(info);
		raiseEventToNode(start_attr, end_attr);
	}
	void GameNodeEditor::ShowLeftPane(float paneWidth)
	{
	    auto& io = ImGui::GetIO();

	    ImGui::BeginChild("Selection", ImVec2(paneWidth, 0));

	    paneWidth = ImGui::GetContentRegionAvailWidth();

	    
		ImGui::BeginHorizontal("Style Editor", ImVec2(paneWidth, 0));
	    ImGui::Spring(0.0f, 0.0f);
	    if (ImGui::Button(u8"调整视图"))
	        ed::NavigateToContent();
		if(ImGui::Button(u8"退出"))
			(*g_isOpen) = false;
	    ImGui::Spring(0.0f);
	    ImGui::Spring();
	    ImGui::EndHorizontal();
		if(ImGui::Button(u8"旋转节点"))
		{
			addNode(new SpinNode());
		}
		if(ImGui::Button(u8"标准输入"))
		{
			addNode(new KeyTriggerNode());
		}
		if(ImGui::Button(u8"按键输入"))
		{
			addNode(new KeyAnyTriggerNode());
		}
		if(ImGui::Button(u8"使用节点"))
		{
			addNode(new UseNode());
		}
		if(ImGui::Button(u8"Vector"))
		{
			addNode(new VectorNode());
		}
		if(ImGui::Button(u8"Constant"))
		{
			addNode(new ConstantIntNode());
		}
		if(ImGui::Button(u8"Toggle"))
		{
			addNode(new ToggleNode());
		}
	    std::vector<ed::NodeId> selectedNodes;
	    std::vector<ed::LinkId> selectedLinks;
	    selectedNodes.resize(ed::GetSelectedObjectCount());
	    selectedLinks.resize(ed::GetSelectedObjectCount());

	    int nodeCount = ed::GetSelectedNodes(selectedNodes.data(), static_cast<int>(selectedNodes.size()));
	    int linkCount = ed::GetSelectedLinks(selectedLinks.data(), static_cast<int>(selectedLinks.size()));

	    selectedNodes.resize(nodeCount);
	    selectedLinks.resize(linkCount);

	    int saveIconWidth     = s_SaveIcon->getSize().x;//Application_GetTextureWidth(s_SaveIcon);
	    int saveIconHeight    = s_SaveIcon->getSize().y;//Application_GetTextureWidth(s_SaveIcon);
	    int restoreIconWidth  = s_RestoreIcon->getSize().x;//Application_GetTextureWidth(s_RestoreIcon);
	    int restoreIconHeight = s_RestoreIcon->getSize().y;//Application_GetTextureWidth(s_RestoreIcon);

	    ImGui::GetWindowDrawList()->AddRectFilled(
	        ImGui::GetCursorScreenPos(),
	        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
	        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
	    ImGui::Spacing(); ImGui::SameLine();
	    ImGui::TextUnformatted(u8"节点列表");
	    ImGui::Indent();
	    for (auto& node : m_gameNodes)
	    {
	        ImGui::PushID(node->m_nodeID);
	        auto start = ImGui::GetCursorScreenPos();

	        bool isSelected = std::find(selectedNodes.begin(), selectedNodes.end(), ax::NodeEditor::NodeId(node->m_nodeID)) != selectedNodes.end();
	        if (ImGui::Selectable((node->name + "##" + std::to_string(node->m_nodeID)).c_str(), &isSelected))
	        {
	            if (io.KeyCtrl)
	            {
	                if (isSelected)
	                    ed::SelectNode(node->m_nodeID, true);
	                else
	                    ed::DeselectNode(node->m_nodeID);
	            }
	            else
	                ed::SelectNode(node->m_nodeID, false);

	            ed::NavigateToSelection();
	        }

	        auto id = std::string("(") + std::to_string(node->m_nodeID) + ")";
	        auto textSize = ImGui::CalcTextSize(id.c_str(), nullptr);
	        auto iconPanelPos = start + ImVec2(
	            paneWidth - ImGui::GetStyle().FramePadding.x - ImGui::GetStyle().IndentSpacing - saveIconWidth - restoreIconWidth - ImGui::GetStyle().ItemInnerSpacing.x * 1,
	            (ImGui::GetTextLineHeight() - saveIconHeight) / 2);
	        ImGui::GetWindowDrawList()->AddText(
	            ImVec2(iconPanelPos.x - textSize.x - ImGui::GetStyle().ItemInnerSpacing.x, start.y),
	            IM_COL32(255, 255, 255, 255), id.c_str(), nullptr);

	        auto drawList = ImGui::GetWindowDrawList();
	        ImGui::SetCursorScreenPos(iconPanelPos);
	        ImGui::SetItemAllowOverlap();

	        ImGui::Dummy(ImVec2(float(saveIconWidth), float(saveIconHeight)));
	        drawList->AddImage(reinterpret_cast<ImTextureID>(s_SaveIcon->handle()), ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 32));


	        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
	        ImGui::SetItemAllowOverlap();
            ImGui::Dummy(ImVec2(float(restoreIconWidth), float(restoreIconHeight)));
            drawList->AddImage(reinterpret_cast<ImTextureID>(s_RestoreIcon->handle()), ImGui::GetItemRectMin(), ImGui::GetItemRectMax(), ImVec2(0, 0), ImVec2(1, 1), IM_COL32(255, 255, 255, 32));
	        ImGui::SameLine(0, 0);
	        ImGui::SetItemAllowOverlap();
	        ImGui::Dummy(ImVec2(0, float(restoreIconHeight)));

	        ImGui::PopID();
	    }
	    ImGui::Unindent();

	    static int changeCount = 0;

	    ImGui::GetWindowDrawList()->AddRectFilled(
	        ImGui::GetCursorScreenPos(),
	        ImGui::GetCursorScreenPos() + ImVec2(paneWidth, ImGui::GetTextLineHeight()),
	        ImColor(ImGui::GetStyle().Colors[ImGuiCol_HeaderActive]), ImGui::GetTextLineHeight() * 0.25f);
	    ImGui::Spacing(); ImGui::SameLine();
	    ImGui::TextUnformatted(u8"选中");

	    ImGui::BeginHorizontal("Selection Stats", ImVec2(paneWidth, 0));
	    ImGui::Text("Changed %d time%s", changeCount, changeCount > 1 ? "s" : "");
	    ImGui::Spring();
	    if (ImGui::Button(u8"全部取消选中"))
	        ed::ClearSelection();
	    ImGui::EndHorizontal();
	    ImGui::Indent();
	    for (int i = 0; i < nodeCount; ++i) ImGui::Text(u8"节点 %s", findNode(selectedNodes[i].Get())->name.c_str());
	    for (int i = 0; i < linkCount; ++i) ImGui::Text(u8"连接 (%p)", selectedLinks[i].AsPointer());
	    ImGui::Unindent();

	    if (ed::HasSelectionChanged())
	        ++changeCount;

	    ImGui::EndChild();
	}

	GameNodeEditorNode* GameNodeEditor::findNode(int nodeID)
	{
		for(auto node:m_gameNodes)
		{
			if(nodeID == node->m_nodeID)
			{
				return node;
			}
		}
		return nullptr;
	}

bool GameNodeEditor::onKeyPress(int keyCode)
{
	//if(BuildingSystem::shared()->getCurrentControlPart() && !BuildingSystem::shared()->getCurrentControlPart()->getIsActivate()) return false;
	for(auto trigger : m_triggerList)
	{
		if(trigger->getNodeClass() == Node_CLASS_KEY_TRIGGER || trigger->getNodeClass() == Node_CLASS_KEY_ANY_TRIGGER)
		{

				static_cast<TriggerNode *>(trigger)->handleKeyPress(keyCode);

		}
	}
	while(!m_rt_exe_chain.empty())
	{
		auto node = m_rt_exe_chain.front();
		m_rt_exe_chain.pop();
		node->execute();
		node->handleExeOut();
	}
	return false;
}

bool GameNodeEditor::onKeyRelease(int keyCode)
{
	//if(BuildingSystem::shared()->getCurrentControlPart() && !BuildingSystem::shared()->getCurrentControlPart()->getIsActivate()) return false;
	for(auto trigger : m_triggerList)
	{
		if(trigger->getNodeClass() == Node_CLASS_KEY_TRIGGER || trigger->getNodeClass() == Node_CLASS_KEY_ANY_TRIGGER)
		{
			static_cast<TriggerNode *>(trigger)->handleKeyRelease(keyCode);
		}
	}
	while(!m_rt_exe_chain.empty())
	{
		auto node = m_rt_exe_chain.front();
		m_rt_exe_chain.pop();
		node->execute();
		node->handleExeOut();
	}
	return false;
}

void GameNodeEditor::findNodeLinksToAttr(NodeAttr* attr, std::vector<GameNodeEditorNode*>&nodeList)
{
	for(auto link : m_links) 
	{
		if(link.InputId == attr->gID)
		{
			nodeList.push_back(findNodeByAttrGid(link.OutputId));
		}
	}
}

GameNodeEditorNode* GameNodeEditor::findNodeByAttr(NodeAttr* attr)
{
	for(auto node :m_gameNodes)
	{
		if(node->getAttrByGid(attr->gID)) 
		{
			return node;
		}
	}
	return nullptr;
}

GameNodeEditorNode* GameNodeEditor::findNodeByAttrGid(unsigned gid)
{
	for(auto node :m_gameNodes)
	{
		if(node->getAttrByGid(gid)) 
		{
			return node;
		}
	}
	return nullptr;
}

GameNodeEditorNode* GameNodeEditor::findNodeLinksFromAttr(NodeAttr* attr)
{
	for(auto link : m_links) 
	{
		if(link.OutputId == attr->gID)
		{
			return findNodeByAttrGid(link.InputId);
		}
	}
	return nullptr;
}

NodeAttr* GameNodeEditor::findAttrLinksFromAttr(NodeAttr* attr)
{
	for(auto link : m_links) 
	{
		if(link.OutputId == attr->gID)
		{
			return findAttr(link.InputId);
		}
	}
	return nullptr;
}

std::vector<NodeAttr*> GameNodeEditor::findAllAttrLinksFromAttr(NodeAttr* attr)
{
	std::vector<NodeAttr * > attrList;
	for(auto link : m_links) 
	{
		if(link.OutputId == attr->gID)
		{
			attrList.push_back(findAttr(link.InputId));
		}
	}
	return attrList;
}

void GameNodeEditor::pushToStack(GameNodeEditorNode* node)
{
	m_rt_exe_chain.push(node);
}

void GameNodeEditor::clearAll()
{
	
	for(auto node : m_gameNodes)
	{
		delete node;
	}
	m_gameNodes.clear();
	m_links.clear();
	m_triggerList.clear();
}

void GameNodeEditor::newNodeEditorDraw(bool* isOpen)
	{
		static bool g_FirstFrame = true;
		g_isOpen = isOpen;
		auto screenSize = Engine::shared()->winSize();
		float m_initW = 500.0f;
		float m_initH = 400.0f;
		ImGui::SetNextWindowSize(ImVec2(m_initW, m_initH), ImGuiCond_Once);
		ImGui::SetNextWindowPos(ImVec2(screenSize.x/ 2.0f - m_initW / 2.0f, screenSize.y / 2.0f - m_initH / 2.0f), ImGuiCond_Once);
		ImGuiIO& io = ImGui::GetIO();
		//ImGui::Begin(u8"节点编辑器", isOpen,             ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::SetNextWindowPos(ImVec2(0, 0));
        ImGui::SetNextWindowSize(io.DisplaySize);
        ImGui::Begin("Content", nullptr,
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove |
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoSavedSettings |
            ImGuiWindowFlags_NoBringToFrontOnFocus);
		ed::SetCurrentEditor(g_Context);
		static float leftPaneWidth  = 300.0f;
	    ShowLeftPane(leftPaneWidth - 4.0f);

	    ImGui::SameLine(0.0f, 12.0f);
	    

	    ed::Begin("My Editor");
		auto cursorTopLeft = ImGui::GetCursorScreenPos();
		util::BlueprintNodeBuilder builder(reinterpret_cast<ImTextureID>(s_HeaderBackground->handle()), s_HeaderBackground->getSize().x, s_HeaderBackground->getSize().y);;



	    int uniqueId = 1;
	    // Start drawing nodes.
		for (size_t i = 0; i < m_gameNodes.size(); i ++) 
		{
			auto node = m_gameNodes[i];

			builder.Begin(node->m_nodeID);
			if (!node->isShowed) 
			{
				ed::SetNodePosition(node->m_nodeID, ImVec2(node->m_origin.x, node->m_origin.y));
				node->isShowed = true;
			}
			auto nodeCol = node->getNodeColor();
			builder.Header(ImVec4(nodeCol.x, nodeCol.y, nodeCol.z, 1));
				ImGui::Spring(0);
                ImGui::TextUnformatted(node->name.c_str());
                ImGui::Spring(1);
                ImGui::Dummy(ImVec2(0, 28));
				ImGui::Spring(0);
			builder.EndHeader();

			
			auto inAttrList = node->getInAttrs();
			for (auto attr : inAttrList) 
			{
				builder.Input(attr->gID);
				drawPinIcon(attr, true, int(1.0 * 255));
				ImGui::TextUnformatted(attr->m_name.c_str());
				builder.EndInput();
			}

			auto outAttrList = node->getOuAttrs();
			for (auto attr : outAttrList) 
			{
				//ed::BeginPin(attr->gID, ed::PinKind::Output);
				builder.Output(attr->gID);
				// in between Begin|EndAttribute calls, you can call ImGui
				// UI functions
				ImGui::TextUnformatted(attr->m_name.c_str());
				drawPinIcon(attr, true, (int)(1.0 * 255));
				//ed::EndPin();
				builder.EndOutput();
			}
			ImGui::Spring(0);
			node->privateDraw();
			builder.End();

			//ed::EndNode();
		}

		for (auto info : m_links)
		{
			// in this case, we just use the array index of the link
			// as the unique identifier
			ImColor color(255, 255, 255);
			float thickness = 1.0f;
			switch(findAttr(info.InputId)->dataType)
			{
			case NodeAttr::DataType::EXECUTE: color = ImColor(255, 255, 255); thickness = 2.0f;break;
			case NodeAttr::DataType::DATA: color = ImColor(255, 255, 0);thickness = 1.0f;break;
			default: ;
			}
			ed::Link(info.Id, info.InputId, info.OutputId, color, thickness);
		}

 // Handle creation action, returns true if editor want to create new object (node or link)
    if (ed::BeginCreate(ImVec4(0, 1, 0, 1), 3.0f))
    {
        ed::PinId inputPinId, outputPinId;
        if (ed::QueryNewLink(&inputPinId, &outputPinId))
        {
            // QueryNewLink returns true if editor want to create new link between pins.
            //
            // Link can be created only for two valid pins, it is up to you to
            // validate if connection make sense. Editor is happy to make any.
            //
            // Link always goes from input to output. User may choose to drag
            // link from output pin or input pin. This determine which pin ids
            // are valid and which are not:
            //   * input valid, output invalid - user started to drag new ling from input pin
            //   * input invalid, output valid - user started to drag new ling from output pin
            //   * input valid, output valid   - user dragged link over other pin, can be validated

            if (inputPinId && outputPinId) // both are valid, let's accept link
            {
            	auto inAttr = findAttr(inputPinId.Get());
            	auto outAttr = findAttr(outputPinId.Get());
            	if(inputPinId == outputPinId)
            	{
            		ed::RejectNewItem(ImColor(255, 0, 0), 5.0f);
            	}
            	else if(inAttr->type == outAttr->type)
            	{
            		ed::RejectNewItem(ImColor(255, 0, 0), 5.0f);
            	}
             	else if(inAttr->dataType != outAttr->dataType)
            	{
            		ed::RejectNewItem(ImColor(255, 0, 0), 5.0f);
            	}
            	else 
				{
	                if (ed::AcceptNewItem())
	                {
                		LinkInfo info = { ++g_uniqueLinkIndex, int(inputPinId.Get()), int(outputPinId.Get()) };
                		
	                    // Since we accepted new link, lets add one to our list of links.
	                    m_links.push_back(info);

	                    // Draw new link.
	                    ed::Link(m_links.back().Id, m_links.back().InputId, m_links.back().OutputId);
                		raiseEventToNode(inputPinId.Get(), outputPinId.Get());
	                }
                }
                // ed::AcceptNewItem() return true when user release mouse button.


                // You may choose to reject connection between these nodes
                // by calling ed::RejectNewItem(). This will allow editor to give
                // visual feedback by changing link thickness and color.
            }
        }
        ed::PinId pinId = 0;
    	// when Dragging
        if (ed::QueryNewNode(&pinId))
        {
	        newLinkPin = pinId.Get();
        }
    } else {
		newLinkPin = 0;
    }
    ed::EndCreate(); // Wraps up object creation action handling.

    // Handle deletion action
    if (ed::BeginDelete())
    {
        // There may be many links marked for deletion, let's loop over them.
        ed::LinkId deletedLinkId;
        while (ed::QueryDeletedLink(&deletedLinkId))
        {
            // If you agree that link can be deleted, accept deletion.
            if (ed::AcceptDeletedItem())
            {
                for (int i = 0; i < m_links.size(); i++)
                {
                    if (m_links[i].Id == deletedLinkId.Get())
                    {
                        m_links.erase(m_links.begin() + i);
                        break;
                    }
                }
            }

            // You may reject link deletion by calling:
            // ed::RejectDeletedItem();
        }
    }
    ed::EndDelete(); // Wrap up deletion action

	ImGui::SetCursorScreenPos(cursorTopLeft);
    if (g_FirstFrame)
		ed::NavigateToContent(0.0f);
	ed::End();

	g_FirstFrame = false;
	ImGui::End();
	}

	NodeAttr* GameNodeEditor::findAttr(int attrID)
	{
		for(auto node : m_gameNodes)
		{
			auto attr = node->getAttrByGid(attrID);
			if(attr) 
			{
				return attr;
			}
		}
		return nullptr;
	}
	ImColor GetIconColor(NodeAttr::DataType type)
	{
	    switch (type)
	    {
		case NodeAttr::DataType::EXECUTE: return ImColor(255, 255, 255);
		case NodeAttr::DataType::DATA: return ImColor(255, 255, 0);
	    case NodeAttr::DataType::RETURN_VALUE: return ImColor(255, 255, 0);
		default: return ImColor(255, 255, 255);
	    }
	}
	static const int            s_PinIconSize = 24;
	void GameNodeEditor::drawPinIcon(const NodeAttr* pin, bool connected, int alpha)
	{
		ax::Drawing::IconType iconType;
		auto type = pin->dataType;
	    ImColor  color = GetIconColor(type);
	    color.Value.w = alpha / 255.0f;
	    switch (type)
	    {
          case NodeAttr::DataType::DATA:
            iconType = IconType::Circle;
            break;
	    case NodeAttr::DataType::EXECUTE:
            iconType = IconType::Flow;
            break;
        case NodeAttr::DataType::RETURN_VALUE:
            iconType = IconType::Grid;
            break;
        default:
            return;
	    }

	    ax::Widgets::Icon(ImVec2(s_PinIconSize, s_PinIconSize), iconType, connected, color, ImColor(32, 32, 32, alpha));
	}
}
