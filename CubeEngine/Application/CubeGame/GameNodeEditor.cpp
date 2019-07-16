#include "GameNodeEditor.h"
#include "2D/imnodes.h"

namespace tzw
{
	void GameNodeEditor::drawIMGUI(bool * isOpen)
	{
		ImGui::Begin("Node Editor", isOpen);
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
		}
		ImGui::End();
	}

	void GameNodeEditor::addNode(GameNodeEditorNode* newNode)
	{
		m_gameNodes.push_back(newNode);
	}
}
