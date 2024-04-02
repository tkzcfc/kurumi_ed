#pragma once

#include "axmol.h"
#include "cocostudio/CocoStudio.h"
#include "type/StringArray.h"
#include "imgui.h"
#include "lua_function/LuaFunction.h"

using namespace ax;
using namespace cocostudio;

class Tools
{
public:
	 
	static std::wstring stringToWString(const std::string& InStr);

	static std::string wstringToString(const std::wstring& InStr);

	static bool openFileMultiSelect(const std::string& InFilter, StringArray* OutPathArr);

	static std::string GbkToUtf8(const std::string& strGbk);

	static std::string Utf8ToGbk(const std::string& strUtf8);

	static std::wstring GbkToUnicode(const std::string& strGbk);

	static std::string UnicodeToGbk(const std::wstring& strUnicode);

	static std::string getFilename(const std::string& InPath);

	static bool isInRect(Node* InNode, const float InX, const float InY);

	// 毫秒
	static long getMillisecond();

	static bool copyFile(const std::string& existingFileName, const std::string& newFileName, bool bFailIfExists);

	static std::string replaceString(const std::string& str, const std::string& targetstr, const std::string& replacestr);

	static void runExe(std::string exe, std::string cmd);

	static PROCESS_INFORMATION* s_ProcessInfomation;

	static bool imgui_inputText(const char* label, std::string& buf, unsigned int buf_size, unsigned int flags = 0);

	static std::vector<std::string> getLogicalDriveStrings();

	static std::string UTS(const std::string& str);

	static std::string STU(const std::string& str);

	static int32_t bor_int32(int32_t a, int32_t b);

	static uint32_t bor_uint32(uint32_t a, uint32_t b);


	// imgui

	static bool          BeginTabItem_NoClose(const char* label, ImGuiTabItemFlags flags = 0);

	static ImVec2 getMouseClickedPos(int index);

	static bool BeginWindow_NoClose(const char* name, ImGuiWindowFlags flags);

	static bool check_AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags = 0);

	static std::string AcceptDragDropPayload(const char* type, ImGuiDragDropFlags flags = 0);

	static const ImVec4& getStyleColor(int type);

	static bool copyFile(const std::string& srcFile, const std::string& dstFile);

	static void enumerateChildren(ax::Node* node, const std::string& name, LuaFunction& handle);

	static std::string prettyJson(const char* json);

	static int imguiComboUserdata(const char* label, int current_item);

	static bool imguiComboBool(const char* label, bool current_item);

	static int imguiCombo_OneLiner(const char* name, int curIndex, const char* items);

	static void helpMarker(const char* desc);

	static std::vector<std::string> splitPlist(const std::string& plist);

	static std::vector<std::string> getArematureMovementNames(cocostudio::Armature* armature);

	static std::vector<int> getArematureMovementDurations(cocostudio::Armature* armature);

	static double gettime();

	static void setImgUIGlobalScale(float scale);

	static float getDPI();

	static float getImgUIGlobalScale() { return imgui_ui_scle; }

	static void relaunch();

	static bool onGUIFontList();

	static bool setImguiFontName(const char* fontName);

	static std::string getImguiFontName();

public:

	static std::map<std::string, std::vector<std::string> > cache_plistSubs;


	static ImGuiStyle cacheStyle;
	static bool initCacheStyle;
	static float imgui_ui_scle;
};
