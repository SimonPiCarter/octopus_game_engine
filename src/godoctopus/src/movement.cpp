#include <movement.h>
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

namespace godot {

void Movement::_process(double_t delta) {
  Node::_process(delta);
  process_movement(delta);
}

void Movement::process_movement(double_t delta) {
    m_Velocity = Vector2(0.0f, 0.0f);
    Input& intutSingleton = *Input::get_singleton();

    if (intutSingleton.is_action_pressed("ui_right")) {
        m_Velocity.x += 1.0f;
    }

    if (intutSingleton.is_action_pressed("ui_left")) {
        m_Velocity.x -= 1.0f;
    }

    if (intutSingleton.is_action_pressed("ui_up")) {
        m_Velocity.y -= 1.0f;

    }

    if (intutSingleton.is_action_pressed("ui_down")) {
        m_Velocity.y += 1.0f;
    }

    set_position(get_position() + (m_Velocity * m_Speed * delta));

    time_emit += delta;
    if (time_emit > 1.0) {
        emit_signal("position_changed", this, get_position());
        time_emit = 0.0;
    }
}

void Movement::_bind_methods() {
    UtilityFunctions::print("Binding methods");

    ClassDB::bind_method(D_METHOD("get_speed"), &Movement::get_speed);
    ClassDB::bind_method(D_METHOD("set_speed", "speed"), &Movement::set_speed);

    ADD_GROUP("Movement", "movement_");
    ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "movement_speed"), "set_speed", "get_speed");

    ADD_SIGNAL(MethodInfo("position_changed", PropertyInfo(Variant::OBJECT, "node"), PropertyInfo(Variant::VECTOR2, "new_pos")));
}

}
