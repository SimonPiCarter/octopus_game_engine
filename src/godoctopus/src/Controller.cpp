#include "Controller.h"
#include <godot_cpp/core/class_db.hpp>
#include <godot_cpp/variant/utility_functions.hpp>
#include <godot_cpp/classes/input.hpp>

#include "library/model/ModelLoader.hh"
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
#include "serialization/StateDump.hh"
#include "state/entity/Building.hh"
#include "state/entity/Entity.hh"
#include "state/entity/Resource.hh"
#include "state/entity/Unit.hh"
#include "state/entity/buff/TimedBuff.hh"
#include "state/model/entity/TempleModel.hh"
#include "state/player/Player.hh"
#include "state/player/upgrade/Upgrade.hh"
#include "state/State.hh"
#include "step/player/PlayerSpawnStep.hh"
#include "utils/Binary.hh"
#include "utils/Version.hh"

// godot
#include "controller/ControllerStepVisitor.h"
#include "controller/CommandController.h"
#include "production/ProductionCommand.h"
#include "levels/duel/DuelLevel.h"
#include "levels/hero_siege/HeroSiege.h"
#include "levels/Level1.h"
#include "levels/Level2.h"
#include "levels/LevelTestAnchor.h"
#include "levels/LevelTestModelLoader.h"
#include "levels/missions/mission1/Mission1.h"
#include "levels/missions/mission2/Mission2.h"
#include "levels/demo/DemoLevel.h"
#include "levels/model/utils/EntitySpawner.h"

