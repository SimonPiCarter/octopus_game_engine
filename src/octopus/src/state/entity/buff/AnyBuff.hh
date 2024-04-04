#ifndef __AnyBuff__
#define __AnyBuff__

#include "CompositeBuff.hh"
#include "TimedBuff.hh"
#include "ModifierAoEBuff.hh"
#include "ModifierBuff.hh"
#include <variant>

namespace octopus
{

using AnyBuff = std::variant<TimedBuff, ModifierBuff, ModifierAoEBuff, CompositeBuff>;

void apply_buff(AnyBuff const &buff_p, Entity &ent_p);
void revert_buff(AnyBuff const &buff_p, Entity &ent_p);
unsigned long get_duration(AnyBuff const &buff_p);
const std::string & get_id(AnyBuff const &buff_p);
bool is_no_op(AnyBuff const &buff_p);

} // namespace octopus

#endif
