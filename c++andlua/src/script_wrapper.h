#ifndef __GS_SCRIPT_WRAPPER_H__
#define __GS_SCRIPT_WRAPPER_H__

#include <vector>
#include <map>
#include <string>
#include <stdio.h>

#include <lua.hpp>

namespace WEIFEI
{

//#ifdef LUA5_2
//#define LUA_OBJLEN lua_rawlen
//#else
#define LUA_OBJLEN lua_objlen
//#endif

enum LUA_PARAM_NULL
{
	LUA_PARAM_EMPTY,
	LUA_MAX_PARAM = 256,
};

struct lua_ref_t
{
	int key;
};

static void lua_pushtable_ref(lua_State *L, lua_ref_t value)
{
	lua_rawgeti(L,LUA_REGISTRYINDEX,value.key);
}

namespace  LuaParameterSpace__		//LuaParamter 私有的名字空间，其它类和对象不要动
{
	inline  void CommonPush(lua_State * L, bool value) { lua_pushboolean(L, value);}
	inline  void CommonPush(lua_State * L, int value) { lua_pushinteger(L, value);}
	inline  void CommonPush(lua_State * L, void * value) { lua_pushlightuserdata(L, value);}
	inline  void CommonPush(lua_State * L, float value) { lua_pushnumber(L, value);}
	inline  void CommonPush(lua_State * L, double value) { lua_pushnumber(L, value);}
	inline  void CommonPush(lua_State * L, lua_ref_t value) { lua_pushtable_ref(L, value);}
	inline  void CommonPush(lua_State * L, const std::string &str) { lua_pushstring(L, str.c_str());}
	template<typename TYPE>
	inline  void CommonPush(lua_State * L, const std::vector<TYPE> & value)
	{ 
		lua_creattable(L,0, value.size());
		int index = 1;
		for(auto it = value.begin(); it != value.end(); ++it)
		{
			CommonPush(L, *it);lua_rawseti(L, -2, index); index ++;
		}
	}

	struct BaseType
	{
		int _reference_count;

		BaseType() : _reference_count(1) {}
		virtual ~BaseType() {}
		virtual void PushValue(lua_State* L) = 0;

		void Release()
		{
			if((--_reference_count) == 0)
			{
				delete this;
			}
		}
	};

	struct ValueType
	{
		BaseType* _value;

	public:
		ValueType(BaseType* value = NULL) : _value(value) {}
		ValueType(const ValueType& rhs) : _value(rhs._value)
		{
			if(_value) ++_value->_reference_count;
		}

		const ValueType& operator =(const ValueType& rhs)
		{
			if(_value) _value->Release();
			_value = rhs._value;
			if(_value) ++_value->_reference_count;
			return *this;
		}

		~ValueType()
		{
			if(_value) _value->Release();
		}

		void PushArg(lua_State* L) const
		{
			if(!_value) 
				lua_pushnil(L);
			else
				_value->PushValue(L);
		}
	};

	template<typename TRAIT_TYPE> struct ParameterValue : public BaseType
	{
		typedef TRAIT_TYPE value_type;
		value_type _value;
		ParameterValue(const value_type &value) : _value(value) {}
		virtual void PushValue(lua_State* L) { CommonPush(L,_value); }
	};

	template<> struct ParameterValue<std::string> : public BaseType
	{
		typedef std::string value_type;
		value_type _value;
		ParameterValue(const value_type &value) : _value(value) {}
		virtual void PushValue(lua_State* L) { CommonPush(L,_value);}
	};

	template <typename T > struct ParameterValue<std::vector<T> > : public BaseType
	{
		typedef std::vector<T> value_type;
		const value_type & _value;
		ParameterValue(const value_type &value) : _value(value) {}		//注意，这里保存了引用，小心出错 但为了性能，忍了
		virtual void PushValue(lua_State* L) 
		{ 
			lua_createtable(L, _value.size(), 0);
			for(size_t i =0; i < _value.size(); i ++)
			{
				CommonPush(L, _value[i]);
				lua_rawseti(L, -2, i+1);
			}
		}
	};

	template <typename T > struct ParameterValue<std::map<std::string , T> > : public BaseType
	{
		typedef std::map<std::string , T>  value_type;
		//const value_type & _value;
		value_type _value;
		ParameterValue(const value_type &value) : _value(value) {}		//注意，这里保存了引用，小心出错 但为了性能，忍了
		virtual void PushValue(lua_State* L) 
		{ 
			lua_createtable(L, 0, _value.size());
			//for(size_t i =0; i < _value.size(); i ++)
			for(auto it = _value.begin(); it != _value.end(); ++it)
			{
				CommonPush(L, it->second);
				lua_setfield(L, -2, it->first.c_str());
			}
		}
	};
	
	template<typename TRAIT_TYPE> BaseType* MakeValueT(TRAIT_TYPE value)
	{
		return new ParameterValue<TRAIT_TYPE>(value);
	}

