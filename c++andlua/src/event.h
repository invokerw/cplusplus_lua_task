#ifndef _WEIFEI_EVENT_H_
#define _WEIFEI_EVENT_H_

namespace WEIFEI
{
#if 0
enum SHORTCUT {
	CTRL = VK_LCONTROL,
	SHIFT = VK_LSHIFT,
	ALT = VK_MENU,
	ESC = VK_ESCAPE,
	WIN = VK_LWIN,
	BACK = VK_BACK,
	TAB = VK_TAB,
	ENTER = VK_RETURN,
	HOME = VK_HOME,
	END = VK_END,
	DEL = VK_DELETE,
	INS = VK_INSERT,
	UP = VK_UP,
	DOWN = VK_DOWN,
	RIGHT = VK_RIGHT,
	LEFT = VK_LEFT,
	F1 = VK_F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,
	PRTSC = VK_PRINT,
	A = 65, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
};

void MouseClick(int c);   //0:左键  1:滚轮键  2:右键
void Mouse_Move(int dx, int dy); //dx: 水平方向位移，正向向右   dy：垂直方向位移，正向向下	
void KeyClick(int KEY);
#endif
};

#endif
