#ifndef TZW_GUILISTVIEWITEM_H
#define TZW_GUILISTVIEWITEM_H

#include <functional>
#include <string>
namespace tzw {

class GUIListViewItem
{
public:
    GUIListViewItem();
    std::string m_str;
    std::function<void(GUIListViewItem *)> m_cb;
};

} // namespace tzw

#endif // TZW_GUILISTVIEWITEM_H
