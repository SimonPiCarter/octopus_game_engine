#pragma once

#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>
#include <godot_cpp/classes/atlas_texture.hpp>

namespace godot {

struct EntityInstance
{
	/// @brief offset to apply to the texture to display it
	Vector2 offset;
	Ref<AtlasTexture> texture;
	bool enabled = true;
};

class EntityDrawer : public Node2D {
	GDCLASS(EntityDrawer, Node2D)

public:
	void add_instance(Vector2 const &pos_p, Vector2 const &offset_p, Ref<AtlasTexture> const & texture_p);

	void update_pos();
	std::vector<Vector2> & getNewPos();
	std::vector<EntityInstance> &getInstances() { return _instances; }

	void _ready() override;
	void _draw() override;
	void _process(double delta_p) override;

	// Will be called by Godot when the class is registered
	// Use this to add properties to your class
	static void _bind_methods();

	void setTimeStep(double timeStep_p) { _timeStep = timeStep_p; }

private:
	std::vector<EntityInstance> _instances;

	/// @brief last position of instances to lerp
	std::vector<Vector2> _newPos;
	std::vector<Vector2> _oldPos;

	/// @brief expected duration of a timestep
	double _timeStep = 0.01;

	/// @brief time sine last position update
	double _elapsedTime = 0.;
};

}
