#include "Logger.hh"

namespace octopus
{
bool Logger::_debug = false;

std::ostream &Logger::getNormal()
{
	std::cout.clear();
	return std::cout;
}

std::ostream &Logger::getDebug()
{
	if(!Logger::_debug)
	{
		std::cout.setstate(std::ios_base::badbit);
	}
	return std::cout;
}

}

