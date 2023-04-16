#ifndef __Godoctopus_Controller__
#define __Godoctopus_Controller__

#include <thread>
#include <list>
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

#include "library/Library.hh"
#include "utils/RandomGenerator.hh"
#include "controller/Controller.hh"

namespace octopus
{
    class Entity;
    class State;
} // namespace octopus


namespace godot {

class Controller : public Node {
    GDCLASS(Controller, Node)

public:
    ~Controller();

    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    void init(int size_p);
    void loop();

    bool has_state() const;

    void _process(double delta) override;

    octopus::Entity const * getEntity(int handle_p) const;

	void spawn(int handle_p);
	void kill(int handle_p);
	void move(int handle_p);
	void windup(int handle_p);
	void hp_change(int handle_p, float ratio_p);

    void set_pause(bool paused_p);

    // commands
    void add_move_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p);
    void add_move_target_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int handleTarget_p, int player_p);
    void add_attack_move_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p);
private:
    octopus::Controller * _controller = nullptr;
    std::thread * _controllerThread = nullptr;
	octopus::Library lib_l;
	octopus::RandomGenerator rand_l {42};

    octopus::State const * _state = nullptr;

    bool _over = false;
    bool _paused = false;

	/// @brief handlers for last step used
	bool _first {true};
	std::list<octopus::StepBundle>::const_iterator _lastIt;
};

}

#endif //__Godoctopus_Controller__
