#include "script_wrapper.h"
#include <map>
#include <string>
#ifdef WIN32
#else
#include <sys/time.h>
#include <syslog.h>
#include <unistd.h>
#endif // WIN32

#include "common.h"


namespace WEIFEI
{ 
class LuaPerfMon
{
	const char *_func;
	const char *_detail;

	int64_t _begin;




public:
	LuaPerfMon(const char *func, const char *detail): _func(func), _detail(detail)
	{
		_begin = NowMS();
	}
	~LuaPerfMon()
	{
		int64_t l = NowMS() - _begin;
		if(l>1000) fprintf(stderr, "脚本函数%s(%s)超时:%lld", _func, _detail, l);
	}
};

LuaWrapper::LuaWrapper(lua_State* L) : __L(L),_stack_index(0),_num_result(0)
{
	if(__L)
	{
		_stack_index = lua_gettop(__L);
		//__PRINTF("LuaWrapper::LuaWrapper(lua_State* L) _stack_index=%d\n",_stack_index);
	}
}

LuaWrapper::~LuaWrapper()
{
	ResetStack();
}

#ifdef WIN32
__declspec(thread) int64_t g_lua_infinite_loop_check_begin_ms = 0;
__declspec(thread) int64_t g_lua_infinite_loop_max_ms = 0;
#else
__thread int64_t g_lua_infinite_loop_check_begin_ms = 0;
__thread int64_t g_lua_infinite_loop_max_ms = 0;
#endif // WIN32


static void LuaInfiniteLoopAlert(lua_State *L, lua_Debug *ar)
{
	int64_t now_ms = NowMS();
	if(now_ms > g_lua_infinite_loop_check_begin_ms+g_lua_infinite_loop_max_ms)
	{
		luaL_error(L, "LuaInfiniteLoopAlert");
	}
}

bool LuaWrapper::gExec(const char* funcname,int timeout_ms,int alloc_limit,const LuaParameter& args,int ret_num,const char *detail)
{
	if(args.Size() > LUA_MAX_PARAM) return false;
	if(!__L) return false;
	LuaPerfMon mon(funcname, detail);
	if(_num_result > 0) lua_pop(__L,_num_result);
	_num_result = -1;

	lua_sethook(__L, LuaInfiniteLoopAlert, LUA_MASKCOUNT, 10000);

	g_lua_infinite_loop_check_begin_ms = NowMS();
	g_lua_infinite_loop_max_ms = 10000; //defalut 10s
	if(timeout_ms>0) g_lua_infinite_loop_max_ms=timeout_ms;

	lua_getglobal(__L, "_AddTraceInfo");

	int index = lua_gettop(__L);
	lua_checkstack(__L,args.Size() + 1);
//	lua_getfield(__L,LUA_GLOBALSINDEX,funcname);
	lua_getglobal(__L,funcname);
	args.PushArgs(__L);

	if(alloc_limit)
	{
		void *ud = 0;
		lua_getallocf(__L, &ud);
		if(ud)
		{
			LuaMemory *mem = (LuaMemory*)ud;
			mem->SetAllocLimit(alloc_limit);

			//GLog::log(LOG_ERR, "LuaWrapper::gExec, before call, mem=%p, _total_bytes=%lu", mem, mem->GetTotalBytes());
		}
	}
	if(lua_pcall(__L,args.Size(),ret_num,index))
	{
		if(alloc_limit)
		{
			void *ud = 0;
			lua_getallocf(__L, &ud);
			if(ud)
			{
				LuaMemory *mem = (LuaMemory*)ud;
				mem->RemoveAllocLimit();
			}
		}
		//这里有错误 记录到result中
		_num_result = -1;
		_error = "Error when calling '";
		_error += funcname;
		_error += "':";
		_error += lua_tostring(__L,-1);
		lua_pop(__L,1);

		lua_remove(__L,index);

		return false;
	}
	if(alloc_limit)
	{
		void *ud = 0;
		lua_getallocf(__L, &ud);
		if(ud)
		{
			LuaMemory *mem = (LuaMemory*)ud;
			mem->RemoveAllocLimit();

			//GLog::log(LOG_ERR, "LuaWrapper::gExec, after call, mem=%p, _total_bytes=%lu", mem, mem->GetTotalBytes());
		}
	}

	//将结果记录到Result中
	_num_result = lua_gettop(__L) - index;

	lua_remove(__L,index);

	return true;
}

void LuaWrapper::PopResult() 
{
	if(!__L) return;
	if(_num_result > 0) lua_pop(__L,_num_result);
	_num_result = -1;
}

bool LuaWrapper::GetTable(int index, std::vector<double> &list)
{
	if(!lua_istable(__L,index)) return false;
	list.clear();
	int len = LUA_OBJLEN(__L,index);
	for( int i = 1; i <= len ; i ++)
	{
		lua_rawgeti(__L,index, i);
		double m = lua_tonumber(__L, -1);
		list.push_back(m);
		lua_pop(__L,1);
	}
	return true;
}

bool LuaWrapper::GetTable(int index, std::vector<int> &list)
{
	if(!lua_istable(__L,index)) return false;
	list.clear();
	int len = LUA_OBJLEN(__L,index);
	for( int i = 1; i <= len ; i ++)
	{
		lua_rawgeti(__L,index, i);
		int m = lua_tointeger(__L, -1);
		list.push_back(m);
		lua_pop(__L,1);
	}
	return true;
}

void LuaWrapper::ResetStack()
{
	if(__L)
	{
		lua_settop(__L,_stack_index);
	}
}

void* LuaMemory::Alloc(void *ud, void *ptr, size_t osize, size_t nsize)
{
	LuaMemory *mem = (LuaMemory*)ud;

	if(nsize == 0)
	{
		free(ptr);
		if(osize > 0)
		{
			mem->_total_block--;
			mem->_total_bytes -= osize;
		}
		return 0;
	}
	else
	{
		if(mem->_total_bytes_max>0 && mem->_total_bytes+nsize-osize>mem->_total_bytes_max)
		{
			fprintf(stderr, "LuaMemory, can't alloc more memory, mem=%p, nsize(%lu)-osize(%lu)=%lu, _total_block=%lu, _total_bytes=%lu, _total_bytes_max=%lu\n",
			          mem, nsize, osize, nsize-osize, mem->_total_block, mem->_total_bytes, mem->_total_bytes_max);
			return 0;
		}

		void *p = realloc(ptr, nsize);
		if(p)
		{
			if(osize==0) mem->_total_block++;
			mem->_total_bytes += nsize-osize;
		}
		else
		{
			if(osize > 0)
			{
				mem->_total_block--;
				mem->_total_bytes -= osize;
			}
		}
		return p;
	}
}

}