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

void FileHeader::set_version(int version_p)
{
    _version = version_p;
}

int FileHeader::get_version() const
{
    return _version;
}

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

void FileHeader::set_campaign_id(String const &campaign_id_p)
{
    _campaing_id = campaign_id_p;
}

String const &FileHeader::get_campaign_id() const
{
    return _campaing_id;
}

void FileHeader::set_environment_id(int env_p)
{
    _environment_id = env_p;
}

int FileHeader::get_environment_id() const
{
    return _environment_id;
}

void FileHeader::set_num_players(int num_players_p)
{
    _num_players = num_players_p;
}

int FileHeader::get_num_players() const
{
    return _num_players;
}

void FileHeader::set_idx_first_player(int idx_first_player_p)
{
    _idx_first_player = idx_first_player_p;
}

int FileHeader::get_idx_first_player() const
{
    return _idx_first_player;
}

void FileHeader::set_auto_build_disabled(bool auto_build_disabled_p)
{
	_auto_build_disabled = auto_build_disabled_p;
}
bool FileHeader::get_auto_build_disabled() const
{
	return _auto_build_disabled;
}

void FileHeader::set_difficulty_level(int difficulty_level_p)
{
	_difficulty_level = difficulty_level_p;
}
int FileHeader::get_difficulty_level() const
{
	return _difficulty_level;
}

void FileHeader::_bind_methods()
{
    UtilityFunctions::print("Binding FileHeader methods");

    ClassDB::bind_method(D_METHOD("set_version", "version"), &FileHeader::set_version);
    ClassDB::bind_method(D_METHOD("get_version"), &FileHeader::get_version);
    ClassDB::bind_method(D_METHOD("set_level_filename", "level_filename"), &FileHeader::set_level_filename);
    ClassDB::bind_method(D_METHOD("get_level_filename"), &FileHeader::get_level_filename);
    ClassDB::bind_method(D_METHOD("set_model_filename", "model_filename"), &FileHeader::set_model_filename);
    ClassDB::bind_method(D_METHOD("get_model_filename"), &FileHeader::get_model_filename);
    ClassDB::bind_method(D_METHOD("set_campaign_id", "campaign_id"), &FileHeader::set_campaign_id);
    ClassDB::bind_method(D_METHOD("get_campaign_id"), &FileHeader::get_campaign_id);
    ClassDB::bind_method(D_METHOD("set_environment_id", "environment_id"), &FileHeader::set_environment_id);
    ClassDB::bind_method(D_METHOD("get_environment_id"), &FileHeader::get_environment_id);
    ClassDB::bind_method(D_METHOD("set_num_players", "num_players"), &FileHeader::set_num_players);
    ClassDB::bind_method(D_METHOD("get_num_players"), &FileHeader::get_num_players);
    ClassDB::bind_method(D_METHOD("set_idx_first_player", "idx_first_player"), &FileHeader::set_idx_first_player);
    ClassDB::bind_method(D_METHOD("get_idx_first_player"), &FileHeader::get_idx_first_player);
    ClassDB::bind_method(D_METHOD("set_auto_build_disabled", "auto_build_disabled"), &FileHeader::set_auto_build_disabled);
    ClassDB::bind_method(D_METHOD("get_auto_build_disabled"), &FileHeader::get_auto_build_disabled);
    ClassDB::bind_method(D_METHOD("set_difficulty_level", "difficulty_level"), &FileHeader::set_difficulty_level);
    ClassDB::bind_method(D_METHOD("get_difficulty_level"), &FileHeader::get_difficulty_level);

    ADD_GROUP("FileHeader", "FileHeader_");
}

void loadFromStream(FileHeader &file_header_p, std::istream &file_p)
{
	int version_l = 0;          file_p.read((char*)&version_l, sizeof(version_l));
	std::string model_name_l = octopus::readString(file_p);
	std::string level_name_l = octopus::readString(file_p);
	std::string compaign_id_l = octopus::readString(file_p);
	int env_id_l = 0;           file_p.read((char*)&env_id_l, sizeof(env_id_l));
	int num_players_l = 0;      file_p.read((char*)&num_players_l, sizeof(num_players_l));
	int idx_first_player_l = 0;      file_p.read((char*)&idx_first_player_l, sizeof(idx_first_player_l));

    file_header_p.set_version(version_l);
    file_header_p.set_model_filename(model_name_l.c_str());
    file_header_p.set_level_filename(level_name_l.c_str());
    file_header_p.set_campaign_id(compaign_id_l.c_str());
    file_header_p.set_environment_id(env_id_l);
    file_header_p.set_num_players(num_players_l);
    file_header_p.set_idx_first_player(idx_first_player_l);
}

void saveToStream(FileHeader const &file_header_p, std::ostream &file_p)
{
	int version_l = file_header_p.get_version();
	std::string model_name_l = file_header_p.get_model_filename().utf8().get_data();
	std::string level_name_l = file_header_p.get_level_filename().utf8().get_data();
	std::string compaign_id_l = file_header_p.get_campaign_id().utf8().get_data();
	int env_id_l = file_header_p.get_environment_id();
	int num_players_l = file_header_p.get_num_players();
	int idx_first_player_l = file_header_p.get_idx_first_player();

    file_p.write((char*)&version_l, sizeof(version_l));
    octopus::writeString(file_p, model_name_l);
    octopus::writeString(file_p, level_name_l);
    octopus::writeString(file_p, compaign_id_l);
    file_p.write((char*)&env_id_l, sizeof(env_id_l));
    file_p.write((char*)&num_players_l, sizeof(num_players_l));
    file_p.write((char*)&idx_first_player_l, sizeof(idx_first_player_l));
}

void saveDebugToStream(FileHeader const &file_header_p, std::ostream &file_p)
{
	int version_l = file_header_p.get_version();
	std::string model_name_l = file_header_p.get_model_filename().utf8().get_data();
	std::string level_name_l = file_header_p.get_level_filename().utf8().get_data();
	std::string compaign_id_l = file_header_p.get_campaign_id().utf8().get_data();
	int env_id_l = file_header_p.get_environment_id();
	int num_players_l = file_header_p.get_num_players();
	int idx_first_player_l = file_header_p.get_idx_first_player();

    file_p<<version_l<<std::endl;
    file_p<<model_name_l<<std::endl;
    file_p<<level_name_l<<std::endl;
    file_p<<compaign_id_l<<std::endl;
    file_p<<env_id_l<<std::endl;
    file_p<<num_players_l<<std::endl;
    file_p<<idx_first_player_l<<std::endl;
}

}
