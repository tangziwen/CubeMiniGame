#ifndef TZW_CONSOLEPANEL_H
#define TZW_CONSOLEPANEL_H


#include "Engine/EngineDef.h"
#include "2D/imgui.h"
namespace tzw {

///用于输出调试信息，以及输入脚本的控制台窗口
struct ConsolePanel
{
	TZW_SINGLETON_DECL(ConsolePanel);
	char                  InputBuf[256];
	ImVector<char*>       Items;
	bool                  ScrollToBottom;
	ImVector<char*>       History;
	int                   HistoryPos;    // -1: new line, 0..History.Size-1 browsing history.
	ImVector<const char*> Commands;

	ConsolePanel();
	~ConsolePanel();

	// Portable helpers
	static int   Stricmp(const char* str1, const char* str2);
	static int   Strnicmp(const char* str1, const char* str2, int n);
	static char* Strdup(const char *str);

	void    ClearLog();

	void    AddLog(const char* fmt, ...) IM_FMTARGS(2);

	void    AddLog(const char* fmt, va_list argList);

	void    Draw(const char* title, bool* p_open);

	void    ExecCommand(const char* command_line);

	static int TextEditCallbackStub(ImGuiTextEditCallbackData* data); // In C++11 you are better off using lambdas for this sort of forwarding callbacks

	int     TextEditCallback(ImGuiTextEditCallbackData* data);
};

} // namespace tzw

#endif // TZW_CONSOLEPANEL_H
