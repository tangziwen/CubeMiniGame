#pragma once
#include "2D/GUISystem.h"

namespace tzw {



class GameNodeEditor : public IMGUIObject
{
public:
  // Í¨¹ý IMGUIObject ¼Ì³Ð
  void drawIMGUI() override;

protected:
  std::vector<std::pair<int, int>> m_links;
};
}
