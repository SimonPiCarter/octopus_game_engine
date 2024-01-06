#include "LevelId.hh"

void writeLevelId(std::ofstream &file_p, uint32_t const &id_p, uint32_t const &size_p)
{
    file_p.write((char*)&id_p, sizeof(id_p));
    file_p.write((char*)&size_p, sizeof(size_p));
}
