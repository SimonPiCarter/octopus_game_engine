#include "Handle.hh"

using namespace octopus;

std::ostream & operator<<(std::ostream & os_p, Handle const &handle_p)
{
	os_p << handle_p.index <<", "<<(int)handle_p.revision;
	return os_p;
}