namespace godot {

template<typename T>
std::vector<T> toVector(TypedArray<T> const &array_p)
{
	std::vector<T> vec_l;

	for(size_t i = 0 ; i < array_p.size() ; ++ i)
	{
		vec_l.push_back(array_p[i]);
	}

	return vec_l;
}

Controller::~Controller()
{
	if(_controllerThread)
	{
		_controllerThread->join();
	}
	delete _autoSaveFile;
	delete _autoSaveFileDebug;
	delete _controllerThread;
	delete _controller;
	delete _rand;
	for(auto &&pair_l : _optionManagers)
	{
		delete pair_l.second;
	}
}

void Controller::_process(double delta)
{
	Node::_process(delta);

	if(_controller && _initDone)
	{
		octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
		_state = stateAndSteps_l._state;

		_over |= _state->isOver();
		_paused |= _state->isOver();

		// Every step missing
		for(auto it_l = _lastIt ; it_l != stateAndSteps_l._stepIt ; ++it_l)
		{
			applyControllerStepVisitor(*this, *_state, *it_l->_step);
		}
		_lastIt = stateAndSteps_l._stepIt;
		_controller->setExternalMin(_lastIt->_step->getId());
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
	std::list<octopus::Steppable *> spawners_l = ArenaLevelSteps(_lib, you_l, them_l, true);
	std::list<octopus::Command *> commands_l = ArenaLevelCommands(_lib);
	// enable auto save
	newAutoSaveFile();
	writeLevelId(*_autoSaveFile, LEVEL_ID_ARENA, 10);
	_currentLevel = LEVEL_ID_ARENA;
	_headerWriter = std::bind(writeArenaLevelHeader, std::placeholders::_1, ArenaLevelHeader {you_l, them_l});
	_headerWriter(*_autoSaveFile);
	// init with autosave
	init(commands_l, spawners_l, false, 10, _autoSaveFile);
}

void Controller::load_kamikaze_level(int you_p, int them_p, bool fast_p)
{
	std::list<octopus::Steppable *> spawners_l = ArenaKamikazeSteps(_lib, you_p, them_p, fast_p);
	std::list<octopus::Command *> commands_l = ArenaLevelCommands(_lib);
	// enable auto save
	newAutoSaveFile();
	writeLevelId(*_autoSaveFile, LEVEL_ID_ARENA_KAMIKAZE, 10);
	_currentLevel = LEVEL_ID_ARENA_KAMIKAZE;
	_headerWriter = std::bind(writeArenaKamikazeHeader, std::placeholders::_1, KamikazeHeader {size_t(you_p), size_t(them_p), fast_p});
	_headerWriter(*_autoSaveFile);
	// init with autosave
	init(commands_l, spawners_l, false, 10, _autoSaveFile);
}

void Controller::load_maze_level(int size_p)
{
	std::list<octopus::Steppable *> spawners_l = MazeLevelSteps(_lib, size_p);
	std::list<octopus::Command *> commands_l = MazeLevelCommands(_lib);
	// enable auto save
	newAutoSaveFile();
	writeLevelId(*_autoSaveFile, LEVEL_ID_MAZE, 50);
	_currentLevel = LEVEL_ID_MAZE;
	_headerWriter = std::bind(writeMazeLevelHeader, std::placeholders::_1, size_p);
	_headerWriter(*_autoSaveFile);
	init(commands_l, spawners_l, false, 50, _autoSaveFile);
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

void Controller::load_mission_1(int seed_p, int player_count_p)
{
	delete _rand;
	_rand = new octopus::RandomGenerator(seed_p);

	std::list<octopus::Steppable *> spawners_l = mission::Mission1Steps(_lib, *_rand, player_count_p);
	std::list<octopus::Command *> commands_l = mission::Mission1Commands(_lib, *_rand, player_count_p);
	init(commands_l, spawners_l);
}

void Controller::load_mission_2(int seed_p, godot::LevelModel *level_model_p, int player_count_p)
{
	delete _rand;
	_rand = new octopus::RandomGenerator(seed_p);

	assert(level_model_p);
	std::vector<GodotEntityInfo> info_l = getEntityInfo(level_model_p->getEntities(), player_count_p);

	std::list<octopus::Steppable *> spawners_l = {};
	std::list<octopus::Steppable *> levelsteps_l = mission::Mission2Steps(_lib, *_rand, player_count_p, info_l);
	spawners_l = level_model_p->generateLevelSteps(_lib, player_count_p);
	spawners_l.splice(spawners_l.end(), levelsteps_l);

	std::list<octopus::Command *> commands_l = mission::Mission2Commands(_lib, *_rand, player_count_p);

	init(commands_l, spawners_l);
}

void Controller::load_minimal_model()
{
	loadMinimalModels(_lib);
	fas::loadLibrary(_lib);
}

void Controller::load_hero_siege_level(int seed_p, int player_count_p)
{
	delete _rand;
	_rand = new octopus::RandomGenerator(seed_p);

	std::list<octopus::Steppable *> spawners_l = hero_siege::HeroSiegeLevelSteps(_lib, *_rand, player_count_p);
	std::list<octopus::Command *> commands_l = hero_siege::HeroSiegeLevelCommands(_lib, *_rand, player_count_p);

	// enable auto save
	newAutoSaveFile();
	writeLevelId(*_autoSaveFile, LEVEL_ID_HEROSIEGE, 50);
	_currentLevel = LEVEL_ID_HEROSIEGE;
	_headerWriter = std::bind(hero_siege::writeHeroSiegeLevelHeader, std::placeholders::_1, hero_siege::HeroSiegeLevelHeader{seed_p, (unsigned long)player_count_p});
	_headerWriter(*_autoSaveFile);
	init(commands_l, spawners_l, false, 50, _autoSaveFile);
}

void Controller::load_demo_level(int seed_p, WavePattern const * wavePattern_p, godot::LevelModel *level_model_p, int player_count_p, int difficulty_p)
{
	assert(level_model_p);
	std::vector<GodotEntityInfo> info_l = getEntityInfo(level_model_p->getEntities(), player_count_p);

	delete _rand;
	_rand = new octopus::RandomGenerator(seed_p);
	std::vector<WavePoolInfo> wavesInfo_l;
	// add all patterns
	for(WavePool const * pool_l : wavePattern_p->getWavePools())
	{
		wavesInfo_l.push_back(convertToInfo(pool_l));
	}

	if(wavePattern_p->getPlayer() < 0)
	{
		UtilityFunctions::push_error("Cannot load demo level because player index < 0");
		return;
	}
	unsigned long player_l = wavePattern_p->getPlayer();

	std::list<octopus::Steppable *> spawners_l = {};
	std::list<octopus::Steppable *> levelsteps_l = demo::DemoLevelSteps(_lib, *_rand, wavesInfo_l, player_l, player_count_p, info_l, difficulty_p);
	spawners_l = level_model_p->generateLevelSteps(_lib, player_count_p);
	spawners_l.splice(spawners_l.end(), levelsteps_l);

	std::list<octopus::Command *> commands_l = demo::DemoLevelCommands(_lib, *_rand, player_count_p, difficulty_p);
	// enable auto save
	newAutoSaveFile();
	writeLevelId(*_autoSaveFile, LEVEL_ID_LEVEL_DEMO, 50);
	_currentLevel = LEVEL_ID_LEVEL_DEMO;
	_headerWriter = std::bind(demo::writeDemoLevelHeader, std::placeholders::_1, demo::DemoLevelHeader{seed_p, player_l, difficulty_p, player_count_p, wavesInfo_l});
	_headerWriter(*_autoSaveFile);
	init(commands_l, spawners_l, false, 50, _autoSaveFile);
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
	_currentLevel = LEVEL_ID_LEVEL_1;
	_headerWriter = std::bind(level1::writeWaveLevelHeader, std::placeholders::_1, level1::WaveLevelHeader {seed_p, (unsigned long)nb_wave_p, 3*60*100, 150});
	_headerWriter(*_autoSaveFile);
	init(commands_l, spawners_l, false, 50, _autoSaveFile);
}

void Controller::load_level2(int seed_p, WavePattern const * wavePattern_p, int nb_players_p)
{
	delete _rand;
	_rand = new octopus::RandomGenerator(seed_p);
	std::vector<WavePoolInfo> wavesInfo_l;
	// add all patterns
	for(WavePool const * pool_l : wavePattern_p->getWavePools())
	{
		wavesInfo_l.push_back(convertToInfo(pool_l));
	}

	if(wavePattern_p->getPlayer() < 0)
	{
		UtilityFunctions::push_error("Cannot load level 2 because player index < 0");
		return;
	}
	unsigned long player_l = wavePattern_p->getPlayer();

	std::list<octopus::Steppable *> spawners_l = level2::WaveLevelSteps(_lib, *_rand, wavesInfo_l, player_l, nb_players_p);
	std::list<octopus::Command *> commands_l = level2::WaveLevelCommands(_lib, *_rand, nb_players_p);
	// enable auto save
	newAutoSaveFile();
	writeLevelId(*_autoSaveFile, LEVEL_ID_LEVEL_2, 50);
	_currentLevel = LEVEL_ID_LEVEL_2;
	_headerWriter = std::bind(level2::writeWaveLevelHeader, std::placeholders::_1, level2::WaveLevelHeader{seed_p, player_l, wavesInfo_l});
	_headerWriter(*_autoSaveFile);
	init(commands_l, spawners_l, false, 50, _autoSaveFile);
}

void Controller::load_level_test_anchor(int seed_p)
{
	delete _rand;
	_rand = new octopus::RandomGenerator(seed_p);
	std::list<octopus::Steppable *> spawners_l = level_test_anchor::LevelSteps(_lib, *_rand);
	std::list<octopus::Command *> commands_l = level_test_anchor::LevelCommands(_lib, *_rand);
	// enable auto save
	newAutoSaveFile();
	writeLevelId(*_autoSaveFile, LEVEL_ID_LEVEL_TEST_ANCHOR, 50);
	_currentLevel = LEVEL_ID_LEVEL_TEST_ANCHOR;
	_headerWriter = std::bind(level_test_anchor::writeLevelHeader, std::placeholders::_1, level_test_anchor::TestAnchorHeader {seed_p});
	_headerWriter(*_autoSaveFile);
	init(commands_l, spawners_l, false, 50, _autoSaveFile);
}

void Controller::load_level_test_model_reading(int seed_p, godot::LevelModel *level_model_p, bool buff_prod_p)
{
	delete _rand;
	_rand = new octopus::RandomGenerator(seed_p);
	std::list<octopus::Steppable *> spawners_l = {};
	std::list<octopus::Steppable *> levelsteps_l = level_test_model::LevelSteps(_lib, *_rand, buff_prod_p);
	if(level_model_p)
	{
		spawners_l = level_model_p->generateLevelSteps(_lib, 0);
	}
	spawners_l.splice(spawners_l.end(), levelsteps_l);

	std::list<octopus::Command *> commands_l = level_test_model::LevelCommands(_lib, *_rand);
	// enable auto save
	newAutoSaveFile();
	writeLevelId(*_autoSaveFile, LEVEL_ID_LEVEL_TEST_MODEL, 50);
	_currentLevel = LEVEL_ID_LEVEL_TEST_MODEL;
	_headerWriter = std::bind(level_test_model::writeLevelHeader, std::placeholders::_1, level_test_model::ModelLoaderHeader {seed_p, buff_prod_p});
	_headerWriter(*_autoSaveFile);
	init(commands_l, spawners_l, true, 50, _autoSaveFile);
}

void Controller::load_duel_level(int seed_p, TypedArray<int> const &div_player_1_p, TypedArray<int> const &div_player_2_p)
{
	UtilityFunctions::print("loading duel level with seed ",seed_p);
	delete _rand;
	_rand = new octopus::RandomGenerator(seed_p);
	std::list<octopus::Steppable *> spawners_l = {};
	std::list<octopus::Steppable *> levelsteps_l = duellevel::LevelSteps(_lib, *_rand, 0, false,
			fas::intToDivinityType(toVector(div_player_1_p)), fas::intToDivinityType(toVector(div_player_2_p)));
	spawners_l.splice(spawners_l.end(), levelsteps_l);

	std::list<octopus::Command *> commands_l = duellevel::LevelCommands(_lib, *_rand);
	// enable auto save
	newAutoSaveFile();
	writeLevelId(*_autoSaveFile, LEVEL_ID_DUEL, LEVEL_DUEL_SIZE/5);
	_currentLevel = LEVEL_ID_DUEL;
	_headerWriter = std::bind(duellevel::writeLevelHeader, std::placeholders::_1, duellevel::DuelLevelHeader {seed_p, 0, false});
	_headerWriter(*_autoSaveFile);
	init(commands_l, spawners_l, true, LEVEL_DUEL_SIZE/5, _autoSaveFile);
}

void Controller::load_multi_test_level(int seed_p, int step_cout_p, bool buff_prod_p)
{
	UtilityFunctions::print("loading multi test level with seed ",seed_p);
	delete _rand;
	_rand = new octopus::RandomGenerator(seed_p);
	std::list<octopus::Steppable *> spawners_l = {};
	std::list<octopus::Steppable *> levelsteps_l = duellevel::LevelSteps(_lib, *_rand, step_cout_p, buff_prod_p, {}, {});
	spawners_l.splice(spawners_l.end(), levelsteps_l);

	std::list<octopus::Command *> commands_l = duellevel::LevelCommands(_lib, *_rand);
	// enable auto save
	newAutoSaveFile();
	writeLevelId(*_autoSaveFile, LEVEL_ID_DUEL, LEVEL_DUEL_SIZE/5);
	_currentLevel = LEVEL_ID_DUEL;
	_headerWriter = std::bind(duellevel::writeLevelHeader, std::placeholders::_1, duellevel::DuelLevelHeader {seed_p, step_cout_p, buff_prod_p});
	_headerWriter(*_autoSaveFile);
	init(commands_l, spawners_l, true, LEVEL_DUEL_SIZE/5, _autoSaveFile);
}


void Controller::set_model_filename(String const &filename_p)
{
	_fileHeader.set_model_filename(filename_p);
}

void Controller::set_level_filename(String const &filename_p)
{
	_fileHeader.set_level_filename(filename_p);
}

String Controller::get_model_filename(String const &filename_p)
{
	std::string filename_l(filename_p.utf8().get_data());
	std::ifstream file_l(filename_l, std::ios::in | std::ios::binary);

	FileHeader header_l;
	loadFromStream(header_l, file_l);

	return header_l.get_model_filename();
}

String Controller::get_level_filename(String const &filename_p)
{
	std::string filename_l(filename_p.utf8().get_data());
	std::ifstream file_l(filename_l, std::ios::in | std::ios::binary);

	FileHeader header_l;
	loadFromStream(header_l, file_l);

	return header_l.get_level_filename();
}

void Controller::replay_level(String const &filename_p, bool replay_mode_p, godot::LevelModel *level_model_p)
{
	std::string filename_l(filename_p.utf8().get_data());
	std::ifstream file_l(filename_l, std::ios::in | std::ios::binary);

	loadFromStream(_fileHeader, file_l);

	size_t levelId_l;
	size_t size_l;
	file_l.read((char*)&levelId_l, sizeof(levelId_l));
	file_l.read((char*)&size_l, sizeof(size_l));
	bool divOptionHandler_l = false;

	bool valid_l = true;
	std::pair<std::list<octopus::Steppable *>, std::list<octopus::Command *> > levelInfo_l;
	if(levelId_l == LEVEL_ID_ARENA)
	{
		ArenaLevelHeader header_l;
		levelInfo_l = readArenaLevelHeader(_lib, file_l, header_l);
		_headerWriter = std::bind(writeArenaLevelHeader, std::placeholders::_1, header_l);
	}
	else if(levelId_l == LEVEL_ID_ARENA_KAMIKAZE)
	{
		KamikazeHeader header_l;
		levelInfo_l = readArenaKamikazeHeader(_lib, file_l, header_l);
		_headerWriter = std::bind(writeArenaKamikazeHeader, std::placeholders::_1, header_l);
	}
	else if(levelId_l == LEVEL_ID_MAZE)
	{
		levelInfo_l = readMazeLevelHeader(_lib, file_l);
	}
	else if(levelId_l == LEVEL_ID_LEVEL_1)
	{
		level1::WaveLevelHeader header_l;
		levelInfo_l = level1::readWaveLevelHeader(_lib, file_l, _rand, header_l);
		_headerWriter = std::bind(level1::writeWaveLevelHeader, std::placeholders::_1, header_l);
	}
	else if(levelId_l == LEVEL_ID_LEVEL_2)
	{
		level2::WaveLevelHeader header_l;
		levelInfo_l = level2::readWaveLevelHeader(_lib, file_l, _rand, header_l);
		_headerWriter = std::bind(level2::writeWaveLevelHeader, std::placeholders::_1, header_l);
	}
	else if(levelId_l == LEVEL_ID_LEVEL_TEST_ANCHOR)
	{
		level_test_anchor::TestAnchorHeader header_l;
		levelInfo_l = level_test_anchor::readLevelHeader(_lib, file_l, _rand, header_l);
		_headerWriter = std::bind(level_test_anchor::writeLevelHeader, std::placeholders::_1, header_l);
		divOptionHandler_l = true;
	}
	else if(levelId_l == LEVEL_ID_LEVEL_TEST_MODEL)
	{
		level_test_model::ModelLoaderHeader header_l;
		levelInfo_l = level_test_model::readLevelHeader(_lib, file_l, _rand, header_l);
		_headerWriter = std::bind(level_test_model::writeLevelHeader, std::placeholders::_1, header_l);
		divOptionHandler_l = true;
	}
	else if(levelId_l == LEVEL_ID_DUEL)
	{
		duellevel::DuelLevelHeader header_l;
		levelInfo_l = duellevel::readLevelHeader(_lib, file_l, _rand, header_l);
		_headerWriter = std::bind(duellevel::writeLevelHeader, std::placeholders::_1, header_l);
		divOptionHandler_l = true;
	}
	else if(levelId_l == LEVEL_ID_LEVEL_DEMO)
	{
		std::vector<GodotEntityInfo> info_l = getEntityInfo(level_model_p->getEntities(), 1);

		demo::DemoLevelHeader header_l;
		levelInfo_l = demo::readDemoLevelHeader(_lib, file_l, info_l, _rand, header_l);
		_headerWriter = std::bind(demo::writeDemoLevelHeader, std::placeholders::_1, header_l);
	}
	else
	{
		valid_l = false;
	}

	if(valid_l)
	{

		std::list<octopus::Steppable *> spawners_l;
		if(level_model_p)
		{
			spawners_l = level_model_p->generateLevelSteps(_lib, 0);
		}
		spawners_l.splice(spawners_l.end(), levelInfo_l.first);
		if(replay_mode_p)
		{
			init_replay(levelInfo_l.second, spawners_l, divOptionHandler_l, size_l, file_l);
		}
		else
		{
			init_loading(levelInfo_l.second, spawners_l, divOptionHandler_l, size_l, file_l);
		}
	}
}


godot::FileHeader const * Controller::get_file_header() const
{
	return &_fileHeader;
}

godot::FileHeader const * Controller::read_file_header(String const &filename_p)
{
	std::string filename_l(filename_p.utf8().get_data());
	std::ifstream file_l(filename_l, std::ios::in | std::ios::binary);

	loadFromStream(_fileHeader, file_l);

	return &_fileHeader;
}

void Controller::init(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, bool divOptionManager_p, size_t size_p, std::ofstream *file_p)
{
	UtilityFunctions::print("init controller...");

	for(octopus::Steppable *step_l : spawners_p)
	{
		octopus::PlayerSpawnStep const * player_l = dynamic_cast<octopus::PlayerSpawnStep const *>(step_l);
		if(player_l)
		{
			if(divOptionManager_p)
			{
				_optionManagers[player_l->getPlayerIdx()] = new DivinityOptionManager(player_l->getPlayerIdx());
			}
			else
			{
				_optionManagers[player_l->getPlayerIdx()] = new OptionManager(player_l->getPlayerIdx());
			}
		}
	}

	_initDone = false;
	delete _controller;
	_controller = new octopus::Controller(spawners_p, 0.01, commands_p, 5, size_p);
	_controller->setExternalMin(0);
	_controller->addQueuedLayer();
	_controller->enableORCA();

	if(file_p)
	{
		UtilityFunctions::print("autosave enabled");
		// neeed nb of steps in header of the file
		size_t header_l = 0;
		file_p->write((char*)&header_l, sizeof(header_l));
		_controller->setOnlineSaveFile(file_p);
		_controller->setOnlineSaveFileDebug(_autoSaveFileDebug);
	}

	octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
	_state = stateAndSteps_l._state;
	_lastIt = stateAndSteps_l._steps.begin();

	applyControllerStepVisitor(*this, *_state, stateAndSteps_l._initialStep);

	UtilityFunctions::print("done");
	_initDone = true;
	delete _controllerThread;
	_controllerThread = new std::thread(&Controller::loop, this);
}

void Controller::init_replay(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, bool divOptionManager_p, size_t size_p, std::ifstream &file_p)
{
	UtilityFunctions::print("init controller...");

	for(octopus::Steppable *step_l : spawners_p)
	{
		octopus::PlayerSpawnStep const * player_l = dynamic_cast<octopus::PlayerSpawnStep const *>(step_l);
		if(player_l)
		{
			if(divOptionManager_p)
			{
				_optionManagers[player_l->getPlayerIdx()] = new DivinityOptionManager(player_l->getPlayerIdx());
			}
			else
			{
				_optionManagers[player_l->getPlayerIdx()] = new OptionManager(player_l->getPlayerIdx());
			}
		}
	}

	_initDone = false;
	delete _controller;
	_controller = new octopus::Controller(spawners_p, 0.01, commands_p, 5, size_p);
	_controller->setExternalMin(0);
	_controller->enableORCA();

	octopus::readCommands(file_p, *_controller, _lib);

	_controller->setOngoingStep(1);
	_controller->setReplayMode(true);

	octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
	_state = stateAndSteps_l._state;
	_lastIt = stateAndSteps_l._steps.begin();

	applyControllerStepVisitor(*this, *_state, stateAndSteps_l._initialStep);

	UtilityFunctions::print("done");
	_initDone = true;
	delete _controllerThread;
	_controllerThread = new std::thread(&Controller::loop, this);
}

void Controller::init_loading(std::list<octopus::Command *> const &commands_p, std::list<octopus::Steppable *> const &spawners_p, bool divOptionManager_p, size_t size_p, std::ifstream &file_p)
{
	UtilityFunctions::print("init controller...");

	for(octopus::Steppable *step_l : spawners_p)
	{
		octopus::PlayerSpawnStep const * player_l = dynamic_cast<octopus::PlayerSpawnStep const *>(step_l);
		if(player_l)
		{
			if(divOptionManager_p)
			{
				_optionManagers[player_l->getPlayerIdx()] = new DivinityOptionManager(player_l->getPlayerIdx());
			}
			else
			{
				_optionManagers[player_l->getPlayerIdx()] = new OptionManager(player_l->getPlayerIdx());
			}
		}
	}

	_initDone = false;
	delete _controller;
	_controller = new octopus::Controller(spawners_p, 0.01, commands_p, 5, size_p);
	_controller->setExternalMin(0);
	_controller->enableORCA();

	octopus::readCommands(file_p, *_controller, _lib);

	auto last_l = std::chrono::steady_clock::now();

	octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
	_state = stateAndSteps_l._state;
	_lastIt = stateAndSteps_l._steps.begin();

	applyControllerStepVisitor(*this, *_state, stateAndSteps_l._initialStep);

	UtilityFunctions::print("done");
	_initDone = true;
	delete _controllerThread;
	_controllerThread = new std::thread(&Controller::loading_loop, this);
}

void Controller::loading_loop()
{
	unsigned long totalSteps_l = _controller->getOngoingStep();

	// fast speed loading
	while(!_controller->loop_body())
	{
		double ratio_l = double(_controller->getMetrics()._nbStepsCompiled)/totalSteps_l;
		call_deferred("emit_signal", "loading_state", ratio_l);
	}
	// set up step done
	_stepDone += _controller->getMetrics()._nbStepsCompiled;
	call_deferred("emit_signal", "loading_done");

	_paused = true;

	loop();
}

void Controller::loop()
{
	using namespace std::chrono_literals;

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;

	UtilityFunctions::print("Playing...");

	// control if step is locked
	bool lockStep_l = !_stepControl;
	try
	{
		while(!_over)
		{
			lockStep_l = !_stepControl || _controller->getMetrics()._nbStepsCompiled < _stepDone;
			if(!_paused && lockStep_l)
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
	}
	catch(const std::exception& e)
	{
		UtilityFunctions::print("catched exception ", e.what());
	}

	UtilityFunctions::print("Over");
}

bool Controller::has_state() const
{
	return _state != nullptr;
}

octopus::Entity const * Controller::getEntity(octopus::Handle const &handle_p) const
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

void Controller::spawn(octopus::Handle const & handle_p)
{
	octopus::Entity const &entity_l = *_state->getEntity(handle_p);
	emit_signal("spawn_unit", int(handle_p.index), int(handle_p.revision), entity_l._model._id.c_str(), Vector2(octopus::to_double(entity_l._pos.x), octopus::to_double(entity_l._pos.y)), octopus::to_double(entity_l._model._ray));
}

void Controller::move(octopus::Handle const & handle_p)
{
	octopus::Entity const &entity_l = *_state->getEntity(handle_p);
	emit_signal("move_unit", int(handle_p.index), Vector2(octopus::to_double(entity_l._pos.x), octopus::to_double(entity_l._pos.y)));
}

void Controller::windup(int handle_p)
{
	emit_signal("windup_unit", handle_p);
}

void Controller::kill(octopus::Handle const & handle_p)
{
	octopus::Entity const &entity_l = *_state->getEntity(handle_p);
	if(entity_l._model._isUnit || entity_l._model._isBuilding)
	{
		emit_signal("kill_unit", int(handle_p.index));
	}
	else
	{
		emit_signal("clear_entity", int(handle_p.index));
	}
}

void Controller::hp_change(int handle_p, float ratio_p, float diff_p)
{
	emit_signal("hp_change", handle_p, ratio_p, diff_p);
}

void Controller::set_pause(bool paused_p)
{
	_paused = paused_p;
}

void Controller::set_over(bool over_p)
{
	_over = over_p;
}

void Controller::save_to_file(String const &path_p)
{
	if(has_state())
	{
		std::string path_l(path_p.utf8().get_data());
		std::ofstream file_l(path_l, std::ios::out | std::ios::binary);

		// write Godot header
		saveToStream(_fileHeader, file_l);

		// write octopus engine info
		writeLevelId(file_l, _currentLevel, get_world_size()/5);
		_headerWriter(file_l);
		writeCommands(file_l, *_controller);

		path_l += ".debug";
		std::ofstream fileDebug_l(path_l, std::ios::out);
		saveDebugToStream(_fileHeader, fileDebug_l);
		writeDebugCommands(fileDebug_l, *_controller);
	}
}

void Controller::set_auto_file_path(String const &path_p)
{
	std::string path_l(path_p.utf8().get_data());
	_autoSavePath = path_l;
}

void Controller::set_auto_file_debug(bool debug_p)
{
	_enableAutoSaveFileDebug = debug_p;
}

void Controller::dump_state_as_text(String const &path_p)
{
	if(has_state())
	{
		std::string path_l(path_p.utf8().get_data());
		std::ofstream file_l(path_l);
		streamStateAsSimpleText(file_l, *_state);
	}
}

String Controller::get_hash_version() const
{
	return String(octopus::getHashVersion().c_str());
}


TypedArray<String> Controller::get_models(EntityHandle const * handle_p, int player_p, bool checkRequirements_p) const
{
	TypedArray<String> models_l;
	octopus::Entity const *ent_l = _state->getEntity(castHandle(handle_p));
	// update
	if(ent_l->_model._isBuilder)
	{
		std::list<octopus::BuildingModel const *> buildingGrid_l = octopus::getAvailableBuildingModels(*_state->getPlayer(player_p), checkRequirements_p);
		for(octopus::BuildingModel const * model_l : buildingGrid_l)
		{
			models_l.push_back(model_l->_id.c_str());
		}
	} else if(ent_l->_model._isBuilding)
	{
		std::list<octopus::Upgrade const *> updates_l = octopus::getAvailableUpgrades(
			static_cast<octopus::BuildingModel const &>(ent_l->_model), *_state->getPlayer(player_p), checkRequirements_p);
		for(octopus::Upgrade const * update_l : updates_l)
		{
			models_l.push_back(update_l->_id.c_str());
		}
		std::list<octopus::UnitModel const *> unitGrid_l = octopus::getAvailableUnitModels(
			static_cast<octopus::BuildingModel const &>(ent_l->_model), *_state->getPlayer(player_p), checkRequirements_p);
		for(octopus::UnitModel const * model_l : unitGrid_l)
		{
			models_l.push_back(model_l->_id.c_str());
		}
	}
	return models_l;
}

TypedArray<String> Controller::get_abilities(EntityHandle const * handle_p, int player_p, bool checkRequirements_p) const
{
	TypedArray<String> abilities_l;
	octopus::Entity const *ent_l = _state->getEntity(castHandle(handle_p));
	octopus::Player const * player_l = _state->getPlayer(ent_l->_player);

	for(auto &&pair_l : ent_l->_model._abilities)
	{
		octopus::Ability const & ability_l = pair_l.second;
		bool meetRequirements_l = octopus::meetRequirements(ability_l._requirements, *player_l);
		if(!checkRequirements_p || meetRequirements_l)
		{
			abilities_l.push_back(pair_l.first.c_str());
		}
	}
	return abilities_l;
}

bool Controller::is_done_and_non_repeatable(String const &upgrade_p, int player_p) const
{
	std::string const &upgradeId_l(upgrade_p.utf8().get_data());
	return getUpgradeLvl(*getPlayer(player_p), upgradeId_l) && !_lib.getUpgrade(upgradeId_l)._repeatable;
}

int Controller::get_level(String const &upgrade_p, int player_p) const
{
	std::string const &upgradeId_l(upgrade_p.utf8().get_data());
	return getUpgradeLvl(*getPlayer(player_p), upgradeId_l);
}

bool Controller::is_upgrade(String const &upgrade_p) const
{
	std::string const &upgradeId_l(upgrade_p.utf8().get_data());
	return _lib.hasUpgrade(upgradeId_l);
}
bool Controller::is_unit(String const &model_p) const
{
	std::string const &id_l(model_p.utf8().get_data());
	return _lib.hasUnitModel(id_l);
}
bool Controller::is_building(String const &model_p) const
{
	std::string const &id_l(model_p.utf8().get_data());
	return _lib.hasBuildingModel(id_l);
}
bool Controller::is_resource(String const &model_p) const
{
	std::string const &id_l(model_p.utf8().get_data());
	return _lib.hasResourceModel(id_l);
}

double Controller::get_reload_time(EntityHandle const * handle_p, String const &ability_p) const
{
	std::string const &abilityId_l(ability_p.utf8().get_data());
	octopus::Entity const *ent_l = _state->getEntity(castHandle(handle_p));

	octopus::Ability const & ability_l = getAbility(ent_l->_model, abilityId_l);

	return octopus::getMinReloadTime(ent_l->_model, ability_l._reloadKey);
}

double Controller::get_current_reload_time(EntityHandle const * handle_p, String const &ability_p) const
{
	std::string const &abilityId_l(ability_p.utf8().get_data());
	octopus::Entity const *ent_l = _state->getEntity(castHandle(handle_p));

	octopus::Ability const & ability_l = getAbility(ent_l->_model, abilityId_l);
	// check reload time
	unsigned long reload_l = octopus::getReloadAbilityTime(*ent_l, ability_l._reloadKey, octopus::getMinReloadTime(ent_l->_model, ability_l._reloadKey));

	return reload_l;
}

bool Controller::hasNonStaticBehind(EntityHandle const * handle_p, int height_p, int width_p) const
{
	if(!_state)
	{
		return false;
	}
	return lookUpNonStaticBehind(*_state, castHandle(handle_p), width_p, height_p);
}

bool Controller::hasNonStaticBehindFromPos(Vector2 const &pos_p, int height_p, int width_p) const
{
	if(!_state)
	{
		return false;
	}
	return lookUpNonStaticBehind(*_state, pos_p.x, pos_p.y, width_p, height_p);
}

float Controller::get_model_ray(String const &model_p) const
{
	std::string modelId_l(model_p.utf8().get_data());
	return octopus::to_double(_lib.getEntityModel(modelId_l)._ray);
}

bool Controller::is_grid_free(String const &model_p, int x_p, int y_p) const
{
	if(_state)
	{
		std::string modelId_l(model_p.utf8().get_data());
		return checkGridNode(*_state, x_p, y_p, dynamic_cast<octopus::TempleModel const*>(&_lib.getEntityModel(modelId_l)));
	}
	return false;
}


int Controller::get_world_size() const
{
	return _state->getWorldSize();
}

int Controller::get_steps() const
{
	if(_controller)
	{
		return _controller->getMetrics()._nbStepsCompiled;
	}
	return 0;
}

int Controller::get_team(int player_p) const
{
	return _state->getPlayer(player_p)->_team;
}

PackedInt32Array Controller::get_idle_workers(int player_p) const
{
	PackedInt32Array handles_l;
	for(octopus::Entity const * ent_l : _state->getEntities())
	{
		if(ent_l->_model._id == "worker" && ent_l->_player == player_p
		&& _state->isEntityAlive(ent_l->_handle)
		&& !ent_l->getQueue().hasCommand())
		{
			handles_l.push_back(ent_l->_handle.index);
			handles_l.push_back(ent_l->_handle.revision);
		}
	}
	return handles_l;
}

PackedFloat32Array Controller::get_move_targets(PackedInt32Array const &handles_p)
{
	std::unordered_map<octopus::Vector, bool> mapPoints_l;
	// iterate on every entity to store final points
	for(size_t i = 0 ; i < handles_p.size()/2 ; ++ i)
	{
		octopus::Handle handle_l = castHandle(handles_p[i*2],handles_p[i*2+1]);
		if(_state->hasEntity(handle_l))
		{
			octopus::Entity const * ent_l = _state->getEntity(handle_l);
			if(ent_l->getQueue().hasCommand())
			{
				octopus::CommandBundle const &bundle_l = ent_l->getQueue().getFrontCommand();
				octopus::CommandData const * data_l = getData(bundle_l._var);
				octopus::MoveData const * moveData_l = dynamic_cast<octopus::MoveData const *>(data_l);
				if(moveData_l && !dynamic_cast<octopus::HarvestMoveData const *>(data_l))
				{
					mapPoints_l[moveData_l->_finalPoint] = dynamic_cast<octopus::AttackMoveData const *>(data_l);
				}
			}
		}
	}
	// convert points in packed data
	PackedFloat32Array array_l;
	for(auto &&pair_l : mapPoints_l)
	{
		octopus::Vector const &vec_l = pair_l.first;
		array_l.push_back(vec_l.x.to_double());
		array_l.push_back(vec_l.y.to_double());
		array_l.push_back(pair_l.second?1.:-1.);
	}
	return array_l;
}

PackedInt32Array Controller::get_sub_selection(Rect2 const &rect_p, String const &model_p, int player_p)
{
	octopus::Player const *player_l = _state->getPlayer(player_p);
	std::string modelId_l(model_p.utf8().get_data());
	PackedInt32Array array_l;
	octopus::Box<octopus::Fixed> box_l {rect_p.get_position().x, rect_p.get_position().x+rect_p.get_size().x,
		rect_p.get_position().y, rect_p.get_position().y+rect_p.get_size().y};

	for(octopus::Entity const * ent_l : _state->getEntities())
	{
		bool isBlueprint_l = ent_l->_model._isBuilding && static_cast<octopus::Building const *>(ent_l)->isBlueprint();
		bool isCancelled_l = ent_l->_model._isBuilding && static_cast<octopus::Building const *>(ent_l)->_canceled;

		if(!_state->isEntityAlive(ent_l->_handle) && (!isBlueprint_l || isCancelled_l))
		{
			continue;
		}
		if(ent_l->_model._id != modelId_l && modelId_l != "")
		{
			continue;
		}
		if(!_state->getVisionHandler().isExplored(player_l->_team, *ent_l))
		{
			continue;
		}
		if(ent_l->_model._isUnit && !_state->getVisionHandler().isVisible(player_l->_team, *ent_l))
		{
			continue;
		}

		octopus::Fixed lowerPosX = ent_l->_pos.x-ent_l->_model._ray;
		octopus::Fixed upperPosX = ent_l->_pos.x+ent_l->_model._ray;
		octopus::Fixed lowerPosY = ent_l->_pos.y-ent_l->_model._ray;
		octopus::Fixed upperPosY = ent_l->_pos.y+ent_l->_model._ray;

		if(upperPosX < box_l._lowerX || lowerPosX > box_l._upperX
		|| upperPosY < box_l._lowerY || lowerPosY > box_l._upperY)
		{
			continue;
		}

		array_l.push_back(ent_l->_handle.index);
	}

	return array_l;
}

float Controller::get_res(String const &res_p, int player_p) const
{
	std::string resId_l(res_p.utf8().get_data());
	octopus::Player const *player_l = _state->getPlayer(player_p);
	return octopus::to_double(octopus::getResource(*player_l, resId_l));
}

bool Controller::is_visible(int x, int y, int player_p) const
{
	octopus::Player const *player_l = _state->getPlayer(player_p);
	return _state->getVisionHandler().isVisible(player_l->_team, x, y);
}

bool Controller::is_unit_visible(EntityHandle const * handle_p, int player_p) const
{
	octopus::Player const *player_l = _state->getPlayer(player_p);
	octopus::Entity const &entity_l = *_state->getEntity(castHandle(handle_p));
	return _state->getVisionHandler().isVisible(player_l->_team, entity_l);
}

bool Controller::is_explored(int x, int y, int player_p) const
{
	octopus::Player const *player_l = _state->getPlayer(player_p);
	return _state->getVisionHandler().isExplored(player_l->_team, x, y);
}

bool Controller::is_entity_explored(EntityHandle const * handle_p, int player_p) const
{
	octopus::Player const *player_l = _state->getPlayer(player_p);
	octopus::Entity const &entity_l = *_state->getEntity(castHandle(handle_p));
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

int Controller::get_nb_queued_options(int player_p) const
{
	auto it_l = _optionManagers.find(player_p);
	if(it_l != _optionManagers.end())
	{
		return it_l->second->getQueuedOptionsSize();
	}
	return 0;
}

int Controller::get_nb_options_available(int player_p) const
{
	auto it_l = _optionManagers.find(player_p);
	if(it_l == _optionManagers.end() || it_l->second->getQueuedOptionsSize() == 0)
	{
		return 0;
	}
	return it_l->second->getCurrentOptionSize();
}

int Controller::get_nb_options_chosen(int player_p) const
{
	auto it_l = _optionManagers.find(player_p);
	if(it_l == _optionManagers.end())
	{
		return 0;
	}
	return it_l->second->getChosenOptionsSize();
}

godot::Option *Controller::get_available_option_you(int idx_p, int player_p) const
{
	return _optionManagers.at(player_p)->getPrimaryOption(idx_p);
}

godot::Option *Controller::get_available_option_them(int idx_p, int player_p) const
{
	return _optionManagers.at(player_p)->getSecondaryOption(idx_p);
}

godot::Option *Controller::get_chosen_option_you(int idx_p, int player_p) const
{
	return _optionManagers.at(player_p)->getChosenPrimaryOption(idx_p);
}

godot::Option *Controller::get_chosen_option_them(int idx_p, int player_p) const
{
	return _optionManagers.at(player_p)->getChosenSecondaryOption(idx_p);
}

void Controller::get_productions(TypedArray<EntityHandle> const &handles_p, int max_p)
{
	std::vector<CommandInfo> vecCommands_l;

	for(size_t i = 0 ; i < handles_p.size() ; ++ i)
	{
		octopus::Entity const * ent_l = _state->getEntity(castHandle(handles_p[i]));
		size_t posInQueue_l = 0;
		for(octopus::CommandBundle const &bundle_l : ent_l->getQueue().getList())
		{
			octopus::Command const *cmd_l = getCommandFromVar(bundle_l._var);
			octopus::ProductionData const *data_l = dynamic_cast<octopus::ProductionData const *>(getData(bundle_l._var));
			if(cmd_l && data_l && !data_l->_canceled)
			{
				vecCommands_l.push_back({cmd_l->getHandleCommand(), data_l, bundle_l._idx, posInQueue_l});
			}
			++posInQueue_l;
		}
	}

	std::sort(vecCommands_l.begin(), vecCommands_l.end(), CommandSorter());

	for(size_t i = 0 ; i < std::min<size_t>(vecCommands_l.size(), max_p) ; ++i)
	{
		CommandInfo const &info_l = vecCommands_l[i];
		String model_l(info_l.data->getIdModel().c_str());
		float progress_l = octopus::to_double(info_l.data->_progression/info_l.data->_completeTime*100.);
		emit_signal("production_command", int(info_l.handle.index), int(info_l.handle.revision), int(info_l.idx), model_l, progress_l);
	}
}

void Controller::get_visible_units(int player_p, int ent_registered_p)
{
	octopus::Player const *player_l = _state->getPlayer(player_p);
	_visibleLastCall.resize(_state->getEntities().size(), false);
	_visibleRevisionLastCall.resize(_state->getEntities().size(), 0);
	for(octopus::Entity const *ent_l : _state->getEntities())
	{
		// stop when entites are not registered yet in godot
		if(ent_l->_handle.index >= ent_registered_p)
		{
			break;
		}
		octopus::Building const * buildng_l = static_cast<octopus::Building const *>(ent_l);
		// skip unit out of range
		if(ent_l->_model._isUnit
		&& !_state->getVisionHandler().isVisible(player_l->_team, *ent_l))
		{
			if(_visibleLastCall[ent_l->_handle.index] || _visibleRevisionLastCall[ent_l->_handle.index] != ent_l->_handle.revision)
			{
				emit_signal("hide_unit", int(ent_l->_handle.index));
				_visibleLastCall[ent_l->_handle.index] = false;
				_visibleRevisionLastCall[ent_l->_handle.index] = ent_l->_handle.revision;
			}
		}
		else if(ent_l->isActive()
		|| (ent_l->_model._isBuilding && buildng_l->isBlueprint() && !buildng_l->_canceled))
		{
			if(!_visibleLastCall[ent_l->_handle.index] || _visibleRevisionLastCall[ent_l->_handle.index] != ent_l->_handle.revision)
			{
				emit_signal("show_unit", int(ent_l->_handle.index));
				_visibleLastCall[ent_l->_handle.index] = true;
				_visibleRevisionLastCall[ent_l->_handle.index] = ent_l->_handle.revision;
			}
		}
	}
}

// commands
void Controller::add_debug_command(int peer_p, int param_p)
{
	_queuedCommandsPerPeer.at(peer_p).back().push_back(new octopus::DebugCommand(_lib));
}

void Controller::add_move_commands(int peer_p, PackedInt32Array const &handles_p, Vector2 const &target_p, int player_p, bool queued_p)
{
	if(!_paused)
	{
		godot::add_move_commands(_queuedCommandsPerPeer.at(peer_p).back(), *_state, handles_p, target_p, player_p, queued_p);
	}
}

void Controller::add_move_target_commands(int peer_p, PackedInt32Array const &handles_p, Vector2 const &target_p, PackedInt32Array const & handleTarget_p, int player_p, bool queued_p)
{
	if(!_paused)
	{
		godot::add_move_target_commands(_queuedCommandsPerPeer.at(peer_p).back(), *_state, handles_p, target_p, handleTarget_p, player_p, queued_p);
	}
}

void Controller::add_attack_move_commands(int peer_p, PackedInt32Array const &handles_p, Vector2 const &target_p, int player_p, bool queued_p)
{
	if(!_paused)
	{
		godot::add_attack_move_commands(_queuedCommandsPerPeer.at(peer_p).back(), *_state, handles_p, target_p, player_p, queued_p);
	}
}

void Controller::add_stop_commands(int peer_p, PackedInt32Array const &handles_p, int player_p, bool queued_p)
{
	if(!_paused)
	{
		godot::add_stop_commands(_queuedCommandsPerPeer.at(peer_p).back(), *_state, handles_p, player_p, queued_p);
	}
}

void Controller::add_unit_build_command(int peer_p, PackedInt32Array const &handles_p, String const &model_p, int player_p)
{
	if(!_paused)
	{
		godot::add_unit_build_command(_queuedCommandsPerPeer.at(peer_p).back(), *_state, _lib, handles_p, model_p, player_p);
	}
}

void Controller::add_unit_build_cancel_command(int peer_p, PackedInt32Array const & handle_p, int index_p, int player_p)
{
	if(!_paused)
	{
		godot::add_unit_build_cancel_command(_queuedCommandsPerPeer.at(peer_p).back(), *_state, handle_p, index_p, player_p);
	}
}

void Controller::add_blueprint_command(int peer_p, Vector2 const &target_p, String const &model_p, int player_p, PackedInt32Array const &builders_p, bool queued_p)
{
	if(!_paused)
	{
		godot::add_blueprint_command(_queuedCommandsPerPeer.at(peer_p).back(), *_state, _lib, target_p, model_p, player_p, builders_p, queued_p);
	}
}

void Controller::add_building_cancel_command(int peer_p, PackedInt32Array const & handle_p, int player_p)
{
	octopus::Handle entHandle_l = castHandle(handle_p[0], handle_p[1]);
	if(!_paused && _state->getEntity(entHandle_l)->_model._isBuilding)
	{
		_queuedCommandsPerPeer.at(peer_p).back().push_back(new octopus::BuildingCancelCommand(entHandle_l));
	}
}

void Controller::add_chose_option_command(int peer_p, int option_p, int player_p)
{
	if(!_paused)
	{
		_queuedCommandsPerPeer.at(peer_p).back().push_back(_optionManagers.at(player_p)->newCommandFromOption(option_p));
	}
}

void Controller::add_ability_command(int peer_p, PackedInt32Array const & handles_p, String const &ability_p)
{
	if(!_paused)
	{
		godot::add_ability_command(_queuedCommandsPerPeer.at(peer_p).back(), *_state, handles_p, ability_p);
	}
}

void Controller::set_step_control(int prequeued_p)
{
	for(int i = 0 ; i < prequeued_p-1 ; ++ i)
	{
		_controller->addQueuedLayer();
	}
}

void Controller::next_step()
{
	for(auto &&pair_l : _queuedCommandsPerPeer)
	{
		int peer_l = pair_l.first;
		std::list<octopus::Command*> const &cmds_l = pair_l.second.front();
		for(octopus::Command * cmd_l : cmds_l)
		{
			/// @todo refactor to allow control over multiple player ?
			_controller->queueCommandAsPlayer(cmd_l, _playerPerPeer.at(peer_l));
			//_controller->queueCommand(cmd_l);
		}
		pair_l.second.pop_front();
	}
	++_stepDone;
	_controller->addQueuedLayer();
}

int Controller::get_queued_size() const
{
	return _controller->getQueuedSize();
}

void Controller::add_peer_info(int peer_p, int player_p)
{
	if(player_p>=0)
	{
		_playerPerPeer[peer_p] = player_p;
		// also set up command per peer with one list
		_queuedCommandsPerPeer[peer_p].push_back({});
	}
}

void Controller::step_done_for_peer(int peer_p)
{
	_queuedCommandsPerPeer[peer_p].push_back({});
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
	ClassDB::bind_method(D_METHOD("load_mission_1", "seed", "player_count"), &Controller::load_mission_1);
	ClassDB::bind_method(D_METHOD("load_mission_2", "seed", "level_model", "player_count"), &Controller::load_mission_2);
	ClassDB::bind_method(D_METHOD("load_minimal_model"), &Controller::load_minimal_model);
	ClassDB::bind_method(D_METHOD("load_hero_siege_level", "seed", "nb_players"), &Controller::load_hero_siege_level);
	ClassDB::bind_method(D_METHOD("load_demo_level", "seed", "wave_pattern", "level_model", "player_count", "difficulty"), &Controller::load_demo_level);
	ClassDB::bind_method(D_METHOD("load_level1", "seed", "nb_wave"), &Controller::load_level1);
	ClassDB::bind_method(D_METHOD("load_level2", "seed", "wave_pattern", "nb_players"), &Controller::load_level2);
	ClassDB::bind_method(D_METHOD("load_level_test_anchor", "seed"), &Controller::load_level_test_anchor);
	ClassDB::bind_method(D_METHOD("load_level_test_model_reading", "seed", "level_model", "buff_prod"), &Controller::load_level_test_model_reading);
	ClassDB::bind_method(D_METHOD("load_duel_level", "seed", "div_player_1_p", "div_player_2_p"), &Controller::load_duel_level);
	ClassDB::bind_method(D_METHOD("load_multi_test_level", "seed", "step_count", "buff_prod"), &Controller::load_multi_test_level);

	ClassDB::bind_method(D_METHOD("set_model_filename", "filename"), &Controller::set_model_filename);
	ClassDB::bind_method(D_METHOD("set_level_filename", "filename"), &Controller::set_level_filename);
	ClassDB::bind_method(D_METHOD("get_model_filename", "filename"), &Controller::get_model_filename);
	ClassDB::bind_method(D_METHOD("get_level_filename", "filename"), &Controller::get_level_filename);
	ClassDB::bind_method(D_METHOD("replay_level", "filename", "replay_mode", "level_model"), &Controller::replay_level);

	ClassDB::bind_method(D_METHOD("has_state"), &Controller::has_state);
	ClassDB::bind_method(D_METHOD("set_pause", "pause"), &Controller::set_pause);
	ClassDB::bind_method(D_METHOD("set_over", "over"), &Controller::set_over);
	ClassDB::bind_method(D_METHOD("save_to_file", "path"), &Controller::save_to_file);
	ClassDB::bind_method(D_METHOD("set_auto_file_path", "path"), &Controller::set_auto_file_path);
	ClassDB::bind_method(D_METHOD("set_auto_file_debug", "debug"), &Controller::set_auto_file_debug);
	ClassDB::bind_method(D_METHOD("dump_state_as_text", "path"), &Controller::dump_state_as_text);
	ClassDB::bind_method(D_METHOD("get_hash_version"), &Controller::get_hash_version);
	ClassDB::bind_method(D_METHOD("get_models", "handle", "player", "check_requirements"), &Controller::get_models);
	ClassDB::bind_method(D_METHOD("get_abilities", "handle", "player", "check_requirements"), &Controller::get_abilities);

	ClassDB::bind_method(D_METHOD("is_done_and_non_repeatable", "upgrade", "player"), &Controller::is_done_and_non_repeatable);
	ClassDB::bind_method(D_METHOD("get_level", "upgrade", "player"), &Controller::get_level);
	ClassDB::bind_method(D_METHOD("is_upgrade", "upgrade"), &Controller::is_upgrade);
	ClassDB::bind_method(D_METHOD("is_unit", "model"), &Controller::is_unit);
	ClassDB::bind_method(D_METHOD("is_building", "model"), &Controller::is_building);
	ClassDB::bind_method(D_METHOD("is_resource", "model"), &Controller::is_resource);

	ClassDB::bind_method(D_METHOD("get_reload_time", "handle", "ability"), &Controller::get_reload_time);
	ClassDB::bind_method(D_METHOD("get_current_reload_time", "handle", "ability"), &Controller::get_current_reload_time);
	ClassDB::bind_method(D_METHOD("hasNonStaticBehind", "handle", "height" "width"), &Controller::hasNonStaticBehind);
	ClassDB::bind_method(D_METHOD("get_model_ray", "model"), &Controller::get_model_ray);
	ClassDB::bind_method(D_METHOD("is_grid_free", "model", "x", "y"), &Controller::is_grid_free);
	ClassDB::bind_method(D_METHOD("get_world_size"), &Controller::get_world_size);
	ClassDB::bind_method(D_METHOD("get_steps"), &Controller::get_steps);
	ClassDB::bind_method(D_METHOD("get_team", "player"), &Controller::get_team);
	ClassDB::bind_method(D_METHOD("get_idle_workers", "player"), &Controller::get_idle_workers);
	ClassDB::bind_method(D_METHOD("get_move_targets", "handles"), &Controller::get_move_targets);
	ClassDB::bind_method(D_METHOD("get_sub_selection", "rect", "model", "player"), &Controller::get_sub_selection);

	ClassDB::bind_method(D_METHOD("get_res", "rest", "player"), &Controller::get_res);
	ClassDB::bind_method(D_METHOD("is_visible", "x", "y", "player"), &Controller::is_visible);
	ClassDB::bind_method(D_METHOD("is_unit_visible", "handle", "player"), &Controller::is_unit_visible);
	ClassDB::bind_method(D_METHOD("is_explored", "x", "y", "player"), &Controller::is_explored);
	ClassDB::bind_method(D_METHOD("is_entity_explored", "handle", "player"), &Controller::is_entity_explored);
	ClassDB::bind_method(D_METHOD("getVisibility", "player"), &Controller::getVisibility);
	ClassDB::bind_method(D_METHOD("get_nb_queued_options", "player"), &Controller::get_nb_queued_options);
	ClassDB::bind_method(D_METHOD("get_nb_options_available", "player"), &Controller::get_nb_options_available);
	ClassDB::bind_method(D_METHOD("get_nb_options_chosen", "player"), &Controller::get_nb_options_chosen);
	ClassDB::bind_method(D_METHOD("get_available_option_you", "idx", "player"), &Controller::get_available_option_you);
	ClassDB::bind_method(D_METHOD("get_available_option_them", "idx", "player"), &Controller::get_available_option_them);
	ClassDB::bind_method(D_METHOD("get_chosen_option_you", "idx", "player"), &Controller::get_chosen_option_you);
	ClassDB::bind_method(D_METHOD("get_chosen_option_them", "idx", "player"), &Controller::get_chosen_option_them);

	ClassDB::bind_method(D_METHOD("get_productions", "handles", "max"), &Controller::get_productions);
	ClassDB::bind_method(D_METHOD("get_visible_units", "player", "ent_registered_p"), &Controller::get_visible_units);


	ClassDB::bind_method(D_METHOD("add_debug_command", "peer", "param"), &Controller::add_debug_command);
	ClassDB::bind_method(D_METHOD("add_move_commands", "peer", "handles", "target", "player", "queued"), &Controller::add_move_commands);
	ClassDB::bind_method(D_METHOD("add_move_target_commands", "peer", "handles", "target", "handle_target", "player", "queued"), &Controller::add_move_target_commands);
	ClassDB::bind_method(D_METHOD("add_attack_move_commands", "peer", "handles", "target", "player", "queued"), &Controller::add_attack_move_commands);
	ClassDB::bind_method(D_METHOD("add_stop_commands", "peer", "handles", "player", "queued"), &Controller::add_stop_commands);
	ClassDB::bind_method(D_METHOD("add_unit_build_command", "peer", "handle", "model", "player"), &Controller::add_unit_build_command);
	ClassDB::bind_method(D_METHOD("add_unit_build_cancel_command", "peer", "handle", "index", "player"), &Controller::add_unit_build_cancel_command);
	ClassDB::bind_method(D_METHOD("add_blueprint_command", "peer", "target", "model", "player", "builders", "queued"), &Controller::add_blueprint_command);
	ClassDB::bind_method(D_METHOD("add_building_cancel_command", "peer", "handle", "player"), &Controller::add_building_cancel_command);
	ClassDB::bind_method(D_METHOD("add_chose_option_command", "peer", "option_p", "player"), &Controller::add_chose_option_command);
	ClassDB::bind_method(D_METHOD("add_ability_command", "peer", "handles", "ability"), &Controller::add_ability_command);

	ClassDB::bind_method(D_METHOD("set_step_control", "prequeued_p"), &Controller::set_step_control);
	ClassDB::bind_method(D_METHOD("next_step"), &Controller::next_step);
	ClassDB::bind_method(D_METHOD("get_queued_size"), &Controller::get_queued_size);
	ClassDB::bind_method(D_METHOD("add_peer_info", "peer_p", "player_p"), &Controller::add_peer_info);
	ClassDB::bind_method(D_METHOD("step_done_for_peer", "peer_p"), &Controller::step_done_for_peer);

	ADD_GROUP("Controller", "Controller_");

	ADD_SIGNAL(MethodInfo("spawn_unit", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::INT, "revision"), PropertyInfo(Variant::STRING, "model"), PropertyInfo(Variant::VECTOR2, "pos"), PropertyInfo(Variant::FLOAT, "ray")));
	ADD_SIGNAL(MethodInfo("move_unit", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::VECTOR2, "pos")));
	ADD_SIGNAL(MethodInfo("windup_unit", PropertyInfo(Variant::INT, "handle")));
	ADD_SIGNAL(MethodInfo("kill_unit", PropertyInfo(Variant::INT, "handle")));
	ADD_SIGNAL(MethodInfo("clear_entity", PropertyInfo(Variant::INT, "handle")));
	ADD_SIGNAL(MethodInfo("hp_change", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::FLOAT, "ratio"), PropertyInfo(Variant::FLOAT, "diff")));
	ADD_SIGNAL(MethodInfo("harvest_drop", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::INT, "player"), PropertyInfo(Variant::FLOAT, "qty"), PropertyInfo(Variant::STRING, "type")));
	ADD_SIGNAL(MethodInfo("harvest_unit", PropertyInfo(Variant::INT, "handle")));
	ADD_SIGNAL(MethodInfo("hide_unit", PropertyInfo(Variant::INT, "handle")));
	ADD_SIGNAL(MethodInfo("show_unit", PropertyInfo(Variant::INT, "handle")));
	ADD_SIGNAL(MethodInfo("build", PropertyInfo(Variant::INT, "source"), PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::FLOAT, "progress")));
	ADD_SIGNAL(MethodInfo("option_update"));
	ADD_SIGNAL(MethodInfo("pop_option"));
	ADD_SIGNAL(MethodInfo("remove_rally_point", PropertyInfo(Variant::INT, "handle")));
	ADD_SIGNAL(MethodInfo("set_rally_point", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::VECTOR2, "pos")));
	ADD_SIGNAL(MethodInfo("set_rally_point_target", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::INT, "target")));
	ADD_SIGNAL(MethodInfo("updated_requirements"));
	ADD_SIGNAL(MethodInfo("ability_used", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::STRING, "ability")));
	ADD_SIGNAL(MethodInfo("buff_all", PropertyInfo(Variant::INT, "player"), PropertyInfo(Variant::STRING, "buff"), PropertyInfo(Variant::STRING, "model")));
	ADD_SIGNAL(MethodInfo("buff", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::STRING, "buff"), PropertyInfo(Variant::INT, "duration")));
	ADD_SIGNAL(MethodInfo("spawn_projectile", PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::STRING, "model"),
		PropertyInfo(Variant::VECTOR2, "pos"), PropertyInfo(Variant::VECTOR2, "pos_target")));
	ADD_SIGNAL(MethodInfo("move_projectile", PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::VECTOR2, "pos")));
	ADD_SIGNAL(MethodInfo("end_projectile", PropertyInfo(Variant::INT, "index")));

	/// signals for objective handling
	ADD_SIGNAL(MethodInfo("add_main_objective", PropertyInfo(Variant::STRING, "key_objective"),
		PropertyInfo(Variant::STRING, "text"), PropertyInfo(Variant::INT, "count")));
	ADD_SIGNAL(MethodInfo("add_secondary_objective", PropertyInfo(Variant::STRING, "key_objective"),
		PropertyInfo(Variant::STRING, "text"), PropertyInfo(Variant::INT, "count")));
	ADD_SIGNAL(MethodInfo("set_complete_objective", PropertyInfo(Variant::STRING, "key_objective"), PropertyInfo(Variant::BOOL, "complete")));
	ADD_SIGNAL(MethodInfo("set_fail_objective", PropertyInfo(Variant::STRING, "key_objective"), PropertyInfo(Variant::BOOL, "fail")));
	ADD_SIGNAL(MethodInfo("increment_objective", PropertyInfo(Variant::STRING, "key_objective")));
	ADD_SIGNAL(MethodInfo("decrement_objective", PropertyInfo(Variant::STRING, "key_objective")));
	ADD_SIGNAL(MethodInfo("remove_objective", PropertyInfo(Variant::STRING, "key_objective")));

	/// blockers
	ADD_SIGNAL(MethodInfo("spawn_blockers", PropertyInfo(Variant::FLOAT, "val"), PropertyInfo(Variant::FLOAT, "min"),
		PropertyInfo(Variant::FLOAT, "max"), PropertyInfo(Variant::BOOL, "less"), PropertyInfo(Variant::BOOL, "x")));
	ADD_SIGNAL(MethodInfo("clear_blockers", PropertyInfo(Variant::FLOAT, "val"), PropertyInfo(Variant::FLOAT, "min"),
		PropertyInfo(Variant::FLOAT, "max"), PropertyInfo(Variant::BOOL, "less"), PropertyInfo(Variant::BOOL, "x")));
	ADD_SIGNAL(MethodInfo("impact", PropertyInfo(Variant::STRING, "model"), PropertyInfo(Variant::FLOAT, "x"), PropertyInfo(Variant::FLOAT, "y")));
	ADD_SIGNAL(MethodInfo("set_camera", PropertyInfo(Variant::INT, "x"), PropertyInfo(Variant::INT, "y"), PropertyInfo(Variant::INT, "player")));
	ADD_SIGNAL(MethodInfo("spawn_dialog", PropertyInfo(Variant::STRING, "model"), PropertyInfo(Variant::BOOL, "over")));
	ADD_SIGNAL(MethodInfo("wave"));
	ADD_SIGNAL(MethodInfo("wave_spawn_point", PropertyInfo(Variant::INT, "x"), PropertyInfo(Variant::INT, "y")));
	ADD_SIGNAL(MethodInfo("rune_well_pop_step", PropertyInfo(Variant::INT, "well"), PropertyInfo(Variant::INT, "player")));

	ADD_SIGNAL(MethodInfo("over", PropertyInfo(Variant::INT, "winning_team")));

	ADD_SIGNAL(MethodInfo("production_command", PropertyInfo(Variant::INT, "handle"), PropertyInfo(Variant::INT, "revision"), PropertyInfo(Variant::INT, "index"), PropertyInfo(Variant::STRING, "model"), PropertyInfo(Variant::FLOAT, "progress")));

	ADD_SIGNAL(MethodInfo("loading_state", PropertyInfo(Variant::FLOAT, "loading")));
	ADD_SIGNAL(MethodInfo("loading_done"));
}

std::map<unsigned long, AbstractOptionManager *> &Controller::getOptionManagers()
{
	return _optionManagers;
}
std::map<unsigned long, AbstractOptionManager *> const &Controller::getOptionManagers() const
{
	return _optionManagers;
}

octopus::Library &Controller::getLib()
{
	return _lib;
}

octopus::Library const &Controller::getLib() const
{
	return _lib;
}

octopus::State const * Controller::getState() const
{
	return _state;
}

void Controller::newAutoSaveFile()
{
	delete _autoSaveFile;
	delete _autoSaveFileDebug;
	_autoSaveFile = new std::ofstream(_autoSavePath, std::ios::out | std::ios::binary);
	saveToStream(_fileHeader, *_autoSaveFile);
	if(_enableAutoSaveFileDebug)
	{
		_autoSaveFileDebug = new std::ofstream(_autoSavePath+".d", std::ios::out);
		saveDebugToStream(_fileHeader, *_autoSaveFileDebug);
	}
}

}
