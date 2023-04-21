#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

#include "library/levels/ArenaLevel.hh"
#include "library/levels/MazeLevel.hh"
#include "library/levels/WaveLevel.hh"
#include "library/levels/showcase/AoeShowcase.hh"
#include "library/levels/showcase/ChainingShowcase.hh"
#include "library/levels/showcase/DotShowcase.hh"
#include "library/levels/showcase/LifestealShowcase.hh"

// octopus
#include "command/building/BuildingUnitProductionCommand.hh"
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
#include "controller/CommandController.h"
#include "production/ProductionCommand.h"

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

void Controller::load_wave_level(int playerWave_p, int stepCount_p)
{
    std::list<octopus::Steppable *> spawners_l = WaveLevelSteps(_lib, _rand, 10, stepCount_p, playerWave_p, 250);
    std::list<octopus::Command *> commands_l = WaveLevelCommands(_lib, _rand, 250);
    init(commands_l, spawners_l);
}

void Controller::load_arena_level(int size_you_p, int size_them_p, String model_you_p, String model_them_p)
{
    std::string modelIdYou_l(model_you_p.utf8().get_data());
    std::string modelIdThem_l(model_them_p.utf8().get_data());
    std::list<octopus::Steppable *> spawners_l = ArenaLevelSteps(_lib, size_you_p, size_them_p, modelIdYou_l, modelIdThem_l);
    std::list<octopus::Command *> commands_l = ArenaLevelCommands(_lib);
    init(commands_l, spawners_l);
}

void Controller::load_maze_level(int size_p)
{
    std::list<octopus::Steppable *> spawners_l = MazeLevelSteps(_lib, size_p);
    std::list<octopus::Command *> commands_l = MazeLevelCommands(_lib);
    init(commands_l, spawners_l);
}

void Controller::load_aoe_level(int size_p)
{
    std::list<octopus::Steppable *> spawners_l = aoeShowcaseSteps(_lib, size_p);
    std::list<octopus::Command *> commands_l = aoeShowcaseCommands(_lib);
    init(commands_l, spawners_l);
}

void Controller::load_chaining_level()
{
    std::list<octopus::Steppable *> spawners_l = chainingShowcaseSteps(_lib);
    std::list<octopus::Command *> commands_l = chainingShowcaseCommands(_lib);
    init(commands_l, spawners_l);
}

void Controller::load_dot_level(int size_p)
{
    std::list<octopus::Steppable *> spawners_l = dotShowcaseSteps(_lib, size_p);
    std::list<octopus::Command *> commands_l = dotShowcaseCommands(_lib);
    init(commands_l, spawners_l);
}

void Controller::load_lifesteal_level(int size_p)
{
    std::list<octopus::Steppable *> spawners_l = lifestealShowcaseSteps(_lib, size_p);
    std::list<octopus::Command *> commands_l = lifestealShowcaseCommands(_lib);
    init(commands_l, spawners_l);
}

