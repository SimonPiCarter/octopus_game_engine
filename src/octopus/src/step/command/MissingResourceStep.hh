#ifndef __MissingResourceStep__
#define __MissingResourceStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"

namespace octopus
{
/// @brief this incread production for the given command
class MissingResourceStep : public Steppable
{
	public:
		MissingResourceStep(unsigned long player_p) : _player(player_p) {}

		virtual void apply(State &) const override {}
		virtual void revert(State &) const override {}

		virtual bool isNoOp() const override
		{
			return false;
		}
		virtual void visit(SteppableVisitor * visitor_p) const override
		{
			visitor_p->visit(this);
		}

		unsigned long _player {0};
};
}

#endif
