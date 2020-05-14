#include "script_wrapper.h"
#include "luaenv.h"
#include <iostream>
#include "task.h"

namespace WEIFEI
{

static int script_Log(lua_State *l)
{
	int n = lua_gettop(l);
	if(n != 2) return 0;

	Task *t = (Task*)lua_touserdata(l, 1);
	const char *log = lua_tostring(l, 2);
	t->Log(log);
	return 0;
}

static int script_GetTimeMS(lua_State *l) //毫秒计量
{
	lua_pushnumber(l, (lua_Number)NowMS());
	return 1;
}

static int script_Close(lua_State *l) //毫秒计量
{
	int n = lua_gettop(l);
	if (n != 1) return 0;

	Task *t = (Task*)lua_touserdata(l, 1);
	t->Close();
	return 0;
}
static int script_KeyDown(lua_State *l)
{
	int n = lua_gettop(l);
	if (n != 2) return 0;

	Task *t = (Task*)lua_touserdata(l, 1);
	int key = lua_tointeger(l, 2);
	t->KeyDown(key);
	return 0;
}
static int script_KeyUp(lua_State *l)
{
	int n = lua_gettop(l);
	if (n != 2) return 0;

	Task *t = (Task*)lua_touserdata(l, 1);
	int key = lua_tointeger(l, 2);
	t->KeyUp(key);
	return 0;
}

static int script_ChildKeyDown(lua_State *l)
{
	int n = lua_gettop(l);
	if (n != 2) return 0;

	Task *t = (Task*)lua_touserdata(l, 1);
	int key = lua_tointeger(l, 2);
	t->ChildKeyDown(key);
	return 0;
}
static int script_ChildKeyUp(lua_State *l)
{
	int n = lua_gettop(l);
	if (n != 2) return 0;

	Task *t = (Task*)lua_touserdata(l, 1);
	int key = lua_tointeger(l, 2);
	t->ChildKeyUp(key);
	return 0;
}


static int script_FindWindow(lua_State *l)
{
	int n = lua_gettop(l);
	if (n != 3) return 0;
	Task *t = (Task*)lua_touserdata(l, 1);
	const char *class_name = lua_tostring(l, 2);
	const char *window_name = lua_tostring(l, 3);
	wchar_t *c = U8ToUnicode(class_name);
	wchar_t *w = U8ToUnicode(window_name);
	//HWND hWnd = FindWindow(L"FFXIVGAME", L"最终幻想XIV");
	//HWND hWnd = FindWindow(L"Notepad", L"1.txt - 记事本");
	HWND hWnd = FindWindow(c, w);
	delete []c;
	delete []w;
	if (!hWnd)
	{
		lua_pushboolean(l, false);
		return 1;
	}
	t->SetHWND(hWnd);
	lua_pushboolean(l, true);
	return 1;
}
static int script_FindChildWindow(lua_State *l)
{
	int n = lua_gettop(l);
	if (n != 3) return 0;
	Task *t = (Task*)lua_touserdata(l, 1);
	const char *class_name = lua_tostring(l, 2);
	const char *window_name = lua_tostring(l, 3);
	wchar_t *c = U8ToUnicode(class_name);
	wchar_t *w = U8ToUnicode(window_name);
	HWND hWnd = FindWindowEx(t->GetHWND(), 0, c, w);
	delete[]c;
	delete[]w;
	if (!hWnd)
	{
		lua_pushboolean(l, false);
		return 1;
	}
	t->SetChildHWND(hWnd);
	lua_pushboolean(l, true);
	return 1;
}

lua_State* CreateLuaEnv(const char *path)
{
	//LuaMemory *mem = 0;
	//lua_State *l = luaL_newstate();
	LuaMemory *mem = new LuaMemory();
	lua_State *l = lua_newstate(LuaMemory::Alloc, mem);
	if(l)
	{
		//lua_atpanic(l, panic);

		luaL_openlibs(l);

		lua_register(l, "API_Log", script_Log);
		lua_register(l, "API_GetTimeMS", script_GetTimeMS);
		lua_register(l, "API_FindWindow", script_FindWindow);
		lua_register(l, "API_FindChildWindow", script_FindChildWindow);
		lua_register(l, "API_KeyDown", script_KeyDown);
		lua_register(l, "API_KeyUp", script_KeyUp);
		lua_register(l, "API_ChildKeyDown", script_ChildKeyDown);
		lua_register(l, "API_ChildKeyUp", script_ChildKeyUp);
		lua_register(l, "API_Close", script_Close);


		//关掉一些权限功能
		//整table关
		lua_pushnil(l); lua_setglobal(l, "package");
		lua_pushnil(l); lua_setglobal(l, "io");
		//挑着关
		lua_getglobal(l, "os");
		lua_pushnil(l); lua_setfield(l, -2, "exit");
		lua_pushnil(l); lua_setfield(l, -2, "remove");
		lua_pushnil(l); lua_setfield(l, -2, "rename");
		lua_pushnil(l); lua_setfield(l, -2, "execute");
		lua_pop(l, 1);

		//load src lua
		if(luaL_dofile(l, "./base.lua"))
		{
			std::cerr<<"CreateLuaEnv_For_Level, error, luaL_dofile(./base.lua)"<<std::endl;
			lua_close(l);
			delete mem;
			l = 0;
			return l;
		}
		if(luaL_dofile(l, "./taskbase.lua"))
		{
			std::cerr<<"CreateLuaEnv_For_Level, error, luaL_dofile(./taskbase.lua)"<<std::endl;
			lua_close(l);
			delete mem;
			l = 0;
			return l;
		}
		if(luaL_dofile(l, path))
		{
			std::cerr<<"CreateLuaEnv_For_Level, error, luaL_dofile("<<path<<")"<<std::endl;
			lua_close(l);
			delete mem;
			l = 0;
			return l;
		}
	}
	return l;
}


void DestroyLuaEnv(lua_State *l)
{
	void *ud = 0;
	lua_getallocf(l, &ud);

	lua_close(l);

	if(ud)
	{
		LuaMemory *mem = (LuaMemory*)ud;
		delete mem;
	}
}

}

