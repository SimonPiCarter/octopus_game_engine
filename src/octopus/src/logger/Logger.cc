#include "Logger.hh"

namespace octopus
{
bool Logger::_debug = false;
std::mutex Logger::_mutex;


LoggerHandler::LoggerHandler(std::mutex &mutex_p, bool shouldLog_p) : _mutex(mutex_p), _shouldLog(shouldLog_p) {}
LoggerHandler::LoggerHandler(LoggerHandler const &other_p) : _mutex(other_p._mutex), _shouldLog(other_p._shouldLog) {}

LoggerHandler::~LoggerHandler()
{
	if(_shouldLog)
	{
		std::lock_guard<std::mutex> lock_l(_mutex);
		std::cout<<this->str();
	}
}

LoggerHandler Logger::getNormal()
{
	return LoggerHandler(_mutex, true);
}

DebugLoggerHandler Logger::getDebug()
{
	return DebugLoggerHandler(_mutex, _debug);
}

}

octopus::LoggerVoider operator<<(octopus::LoggerVoider voider_p, std::ostream& (*pf)(std::ostream&))
{
	return voider_p;
}
