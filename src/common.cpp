#include "common.h"

#include <sys/time.h>
#include <ctime>
#include <chrono>

std::string currentTime()
{
	std::time_t time = std::time(NULL);
	char timeStr[50];
	std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d_%H-%M-%S", std::localtime(&time));
	return timeStr;
}

std::string currentTimeMs()
{
	char timeStr[50];
	struct timeval tv;
	gettimeofday(&tv, NULL);
	std::time_t now = tv.tv_sec;
	struct tm *tm = std::localtime(&now);

	if (tm == nullptr)
		return currentTime();

	sprintf(timeStr, "%04d-%02d-%02d_%02d-%02d-%02d.%03d",
			tm->tm_year + 1900,
			tm->tm_mon + 1,
			tm->tm_mday,
			tm->tm_hour,
			tm->tm_min,
			tm->tm_sec,
			static_cast<int>(tv.tv_usec / 1000));

	return timeStr;
}

int64_t timestamp()
{
	return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}

int64_t timestamp_micro()
{
	return std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
}
