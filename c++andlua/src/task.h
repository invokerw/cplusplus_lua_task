#ifndef __WEIFEI_TASK_H__ 
#define __WEIFEI_TASK_H__ 

#include <lua.hpp>
#include <string>
#include <set>
#include <vector>
#include <map>
#include "common.h"

namespace WEIFEI
{
class Task
{
	lua_State *_script; 
	std::string _name;
	std::string _param;
	
	bool _close;
#ifdef WIN32
	HWND _hwnd;
	HWND _child;
#endif

	std::vector<ScriptMessage> _script_msgs;
	std::multimap<int64_t, ScriptMessage> _delay_script_msgs;

public:
	Task();
	~Task();
	
	bool Init(const char *script_path, const char *param);
	bool Tick();
	void OnRecvScriptMsg(const char *msg, const char *arg, int64_t src, int delay_ms);
	const std::string& GetName(){return _name;}
	
	// for lua
	void Close() { _close = true; }

	void SetHWND(HWND hwnd) { _hwnd = hwnd; }
	HWND GetHWND() { return _hwnd; }
	void SetChildHWND(HWND child) { _child = child; }
	void KeyDown(int KEY);
	void KeyUp(int KEY);
	void ChildKeyDown(int KEY);
	void ChildKeyUp(int KEY);

	void MouseMove();
	void LeftMouseDown();
	void LeftMouseUp();
	void RightMouseDown();
	void RightMouseUp();

	
};
class TaskManager
{
	int _id_stub;
	std::map<int, Task*> _map;

	TaskManager():_id_stub(0){}
	~TaskManager() {}
public:
	static TaskManager* GetInstance()
	{
		static TaskManager _instance;
		return &_instance;
	}
	void Register(Task *t)
	{
		_map.insert(std::make_pair(_id_stub++, t));
	}
	void Tick()
	{
		std::set<int> _will_delete;
		for (auto it = _map.begin(); it != _map.end(); ++it)
		{
			Task *t = it->second;
			bool ret = t->Tick();
			if (!t)
			{
				_will_delete.insert(it->first);
				delete t;
			}

		}
		for (auto it = _will_delete.begin(); it != _will_delete.end(); ++it)
		{
			_map.erase(*it);
		}
	}


};
};


#endif