	template<typename T > BaseType* MakeValueT(const std::vector<T> &value)
	{
		return new ParameterValue<std::vector<T> >(value);
	}

	template<typename T > BaseType* MakeValueT(const std::map<std::string, T> &value)
	{
		//return new ParameterValue<std::vector<std::string, T> >(value);
		return new ParameterValue<std::map<std::string, T> >(value);
	}

	template <typename VALUE_TYPE> inline ValueType MakeValue(VALUE_TYPE value) { return ValueType(MakeValueT(value)); }
	template <typename T> inline ValueType MakeValue(const std::vector<T> & value) { return ValueType(MakeValueT(value)); }
	template <typename T> inline ValueType MakeValue(const std::map<std::string, T> & value) { return ValueType(MakeValueT(value)); }
	template <> inline ValueType MakeValue(const char* value) { return ValueType(MakeValueT(std::string(value))); }
	template <> inline ValueType MakeValue(LUA_PARAM_NULL value) { return ValueType(); }

}

class LuaParameter
{
private:
	std::vector< LuaParameterSpace__::ValueType> _list;

#define MakeValue LuaParameterSpace__::MakeValue

	template <typename L1, typename L2>
	void AddParameter(const L1 &l1, const L2 &l2) 
	{
		_list.push_back(MakeValue(l1)); 
		_list.push_back(MakeValue(l2)); 
	}

	template <typename L1, typename L2, typename L3>
	void AddParameter(const L1 &l1, const L2 &l2, const L3 & l3) 
	{
		_list.push_back(MakeValue(l1)); 
		_list.push_back(MakeValue(l2)); 
		_list.push_back(MakeValue(l3)); 
	}

	template <typename L1, typename L2, typename L3, typename... ARG_TYPES>
	void AddParameter(const L1 &l1, const L2 & l2, const L3 & l3, const ARG_TYPES&... args)
	{
		AddParameter(l1, l2, l3);
		AddParameter(args...);
	}

public:
	LuaParameter() {}

	template <typename... ARG_TYPES>
	explicit LuaParameter(const ARG_TYPES&... args)
	{
		_list.reserve(20);
		AddParameter(args...);
	}

	template <typename L1>
	void AddParameter(const L1 &l1) 
	{
		_list.push_back(MakeValue(l1)); 
	}


	void PushArgs(lua_State* L) const
	{
		for(size_t i = 0;i < _list.size();++i)
		{
			_list[i].PushArg(L);
		}
	}

	size_t Size() const
	{
		return _list.size();
	}
#undef MakeValue
};

/*
class LuaResult
{
	int _num_result;
	lua_State * __L;
	std::string _err;
	bool _auto_pop;
public:
	LuaResult(bool auto_pop = false):_num_result(0),__L(NULL),_auto_pop(auto_pop)
	{}

	int Count() { return _num_result;}
	void Pop() { 
		if(_num_result > 0) lua_pop(__L,_num_result); 
		_num_result = 0;
	}
	const char * ErrorMsg() { return _err.c_str();}

	friend class LuaWrapper;
};
*/

class LuaWrapper
{
	lua_State* __L;
	int _stack_index;
	int _num_result;
	std::string _error;

public:
	LuaWrapper(lua_State* L);
	~LuaWrapper();
	lua_State * GetL() { return __L;}
	bool gExec(const char* funcname,int timeout_ms/*最大执行时间*/,int alloc_limit/*最多分配内存*/,const LuaParameter& args = LuaParameter(),int ret_num = LUA_MULTRET, const char *detail="");
	void PopResult();
	void ResetStack();

	int ResultCount() const { return _num_result; }
	const char* ErrorMsg() { return _error.c_str(); }
	bool GetTable(int index, std::vector<double> &list);
	bool GetTable(int index, std::vector<int> &list);
};

class LuaMemory
{
	size_t _total_block; //已分配多少内存(块)
	size_t _total_bytes; //已分配多少内存(bytes)
	size_t _total_bytes_max; //最多能分配多少内存(bytes), 超出则拒绝分配，0表示无限制

public:
	LuaMemory(): _total_block(0), _total_bytes(0), _total_bytes_max(0) {}

	//lua在非protect模式下，内存分配失败后会直接结束进程，所以限制功能仅用在lua_pcall时)
	void SetAllocLimit(size_t n) { _total_bytes_max=_total_bytes+n; } //从现在起，最多还能再分配多少内存
	void RemoveAllocLimit() { _total_bytes_max=0; } //关掉分配限制

	size_t GetTotalBytes() const { return _total_bytes; }

	//用法: lua_newstate(LuaMemory::Alloc, new LuaMemory());
	static void* Alloc(void *ud, void *ptr, size_t osize, size_t nsize);
};

//int XScriptLoad(lua_State * __L, const char * pathname);	//可以缓存性的读取LUA文件，内部会从内存中加载，其返回值等同于luaL_dofile
//void  XScriptClearCache();

}
#endif

