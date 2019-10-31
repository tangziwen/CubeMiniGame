#include "GUISystem.h"
#include "imgui.h"
#define IMGUI_DEFINE_MATH_OPERATORS
#include "GL/glew.h"
#include "Engine/Engine.h"
#include "Shader/ShaderMgr.h"
#include "Rendering/RenderBuffer.h"
#include "Math/Matrix44.h"
#include "Event/EventMgr.h"
#include "imnodes.h"
#include "ScriptPy/ScriptPyMgr.h"
#include "BackEnd/RenderBackEnd.h"

namespace tzw
{
	ShaderProgram * g_imguiShader;
	RenderBuffer * m_arrayBuf;
	RenderBuffer * m_indexBuf;
	static bool g_MouseJustPressed[3] = { false, false, false };
	TZW_SINGLETON_IMPL(GUISystem);
	static GLuint       g_FontTexture = 0;
	// This is the main rendering function that you have to implement and provide to ImGui (via setting up 'RenderDrawListsFn' in the ImGuiIO structure)
	void ImGui_ImplGlfwGL2_RenderDrawLists(ImDrawData* draw_data)
		// Note that this implementation is little overcomplicated because we are saving/setting up/restoring every OpenGL state explicitly, in order to be able to run within any OpenGL engine that doesn't do so. 
		// If text or lines are blurry when integrating ImGui in your engine: in your Render function, try translating your projection matrix by (0.5f,0.5f) or (0.375f,0.375f)
	{
		int errorCode = 0;
		// Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
		ImGuiIO& io = ImGui::GetIO();
		auto fb_width = static_cast<int>(io.DisplaySize.x * io.DisplayFramebufferScale.x);
		auto fb_height = static_cast<int>(io.DisplaySize.y * io.DisplayFramebufferScale.y);
		if (fb_width == 0 || fb_height == 0)
			return;
		draw_data->ScaleClipRects(io.DisplayFramebufferScale);
		// We are using the OpenGL fixed pipeline to make the example code simpler to read!
		// Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, vertex/texcoord/color pointers, polygon fill.
		GLint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		GLint last_polygon_mode[2]; glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode);
		GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
		GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
		 
		glEnable(GL_BLEND);
		glEnable(GL_SCISSOR_TEST);
		RenderBackEnd::shared()->setDepthTestEnable(false);
		RenderBackEnd::shared()->setBlendFactor(RenderFlag::BlendingFactor::SrcAlpha, RenderFlag::BlendingFactor::OneMinusSrcAlpha);
		//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
		glDisable(GL_CULL_FACE);
		//glDisable(GL_DEPTH_TEST);

		//// Setup viewport, orthographic projection matrix
		glViewport(0, 0, static_cast<int>(io.DisplaySize.x), static_cast<int>(io.DisplaySize.y));
		Matrix44 projection;
		projection.ortho(0.0f, io.DisplaySize.x, io.DisplaySize.y, 0.0f, 0.1f, 10.0f);
		g_imguiShader->use();
		 
		g_imguiShader->setUniformMat4v("TU_projMat", projection.data());
		 
		g_imguiShader->setUniformInteger("TU_tex1", 0);
		 
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		 
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		 
		// Render command lists
#define OFFSETOF(TYPE, ELEMENT) ((size_t)&(((TYPE *)0)->ELEMENT))
		for (int n = 0; n < draw_data->CmdListsCount; n++)
		{

			const ImDrawList* cmd_list = draw_data->CmdLists[n];
			const ImDrawVert* vtx_buffer = cmd_list->VtxBuffer.Data;
			const ImDrawIdx* idx_buffer = cmd_list->IdxBuffer.Data;
			 
			m_arrayBuf->use();
			m_arrayBuf->allocate((void *)vtx_buffer, cmd_list->VtxBuffer.size() * sizeof(ImDrawVert));


			 

			g_imguiShader->use();
			int vertexLocation = g_imguiShader->attributeLocation("a_position");
			g_imguiShader->enableAttributeArray(vertexLocation);
			g_imguiShader->setAttributeBuffer(vertexLocation, GL_FLOAT, (int)(OFFSETOF(ImDrawVert, pos)), 2, sizeof(ImDrawVert));
			 

			int texcoordLocation = g_imguiShader->attributeLocation("a_texcoord");
			g_imguiShader->enableAttributeArray(texcoordLocation);
			g_imguiShader->setAttributeBuffer(texcoordLocation, GL_FLOAT, (int)(OFFSETOF(ImDrawVert, uv)), 2, sizeof(ImDrawVert));
			 

			int colorLocation = g_imguiShader->attributeLocation("a_color");
			g_imguiShader->enableAttributeArray(colorLocation);
			g_imguiShader->setAttributeBuffer(colorLocation, GL_UNSIGNED_BYTE, (int)(OFFSETOF(ImDrawVert, col)), 4, sizeof(ImDrawVert));
			 

			for (int cmd_i = 0; cmd_i < cmd_list->CmdBuffer.Size; cmd_i++)
			{


				glDisable(GL_CULL_FACE);
				const ImDrawCmd* pcmd = &cmd_list->CmdBuffer[cmd_i];
				if (pcmd->UserCallback)
				{
					pcmd->UserCallback(cmd_list, pcmd);
				}
				else
				{
					m_indexBuf->use();
					m_indexBuf->allocate((void *)idx_buffer, (GLsizei)pcmd->ElemCount * sizeof(ImDrawIdx));
					glActiveTexture(GL_TEXTURE0);
					glBindTexture(GL_TEXTURE_2D, (GLuint)(intptr_t)pcmd->TextureId);
					glScissor((int)pcmd->ClipRect.x, (int)(fb_height - pcmd->ClipRect.w), (int)(pcmd->ClipRect.z - pcmd->ClipRect.x), (int)(pcmd->ClipRect.w - pcmd->ClipRect.y));
					glDrawElements(GL_TRIANGLES, (GLsizei)pcmd->ElemCount, sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, 0);
				}
				idx_buffer += pcmd->ElemCount;
			}
		}
#undef OFFSETOF

