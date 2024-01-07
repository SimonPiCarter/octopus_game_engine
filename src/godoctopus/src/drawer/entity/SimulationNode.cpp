
#include "SimulationNode.h"
#include <godot_cpp/variant/utility_functions.hpp>

#include <algorithm>
#include <chrono>

#include "visitor/SimulationVisitor.h"

#include "library/levels/ArenaLevel.hh"

#include "controller/Controller.hh"
#include "command/data/AttackData.hh"
#include "library/Library.hh"
#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "step/entity/spawn/UnitSpawnStep.hh"
#include "step/player/PlayerSpawnStep.hh"

namespace godot
{
	SimulationNode::~SimulationNode()
	{
		_over = true;
		delete _controllerThread;
		delete _controller;
		delete _lib;
	}

	void SimulationNode::init_demo(
		TypedArray<Vector2i> const &team_1_p, TypedArray<Vector2i> const &team_2_p,
		Ref<AtlasTexture> const & texture_p, Ref<AtlasTexture> const & texture2_p,
		Ref<SpriteFrames> const & animation_p)
	{
		delete _controllerThread;
		delete _controller;
		delete _lib;

		_animation = animation_p;

		size_t nb_l = 1;
		_lib = new octopus::Library();
		std::list<octopus::Steppable *> spawners_l = {
			new octopus::PlayerSpawnStep(0,0),
			new octopus::PlayerSpawnStep(1,1),
		};

		octopus::UnitModel unitModel_l { false, 0.5, 0.1, 100};
		unitModel_l._range = 0.5;
		unitModel_l._windup = 25;
		unitModel_l._fullReload = 50;
		unitModel_l._damage = 10;

		_lib->registerUnitModel("model", unitModel_l);
		octopus::UnitModel const &model_l = _lib->getUnitModel("model");

		unsigned long handle_l = 0;
		for(size_t idx_l = 0 ; idx_l < team_1_p.size(); ++idx_l)
		{
			Vector2i const &vec_l = team_1_p[idx_l];
			for(size_t i = 0 ; i < nb_l ; ++ i)
			{
				octopus::Unit unit_l {octopus::Vector(vec_l.x, vec_l.y), false, model_l};
				unit_l._player = 0;
				spawners_l.push_back(new octopus::UnitSpawnStep(octopus::Handle(handle_l++, 0), unit_l));
			}
		}
		for(size_t idx_l = 0 ; idx_l < team_2_p.size(); ++idx_l)
		{
			Vector2i const &vec_l = team_2_p[idx_l];
			for(size_t i = 0 ; i < nb_l ; ++ i)
			{
				octopus::Unit unit_l {octopus::Vector(vec_l.x, vec_l.y), false, model_l};
				unit_l._player = 1;
				spawners_l.push_back(new octopus::UnitSpawnStep(octopus::Handle(handle_l++, 0), unit_l));
			}
		}

		double stepTime_l = 0.02;
		if(_entityDrawer)
		{
			for(size_t i = 0 ; i < team_1_p.size()*nb_l ; ++i)
			{
				_entityDrawer->add_instance(Vector2(0,0), Vector2(-1,-1), texture_p);
			}
			for(size_t i = 0 ; i < team_2_p.size()*nb_l ; ++i)
			{
				_entityDrawer->add_instance(Vector2(0,0), Vector2(-1,-1), texture2_p);
			}
			_entityDrawer->setTimeStep(stepTime_l);
		}

		_controller = new octopus::Controller(spawners_l, stepTime_l, {}, 5, 50);
		_controller->enableORCA();
		_controller->setExternalMin(0);
		_controller->addQueuedLayer();

		octopus::StateAndSteps stateAndSteps_l = _controller->queryStateAndSteps();
		_state = stateAndSteps_l._state;
		_lastIt = stateAndSteps_l._steps.begin();

		_controllerThread = new std::thread(&SimulationNode::loop, this);
	}

	void SimulationNode::trigger_demo(Vector2 const &target, int start, int end)
	{
		std::list<octopus::Handle> handles_l;
		for(int i = start ; i < end; ++ i)
		{
			handles_l.push_back(octopus::Handle(i, 0));
		}
		_controller->commitCommand(new octopus::EntityFlockMoveCommand(handles_l, octopus::Vector(target.x, target.y), true, true));
	}

	void SimulationNode::_ready()
	{}

	double SimulationNode::get_avg_last_compile_time(int numbers_p) const
	{
		if(!_controller)
		{
			return 0.;
		}
		std::vector<double> const &compile_times = _controller->getMetrics()._vecTimeCompilingSteps;
		int counted_l = 0;
		double time_l = 0;
		for(int i = int(compile_times.size())-1 ; i >= 0 && counted_l < numbers_p ; --i)
		{
			++counted_l;
			time_l += compile_times[i];
		}
		return time_l/double(counted_l);
	}

