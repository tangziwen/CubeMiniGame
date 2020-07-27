#ifndef TZW_ENGINEINFO_H
#define TZW_ENGINEINFO_H

#include "AppEntry.h"
#include <string>
#include "EngineSrc/Math/vec2.h"
#include "../BackEnd/WindowBackEndMgr.h"

namespace tzw {
class RenderBackEndBase;
class Engine
{
public:
    static Engine *shared();
    float windowWidth() const;
    void setWindowSize(float w, float h);
    static int run(int argc, char *argv[],AppEntry * delegate);
    float windowHeight() const;
    AppEntry *delegate() const;
    void setDelegate(AppEntry *delegate);
    void initSingletons();
    vec2 winSize();
	vec2 getMousePos();
    float deltaTime() const;
    void update(float delta);
    void onStart();
    float FPS();
    int getDrawCallCount();
    void setDrawCallCount(int drawCallCount);
    void resetDrawCallCount();
    void increaseVerticesIndicesCount(int v,int i);
    void resetVerticesIndicesCount();
    std::string getFilePath(std::string path);
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
	void loadConfig();
	void saveConfig();
	void changeScreenSetting(int w, int h, bool isFullScreen);
    RenderBackEndBase * getRenderBackEnd();
    void setRenderBackEnd(RenderBackEndBase * backend);
	bool isIsFullScreen() const;
	void setIsFullScreen(const bool isFullScreen);
    void setRenderDeviceType(RenderDeviceType newType);
    RenderDeviceType getRenderDeviceType();
private:
    RenderDeviceType m_type;
	WindowBackEnd * m_winBackEnd{};
    static Engine * m_instance;
    int m_drawCallCount{};
    int m_verticesCount{};
    int m_indicesCount{};
    int m_logicUpdateTime{};
    int m_applyRenderTime{};
    Engine();
    float m_deltaTime{};
    float m_windowWidth{};
    float m_windowHeight{};
	bool m_isFullScreen;
    RenderBackEndBase * m_renderBackEnd;
    bool m_isEnableOutLine;
    AppEntry * m_delegate{};
};

} // namespace tzw

#endif // TZW_ENGINEINFO_H
