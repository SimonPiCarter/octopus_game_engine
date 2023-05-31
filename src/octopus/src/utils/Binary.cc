#include "Binary.hh"

namespace octopus
{

void writeString(std::ostream &os_p, std::string const str_p)
{
	size_t size_l = str_p.size();
	os_p.write((char*)&size_l, sizeof(size_l));
	os_p.write(&str_p[0], size_l);
}

std::string readString(std::istream &is_p)
{
	std::string str_l;
	size_t size_l;
	is_p.read((char*)&size_l, sizeof(size_l));
	str_l.resize(size_l);
	is_p.read(&str_l[0], size_l);
	return str_l;
}

} // namespace octopus
