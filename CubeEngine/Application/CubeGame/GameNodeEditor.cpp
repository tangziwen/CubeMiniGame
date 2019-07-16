#include "GameNodeEditor.h"
#include "2D/imnodes.h"

namespace tzw
{
	void GameNodeEditor::drawIMGUI()
	{
		imnodes::BeginNodeEditor();

		imnodes::BeginNode(2);
		imnodes::Name("output node");
	    imnodes::BeginInputAttribute(3);
	    ImGui::Text("input2");
		imnodes::EndAttribute();
		const int output_attr_id = 2;
		imnodes::BeginOutputAttribute(output_attr_id);
		// in between Begin|EndAttribute calls, you can call ImGui
		// UI functions
		ImGui::Text("output pin");
		imnodes::EndAttribute();
		imnodes::EndNode();

		imnodes::BeginNode(3);
		imnodes::Name("output node");
	    imnodes::BeginInputAttribute(5);
	    ImGui::Text("input2");
		imnodes::EndAttribute();

		imnodes::BeginInputAttribute(6);
	    ImGui::Text("input2");
		imnodes::EndAttribute();

		imnodes::BeginOutputAttribute(4);
		// in between Begin|EndAttribute calls, you can call ImGui
		// UI functions
		ImGui::Text("output pin");
		imnodes::EndAttribute();
		imnodes::EndNode();

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
	}

}
