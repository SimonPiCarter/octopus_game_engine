#ifndef __BasedBuff__
#define __BasedBuff__

#include <string>

namespace octopus
{

class State;
class Entity;

struct BaseBuff
{
    /// @brief 0 for infinite buff
	unsigned long _duration {0};

	std::string _id;

	/// @brief check if the buff apply to the entity
	/// @param ent_p the entity to test
	/// @return true if the buff apply to the entity
	virtual bool isApplying(State const &state_p, Entity const &ent_p) const = 0;

	/// @brief check if the buff apply to the entity
	/// @param source_p the entity buffing
	/// @param ent_p the entity to test
	/// @return true if the buff apply to the entity
	virtual bool isApplying(State const &state_p, Entity const &source_p, Entity const &ent_p) const = 0;

	virtual void apply(Entity &ent_p) const = 0;
	virtual void revert(Entity &ent_p) const = 0;
};

} // namespace octopus

#endif