void Controller::init(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p)
{
    UtilityFunctions::print("init controller...");
    delete _controller;
	_controller = new octopus::Controller(spawners_p, 0.01, commands_p, 5, 50);
	_controller->enableORCA();
    UtilityFunctions::print("done");

    octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
    _state = stateAndSteps_l._state;
    _lastIt = stateAndSteps_l._steps.begin();

    ControllerStepVisitor vis_l(*this, _state);
    // visit intial steps
    for(octopus::Steppable const * steppable_l : stateAndSteps_l._initialStep.getSteppable())
    {
        vis_l(steppable_l);
    }

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
            _controller->update(std::min(0.01, elapsed_l));
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

octopus::Player const * Controller::getPlayer(int player_p) const
{
    if(!_state)
    {
        UtilityFunctions::print("Cannot get player with no state");
        return nullptr;
    }
    return _state->getPlayer(player_p);
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

bool Controller::is_building(String const &model_p) const
{
    std::string modelId_l(model_p.utf8().get_data());
    return _lib.hasBuildingModel(modelId_l);
}

int Controller::get_world_size() const
{
    return _state->getWorldSize();
}

float Controller::get_steel(int player_p) const
{
    octopus::Player const *player_l = _state->getPlayer(player_p);
    return octopus::getResource(*player_l, octopus::ResourceType::Steel);
}
float Controller::get_food(int player_p) const
{
    octopus::Player const *player_l = _state->getPlayer(player_p);
    return octopus::getResource(*player_l, octopus::ResourceType::Food);
}
float Controller::get_gas(int player_p) const
{
    octopus::Player const *player_l = _state->getPlayer(player_p);
    return octopus::getResource(*player_l, octopus::ResourceType::Gas);
}
float Controller::get_anchor(int player_p) const
{
    octopus::Player const *player_l = _state->getPlayer(player_p);
    return octopus::getResource(*player_l, octopus::ResourceType::Anchor);
}
float Controller::get_ether(int player_p) const
{
    octopus::Player const *player_l = _state->getPlayer(player_p);
    return octopus::getResource(*player_l, octopus::ResourceType::Ether);
}

bool Controller::is_visible(int x, int y, int player_p) const
{
    octopus::Player const *player_l = _state->getPlayer(player_p);
    return _state->getVisionHandler().isVisible(player_l->_team, x, y);
}

bool Controller::is_unit_visible(int handle_p, int player_p) const
{
    octopus::Player const *player_l = _state->getPlayer(player_p);
	octopus::Entity const &entity_l = *_state->getEntity(handle_p);
    return _state->getVisionHandler().isVisible(player_l->_team, entity_l);
}

bool Controller::is_explored(int x, int y, int player_p) const
{
    octopus::Player const *player_l = _state->getPlayer(player_p);
    return _state->getVisionHandler().isExplored(player_l->_team, x, y);
}

PackedByteArray Controller::getVisibility(int player_p) const
{
    PackedByteArray array_l;
    octopus::Player const *player_l = _state->getPlayer(player_p);
    for(unsigned long i = 0; i < _state->getWorldSize(); ++i)
	{
		for(unsigned long j = 0; j < _state->getWorldSize(); ++j)
		{
			int alpha_l = _state->getVisionHandler().isVisible(player_l->_team, j, i)?255:120;
			if(!_state->getVisionHandler().isExplored(player_l->_team, j, i))
			{
				alpha_l = 0;
			}
			array_l.append(alpha_l);
		}
	}
    return array_l;
}

void Controller::get_productions(TypedArray<int> const &handles_p, int max_p)
{
    std::vector<CommandInfo> vecCommands_l;

    for(size_t i = 0 ; i < handles_p.size() ; ++ i)
    {
        int idx_l = handles_p[i];
		octopus::Entity const * ent_l = _state->getEntity(idx_l);
		if(!ent_l->getQueue().hasCommand())
		{
			continue;
		}
		size_t posInQueue_l = 0;
		auto it_l = ent_l->getQueue().getCurrentCommand();
		while(it_l != ent_l->getQueue().getEnd())
		{
			octopus::BuildingUnitProductionCommand const *cmd_l = dynamic_cast<octopus::BuildingUnitProductionCommand const *>(it_l->_cmd);
			octopus::UnitProductionData const *data_l = dynamic_cast<octopus::UnitProductionData const *>(it_l->_data);
			if(cmd_l && data_l && !data_l->_canceled)
			{
				vecCommands_l.push_back({cmd_l, data_l, it_l->_id, posInQueue_l});
			}
			++it_l;
			++posInQueue_l;
		}
	}

	std::sort(vecCommands_l.begin(), vecCommands_l.end(), CommandSorter());

	for(size_t i = 0 ; i < std::min<size_t>(vecCommands_l.size(), max_p) ; ++i)
	{
        CommandInfo const &info_l = vecCommands_l[i];
        String model_l(info_l.cmd->getModel()._id.c_str());
        float progress_l = 100.f*vecCommands_l[i].data->_progression/vecCommands_l[i].data->_completeTime;
        emit_signal("production_command", int(info_l.cmd->getHandleCommand()), int(info_l.idx), model_l, progress_l);
	}
}

void Controller::get_visible_units(int player_p, int ent_registered_p)
{
    octopus::Player const *player_l = _state->getPlayer(player_p);
    _visibleLastCall.resize(_state->getEntities().size(), true);
	for(octopus::Entity const *ent_l : _state->getEntities())
	{
        // stop when entites are not registered yet in godot
        if(ent_l->_handle >= ent_registered_p)
        {
            break;
        }
		// skip unit out of range
		if(ent_l->_model._isUnit
		&& !_state->getVisionHandler().isVisible(player_l->_team, *ent_l))
		{
            if(_visibleLastCall[ent_l->_handle])
            {
			    emit_signal("hide_unit", int(ent_l->_handle));
                _visibleLastCall[ent_l->_handle] = false;
            }
		}
        else if(ent_l->_alive)
        {
            if(!_visibleLastCall[ent_l->_handle])
            {
                emit_signal("show_unit", int(ent_l->_handle));
                _visibleLastCall[ent_l->_handle] = true;
            }
        }
    }
}

void Controller::add_move_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p)
{
    godot::add_move_commands(*_controller, *_state, handles_p, target_p, player_p);
}

void Controller::add_move_target_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int handleTarget_p, int player_p)
{
    godot::add_move_target_commands(*_controller, *_state, handles_p, target_p, handleTarget_p, player_p);
}

void Controller::add_attack_move_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p)
{
    godot::add_attack_move_commands(*_controller, *_state, handles_p, target_p, player_p);
}

void Controller::add_stop_commands(TypedArray<int> const &handles_p, int player_p)
{
    godot::add_stop_commands(*_controller, *_state, handles_p, player_p);
}

void Controller::add_unit_build_command(TypedArray<int> const &handles_p, String const &model_p, int player_p)
{
    godot::add_unit_build_command(*_controller, *_state, _lib, handles_p, model_p, player_p);
}

void Controller::add_unit_build_cancel_command(int handle_p, int index_p, int player_p)
{
    godot::add_unit_build_cancel_command(*_controller, *_state, handle_p, index_p, player_p);
}

