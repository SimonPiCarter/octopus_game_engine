#ifndef __STEPPABLE__
#define __STEPPABLE__

namespace octopus
{
	class State;

	/// @brief define an interface for
	/// apply and revert on a Steppable, they
	/// are contained in a Step object that
	/// represent a step from one state to another
	/// It is mandatory that Stappable apply and
	/// revert does the exact opposite actions
	class Steppable
	{
		public:
			virtual ~Steppable() {}
			/// @brief apply this Steppable to the given state
			virtual void apply(State &state_p) const = 0;
			/// @brief revert this Steppable to the given state
			virtual void revert(State &state_p) const = 0;

			/// @brief return true if this Steppable does no operation on the State it would be applied
			virtual bool isNoOp() const = 0;

	};
}

#endif
