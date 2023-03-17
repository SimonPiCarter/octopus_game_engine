#include "TickingStep.hh"

#include "state/State.hh"
#include "state/entity/Buff.hh"
#include "state/entity/Entity.hh"
#include "state/player/Player.hh"
#include "logger/Logger.hh"

namespace octopus
{

double const TickingStep::_anchorLoss = 0.01;

void TickingStep::apply(State &state_p, SteppableData *) const
{
	Logger::getDebug() << "TickingStep :: apply "<<std::endl;
	for(Entity *ent_l : state_p.getEntities())
	{
		if(!ent_l->_alive)
		{
			continue;
		}
		++ent_l->_reload;

		// Handle buffs
		for(auto &&pair_l : ent_l->_timeSinceBuff)
		{
			std::string const &id_l = pair_l.first;
			unsigned long &time_l = pair_l.second;
			TyppedBuff const &buff_l = ent_l->_registeredBuff[id_l];
			// need to apply buff
			if(time_l == 0)
			{
				buff_l.apply(*ent_l);
			}
			// need to revert buff (only when we go the exact duration to avoid reverting multiple times)
			if(time_l+1 == buff_l._duration && buff_l._duration != 0)
			{
				buff_l.revert(*ent_l);
			}
			// increment time
			++ time_l;
		}
	}

	for(Player *player_l : state_p.getPlayers())
	{
		for(auto &&pair_l : player_l->_divAnchor)
		{
			pair_l.second -= _anchorLoss;
		}
	}
}

void TickingStep::revert(State &state_p, SteppableData *) const
{
	Logger::getDebug() << "TickingStep :: revert "<<std::endl;
	for(Entity *ent_l : state_p.getEntities())
	{
		if(ent_l->_reload > 0)
		{
			--ent_l->_reload;
		}

		// Handle buffs
		for(auto &&pair_l : ent_l->_timeSinceBuff)
		{
			std::string const &id_l = pair_l.first;
			unsigned long &time_l = pair_l.second;
			TyppedBuff const &buff_l = ent_l->_registeredBuff[id_l];

			// need to revert buff
			if(time_l == 1)
			{
				buff_l.revert(*ent_l);
			}
			// need to apply back buff (only when we go the exact duration to avoid reverting multiple times)
			if(time_l == buff_l._duration && buff_l._duration != 0)
			{
				buff_l.apply(*ent_l);
			}

			if(time_l > 0)
			{
				// decrement time
				-- time_l;
			}
		}
	}

	for(Player *player_l : state_p.getPlayers())
	{
		for(auto &&pair_l : player_l->_divAnchor)
		{
			pair_l.second += _anchorLoss;
		}
	}
}

bool TickingStep::isNoOp() const
{
	return false;
}

} // namespace octopus

