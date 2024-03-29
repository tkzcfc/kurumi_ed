
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

using namespace ax;

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

}
