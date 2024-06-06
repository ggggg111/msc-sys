#ifndef __LOG_H__
#define __LOG_H__

#include <windows.h>
#include <stdio.h>
#include <string>
#include <queue>
#include <list>

#define CONSOLE_LOG(format, ...) log(format, __VA_ARGS__)

void log(const char* format, ...);

extern std::list<std::string> Logs;

#endif /* __LOG_H__ */