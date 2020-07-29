#include "Engine.h"
#include "../Scene/SceneMgr.h"
#include "../Texture/TextureMgr.h"
#include "../Rendering/Renderer.h"
#include "../Game/ConsolePanel.h"
#include "../BackEnd/RenderBackEndBase.h"
#include "../Event/EventMgr.h"
#include "time.h"
#include "EngineSrc/3D/Effect/EffectMgr.h"
#include "WorkerThreadSystem.h"
#include "AudioSystem/AudioSystem.h"
#include <windows.h>
#include "ScriptPy/ScriptPyMgr.h"
#include "2D/GUISystem.h"
#include <rapidjson/rapidjson.h>
#define CLOCKS_TO_MS(c) int((c * 1.0f)/CLOCKS_PER_SEC * 1000 + 0.5f)
#include "Collision/PhysicsMgr.h"
#include "Utility/log/Log.h"
#include "BackEnd/RenderBackEnd.h"
#include "Utility/file/Tfile.h"
#include "rapidjson/document.h"


#include "Base/uuid4.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/prettywriter.h"
#include "Base/TimerMgr.h"
#include "DebugSystem.h"
#include "BackEnd/VkRenderBackEnd.h"

namespace tzw {

Engine * Engine::m_instance = nullptr;

Engine *Engine::shared()
{
    if(!m_instance)
    {
        m_instance = new Engine();
    }
    return m_instance;
}

Engine::Engine(): m_winBackEnd(nullptr)
	{
		m_isEnableOutLine = false;
	}

bool Engine::isIsFullScreen() const
{
	return m_isFullScreen;
}

void Engine::setIsFullScreen(const bool isFullScreen)
{
	m_isFullScreen = isFullScreen;
	m_winBackEnd->setIsFullScreen(m_isFullScreen);
}

void Engine::setRenderDeviceType(RenderDeviceType newType)
{
	m_type = newType;
}

RenderDeviceType Engine::getRenderDeviceType()
{
	return m_type;
}

bool Engine::getIsEnableOutLine() const
{
    return m_isEnableOutLine;
}

void Engine::setIsEnableOutLine(bool isEnableOutLine)
{
	m_isEnableOutLine = isEnableOutLine;
}

void Engine::setClearColor(float r, float g, float b)
{
	RenderBackEnd::shared()->setClearColor(r, g, b);
}

void Engine::setUnlimitedCursor(bool enable)
{
	m_winBackEnd->setUnlimitedCursor(enable);
}

std::string Engine::getWorkingDirectory()
{
	int length = 32;
	char str[32];
	GetCurrentDirectory(length,str);
	return str;
}

int Engine::getMouseButton(int mouseButton)
{
	return m_winBackEnd->getMouseButton(mouseButton);
}

void Engine::loadConfig()
{
	rapidjson::Document doc;

	auto data = Tfile::shared()->getData("config.json",true);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	if (doc.HasParseError())
	{
			tlog("[error] get json data err! %s %d offset %d",
				"config.json",
				doc.GetParseError(),
				doc.GetErrorOffset());
		return;
	}
	auto width = doc["width"].GetInt();
	auto height = doc["height"].GetInt();
	m_windowWidth = width;
	m_windowHeight = height;
	auto enable3D = doc["3DEnable"].GetBool();
	auto enable2D = doc["2DEnable"].GetBool();
	Renderer::shared()->setEnable3DRender(enable3D);
	Renderer::shared()->setEnableGUIRender(enable2D);
	Renderer::shared()->setSkyEnable(doc["SkyEnable"].GetBool());
	Renderer::shared()->setFogEnable(doc["FogEnable"].GetBool());
	Renderer::shared()->setSsaoEnable(doc["SSAOEnable"].GetBool());
	Renderer::shared()->setBloomEnable(doc["BloomEnable"].GetBool());
	Renderer::shared()->setHdrEnable(doc["HDREnable"].GetBool());
	Renderer::shared()->setAaEnable(doc["AAEnable"].GetBool());
	Renderer::shared()->setShadowEnable(doc["ShadowEnable"].GetBool());
	RenderBackEnd::shared()->setIsCheckGL(doc["IsGraphicsDebugCheck"].GetBool());

	m_isFullScreen = doc["IsFullScreen"].GetBool();

	TranslationMgr::shared()->load(doc["Language"].GetString()); 
}

void Engine::saveConfig()
{
	rapidjson::Document doc;

	auto data = Tfile::shared()->getData("config.json",true);
	doc.Parse<rapidjson::kParseDefaultFlags>(data.getString().c_str());
	if (doc.HasParseError())
	{
			tlog("[error] get json data err! %s %d offset %d",
				"config.json",
				doc.GetParseError(),
				doc.GetErrorOffset());
		return;
	}
	doc["width"].SetInt(m_windowWidth);
	doc["height"].SetInt(m_windowHeight);


	doc["3DEnable"].SetBool(Renderer::shared()->enable3DRender());
	doc["2DEnable"].SetBool(Renderer::shared()->enableGUIRender());

	doc["SkyEnable"].SetBool(Renderer::shared()->isSkyEnable());
	doc["FogEnable"].SetBool(Renderer::shared()->isFogEnable());
	doc["SSAOEnable"].SetBool(Renderer::shared()->isSsaoEnable());
	doc["BloomEnable"].SetBool(Renderer::shared()->isBloomEnable());
	doc["HDREnable"].SetBool(Renderer::shared()->isHdrEnable());
	doc["AAEnable"].SetBool(Renderer::shared()->isAaEnable());
	doc["ShadowEnable"].SetBool(Renderer::shared()->isShadowEnable());
	doc["IsGraphicsDebugCheck"].SetBool(RenderBackEnd::shared()->isCheckGL());
	
	doc["IsFullScreen"].SetBool(m_isFullScreen);

	auto thestr = TranslationMgr::shared()->getCurrLanguage();
	doc["Language"].SetString(rapidjson::StringRef(thestr));
	rapidjson::StringBuffer buffer;
	auto file = fopen("config.json", "w");
	char writeBuffer[65536];
	rapidjson::FileWriteStream stream(file, writeBuffer, sizeof(writeBuffer));
	rapidjson::PrettyWriter<rapidjson::FileWriteStream> writer(stream);
	writer.SetIndent('\t', 1);
	doc.Accept(writer);
	fclose(file);
}

void Engine::changeScreenSetting(int w, int h, bool isFullScreen)
{
	m_winBackEnd->changeScreenSetting(w, h, isFullScreen);
	m_windowHeight = h;
	m_windowWidth = w;
	m_isFullScreen = isFullScreen;
	Renderer::shared()->onChangeScreenSize(w, h);
}

RenderBackEndBase* Engine::getRenderBackEnd()
{
	return m_renderBackEnd;
}

void Engine::setRenderBackEnd(RenderBackEndBase* backend)
{
	m_renderBackEnd = backend;
}

int Engine::getDrawCallCount()
{
    return m_drawCallCount;
}

void Engine::setDrawCallCount(int drawCallCount)
{
    m_drawCallCount = drawCallCount;
}

void Engine::resetDrawCallCount()
{
    m_drawCallCount = 0;
}

void Engine::increaseVerticesIndicesCount(int v, int i)
{
    m_verticesCount += v;
    m_indicesCount += i;
}

void Engine::resetVerticesIndicesCount()
{
    m_verticesCount = 0;
    m_indicesCount = 0;
}

std::string Engine::getFilePath(std::string path)
{
    return "./Res/" + path;
}

int Engine::getApplyRenderTime() const
{
    return m_applyRenderTime;
}

int Engine::getLogicUpdateTime() const
{
    return m_logicUpdateTime;
}

int Engine::getIndicesCount() const
{
    return m_indicesCount;
}

int Engine::getVerticesCount() const
{
    return m_verticesCount;
}


float Engine::deltaTime() const
{
    return m_deltaTime;
}


void Engine::update(float delta)
{
    m_deltaTime = delta;
    int logicBefore = clock();
	DebugSystem::shared()->handleDraw(delta);
	PhysicsMgr::shared()->stepSimulation(delta);
	TimerMgr::shared()->handle(delta);
	WorkerThreadSystem::shared()->mainThreadUpdate();
	EventMgr::shared()->apply(delta);
    shared()->delegate()->onUpdate(delta);
    SceneMgr::shared()->doVisit();
	resetDrawCallCount();
    m_logicUpdateTime = CLOCKS_TO_MS(clock() - logicBefore);
    int applyRenderBefore = clock();
	resetVerticesIndicesCount();
	if(m_type == RenderDeviceType::OpenGl_Device)
	{
		DebugSystem::shared()->doRender(delta);
		Renderer::shared()->renderAll();
	
	}else
	{
		VKRenderBackEnd::shared()->RenderScene();
	}
	AudioSystem::shared()->update();
	
    m_applyRenderTime = CLOCKS_TO_MS(clock() - applyRenderBefore);

	
}

void Engine::onStart()
{
	initLogSystem();
	tlog("Cube-Engine By tzw%s", EngineDef::versionStr);
	RenderBackEnd::shared()->printFullDeviceInfo();
    Engine::shared()->initSingletons();
    Engine::shared()->delegate()->onStart();
	ScriptPyMgr::shared()->doScriptInit();
	if(m_type == RenderDeviceType::OpenGl_Device){
		GUISystem::shared()->initGUI();
		Renderer::shared()->init();
	}

	AudioSystem::shared()->init();
	uuid4_init();
	//WorkerThreadSystem::shared()->init();
}

float Engine::FPS()
{
    return 1.0f/deltaTime();
}

AppEntry *Engine::delegate() const
{
    return m_delegate;
}

void Engine::setDelegate(AppEntry *delegate)
{
    m_delegate = delegate;
}

void Engine::initSingletons()
{
    SceneMgr::shared()->init();
    TextureMgr::shared();
    Renderer::shared();
    EffectMgr::shared();
	ScriptPyMgr::shared()->init();
}

vec2 Engine::winSize()
{
    return vec2(m_windowWidth,m_windowHeight);
}

tzw::vec2 Engine::getMousePos()
{
	double x, y;
	m_winBackEnd->getMousePos(&x, &y);
	return vec2(x, y);
}

float Engine::windowHeight() const
{
    return m_windowHeight;
}

void Engine::setWindowSize(float w, float h)
{
    m_windowHeight = h;
	m_windowWidth = w;
	m_winBackEnd->setWinSize(w, h);
}

float Engine::windowWidth() const
{
    return m_windowWidth;
}

int Engine::run(int argc, char *argv[], AppEntry * delegate)
{
    shared()->setDelegate(delegate);
	Tfile::shared()->addSearchPath("./Asset/");
	Tfile::shared()->addSearchPath("./");
	Tfile::shared()->addSearchZip("Asset.pkg");
	shared()->loadConfig();
	shared()->m_winBackEnd = WindowBackEndMgr::shared()->getWindowBackEnd(TZW_WINDOW_GLFW);
	shared()->m_winBackEnd->prepare(shared()->windowWidth(), shared()->windowHeight(), shared()->m_isFullScreen);
	shared()->m_winBackEnd->run();
    return 0;
}
} // namespace tzw

