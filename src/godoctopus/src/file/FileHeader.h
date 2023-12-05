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

    void set_level_filename(String const &filename_p);
    String const & get_level_filename() const;
    void set_model_filename(String const &filename_p);
    String const & get_model_filename() const;

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

private:
    String _level_name = "";
    String _model_name = "";
};

void loadFromStream(FileHeader &file_header_p, std::istream &file_p);
void saveToStream(FileHeader const &file_header_p, std::ostream &file_p);
void saveDebugToStream(FileHeader const &file_header_p, std::ostream &file_p);

}

#endif //__Godoctopus_FileHeader__
