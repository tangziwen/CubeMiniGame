#include "GameNodeEditor.h"
#include "2D/imnodes.h"

namespace tzw
{
	void GameNodeEditor::drawIMGUI(bool * isOpen)
	{
		ImGui::Begin("½Úµã±à¼­Æ÷", isOpen);
		if (ImGui::Button("Key Binding")) 
		{
			static int idx = 0;
			auto node  = new GameNodeEditorNode();
			node->addOut("to");
			char a[512];
			sprintf(a, "onKeyPress#%d",idx);
			idx += 1;
			node->name = a;
			addNode(node);
		}
		imnodes::BeginNodeEditor();

		for (size_t i = 0; i < m_gameNodes.size(); i ++) 
		{
			auto node = m_gameNodes[i];
			imnodes::BeginNode(i);
			imnodes::Name(node->name.c_str());

			node->privateDraw();
			auto inAttrList = node->getInAttrs();
			for (auto attr : inAttrList) 
			{
			    imnodes::BeginInputAttribute(attr->gID);
			    ImGui::Text(attr->m_name.c_str());
				imnodes::EndAttribute();
			}

			auto outAttrList = node->getOuAttrs();
			for (auto attr : outAttrList) 
			{
				imnodes::BeginOutputAttribute(attr->gID);
				// in between Begin|EndAttribute calls, you can call ImGui
				// UI functions
				ImGui::Text(attr->m_name.c_str());
				imnodes::EndAttribute();
			}
			imnodes::EndNode();
		}

		for (int i = 0; i < m_links.size(); ++i)
		{
			const std::pair<int, int> p = m_links[i];
			// in this case, we just use the array index of the link
			// as the unique identifier
			imnodes::Link(i, p.first, p.second);
		}
		imnodes::EndNodeEditor();



		int start_attr, end_attr;
		if (imnodes::IsLinkCreated(&start_attr, &end_attr))
		{
			m_links.push_back(std::make_pair(start_attr, end_attr));
			raiseEventToNode(start_attr, end_attr);
		}
		ImGui::End();
	}

	void GameNodeEditor::addNode(GameNodeEditorNode* newNode)
	{
		m_gameNodes.push_back(newNode);
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
			const std::pair<int, int> p = *i;
			if(node->checkInNodeAttr(p.first) || node->checkOutNodeAttr(p.second))
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
		for (size_t i = 0; i < m_gameNodes.size(); i ++) 
		{
			auto node = m_gameNodes[i];
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
}
