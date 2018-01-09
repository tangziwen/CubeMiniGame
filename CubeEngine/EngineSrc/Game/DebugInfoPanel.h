#ifndef TZW_DEBUGINFOPANEL_H
#define TZW_DEBUGINFOPANEL_H

namespace tzw {
class DebugInfoPanel
{
public:
    DebugInfoPanel();
	void drawIMGUI(bool * isOpen);
    void setInfo();
	void updateInfo();
private:
	float currFPS;
	int drawCall;
	int logicUpdateTime;
	int renderUpdateTime;
	int verticesCount;
};

} // namespace tzw

#endif // TZW_DEBUGINFOPANEL_H
