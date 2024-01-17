#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>

#include <thread>
#include "controller/Controller.hh"

#include "EntityDrawer.h"
#include "AnimationDrawer.h"


namespace octopus
{
	class Library;
} // namespace octopus


namespace godot {

class Controller;

class SimulationNode : public Node2D {
	GDCLASS(SimulationNode, Node2D)

public:
	~SimulationNode();

	void init_demo(
		TypedArray<Vector2i> const &team_1_p, TypedArray<Vector2i> const &team_2_p,
		Ref<AtlasTexture> const & texture_p, Ref<AtlasTexture> const & texture2_p, Ref<SpriteFrames> const & animation_p);
	void trigger_demo(Vector2 const &target, int start, int end);

	void set_over(bool over_p) { _over = over_p; }
	double get_avg_last_compile_time(int numbers_p) const;

	void _ready() override;
	void _process(double delta_p) override;

	EntityDrawer * getEntityDrawer() const;
	void setEntityDrawer(EntityDrawer *entityDrawer_p);

	AnimationDrawer * getAnimationDrawer() const;
	void setAnimationDrawer(AnimationDrawer *AnimationDrawer_p);

	void SimulationNode::pop_wind_up(octopus::Handle const &handle_p);

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

private:
	octopus::Controller * _controller = nullptr;
	octopus::State const * _state = nullptr;
	octopus::Library *_lib = nullptr;
	std::thread * _controllerThread = nullptr;
	std::list<octopus::StepBundle>::const_iterator _lastIt;

	bool _over = false;

	void loop();

	EntityDrawer * _entityDrawer = nullptr;
	AnimationDrawer * _animationDrawer = nullptr;

	/// to be removed at some point
	Ref<SpriteFrames> _animation;
};

}