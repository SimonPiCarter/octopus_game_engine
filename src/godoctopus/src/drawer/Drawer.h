#pragma once


#include <thread>
#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class Controller;

struct bullet
{
	Vector2 pos;
	double speed = 0;
};

class Drawer : public Node2D {
    GDCLASS(Drawer, Node2D)

public:
    void set_texture(Texture *texture_p) { _texture = texture_p; _ref = Ref(_texture); }
    Texture * get_texture() const { return _texture; }

	void set_number(int num) { _number = num; }
	int get_number() const { return _number; }

	Vector2 const &get_bullet_pos(int index) const { return _bullets[index].pos; }

	void update();

	void _ready() override;
	// void _process(double delta) override;
	void move(double delta);
	// void _draw() override;


    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

private:
    Texture * _texture = nullptr;
	Ref<Texture> _ref;

	int _number = 0;

	std::vector<bullet> _bullets;
};

}
