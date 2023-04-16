#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

#include "library/levels/MazeLevel.hh"
#include "library/levels/ArenaLevel.hh"
#include "library/levels/WaveLevel.hh"


// octopus
#include "command/entity/EntityAttackCommand.hh"
#include "command/entity/EntityAttackMoveCommand.hh"
#include "command/entity/EntityBuildingCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "command/unit/UnitDropCommand.hh"
#include "command/unit/UnitHarvestCommand.hh"
#include "controller/Controller.hh"
#include "logger/Logger.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Resource.hh"
#include "state/entity/Unit.hh"
#include "state/player/Player.hh"
#include "state/State.hh"

// godot
#include "controller/ControllerStepVisitor.h"

namespace godot {

Controller::~Controller()
{
    _over = true;
    if(_controllerThread)
    {
        _controllerThread->join();
    }
    delete _controllerThread;
    delete _controller;
}

void Controller::_process(double delta)
{
    Node::_process(delta);

    if(_controller)
    {
        octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
        _state = stateAndSteps_l._state;

        ControllerStepVisitor vis_l(*this, _state);

        if(_first)
        {
            _lastIt = stateAndSteps_l._steps.begin();
            _first = false;

            // visit intial steps
            for(octopus::Steppable const * steppable_l : stateAndSteps_l._initialStep.getSteppable())
            {
                vis_l(steppable_l);
            }
        }

        // Every step missing
        for(auto it_l = _lastIt ; it_l != stateAndSteps_l._stepIt ; ++it_l)
        {
            // Visit every stepapble in the step
            for(octopus::Steppable const * steppable_l : it_l->_step->getSteppable())
            {
                vis_l(steppable_l);
            }
        }
        _lastIt = stateAndSteps_l._stepIt;
    }
}

void Controller::init(int size_p)
{
    UtilityFunctions::print("init");
    UtilityFunctions::print("init steps...");
	// std::list<octopus::Steppable *> spawners_l = ArenaLevelSteps(lib_l, size_p);
	// std::list<octopus::Command *> commands_l = ArenaLevelCommands(lib_l);
	std::list<octopus::Steppable *> spawners_l = WaveLevelSteps(lib_l, rand_l, 15, 3*60*100, 0, 250);
	std::list<octopus::Command *> commands_l = WaveLevelCommands(lib_l, rand_l, 250);
    UtilityFunctions::print("ok");

    UtilityFunctions::print("delete old controller...");
    delete _controller;
    UtilityFunctions::print("init controller...");
	_controller = new octopus::Controller(spawners_l, 0.01, commands_l, 1, 50);
	_controller->enableORCA();
    UtilityFunctions::print("ok");

    octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
    _state = stateAndSteps_l._state;

    delete _controllerThread;
	_controllerThread = new std::thread(&Controller::loop, this);
}

void Controller::loop()
{
	using namespace std::chrono_literals;

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;

    UtilityFunctions::print("Playing...");

	while(!_over)
	{
        if(!_paused)
        {
            // update controller
            _controller->update(elapsed_l);
        }
		while(!_controller->loop_body()) {}

		auto cur_l = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
		elapsed_l = elapsed_seconds_l.count();
		last_l = cur_l;
	}

    UtilityFunctions::print("Over");
}

bool Controller::has_state() const
{
    return _state != nullptr;
}

octopus::Entity const * Controller::getEntity(int handle_p) const
{
    if(!_state)
    {
        UtilityFunctions::print("Cannot get entity with no state");
        return nullptr;
    }
    return _state->getEntity(handle_p);
}

void Controller::spawn(int handle_p)
{
	octopus::Entity const &entity_l = *_state->getEntity(handle_p);
    emit_signal("spawn_unit", handle_p, entity_l._model._id.c_str(), Vector2(octopus::to_double(entity_l._pos.x), octopus::to_double(entity_l._pos.y)), entity_l._model._ray);
}

void Controller::move(int handle_p)
{
	octopus::Entity const &entity_l = *_state->getEntity(handle_p);
    emit_signal("move_unit", handle_p, Vector2(octopus::to_double(entity_l._pos.x), octopus::to_double(entity_l._pos.y)));
}

void Controller::windup(int handle_p)
{
    emit_signal("windup_unit", handle_p);
}

void Controller::kill(int handle_p)
{
    emit_signal("kill_unit", handle_p);
}

void Controller::hp_change(int handle_p, float ratio_p)
{
    emit_signal("hp_change", handle_p, ratio_p);
}

void Controller::set_pause(bool paused_p)
{
    _paused = paused_p;
}

TypedArray<String> Controller::get_models(int handle_p, int player_p) const
{
    TypedArray<String> models_l;
    octopus::Entity const *ent_l = _state->getEntity(handle_p);
	// update
	if(ent_l->_model._isBuilder)
	{
		std::list<octopus::BuildingModel const *> buildingGrid_l = octopus::getAvailableBuildingModels(*_state->getPlayer(player_p));
		for(octopus::BuildingModel const * model_l : buildingGrid_l)
		{
            models_l.push_back(model_l->_id.c_str());
		}

	} else if(ent_l->_model._isBuilding)
	{
		std::list<octopus::UnitModel const *> unitGrid_l = octopus::getAvailableUnitModels(
			static_cast<octopus::BuildingModel const &>(ent_l->_model), *_state->getPlayer(player_p));
		for(octopus::UnitModel const * model_l : unitGrid_l)
		{
            models_l.push_back(model_l->_id.c_str());
		}
	}
    return models_l;
}

void Controller::add_move_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p)
{
    for(size_t i = 0 ; i < handles_p.size() ; ++ i)
    {
        int idx_l = handles_p[i];
        _controller->commitCommandAsPlayer(new octopus::EntityMoveCommand(idx_l, idx_l, octopus::Vector(target_p.x, target_p.y), 0, {octopus::Vector(target_p.x, target_p.y)}, true), player_p);
    }
}

