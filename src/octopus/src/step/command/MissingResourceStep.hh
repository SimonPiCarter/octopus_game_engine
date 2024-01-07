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
		MissingResourceStep(unsigned long player_p, std::string const &resource_p) : _player(player_p), _resource(resource_p) {}

		virtual void apply(State &) const override {}
		virtual void revert(State &, SteppableData const *) const override {}

		virtual bool isNoOp() const override
		{
			return false;
		}
		virtual void visit(SteppableVisitor * visitor_p) const override
		{
			visitor_p->visit(this);
		}

		unsigned long _player {0};
		std::string _resource;

		static std::string MissingRequirement;
		static std::string BadPlacement;
};
}

#endif
