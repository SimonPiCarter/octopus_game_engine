#ifndef __Binary__
#define __Binary__

#include <string>
#include <iostream>

namespace octopus
{

void writeString(std::ostream &os_p, std::string const str_p);
std::string readString(std::istream &is_p);

} // namespace octopus


#endif