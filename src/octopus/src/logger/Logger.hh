#ifndef __LOGGER__
#define __LOGGER__

#include <mutex>
#include <sstream>
#include <iostream>

namespace octopus
{
	/// @brief Logger handler used to log
	/// it handles mutex
	class LoggerHandler : public std::ostringstream
	{
	public:
		explicit LoggerHandler(std::mutex &mutex_p, bool shouldLog_p);
		LoggerHandler(LoggerHandler const &other_p);

		~LoggerHandler();

	private:
		std::mutex &_mutex;
		bool _shouldLog;
	};

	class Logger
	{
	public:
		static LoggerHandler getNormal();
		static LoggerHandler getDebug();

		static void enable_debug() { _debug = true; }
		static void disable_debug() { _debug = false; }

	private:
		static bool _debug;
		static std::mutex _mutex;
	};
}

#endif
