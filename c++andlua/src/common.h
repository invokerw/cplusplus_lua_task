#ifndef _WEIFEI_COMMON_H_
#define _WEIFEI_COMMON_H_

#include <stdio.h>
#include <stdint.h>
#ifdef WIN32
#include <windows.h>
#include <time.h>
#else
#include <sys/time.h>
#endif // WIN32



namespace WEIFEI
{
inline time_t Now() { return time(0); } 

inline int64_t NowMS()
{
#ifdef WIN32
	// 从1601年1月1日0:0:0:000到1970年1月1日0:0:0:000的时间(单位100ns)
	constexpr auto EPOCHFILETIME = (116444736000000000UL);
	FILETIME ft;
	LARGE_INTEGER li;
	int64_t tt = 0;
	GetSystemTimeAsFileTime(&ft);
	li.LowPart = ft.dwLowDateTime;
	li.HighPart = ft.dwHighDateTime;
	// 从1970年1月1日0:0:0:000到现在的微秒数(UTC时间)
	tt = (li.QuadPart - EPOCHFILETIME) / (10*1000);
	return tt;
#else
	struct timeval tv;
	gettimeofday(&tv, nullptr);
	return (tv.tv_sec*1000L+tv.tv_usec/1000);
#endif
}

struct ScriptMessage
{
	std::string _msg;
	std::string _arg;
	int64_t _src;
public:
	ScriptMessage(const char *msg, const char *arg, int64_t src): _msg(msg), _arg(arg), _src(src) {}
};

}

#endif
