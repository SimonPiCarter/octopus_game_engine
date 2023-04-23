#ifndef __BuildingRemoveRallyPointStep__
#define __BuildingRemoveRallyPointStep__

#include "state/Handle.hh"
#include "utils/Vector.hh"
#include "step/Steppable.hh"

namespace octopus
{


/// @brief this is aimed at setting building rally point
class BuildingRemoveRallyPointStep : public Steppable
{
	public:
		BuildingRemoveRallyPointStep(Handle const &handle_p)
            : _handle(handle_p) {}

		virtual void apply(State &state_p) const override;
		virtual void revert(State &state_p, SteppableData const *) const override;

		virtual bool isNoOp() const override { return false; }
		virtual void visit(SteppableVisitor * visitor_p) const override
		{
			visitor_p->visit(this);
		}

        virtual SteppableData * newData(State const &) const;

    private:
		Handle const _handle;
};
}

#endif
