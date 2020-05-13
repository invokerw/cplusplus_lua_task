
g_ptr = nil --pointer to scene

function TASK_Close()
	API_Close();
end

function TASK_FindWindow(class_name, window_name)
	return API_FindWindow(g_ptr, class_name, window_name)
end

function TASK_KeyDown(key)
	API_KeyDown(g_ptr, key)
end
function TASK_KeyUp(key)
	API_KeyUp(g_ptr, key)
end

function TASK_FindChildWindow(class_name, window_name)
	return API_FindChildWindow(g_ptr, class_name, window_name)
end

function TASK_ChildKeyDown(key)
	API_ChildKeyDown(g_ptr, key)
end
function TASK_ChildKeyUp(key)
	API_ChildKeyUp(g_ptr, key)
end

VK_NUMPAD0     =   0x60
VK_NUMPAD1     =   0x61
VK_NUMPAD2     =   0x62
VK_NUMPAD3     =   0x63
VK_NUMPAD4     =   0x64
VK_NUMPAD5     =   0x65
VK_NUMPAD6     =   0x66
VK_NUMPAD7     =   0x67
VK_NUMPAD8     =   0x68
VK_NUMPAD9     =   0x69

VK_F1          =   0x70
VK_F2          =   0x71
VK_F3          =   0x72
VK_F4          =   0x73
VK_F5          =   0x74
VK_F6          =   0x75
VK_F7          =   0x76
VK_F8          =   0x77
VK_F9          =   0x78
VK_F10         =   0x79
VK_F11         =   0x7A
VK_F12         =   0x7B

VK_LSHIFT      =   0xA0
VK_RSHIFT      =   0xA1
VK_LCONTROL    =   0xA2
VK_RCONTROL    =   0xA3

VK_ESCAPE      =   0x1B
VK_RETURN      =   0x0D
VK_TAB         =   0x09
VK_CAPITAL     =   0x14
VK_HOME        =   0x24
VK_PRIOR       =   0x21
VK_NEXT        =   0x22
VK_END         =   0x23
VK_INSERT      =   0x2D
VK_DELETE      =   0x2E
VK_SPACE       =   0x20

VK_LEFT        =   0x25
VK_RIGHT       =   0x27
VK_DOWN        =   0x28
VK_UP          =   0x26 

VK_SHIFT       =   0x10
VK_CONTROL     =   0x11
VK_MENU        =   0x12
--[[
/*
 * VK_0 - VK_9 are the same as ASCII '0' - '9' (0x30 - 0x39)
 * 0x3A - 0x40 : unassigned
 * VK_A - VK_Z are the same as ASCII 'A' - 'Z' (0x41 - 0x5A)
 */
 ]]