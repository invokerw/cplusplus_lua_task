#include <lua.hpp>

namespace WEIFEI
{

lua_State* CreateLuaEnv(const char *path);
void DestroyLuaEnv(lua_State *l);

}

