#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

#include "library/levels/ArenaLevel.hh"
#include "library/levels/LevelId.hh"
#include "library/levels/MazeLevel.hh"
#include "library/levels/WaveLevel.hh"
#include "library/levels/showcase/AoeShowcase.hh"
#include "library/levels/showcase/ChainingShowcase.hh"
#include "library/levels/showcase/DotShowcase.hh"
#include "library/levels/showcase/LifestealShowcase.hh"

// octopus
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/building/BuildingCancelCommand.hh"
#include "controller/Controller.hh"
#include "logger/Logger.hh"
#include "serialization/CommandSerialization.hh"
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
#include "levels/Level1.h"

namespace godot {

Controller::~Controller()
{
    if(_controllerThread)
    {
        _controllerThread->join();
    }
    delete _autoSaveFile;
    delete _controllerThread;
    delete _controller;
    delete _rand;
}

void Controller::_process(double delta)
{
    Node::_process(delta);

    if(_controller)
    {
        octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
        _state = stateAndSteps_l._state;

        _over |= _state->isOver();
        _paused |= _state->isOver();

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

        if(_over && _state->hasWinningTeam())
        {
            emit_signal("over", int(_state->getWinningTeam()));
        }
    }
}

void Controller::load_wave_level(int playerWave_p, int stepCount_p)
{
    delete _rand;
    _rand = new octopus::RandomGenerator(42);
    std::list<octopus::Steppable *> spawners_l = WaveLevelSteps(_lib, *_rand, 10, stepCount_p, playerWave_p, 250);
    std::list<octopus::Command *> commands_l = WaveLevelCommands(_lib, *_rand, 250);
    init(commands_l, spawners_l);
}

void Controller::load_arena_level(TypedArray<int> const &size_you_p, TypedArray<int> const &size_them_p, TypedArray<String> const &model_you_p, TypedArray<String> const &model_them_p)
{
    std::vector<ArenaInfo> you_l;
    std::vector<ArenaInfo> them_l;
    for(size_t i = 0 ; i < size_you_p.size() && i < model_you_p.size() ; ++ i)
    {
        String str_l = model_you_p[i];
        you_l.push_back({size_you_p[i], std::string(str_l.utf8().get_data())});
    }
    for(size_t i = 0 ; i < size_them_p.size() && i < model_them_p.size() ; ++ i)
    {
        String str_l = model_them_p[i];
        them_l.push_back({size_them_p[i], std::string(str_l.utf8().get_data())});
    }
    std::list<octopus::Steppable *> spawners_l = ArenaLevelSteps(_lib, you_l, them_l);
    std::list<octopus::Command *> commands_l = ArenaLevelCommands(_lib);
    // enable auto save
    newAutoSaveFile();
    writeLevelId(*_autoSaveFile, LEVEL_ID_ARENA, 10);
    writeArenaLevelHeader(*_autoSaveFile, you_l, them_l);
    // init with autosave
    init(commands_l, spawners_l, 10, _autoSaveFile);
}

void Controller::load_kamikaze_level(int you_p, int them_p, bool fast_p)
{
    std::list<octopus::Steppable *> spawners_l = ArenaKamikazeSteps(_lib, you_p, them_p, fast_p);
    std::list<octopus::Command *> commands_l = ArenaLevelCommands(_lib);
    // enable auto save
    newAutoSaveFile();
    writeLevelId(*_autoSaveFile, LEVEL_ID_ARENA_KAMIKAZE, 10);
    writeArenaKamikazeHeader(*_autoSaveFile, you_p, them_p, fast_p);
    // init with autosave
    init(commands_l, spawners_l, 10, _autoSaveFile);
}

void Controller::load_maze_level(int size_p)
{
    std::list<octopus::Steppable *> spawners_l = MazeLevelSteps(_lib, size_p);
    std::list<octopus::Command *> commands_l = MazeLevelCommands(_lib);
    // enable auto save
    newAutoSaveFile();
    writeLevelId(*_autoSaveFile, LEVEL_ID_MAZE, 50);
    writeMazeLevelHeader(*_autoSaveFile, size_p);
    init(commands_l, spawners_l, 50, _autoSaveFile);
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

void Controller::load_level1(int seed_p, int nb_wave_p)
{
    delete _rand;
    _rand = new octopus::RandomGenerator(seed_p);
    std::list<octopus::Steppable *> spawners_l = level1::WaveLevelSteps(_lib, *_rand, nb_wave_p, 3*60*100, 150);
    std::list<octopus::Command *> commands_l = level1::WaveLevelCommands(_lib, *_rand, 150);
    // enable auto save
    newAutoSaveFile();
    writeLevelId(*_autoSaveFile, LEVEL_ID_LEVEL_1, 50);
    level1::writeWaveLevelHeader(*_autoSaveFile, seed_p, nb_wave_p, 3*60*100, 150);
    init(commands_l, spawners_l, 50, _autoSaveFile);
}

void Controller::replay_level(String const &filename_p)
{
    std::string filename_l(filename_p.utf8().get_data());
    std::ifstream file_l(filename_l, std::ios::in | std::ios::binary);

    size_t levelId_l;
    size_t size_l;
    file_l.read((char*)&levelId_l, sizeof(levelId_l));
    file_l.read((char*)&size_l, sizeof(size_l));

    bool valid_l = true;
    std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > levelInfo_l;
    if(levelId_l == LEVEL_ID_ARENA)
    {
        levelInfo_l = readArenaLevelHeader(_lib, file_l);
    }
    else if(levelId_l == LEVEL_ID_ARENA_KAMIKAZE)
    {
        levelInfo_l = readArenaKamikazeHeader(_lib, file_l);
    }
    else if(levelId_l == LEVEL_ID_MAZE)
    {
        levelInfo_l = readMazeLevelHeader(_lib, file_l);
    }
    else if(levelId_l == LEVEL_ID_LEVEL_1)
    {
        levelInfo_l = level1::readWaveLevelHeader(_lib, file_l, _rand);
    }
    else
    {
        valid_l = false;
    }

    if(valid_l)
    {
        init_replay(levelInfo_l.second, levelInfo_l.first, size_l, file_l);
    }
}

void Controller::init(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, size_t size_p, std::ofstream *file_p)
{
    UtilityFunctions::print("init controller...");
    delete _controller;
	_controller = new octopus::Controller(spawners_p, 0.01, commands_p, 5, size_p);
	_controller->enableORCA();
    UtilityFunctions::print("done");

    if(file_p)
    {
        UtilityFunctions::print("autosave enabled");
        // neeed nb of steps in header of the file
        size_t header_l = 0;
        file_p->write((char*)&header_l, sizeof(header_l));
        _controller->setOnlineSaveFile(file_p);
    }

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

void Controller::init_replay(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, size_t size_p, std::ifstream &file_p)
{
    UtilityFunctions::print("init controller...");
    delete _controller;
	_controller = new octopus::Controller(spawners_p, 0.01, commands_p, 5, size_p);
	_controller->enableORCA();
    UtilityFunctions::print("done");

	octopus::readCommands(file_p, *_controller, _lib);

	_controller->setOngoingStep(1);
	_controller->setReplayMode(true);

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
	octopus::Entity const &entity_l = *_state->getEntity(handle_p);
    if(entity_l._model._isUnit)
    {
        emit_signal("kill_unit", handle_p);
    }
    else
    {
        emit_signal("clear_entity", handle_p);
    }
}

void Controller::hp_change(int handle_p, float ratio_p)
{
    emit_signal("hp_change", handle_p, ratio_p);
}

void Controller::set_pause(bool paused_p)
{
    _paused = paused_p;
}

void Controller::set_over(bool over_p)
{
    _over = over_p;
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

int Controller::get_steps() const
{
    return _controller->getMetrics()._nbStepsCompiled;
}

int Controller::get_team(int player_p) const
{
    return _state->getPlayer(player_p)->_team;
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

bool Controller::is_entity_explored(int handle_p, int player_p) const
{
    octopus::Player const *player_l = _state->getPlayer(player_p);
	octopus::Entity const &entity_l = *_state->getEntity(handle_p);
    return _state->getVisionHandler().isExplored(player_l->_team, entity_l);
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

int Controller::get_nb_options_available(int player_p) const
{
    auto it_l = _optionManagers.find(player_p);
	if(it_l == _optionManagers.end())
    {
        return 0;
    }
    return it_l->second.getCurrentOptionSize();
}

int Controller::get_nb_options_chosen(int player_p) const
{
    auto it_l = _optionManagers.find(player_p);
	if(it_l == _optionManagers.end())
    {
        return 0;
    }
    return it_l->second.getChosenOptionsSize();
}

godot::Option *Controller::get_available_option_you(int idx_p, int player_p) const
{
    godot::Option * opt_l = memnew(Option);
    opt_l->set_option(_optionManagers.at(player_p).getOption(idx_p)._playerOption);
    return opt_l;
}

godot::Option *Controller::get_available_option_them(int idx_p, int player_p) const
{
    godot::Option * opt_l = memnew(Option);
    opt_l->set_option(_optionManagers.at(player_p).getOption(idx_p)._enemyOption);
    return opt_l;
}

godot::Option *Controller::get_chosen_option_you(int idx_p, int player_p) const
{
    godot::Option * opt_l = memnew(Option);
    opt_l->set_option(_optionManagers.at(player_p).getChosenOption(idx_p)._playerOption);
    return opt_l;
}

godot::Option *Controller::get_chosen_option_them(int idx_p, int player_p) const
{
    godot::Option * opt_l = memnew(Option);
    opt_l->set_option(_optionManagers.at(player_p).getChosenOption(idx_p)._enemyOption);
    return opt_l;
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

void Controller::add_move_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p, bool queued_p)
{
    if(!_paused)
    {
        godot::add_move_commands(*_controller, *_state, handles_p, target_p, player_p, queued_p);
    }
}

void Controller::add_move_target_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int handleTarget_p, int player_p, bool queued_p)
{
    if(!_paused)
    {
        godot::add_move_target_commands(*_controller, *_state, handles_p, target_p, handleTarget_p, player_p, queued_p);
    }
}

void Controller::add_attack_move_commands(TypedArray<int> const &handles_p, Vector2 const &target_p, int player_p, bool queued_p)
{
    if(!_paused)
    {
        godot::add_attack_move_commands(*_controller, *_state, handles_p, target_p, player_p, queued_p);
    }
}

void Controller::add_stop_commands(TypedArray<int> const &handles_p, int player_p, bool queued_p)
{
    if(!_paused)
    {
        godot::add_stop_commands(*_controller, *_state, handles_p, player_p, queued_p);
    }
}

void Controller::add_unit_build_command(TypedArray<int> const &handles_p, String const &model_p, int player_p)
{
    if(!_paused)
    {
        godot::add_unit_build_command(*_controller, *_state, _lib, handles_p, model_p, player_p);
    }
}

void Controller::add_unit_build_cancel_command(int handle_p, int index_p, int player_p)
{
    if(!_paused)
    {
        godot::add_unit_build_cancel_command(*_controller, *_state, handle_p, index_p, player_p);
    }
}

void Controller::add_blueprint_command(Vector2 const &target_p, String const &model_p, int player_p)
{
    if(!_paused)
    {
        godot::add_blueprint_command(*_controller, *_state, _lib, target_p, model_p, player_p);
    }
}

void Controller::add_building_cancel_command(int handle_p, int player_p)
{
    if(!_paused && _state->getEntity(handle_p)->_model._isBuilding)
    {
        _controller->commitCommandAsPlayer(new octopus::BuildingCancelCommand(handle_p), player_p);
    }
}

void Controller::add_chose_option_command(int option_p, int player_p)
{
    if(!_paused)
    {
        _controller->commitCommandAsPlayer(getOptionManagers().at(player_p).newCommandFromOption(option_p), player_p);
    }
}

void Controller::_bind_methods()
{
    UtilityFunctions::print("Binding Controller methods");

    ClassDB::bind_method(D_METHOD("load_wave_level", "player_wave", "step_count"), &Controller::load_wave_level);
    ClassDB::bind_method(D_METHOD("load_arena_level", "size_you", "size_them", "model_you", "model_them"), &Controller::load_arena_level);
    ClassDB::bind_method(D_METHOD("load_kamikaze_level", "size_you", "size_them", "fast"), &Controller::load_kamikaze_level);
    ClassDB::bind_method(D_METHOD("load_maze_level", "size"), &Controller::load_maze_level);
    ClassDB::bind_method(D_METHOD("load_aoe_level", "size"), &Controller::load_aoe_level);
    ClassDB::bind_method(D_METHOD("load_chaining_level"), &Controller::load_chaining_level);
    ClassDB::bind_method(D_METHOD("load_dot_level", "size"), &Controller::load_dot_level);
    ClassDB::bind_method(D_METHOD("load_lifesteal_level", "size"), &Controller::load_lifesteal_level);
    ClassDB::bind_method(D_METHOD("load_level1", "seed", "nb_wave"), &Controller::load_level1);

    ClassDB::bind_method(D_METHOD("replay_level", "filename"), &Controller::replay_level);

    ClassDB::bind_method(D_METHOD("has_state"), &Controller::has_state);
    ClassDB::bind_method(D_METHOD("set_pause", "pause"), &Controller::set_pause);
    ClassDB::bind_method(D_METHOD("set_over", "over"), &Controller::set_over);
    ClassDB::bind_method(D_METHOD("get_models", "handle", "player"), &Controller::get_models);
    ClassDB::bind_method(D_METHOD("is_building", "handle"), &Controller::is_building);
    ClassDB::bind_method(D_METHOD("get_world_size"), &Controller::get_world_size);
    ClassDB::bind_method(D_METHOD("get_steps"), &Controller::get_steps);
    ClassDB::bind_method(D_METHOD("get_team", "player"), &Controller::get_team);

    ClassDB::bind_method(D_METHOD("get_steel", "player"), &Controller::get_steel);
    ClassDB::bind_method(D_METHOD("get_food", "player"), &Controller::get_food);
    ClassDB::bind_method(D_METHOD("get_gas", "player"), &Controller::get_gas);
    ClassDB::bind_method(D_METHOD("get_anchor", "player"), &Controller::get_anchor);
    ClassDB::bind_method(D_METHOD("get_ether", "player"), &Controller::get_ether);
    ClassDB::bind_method(D_METHOD("is_visible", "x", "y", "player"), &Controller::is_visible);
    ClassDB::bind_method(D_METHOD("is_unit_visible", "handle", "player"), &Controller::is_unit_visible);
    ClassDB::bind_method(D_METHOD("is_explored", "x", "y", "player"), &Controller::is_explored);
    ClassDB::bind_method(D_METHOD("is_entity_explored", "handle", "player"), &Controller::is_entity_explored);
    ClassDB::bind_method(D_METHOD("getVisibility", "player"), &Controller::getVisibility);
    ClassDB::bind_method(D_METHOD("get_nb_options_available", "player"), &Controller::get_nb_options_available);
    ClassDB::bind_method(D_METHOD("get_nb_options_chosen", "player"), &Controller::get_nb_options_chosen);
    ClassDB::bind_method(D_METHOD("get_available_option_you", "idx", "player"), &Controller::get_available_option_you);
    ClassDB::bind_method(D_METHOD("get_available_option_them", "idx", "player"), &Controller::get_available_option_them);
    ClassDB::bind_method(D_METHOD("get_chosen_option_you", "idx", "player"), &Controller::get_chosen_option_you);
    ClassDB::bind_method(D_METHOD("get_chosen_option_them", "idx", "player"), &Controller::get_chosen_option_them);

    ClassDB::bind_method(D_METHOD("get_productions", "handles", "max"), &Controller::get_productions);
    ClassDB::bind_method(D_METHOD("get_visible_units", "player", "ent_registered_p"), &Controller::get_visible_units);

    ClassDB::bind_method(D_METHOD("add_move_commands", "handles", "target", "player", "queued"), &Controller::add_move_commands);
    ClassDB::bind_method(D_METHOD("add_move_target_commands", "handles", "target", "handle_target", "player", "queued"), &Controller::add_move_target_commands);
    ClassDB::bind_method(D_METHOD("add_attack_move_commands", "handles", "target", "player", "queued"), &Controller::add_attack_move_commands);
    ClassDB::bind_method(D_METHOD("add_stop_commands", "handles", "player", "queued"), &Controller::add_stop_commands);
    ClassDB::bind_method(D_METHOD("add_unit_build_command", "handle", "model", "player"), &Controller::add_unit_build_command);
    ClassDB::bind_method(D_METHOD("add_unit_build_cancel_command", "handle", "index", "player"), &Controller::add_unit_build_cancel_command);
    ClassDB::bind_method(D_METHOD("add_blueprint_command", "target", "model", "player"), &Controller::add_blueprint_command);
    ClassDB::bind_method(D_METHOD("add_building_cancel_command", "handle", "player"), &Controller::add_building_cancel_command);
    ClassDB::bind_method(D_METHOD("add_chose_option_command", "option_p", "player"), &Controller::add_chose_option_command);

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
    ADD_SIGNAL(MethodInfo("option_update"));
    ADD_SIGNAL(MethodInfo("pop_option"));
	ADD_SIGNAL(MethodInfo("remove_rally_point", PropertyInfo(Variant::INT, "handle")));
	ADD_SIGNAL(MethodInfo("set_rally_point", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::VECTOR2, "pos")));
	ADD_SIGNAL(MethodInfo("set_rally_point_target", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::INT, "target")));

    ADD_SIGNAL(MethodInfo("set_camera", PropertyInfo(Variant::INT, "x"), PropertyInfo(Variant::INT, "y")));
    ADD_SIGNAL(MethodInfo("spawn_dialog", PropertyInfo(Variant::STRING, "model")));
    ADD_SIGNAL(MethodInfo("wave"));

    ADD_SIGNAL(MethodInfo("over", PropertyInfo(Variant::INT, "winning_team")));

    ADD_SIGNAL(MethodInfo("production_command", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::STRING, "model"), PropertyInfo(Variant::FLOAT, "progress")));
}

std::map<unsigned long, OptionManager> &Controller::getOptionManagers()
{
    return _optionManagers;
}
std::map<unsigned long, OptionManager> const &Controller::getOptionManagers() const
{
    return _optionManagers;
}

void Controller::newAutoSaveFile()
{
    if(_autoSaveFile)
    {
        delete _autoSaveFile;
    }
    _autoSaveFile = new std::ofstream("autosave.fas", std::ios::out | std::ios::binary);
}

}
