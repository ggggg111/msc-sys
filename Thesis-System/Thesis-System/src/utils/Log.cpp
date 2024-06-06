#include <iostream>

#include "gui/Console.h"
#include "Log.h"

void log(const char* format, ...)
{
	static char tmp_string[4096];
	static va_list ap;

	va_start(ap, format);
	vsprintf_s(tmp_string, 4096, format, ap);
	va_end(ap);

	Logs.push_back(std::string(tmp_string));
}