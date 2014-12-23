/**
@Author:ZUHD
@Date:2014-11-27
*/
#include "sys.h"
#include "log.h"

static void sys_log(const char* fmt, ...)
{
		va_list args;
		char szbuffer[512];
		va_start(args, fmt);
		vsnprintf(szbuffer, sizeof(szbuffer), fmt, args);
		va_end(args);
		syslog(LOG_INFO|LOG_USER, szbuffer);
		return;
}

void log(const char* fmt, ...)
{	
		umask(0);
		va_list args;
		char szbuffer[512];
		va_start(args, fmt);
		vsnprintf(szbuffer, sizeof(szbuffer), fmt, args);		
		va_end(args);		
		return;
}