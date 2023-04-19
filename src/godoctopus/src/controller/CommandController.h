#ifndef __Godoctopus_CommandController__
#define __Godoctopus_CommandController__

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

namespace octopus
{
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
void add_move_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p);
void add_move_target_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int handleTarget_p, int player_p);
void add_attack_move_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p);
void add_stop_commands(octopus::Controller &controller_p, octopus::State const &state_p, TypedArray<int> const &handles_p, int player_p);
// production
void add_unit_build_command(octopus::Controller &controller_p, octopus::State const &state_p, octopus::Library const &lib_p, TypedArray<int> const &handles_p, String const &model_p, int player_p);
void add_unit_build_cancel_command(octopus::Controller &controller_p, octopus::State const &state_p, int handle_p, int index_p, int player_p);
// building
void add_blueprint_command(octopus::Controller &controller_p, octopus::State const &state_p, octopus::Library const &lib_p, Vector2 const &target_p, String const &model_p, int player_p);

} // namespace godot

#endif
