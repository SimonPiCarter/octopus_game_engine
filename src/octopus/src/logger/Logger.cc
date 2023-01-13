#include "Logger.hh"

namespace octopus
{

std::ostream &Logger::getNormal()
{
	return std::cout;
}

std::ostream &Logger::getDebug()
{
	return std::cerr;
}

}

