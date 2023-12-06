#ifndef __Godoctopus_FileHeader__
#define __Godoctopus_FileHeader__

#include <thread>
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

namespace godot {

/// @brief This class contains all information conainted in the header of the save file
/// - Json Level file to load (empty if none)
/// - Json model file to load (empty if none)
class FileHeader : public Node {
    GDCLASS(FileHeader, Node)

public:
    ~FileHeader();

    void set_version(int version_p);
    int get_version() const;

    void set_level_filename(String const &filename_p);
    String const & get_level_filename() const;

    void set_model_filename(String const &filename_p);
    String const & get_model_filename() const;

    void set_campaign_id(String const &campaign_id_p);
    String const &get_campaign_id() const;

    void set_environment_id(int env_p);
    int get_environment_id() const;

    void set_num_players(int num_players_p);
    int get_num_players() const;

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

private:
    /// @brief version id used to be able to parse old version (for example if parameters have been added or removed)
    int _version = 0;
    /// @brief path to the json level to be read in Godot code
    String _level_name = "";
    /// @brief path the the json model to be read
    String _model_name = "";
    /// @brief identifier of the campaing linked to this game
    String _campaing_id = "";
    /// @brief environment id of the save
    int _environment_id = 0;
    /// @brief number of players registered in this save
    int _num_players = 1;
};

void loadFromStream(FileHeader &file_header_p, std::istream &file_p);
void saveToStream(FileHeader const &file_header_p, std::ostream &file_p);
void saveDebugToStream(FileHeader const &file_header_p, std::ostream &file_p);

}

#endif //__Godoctopus_FileHeader__
