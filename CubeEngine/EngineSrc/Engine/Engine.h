#ifndef TZW_ENGINEINFO_H
#define TZW_ENGINEINFO_H

#include "AppEntry.h"
#include <string>
#include "EngineSrc/Math/vec2.h"
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
    float deltaTime() const;
    void update(float delta);
    void onStart(int width, int height);
    float FPS();
    int getDrawCallCount();
    void increaseDrawCallCount();
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
private:
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
