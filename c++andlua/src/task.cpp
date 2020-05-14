#include "task.h"
#include "script_wrapper.h"
#include <iostream>
#include "luaenv.h"
#include <string.h>

namespace WEIFEI
{
	Task::Task() :_script(nullptr), _close(false), _hwnd(nullptr), _child(nullptr)
{

}
Task::~Task()
{
	if(_script) DestroyLuaEnv(_script);
}

bool Task::Init(const char *script_path, const char *param)
{
	_script = CreateLuaEnv(script_path);
	if(!_script) return false;
	_name = script_path;
	_param = param;
	LuaParameter lp;
	LuaWrapper lw(_script);
	if(!lw.gExec("_Start", 1000, 20*1024, lp))
	{
		fprintf(stderr, "\033[31mTick::Init, gExec, error, _Start, %s\033[0m\n", lw.ErrorMsg());
#ifdef WIN32
		if(strcmp(lw.ErrorMsg(), "not enough memory")==0)
#else
		if (strcasestr(lw.ErrorMsg(), "not enough memory"))
#endif // WIN32
		{
			std::cerr << "Task::Init, gExec, memory reached limit, force do lua gc" << std::endl;
			lua_gc(_script, LUA_GCCOLLECT, 0);
		}
	}
	return true;
}

void Task::Log(const char *str)
{

	std::string s = "WEIFEI->" + GetName();
	s += str;
	LOG(s.c_str());
}

bool Task::Tick()
{
	if (_close) return false;
	if(!_script) return false;

	LuaParameter lp;
	lp.AddParameter(this);
	LuaWrapper lw(_script);
	if(!lw.gExec("_Run", 1000, 20*1024, lp))
	{
		fprintf(stderr, "\033[31mTick::Tick, gExec, error, _Run, %s\033[0m\n", lw.ErrorMsg());
#ifdef WIN32
		if (strcmp(lw.ErrorMsg(), "not enough memory") == 0)
#else
		if (strcasestr(lw.ErrorMsg(), "not enough memory"))
#endif // WIN32
		{
			std::cerr << "Task::Tick, gExec, memory reached limit, force do lua gc" << std::endl;
			lua_gc(_script, LUA_GCCOLLECT, 0);
		}

	}
	//避免疯狂上涨吧
	lua_gc(_script, LUA_GCCOLLECT, 0);

	std::vector<ScriptMessage> script_msgs;
	if(!_script_msgs.empty()) script_msgs.swap(_script_msgs);
	if(!_delay_script_msgs.empty())
	{
		int64_t now_ms = NowMS();
		auto it = _delay_script_msgs.begin();
		while(it != _delay_script_msgs.end())
		{
			int64_t t = it->first; 
			if(t>now_ms) break;
    
			const ScriptMessage& msg = it->second;
			script_msgs.push_back(msg);
    
			it = _delay_script_msgs.erase(it);
			it = _delay_script_msgs.begin();
		}
    
	}
	if(!script_msgs.empty())
	{
		for(auto it=script_msgs.begin(); it!=script_msgs.end(); ++it)
		{
			const ScriptMessage& msg = *it;
    
			LuaParameter lp;
			lp.AddParameter(this); //ptr,msg,arg,src
			lp.AddParameter((const char*)msg._msg.c_str());
			lp.AddParameter((const char*)msg._arg.c_str());
			lp.AddParameter((double)msg._src);
			LuaWrapper lw(_script);
			if(!lw.gExec("_OnScriptMessage", 1000, 20*1024, lp))
			{
				fprintf(stderr, "\033[31mTask::Tick, gExec, error, _OnScriptMessage, %s\033[0m\n", lw.ErrorMsg());
    
#ifdef WIN32
				if (strcmp(lw.ErrorMsg(), "not enough memory") == 0)
#else
				if (strcasestr(lw.ErrorMsg(), "not enough memory"))
#endif // WIN32
				{
					std::cerr << "Task::Tick, gExec, memory reached limit, force do lua gc" << std::endl;
					lua_gc(_script, LUA_GCCOLLECT, 0);
				}
			}
		}

	}
	return false;
}

void Task::OnRecvScriptMsg(const char *msg, const char *arg, int64_t src, int delay_ms)
{
	if(delay_ms <= 0)
	{
		_script_msgs.push_back(ScriptMessage(msg, arg, src));
		return;
	}
	_delay_script_msgs.insert(std::make_pair(NowMS()+delay_ms, ScriptMessage(msg, arg, src)));
}
void Task::KeyDown(int KEY)
{
	if(_hwnd) ::PostMessage(_hwnd, WM_KEYDOWN, KEY, 1);
}
void Task::KeyUp(int KEY)
{
	if (_hwnd) ::PostMessage(_hwnd, WM_KEYUP, KEY, 1);
}
void Task::MouseMove()
{
	//处理鼠标悬浮事件，这里的MAKELPARAM(width*x/oriWidth,height*y/oriHeight)表示鼠标位置
	if (_hwnd) ::PostMessage(_hwnd, WM_MOUSEMOVE, 0, MAKELPARAM(0, 0));
}
void Task::LeftMouseDown()
{
	if (_hwnd) ::PostMessage(_hwnd, WM_LBUTTONDOWN, MK_LBUTTON, MAKELPARAM(50, 50));
}
void Task::LeftMouseUp()
{
	if (_hwnd) ::PostMessage(_hwnd, WM_LBUTTONUP, MK_LBUTTON, MAKELPARAM(50, 50));
}
void Task::RightMouseDown()
{
	if (_hwnd) ::PostMessage(_hwnd, WM_RBUTTONDOWN, MK_LBUTTON, MAKELPARAM(50, 50));
}
void Task::RightMouseUp()
{
	if (_hwnd) ::PostMessage(_hwnd, WM_RBUTTONUP, MK_LBUTTON, MAKELPARAM(50, 50));
}
void Task::ChildKeyDown(int KEY)
{
	if (_child)
	{
		::PostMessage(_child, WM_KEYDOWN, KEY, 1);
	}
}
void Task::ChildKeyUp(int KEY)
{
	if (_child)
	{
		::PostMessage(_child, WM_KEYUP, KEY, 1);
	}
}

};


