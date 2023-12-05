#include "FileHeader.h"
#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

#include <sstream>

#include <utils/Binary.hh>

namespace godot {

FileHeader::~FileHeader()
{}

void FileHeader::set_level_filename(String const &filename_p)
{
    _level_name = filename_p;
}

String const & FileHeader::get_level_filename() const
{
    return _level_name;
}

void FileHeader::set_model_filename(String const &filename_p)
{
    _model_name = filename_p;
}

String const & FileHeader::get_model_filename() const
{
    return _model_name;
}

void FileHeader::_bind_methods()
{
    UtilityFunctions::print("Binding FileHeader methods");

    ClassDB::bind_method(D_METHOD("set_level_filename", "filename"), &FileHeader::set_level_filename);
    ClassDB::bind_method(D_METHOD("get_level_filename"), &FileHeader::get_level_filename);
    ClassDB::bind_method(D_METHOD("set_model_filename", "filename"), &FileHeader::set_model_filename);
    ClassDB::bind_method(D_METHOD("get_model_filename"), &FileHeader::get_model_filename);

    ADD_GROUP("FileHeader", "FileHeader_");
}

void loadFromStream(FileHeader &file_header_p, std::istream &file_p)
{
	std::string model_name_l = octopus::readString(file_p);
	std::string level_name_l = octopus::readString(file_p);

    file_header_p.set_model_filename(model_name_l.c_str());
    file_header_p.set_level_filename(level_name_l.c_str());
}

void saveToStream(FileHeader const &file_header_p, std::ostream &file_p)
{
	std::string model_name_l = file_header_p.get_model_filename().utf8().get_data();
	std::string level_name_l = file_header_p.get_level_filename().utf8().get_data();

    octopus::writeString(file_p, model_name_l);
    octopus::writeString(file_p, level_name_l);
}

void saveDebugToStream(FileHeader const &file_header_p, std::ostream &file_p)
{
	std::string model_name_l = file_header_p.get_model_filename().utf8().get_data();
	std::string level_name_l = file_header_p.get_level_filename().utf8().get_data();

    file_p<<model_name_l<<std::endl;
    file_p<<level_name_l<<std::endl;
}

}
