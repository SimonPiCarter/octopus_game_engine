#include "Handle.hh"

std::ostream & operator<<(std::ostream & os_p, octopus::Handle const &handle_p)
{
	os_p << handle_p.index <<", "<<(int)handle_p.revision;
	return os_p;
}