	void SimulationNode::_process(double delta_p)
	{
		if(!_controller)
		{
			return;
		}
		octopus::State const * oldState_l = _state;
		octopus::StateAndSteps newState_l = _controller->queryStateAndSteps();
		_state = newState_l._state;
		if(_entityDrawer && _animationDrawer
		&& _state && _state != oldState_l)
		{
			_entityDrawer->update_pos();
			std::vector<Vector2> & newPos_l = _entityDrawer->getNewPos();
			std::vector<EntityInstance> & instances_l = _entityDrawer->getInstances();

			for(size_t i = 0 ; i < newPos_l.size() && i < _state->getEntities().size() ; ++ i)
			{
				Vector2 & pos_l = newPos_l[i];
				octopus::Entity const *ent_l = _state->getLoseEntity(i);
				pos_l.x = ent_l->_pos.x.to_double() * 32;
				pos_l.y = ent_l->_pos.y.to_double() * 32;
				instances_l[i].enabled = ent_l->_alive;
			}

			SimulationVisitor vis_l(this);
			// Every step missing
			for(auto it_l = _lastIt ; it_l != newState_l._stepIt ; ++it_l)
			{
				for(octopus::Steppable const * steppable_l : it_l->_step->getSteppable())
				{
					vis_l(steppable_l);
				}
			}
			_lastIt = newState_l._stepIt;
			_controller->setExternalMin(_lastIt->_step->getId());
		}
	}

	EntityDrawer *SimulationNode::getEntityDrawer() const
	{
		return _entityDrawer;
	}

	void SimulationNode::setEntityDrawer(EntityDrawer * entityDrawer_p)
	{
		_entityDrawer = entityDrawer_p;
	}

	AnimationDrawer *SimulationNode::getAnimationDrawer() const
	{
		return _animationDrawer;
	}

	void SimulationNode::setAnimationDrawer(AnimationDrawer * animationDrawer_p)
	{
		_animationDrawer = animationDrawer_p;
	}

	void SimulationNode::_bind_methods()
	{
		ClassDB::bind_method(D_METHOD("init_demo", "pos_1", "pos_2", "texture_1", "texture_2"), &SimulationNode::init_demo);
		ClassDB::bind_method(D_METHOD("trigger_demo", "target" ,"start", "end"), &SimulationNode::trigger_demo);
		ClassDB::bind_method(D_METHOD("set_over", "over"), &SimulationNode::set_over);
		ClassDB::bind_method(D_METHOD("get_avg_last_compile_time", "nb"), &SimulationNode::get_avg_last_compile_time);

		ClassDB::bind_method(D_METHOD("getEntityDrawer"), &SimulationNode::getEntityDrawer);
		ClassDB::bind_method(D_METHOD("setEntityDrawer", "entity_drawer"), &SimulationNode::setEntityDrawer);

		ClassDB::bind_method(D_METHOD("getAnimationDrawer"), &SimulationNode::getAnimationDrawer);
		ClassDB::bind_method(D_METHOD("setAnimationDrawer", "Animation_drawer"), &SimulationNode::setAnimationDrawer);

		ADD_GROUP("SimulationNode", "SimulationNode_");
		ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "entity_drawer", PROPERTY_HINT_OBJECT_ID, "EntityDrawer"), "setEntityDrawer", "getEntityDrawer");
		ADD_PROPERTY(PropertyInfo(Variant::NODE_PATH, "animation_drawer", PROPERTY_HINT_OBJECT_ID, "AnimationDrawer"), "setAnimationDrawer", "getAnimationDrawer");
	}

	void SimulationNode::pop_wind_up(octopus::Handle const &handle_p)
	{
		if(_animationDrawer && _state)
		{
			octopus::Entity const *ent_l = _state->getEntity(handle_p);
			Vector2 pos_l;
			pos_l.x = ent_l->_pos.x.to_double() * 32;
			pos_l.y = ent_l->_pos.y.to_double() * 32;
			_animationDrawer->add_instance(pos_l, Vector2(-16,-16), _animation);
		}
	}

	void SimulationNode::loop()
	{
		using namespace std::chrono_literals;

		auto last_l = std::chrono::steady_clock::now();
		double elapsed_l = 0.;

		UtilityFunctions::print("Playing...");

		try
		{
			while(!_over)
			{
				// update controller
				_controller->update(elapsed_l);
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

		if(_controller)
		{
			std::stringstream ss_l;
			streamMetrics(ss_l, _controller->getMetrics());
			String metrics_l(ss_l.str().c_str());
			UtilityFunctions::print(metrics_l);
			UtilityFunctions::print("los_check_l = ", octopus::EntityMoveCommand::los_check_l);
			UtilityFunctions::print("unlock_l = ", octopus::EntityMoveCommand::unlock_l);
			UtilityFunctions::print("field_l = ", octopus::EntityMoveCommand::field_l);
			UtilityFunctions::print("prog_l = ", octopus::EntityMoveCommand::prog_l);
			UtilityFunctions::print("total_l = ", octopus::EntityMoveCommand::total_l);
		}

		UtilityFunctions::print("Over");
	}

} // namespace godot