void Controller::add_move_target_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int handleTarget_p, int player_p)
{
    for(size_t i = 0 ; i < handles_p.size() ; ++ i)
    {
        int idx_l = handles_p[i];
        octopus::Vector worldPos_l(target_p.x, target_p.y);

        const octopus::Entity * cur_l = _state->getEntity(idx_l);
		const octopus::Unit * unit_l = dynamic_cast<const octopus::Unit *>(cur_l);
		const octopus::Player * player_l = _state->getPlayer(cur_l->_player);

		bool isStatic_l = cur_l->_model._isStatic;

		if(isStatic_l)
        {
            continue;
        }

        const octopus::Entity * target_l = _state->getEntity(handleTarget_p);
        const octopus::Building * targetBuilding_l = dynamic_cast<const octopus::Building *>(target_l);
        const octopus::Resource * targetResource_l = dynamic_cast<const octopus::Resource *>(target_l);
        const octopus::Player * targetPlayer_l = _state->getPlayer(target_l->_player);

        if(targetResource_l
        && unit_l
        && unit_l->_unitModel._maxQuantity.find(targetResource_l->_type) != unit_l->_unitModel._maxQuantity.end())
        {
            octopus::UnitHarvestCommand * command_l = new octopus::UnitHarvestCommand(
                idx_l,
                idx_l,
                handleTarget_p,
                worldPos_l,
                0,
                {worldPos_l},
                true
            );
            _controller->commitCommandAsPlayer(command_l, player_p);
        }
        else if(target_l && !targetResource_l
        && player_l->_team != targetPlayer_l->_team)
        {
            octopus::EntityAttackCommand * command_l = new octopus::EntityAttackCommand(
                idx_l,
                idx_l,
                handleTarget_p,
                true
            );
            _controller->commitCommandAsPlayer(command_l, player_p);
        }
        else if(targetBuilding_l
        && !targetBuilding_l->isBuilt())
        {
            octopus::EntityBuildingCommand * command_l = new octopus::EntityBuildingCommand(
                idx_l,
                idx_l,
                handleTarget_p,
                worldPos_l,
                0,
                {worldPos_l},
                true
            );
            _controller->commitCommandAsPlayer(command_l, player_p);
        }
        else if(unit_l && targetBuilding_l
        && targetBuilding_l->_buildingModel.isDeposit(unit_l->_typeOfResource)
        && unit_l->_quantityOfResource > 0)
        {
            octopus::UnitDropCommand * command_l = new octopus::UnitDropCommand(
                idx_l,
                idx_l,
                handleTarget_p,
                worldPos_l,
                0,
                {worldPos_l},
                true
            );
            _controller->commitCommandAsPlayer(command_l, player_p);
        }
        else
        {
            octopus::EntityMoveCommand * command_l = new octopus::EntityMoveCommand(
                idx_l,
                idx_l,
                worldPos_l,
                0,
                {worldPos_l},
                true
            );
            _controller->commitCommandAsPlayer(command_l, player_p);
        }
    }
}

void Controller::add_attack_move_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p)
{
    for(size_t i = 0 ; i < handles_p.size() ; ++ i)
    {
        int idx_l = handles_p[i];
        _controller->commitCommandAsPlayer(new octopus::EntityAttackMoveCommand(idx_l, idx_l, octopus::Vector(target_p.x, target_p.y), 0, {octopus::Vector(target_p.x, target_p.y)}, true), player_p);
    }
}

void Controller::_bind_methods()
{
    UtilityFunctions::print("Binding Controller methods");

    ClassDB::bind_method(D_METHOD("init", "size"), &Controller::init);
    ClassDB::bind_method(D_METHOD("has_state"), &Controller::has_state);
    ClassDB::bind_method(D_METHOD("set_pause", "pause"), &Controller::set_pause);
    ClassDB::bind_method(D_METHOD("get_models", "handle", "player"), &Controller::get_models);

    ClassDB::bind_method(D_METHOD("add_move_commands", "handles", "target", "player"), &Controller::add_move_commands);
    ClassDB::bind_method(D_METHOD("add_move_target_commands", "handles", "target", "handle_target", "player"), &Controller::add_move_target_commands);
    ClassDB::bind_method(D_METHOD("add_attack_move_commands", "handles", "target", "player"), &Controller::add_attack_move_commands);

    ADD_GROUP("Controller", "Controller_");

    ADD_SIGNAL(MethodInfo("spawn_unit", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::STRING, "model"), PropertyInfo(Variant::VECTOR2, "pos"), PropertyInfo(Variant::FLOAT, "ray")));
    ADD_SIGNAL(MethodInfo("move_unit", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::VECTOR2, "pos")));
    ADD_SIGNAL(MethodInfo("windup_unit", PropertyInfo(Variant::INT, "handle")));
    ADD_SIGNAL(MethodInfo("kill_unit", PropertyInfo(Variant::INT, "handle")));
    ADD_SIGNAL(MethodInfo("hp_change", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::FLOAT, "ratio")));
    ADD_SIGNAL(MethodInfo("harvest_unit", PropertyInfo(Variant::INT, "handle")));
}

}
