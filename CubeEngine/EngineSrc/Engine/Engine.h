#ifndef TZW_ENGINEINFO_H
#define TZW_ENGINEINFO_H

#include "AppEntry.h"
#include <string>
#include "EngineSrc/Math/vec2.h"
#include "../BackEnd/WindowBackEndMgr.h"
namespace tzw {
class Engine
{
public:
    static Engine *shared();
    float windowWidth() const;
    void setWindowWidth(float windowWidth);
    static int run(int argc, char *argv[],AppEntry * delegate);
    float windowHeight() const;
    void setWindowHeight(float windowHeight);
    AppEntry *delegate() const;
    void setDelegate(AppEntry *delegate);
    void initSingletons();
    vec2 winSize();
	vec2 getMousePos();
    float deltaTime() const;
    void update(float delta);
    void onStart(int width, int height);
    float FPS();
    int getDrawCallCount();
    void setDrawCallCount(int drawCallCount);
    void resetDrawCallCount();
    void increaseVerticesIndicesCount(int v,int i);
    void resetVerticesIndicesCount();
    std::string getUserPath(std::string path);
    int getApplyRenderTime() const;
    int getLogicUpdateTime() const;
    int getIndicesCount() const;
    int getVerticesCount() const;
    bool getIsEnableOutLine() const;
    void setIsEnableOutLine(bool isEnableOutLine);
	void setClearColor(float r, float g, float b);
	void setUnlimitedCursor(bool enable);
	std::string getWorkingDirectory();
	int getMouseButton(int mouseButton);
private:
	WindowBackEnd * m_winBackEnd;
    static Engine * m_instance;
    int m_drawCallCount;
    int m_verticesCount;
    int m_indicesCount;
    int m_logicUpdateTime;
    int m_applyRenderTime;
    Engine();
    float m_deltaTime;
    float m_windowWidth;
    float m_windowHeight;
    bool m_isEnableOutLine;
    AppEntry * m_delegate;
};

} // namespace tzw

#endif // TZW_ENGINEINFO_H
