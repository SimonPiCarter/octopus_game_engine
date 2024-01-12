#ifndef __BuildingCancelStep__
#define __BuildingCancelStep__

#include "state/Handle.hh"
#include "step/Steppable.hh"
#include "utils/Fixed.hh"
#include <map>

namespace octopus
{
/// @brief this is aimed at building building
class BuildingCancelStep : public Steppable
{
	public:
		BuildingCancelStep(Handle const &handle_p, bool new_p, std::map<std::string, Fixed> const & delta_p)
			: _handle(handle_p), _new(new_p), _deltaResources(delta_p) {}

		virtual void consolidate(State const &state_p, Step const &step_p) override;

		virtual void apply(State &state_p) const override;
		virtual void revert(State &state_p, SteppableData const *) const override;

		virtual bool isNoOp() const override { return _old == _new; }
		virtual void visit(SteppableVisitor * visitor_p) const override
		{
			visitor_p->visit(this);
		}

		Handle const _handle;
		bool _old;
		bool const _new;
		std::map<std::string, Fixed> _deltaResources;
};
}

#endif
