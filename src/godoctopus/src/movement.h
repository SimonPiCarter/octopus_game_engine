#ifndef GDEXTENSION_GAMEPLAY_MOVEMENT_H_
#define GDEXTENSION_GAMEPLAY_MOVEMENT_H_

#include <cstdint>
#include <godot_cpp/godot.hpp>
#include <godot_cpp/classes/node2d.hpp>

namespace godot {

class Movement : public Node2D {
    GDCLASS(Movement, Node2D)

    public:


    // Will be called by Godot when the class is registered
    // Use this to add properties to your class
    static void _bind_methods();

    void _process(double_t delta) override;

    // property setter
    void set_speed(float speed) {
        m_Speed = speed;
    }

    // property getter
    [[nodiscard]] float get_speed() const {
        return m_Speed;
    }

    private:
    Vector2 m_Velocity;

    // This will be a property later (look into _bind_methods)
    float m_Speed = 500.0f;
    void process_movement(double_t delta);


    float time_emit = 0.f;
};

}

#endif //GDEXTENSION_GAMEPLAY_MOVEMENT_H_
