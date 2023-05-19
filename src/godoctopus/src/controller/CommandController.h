#ifndef __Godoctopus_CommandController__
#define __Godoctopus_CommandController__

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

namespace octopus
{
    class Command;
    class Controller;
    class Entity;
    class Library;
    class Player;
    class State;
} // namespace octopus


namespace godot
{

// commands
// move & attack
void add_move_commands(std::list<octopus::Command*> &list_r, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p, bool queued_p);
void add_move_target_commands(std::list<octopus::Command*> &list_r, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int handleTarget_p, int player_p, bool queued_p);
void add_attack_move_commands(std::list<octopus::Command*> &list_r, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p, bool queued_p);
void add_stop_commands(std::list<octopus::Command*> &list_r, octopus::State const &state_p, TypedArray<int> const &handles_p, int player_p, bool queued_p);
// production
void add_unit_build_command(std::list<octopus::Command*> &list_r, octopus::State const &state_p, octopus::Library const &lib_p, TypedArray<int> const &handles_p, String const &model_p, int player_p);
void add_unit_build_cancel_command(std::list<octopus::Command*> &list_r, octopus::State const &state_p, int handle_p, int index_p, int player_p);
// building
void add_blueprint_command(std::list<octopus::Command*> &list_r, octopus::State const &state_p, octopus::Library const &lib_p, Vector2 const &target_p, String const &model_p, int player_p, TypedArray<int> const &builders_p);

} // namespace godot

#endif
