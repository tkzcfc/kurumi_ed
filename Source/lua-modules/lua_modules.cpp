
#include "lua_modules.h"

#if __cplusplus
extern "C" {
#endif

//lfs
#include "lfs/lfs.h"

	extern int luaopen_subprocess(lua_State *L);
	extern int luaopen_cjson_safe(lua_State *l);

#if __cplusplus
} /// extern "C"
#endif

#include "imgui_lua/imgui_tolua.h"
#include "game/lua_game.h"

#include "axmol.h"
#include "ImGuiPresenter.h"

using namespace ax;

namespace Lua_Print
{
int get_string_for_print(lua_State* L, std::string* out)
{
    int n = lua_gettop(L); /* number of arguments */
    int i;

    lua_getglobal(L, "tostring");
    for (i = 1; i <= n; i++)
    {
        const char* s;
        lua_pushvalue(L, -1); /* function to be called */
        lua_pushvalue(L, i);  /* value to print */
        lua_call(L, 1, 1);
        size_t sz;
        s = lua_tolstring(L, -1, &sz); /* get result */
        if (s == NULL)
            return luaL_error(L, "'tostring' must return a string to 'print'");
        if (i > 1)
            out->append("\t");
        out->append(s, sz);
        lua_pop(L, 1); /* pop result */
    }
    return 0;
}

int lua_print(lua_State* L)
{
    std::string t;
    get_string_for_print(L, &t);
    AXLOGD("[LUA-print] {}", t);
    t.append("\n");
    lua_pushstring(L, t.c_str());
    return 1;
}

int lua_release_print(lua_State* L)
{
    std::string t;
    get_string_for_print(L, &t);
    AXLOG_WITH_LEVEL(ax::LogLevel::Silent, "[LUA-print] {}", t);
    t.append("\n");
    lua_pushstring(L, t.c_str());
    return 1;
}
}

int addImGuiFont(lua_State* L)
{
    std::string fontFile = ((std::string)tolua_tocppstring(L, 1, 0));
    float fontSize       = ((float)tolua_tonumber(L, 2, 0));
    int glyphRange       = ((int)tolua_tonumber(L, 3, 0));

    extension::ImGuiPresenter::getInstance()->addFont(fontFile, fontSize,
                                                      (extension::ImGuiPresenter::CHS_GLYPH_RANGE)glyphRange);
    return 0;
}

void preload_lua_modules(lua_State *L)
{
	//lfs
	luaopen_lfs(L);

	luaopen_main(L);

	luaopen_game(L);

	luaopen_subprocess(L);

	//////////////////////////////////////////////////////////////////////////
	static luaL_Reg _lib[] = {
		{ "cjson", luaopen_cjson_safe },
		{ nullptr, nullptr }
	};
	lua_getglobal(L, "package");
	lua_getfield(L, -1, "preload");

	auto lib = _lib;
	for (; lib->func; lib++)
	{
		lua_pushcfunction(L, lib->func);
		lua_setfield(L, -2, lib->name);
	}
	lua_pop(L, 2);
	//////////////////////////////////////////////////////////////////////////

    
    // Register our version of the global "print" function
    const luaL_Reg global_functions[] = {
        {"print", Lua_Print::lua_print},
        {"release_print", Lua_Print::lua_release_print},
        {"addImGuiFont", addImGuiFont},
        {nullptr, nullptr}
    };
    luaL_register(L, "_G", global_functions);
}
