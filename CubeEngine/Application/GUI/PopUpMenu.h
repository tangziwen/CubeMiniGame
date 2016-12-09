#ifndef POP_UP_MENU_H
#define POP_UP_MENU_H
#include "Interface/Drawable2D.h"
#include "2D/Button.h"
#include <functional>
#include <string>
#include <vector>
namespace tzwS{
	class PopUpMenu : public tzw::Drawable2D
	{
	public:
		PopUpMenu();
		void insert(std::string theStr, const std::function<void (tzw::Button *)> & theCallBack);
		void finish();
	private:
		tzw::GUIFrame * m_frameBG;
		std::vector<tzw::Button * > m_btnList;
	};
}

#endif
