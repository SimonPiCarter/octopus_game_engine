#include "Logger.hh"

namespace octopus
{
bool Logger::_debug = false;
std::mutex Logger::_mutex;
std::ofstream Logger::_logFile = std::ofstream("octopus.log");


LoggerHandler::LoggerHandler(std::mutex &mutex_p, std::ofstream &logFile_p, bool shouldLog_p) : _mutex(mutex_p), _logFile(logFile_p), _shouldLog(shouldLog_p) {}
LoggerHandler::LoggerHandler(LoggerHandler const &other_p) : _mutex(other_p._mutex), _logFile(other_p._logFile), _shouldLog(other_p._shouldLog) {}

LoggerHandler::~LoggerHandler()
{
	if(_shouldLog)
	{
		std::lock_guard<std::mutex> lock_l(_mutex);
		std::cout<<this->str()<<std::flush;
		_logFile<<this->str()<<std::flush;
	}
}

LoggerHandler Logger::getNormal()
{
	return LoggerHandler(_mutex, _logFile, true);
}

DebugLoggerHandler Logger::getDebug()
{
	return DebugLoggerHandler(_mutex, _logFile, _debug);
}

}

octopus::LoggerVoider operator<<(octopus::LoggerVoider voider_p, std::ostream& (*pf)(std::ostream&))
{
	return voider_p;
}