		// Restore modified state
		glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(last_texture));
		//glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
		glScissor(last_scissor_box[0], last_scissor_box[1], static_cast<GLsizei>(last_scissor_box[2]), static_cast<GLsizei>(last_scissor_box[3]));
	}

	GUISystem::GUISystem()
	{
		
	}

	void GUISystem::renderData()
	{
		if (!m_isInit) return;
		 

		for (auto obj : m_objList)
		{
			obj->drawIMGUI();
		}
		ScriptPyMgr::shared()->doScriptUIUpdate();
		ImGui::Render();
	}

	bool GUISystem::ImGui_ImplGlfwGL2_CreateDeviceObjects()
	{
		// Build texture atlas
		ImGuiIO& io = ImGui::GetIO();
		unsigned char* pixels;
		int width, height;
		io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bits (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.
																  // Upload texture to graphics system
		GLint last_texture;
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
		glGenTextures(1, &g_FontTexture);
		glBindTexture(GL_TEXTURE_2D, g_FontTexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

		// Store our identifier
		io.Fonts->TexID = reinterpret_cast<void *>(static_cast<intptr_t>(g_FontTexture));

		// Restore state
		glBindTexture(GL_TEXTURE_2D, last_texture);

		return true;
	}


	bool GUISystem::onMouseRelease(int button, vec2 pos)
	{
		return false;
	}

	bool GUISystem::onMousePress(int button, vec2 pos)
	{
		g_MouseJustPressed[button] = true;
		return false;
	}

	void GUISystem::addObject(IMGUIObject * obj)
	{
		m_objList.push_back(obj);
	}

	bool GUISystem::onCharInput(unsigned int c)
	{
		ImGuiIO& io = ImGui::GetIO();
		if (c > 0 && c < 0x10000)
			io.AddInputCharacter(static_cast<unsigned short>(c));
		return false;
	}

	bool GUISystem::onScroll(vec2 offset)
	{
	    ImGuiIO& io = ImGui::GetIO();
	    io.MouseWheelH += offset.x;
	    io.MouseWheel += offset.y;
		return false;
	}

	void GUISystem::renderIMGUI()
	{
		auto& io = ImGui::GetIO();
		io.DeltaTime = Engine::shared()->deltaTime();
		
		NewFrame();
		
		renderData();
		
	}

	void GUISystem::initGUI()
	{
		EventMgr::shared()->addFixedPiorityListener(this);
		g_imguiShader = ShaderMgr::shared()->getByPath("./Res/Shaders/IMGUI_v.glsl", "./Res/Shaders/IMGUI_f.glsl");
		m_arrayBuf = new RenderBuffer(RenderBuffer::Type::VERTEX);
		m_arrayBuf->create();
		m_indexBuf = new RenderBuffer(RenderBuffer::Type::INDEX);
		m_indexBuf->create();

		ImGui::CreateContext();
		imnodes::Initialize();
		ImGuiIO& io = ImGui::GetIO();
		io.KeyMap[ImGuiKey_Tab] = TZW_KEY_TAB;                     // Keyboard mapping. ImGui will use those indices to peek into the io.KeyDown[] array.
		io.KeyMap[ImGuiKey_LeftArrow] = TZW_KEY_LEFT;
		io.KeyMap[ImGuiKey_RightArrow] = TZW_KEY_RIGHT;
		io.KeyMap[ImGuiKey_UpArrow] = TZW_KEY_UP;
		io.KeyMap[ImGuiKey_DownArrow] = TZW_KEY_DOWN;
		io.KeyMap[ImGuiKey_PageUp] = TZW_KEY_PAGE_UP;
		io.KeyMap[ImGuiKey_PageDown] = TZW_KEY_PAGE_DOWN;
		io.KeyMap[ImGuiKey_Home] = TZW_KEY_HOME;
		io.KeyMap[ImGuiKey_End] = TZW_KEY_END;
		io.KeyMap[ImGuiKey_Delete] = TZW_KEY_DELETE;
		io.KeyMap[ImGuiKey_Backspace] = TZW_KEY_BACKSPACE;
		io.KeyMap[ImGuiKey_Enter] = TZW_KEY_ENTER;
		io.KeyMap[ImGuiKey_Escape] = TZW_KEY_ESCAPE;
		io.KeyMap[ImGuiKey_A] = TZW_KEY_A;
		io.KeyMap[ImGuiKey_C] = TZW_KEY_C;
		io.KeyMap[ImGuiKey_V] = TZW_KEY_V;
		io.KeyMap[ImGuiKey_X] = TZW_KEY_X;
		io.KeyMap[ImGuiKey_Y] = TZW_KEY_Y;
		io.KeyMap[ImGuiKey_Z] = TZW_KEY_Z;
		//ImGui::StyleColorsClassic();
		io.RenderDrawListsFn = ImGui_ImplGlfwGL2_RenderDrawLists;
		ImGui::StyleColorsDark();
		ImGuiStyle& style = ImGui::GetStyle();
		style.WindowBorderSize = 2.0f;
		style.FrameBorderSize = 2.0f;
		style.FrameRounding = 6.0f;
		style.GrabRounding = style.FrameRounding;

		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them. 
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple. 
		// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Read 'extra_fonts/README.txt' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("./Res/font/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("./Res/font/Cousine-Regular.ttf", 15.0f);
		//io.Fonts->AddFontFromFileTTF("./Res/font/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("./Res/font/ProggyTiny.ttf", 10.0f);

		ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\simhei.ttf", 14.0f, NULL, io.Fonts->GetGlyphRangesChineseFull());
		IM_ASSERT(font != NULL);
		m_isInit = true;
	}

	bool GUISystem::onKeyPress(int keyCode)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[keyCode] = true;

		io.KeyCtrl = io.KeysDown[TZW_KEY_LEFT_CONTROL] || io.KeysDown[TZW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[TZW_KEY_LEFT_SHIFT] || io.KeysDown[TZW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[TZW_KEY_LEFT_ALT] || io.KeysDown[TZW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[TZW_KEY_LEFT_SUPER] || io.KeysDown[TZW_KEY_RIGHT_SUPER];
		return false;
	}

	bool GUISystem::onKeyRelease(int keyCode)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.KeysDown[keyCode] = false;

		io.KeyCtrl = io.KeysDown[TZW_KEY_LEFT_CONTROL] || io.KeysDown[TZW_KEY_RIGHT_CONTROL];
		io.KeyShift = io.KeysDown[TZW_KEY_LEFT_SHIFT] || io.KeysDown[TZW_KEY_RIGHT_SHIFT];
		io.KeyAlt = io.KeysDown[TZW_KEY_LEFT_ALT] || io.KeysDown[TZW_KEY_RIGHT_ALT];
		io.KeySuper = io.KeysDown[TZW_KEY_LEFT_SUPER] || io.KeysDown[TZW_KEY_RIGHT_SUPER];
		return false;
	}

	void GUISystem::NewFrame()
	{
		if (!m_isInit) return;
		if (!g_FontTexture)
		{
			ImGui_ImplGlfwGL2_CreateDeviceObjects();
		}
		auto& io = ImGui::GetIO();
		auto h = Engine::shared()->windowHeight();
		auto w = Engine::shared()->windowWidth();

		io.DisplaySize = ImVec2(w, h);
		io.DisplayFramebufferScale = ImVec2(1.0, 1.0);
		auto mousePos = Engine::shared()->getMousePos();
		io.MousePos = ImVec2(mousePos.x, mousePos.y);

		for (int i = 0; i < 3; i++)
		{
		    // If a mouse press event came, always pass it as "mouse held this frame", so we don't miss click-release events that are shorter than 1 frame.
		    io.MouseDown[i] = g_MouseJustPressed[i] || Engine::shared()->getMouseButton(i) != 0;
		    g_MouseJustPressed[i] = false;
		}
		ImGui::NewFrame();
	}

}
