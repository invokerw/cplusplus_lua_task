#ifdef WIN32
#include <windows.h>
#include <time.h>
#endif // WIN32

namespace WEIFEI
{
#if 0
#ifdef WIN32

	int mousemap[3][2] =
	{
		//对应的值:0x0002                   0x0004            
		{MOUSEEVENTF_LEFTDOWN,   MOUSEEVENTF_LEFTUP   },
		//         0x0020					0x0040
		{MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_MIDDLEUP },
		//			0x0008					0x0010
		{MOUSEEVENTF_RIGHTDOWN,  MOUSEEVENTF_RIGHTUP  }
	};
#endif // WIN32

	void MouseClick(int c)   //0:左键  1:滚轮键  2:右键
	{
#ifdef WIN32
		mouse_event(mousemap[c][0], 0, 0, 0, 0);
		mouse_event(mousemap[c][1], 0, 0, 0, 0);
#endif
	}
	/*
	void Mouse_Move(int dx, int dy)		//dx: 水平方向位移，正向向右   dy：垂直方向位移，正向向下
	{
#ifdef WIN32
		POINT p = { 0,0 };
		GetCursorPos(&p);
		SetCursorPos(p.x + dx, p.y + dy);
#endif // WIN32
	}
	*/
	void Mouse_Move(int dx, int dy) //dx: 水平方向位移，正向向右   dy：垂直方向位移，正向向下	
	{
#ifdef WIN32
		mouse_event(MOUSEEVENTF_MOVE, dx, dy, 0, 0);
#endif // WIN32
	}
	void KeyClick(int KEY)
	{
#ifdef WIN32
		keybd_event(KEY, 0, KEYEVENTF_EXTENDEDKEY | 0, 0);            //相当于 keybd_event(KEY,0,0,0);
		keybd_event(KEY, 0, KEYEVENTF_EXTENDEDKEY | KEYEVENTF_KEYUP, 0); //相当于 keybd_event(KEY,0,2,0);
#endif // WIN32
	}
#endif
};