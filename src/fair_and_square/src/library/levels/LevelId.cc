#include "LevelId.hh"

void writeLevelId(std::ofstream &file_p, size_t const &id_p, size_t const &size_p)
{
    file_p.write((char*)&id_p, sizeof(id_p));
    file_p.write((char*)&size_p, sizeof(size_p));
}
