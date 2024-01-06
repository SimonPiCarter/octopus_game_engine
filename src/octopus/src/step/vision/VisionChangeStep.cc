#include "VisionChangeStep.hh"

#include "state/State.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/player/Player.hh"
#include "state/vision/PatternHandler.hh"
#include "state/vision/VisionGrid.hh"
#include "step/entity/EntityMoveStep.hh"
#include "step/Step.hh"
#include "logger/Logger.hh"

namespace octopus
{


VisionChangeStep::VisionChangeStep(unsigned long team_p, std::unordered_map<uint32_t , std::unordered_map<uint32_t, long long> > &&delta_p, bool exploration_p)
	: _team(team_p), _delta(delta_p), _exploration(exploration_p)
{
	uint32_t count_l = 0;
	uint32_t kept_l = 0;
	// clean up zero element in the map
	for(auto && itFirst_l = _delta.begin() ; itFirst_l != _delta.end() ; ++ itFirst_l)
	{
		std::unordered_map<uint32_t, long long> &mapSecond_l = itFirst_l->second;
		for(auto && itSecond_l = mapSecond_l.begin() ; itSecond_l != mapSecond_l.end() ;)
		{
			long long const & val_l = itSecond_l->second;
			if(val_l == 0)
			{
				++count_l;
				itSecond_l = mapSecond_l.erase(itSecond_l);
			}
			else
			{
				++kept_l;
				++itSecond_l;
			}
		}
	}
}

void VisionChangeStep::apply(State &state_p) const
{
	Logger::getDebug() << "VisionChangeStep :: apply " << this->_team <<std::endl;

	VisionGrid & grid_l = *getVisionGrid(state_p, _team);

	for(auto && pairMap_l : _delta)
	{
		for(auto && pair_l : pairMap_l.second)
		{
			grid_l.incrementVisionGrid(pairMap_l.first, pair_l.first, pair_l.second, _exploration);
		}
	}
}

void VisionChangeStep::revert(State &state_p, SteppableData const *) const
{
	Logger::getDebug() << "VisionChangeStep :: apply " << this->_team <<std::endl;

	VisionGrid & grid_l = *getVisionGrid(state_p, _team);

	for(auto && pairMap_l : _delta)
	{
		for(auto && pair_l : pairMap_l.second)
		{
			grid_l.incrementVisionGrid(pairMap_l.first, pair_l.first, -pair_l.second, _exploration);
		}
	}
}

std::list<VisionChangeStep *> newVisionChangeStep(State const &state_p, Step const &step_p, long worldSize_p, PatternHandler &handler_p)
{
	std::unordered_map<unsigned long , std::unordered_map<uint32_t , std::unordered_map<uint32_t, long long> > > deltaPerTeam_l;
	std::unordered_map<unsigned long , std::unordered_map<uint32_t , std::unordered_map<uint32_t, long long> > > deltaPerTeamExploration_l;

	std::vector<Fixed> hitpoints_l;
	std::vector<bool> dead_l;
	std::vector<bool> alreadyDead_l;
	hitpoints_l.reserve(state_p.getEntities().size());
	dead_l.reserve(state_p.getEntities().size());
	alreadyDead_l.reserve(state_p.getEntities().size());
	for(Entity const * ent_l : state_p.getEntities())
	{
		hitpoints_l.push_back(ent_l->_hp);
		dead_l.push_back(false);
		alreadyDead_l.push_back(!ent_l->_alive);
	}
	// need to tag all dead entities to avoid updating them
	// only do so by checking hit point changes
	for(std::pair<Handle, Fixed> pair_l : step_p.getHpChange())
	{
		if(hitpoints_l[pair_l.first.index] + pair_l.second < 1e-3)
		{
			dead_l[pair_l.first.index] = true;
		}
	}

	// decrease count for dead entities
	for(Entity const * ent_l : state_p.getEntities())
	{
		unsigned long team_l = state_p.getPlayer(ent_l->_player)->_team;
		if(!dead_l[ent_l->_handle.index] || alreadyDead_l[ent_l->_handle.index])
		{
			continue;
		}
		VisionPattern const &pattern_l = handler_p.getPattern(ent_l->_model._lineOfSight.to_int());
		for(std::pair<long, long> const &pair_l : pattern_l)
		{
			unsigned long x = std::max(0l, std::min<long>(to_int(pair_l.first+ent_l->_pos.x), worldSize_p-1));
			unsigned long y = std::max(0l, std::min<long>(to_int(pair_l.second+ent_l->_pos.y), worldSize_p-1));

			deltaPerTeam_l[team_l][x][y] -= 1;
		}
	}

	// for each entity move step
	for(EntityMoveStep const * moveStep_l : step_p.getEntityMoveStep())
	{
		Entity const * ent_l = state_p.getEntity(moveStep_l->_handle);
		// skip treatmen for dead entities
		if(dead_l[ent_l->_handle.index] || alreadyDead_l[ent_l->_handle.index])
		{
			continue;
		}
		// skip no op step
		if(moveStep_l->isNoOp())
		{
			continue;
		}
		unsigned long team_l = state_p.getPlayer(ent_l->_player)->_team;

		long newx_l = to_int(ent_l->_pos.x+moveStep_l->_move.x);
		long newy_l = to_int(ent_l->_pos.y+moveStep_l->_move.y);
		long dx = newx_l - to_int(ent_l->_pos.x);
		long dy = newy_l - to_int(ent_l->_pos.y);

		VisionPattern const &pattern_l = handler_p.getMovementPattern(ent_l->_model._lineOfSight.to_int(), dx, dy);

		for(std::pair<long, long> const &pair_l : pattern_l)
		{
			unsigned long x = std::max(0l, std::min<long>(to_int(pair_l.first+newx_l), worldSize_p-1));
			unsigned long y = std::max(0l, std::min<long>(to_int(pair_l.second+newy_l), worldSize_p-1));

			deltaPerTeam_l[team_l][x][y] += 1;
			deltaPerTeamExploration_l[team_l][x][y] += 1;
		}

		VisionPattern const &patternOpposite_l = handler_p.getMovementPattern(ent_l->_model._lineOfSight.to_int(), -dx, -dy);

		for(std::pair<long, long> const &pair_l : patternOpposite_l)
		{
			unsigned long x = std::max(0l, std::min<long>(to_int(pair_l.first+ent_l->_pos.x), worldSize_p-1));
			unsigned long y = std::max(0l, std::min<long>(to_int(pair_l.second+ent_l->_pos.y), worldSize_p-1));

			deltaPerTeam_l[team_l][x][y] -= 1;
		}
	}

	std::list<VisionChangeStep *> steps_l;

	for(auto &&deltaPair_l : deltaPerTeam_l)
	{
		steps_l.push_back(new VisionChangeStep(deltaPair_l.first, std::move(deltaPair_l.second)));
	}
	for(auto &&deltaPair_l : deltaPerTeamExploration_l)
	{
		steps_l.push_back(new VisionChangeStep(deltaPair_l.first, std::move(deltaPair_l.second), true));
	}

	return steps_l;
}

} // namespace octopus