void Controller::add_blueprint_command(Vector2 const &target_p, String const &model_p, int player_p)
{
    godot::add_blueprint_command(*_controller, *_state, _lib, target_p, model_p, player_p);
}

void Controller::_bind_methods()
{
    UtilityFunctions::print("Binding Controller methods");

    ClassDB::bind_method(D_METHOD("load_wave_level", "player_wave", "step_count"), &Controller::load_wave_level);
    ClassDB::bind_method(D_METHOD("load_arena_level", "size_you", "size_them", "model_you", "model_them"), &Controller::load_arena_level);
    ClassDB::bind_method(D_METHOD("load_maze_level", "size"), &Controller::load_maze_level);
    ClassDB::bind_method(D_METHOD("load_aoe_level", "size"), &Controller::load_aoe_level);
    ClassDB::bind_method(D_METHOD("load_chaining_level"), &Controller::load_chaining_level);
    ClassDB::bind_method(D_METHOD("load_dot_level", "size"), &Controller::load_dot_level);
    ClassDB::bind_method(D_METHOD("load_lifesteal_level", "size"), &Controller::load_lifesteal_level);

    ClassDB::bind_method(D_METHOD("has_state"), &Controller::has_state);
    ClassDB::bind_method(D_METHOD("set_pause", "pause"), &Controller::set_pause);
    ClassDB::bind_method(D_METHOD("get_models", "handle", "player"), &Controller::get_models);
    ClassDB::bind_method(D_METHOD("is_building", "handle"), &Controller::is_building);
    ClassDB::bind_method(D_METHOD("get_world_size"), &Controller::get_world_size);

    ClassDB::bind_method(D_METHOD("get_steel", "player"), &Controller::get_steel);
    ClassDB::bind_method(D_METHOD("get_food", "player"), &Controller::get_food);
    ClassDB::bind_method(D_METHOD("get_gas", "player"), &Controller::get_gas);
    ClassDB::bind_method(D_METHOD("get_anchor", "player"), &Controller::get_anchor);
    ClassDB::bind_method(D_METHOD("get_ether", "player"), &Controller::get_ether);
    ClassDB::bind_method(D_METHOD("is_visible", "x", "y", "player"), &Controller::is_visible);
    ClassDB::bind_method(D_METHOD("is_unit_visible", "handle", "player"), &Controller::is_unit_visible);
    ClassDB::bind_method(D_METHOD("is_explored", "x", "y", "player"), &Controller::is_explored);
    ClassDB::bind_method(D_METHOD("getVisibility", "player"), &Controller::getVisibility);
    ClassDB::bind_method(D_METHOD("get_productions", "handles", "max"), &Controller::get_productions);
    ClassDB::bind_method(D_METHOD("get_visible_units", "player", "ent_registered_p"), &Controller::get_visible_units);

    ClassDB::bind_method(D_METHOD("add_move_commands", "handles", "target", "player"), &Controller::add_move_commands);
    ClassDB::bind_method(D_METHOD("add_move_target_commands", "handles", "target", "handle_target", "player"), &Controller::add_move_target_commands);
    ClassDB::bind_method(D_METHOD("add_attack_move_commands", "handles", "target", "player"), &Controller::add_attack_move_commands);
    ClassDB::bind_method(D_METHOD("add_stop_commands", "handles", "player"), &Controller::add_stop_commands);
    ClassDB::bind_method(D_METHOD("add_unit_build_command", "handle", "model", "player"), &Controller::add_unit_build_command);
    ClassDB::bind_method(D_METHOD("add_unit_build_cancel_command", "handle", "index", "player"), &Controller::add_unit_build_cancel_command);
    ClassDB::bind_method(D_METHOD("add_blueprint_command", "target", "model", "player"), &Controller::add_blueprint_command);

    ADD_GROUP("Controller", "Controller_");

    ADD_SIGNAL(MethodInfo("spawn_unit", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::STRING, "model"), PropertyInfo(Variant::VECTOR2, "pos"), PropertyInfo(Variant::FLOAT, "ray")));
    ADD_SIGNAL(MethodInfo("move_unit", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::VECTOR2, "pos")));
    ADD_SIGNAL(MethodInfo("windup_unit", PropertyInfo(Variant::INT, "handle")));
    ADD_SIGNAL(MethodInfo("kill_unit", PropertyInfo(Variant::INT, "handle")));
    ADD_SIGNAL(MethodInfo("clear_entity", PropertyInfo(Variant::INT, "handle")));
    ADD_SIGNAL(MethodInfo("hp_change", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::FLOAT, "ratio")));
    ADD_SIGNAL(MethodInfo("harvest_unit", PropertyInfo(Variant::INT, "handle")));
    ADD_SIGNAL(MethodInfo("hide_unit", PropertyInfo(Variant::INT, "handle")));
    ADD_SIGNAL(MethodInfo("show_unit", PropertyInfo(Variant::INT, "handle")));
    ADD_SIGNAL(MethodInfo("build", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::FLOAT, "progress")));

    ADD_SIGNAL(MethodInfo("production_command", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::STRING, "model"), PropertyInfo(Variant::FLOAT, "progress")));
}

}
