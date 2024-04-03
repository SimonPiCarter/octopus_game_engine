#include "AnyBuff.hh"

namespace octopus
{

void apply_buff(AnyBuff const &buff_p, Entity &ent_p)
{
    std::visit([&](auto && arg) { arg.apply(ent_p); }, buff_p);
}

void revert_buff(AnyBuff const &buff_p, Entity &ent_p)
{
    std::visit([&](auto && arg) { arg.revert(ent_p); }, buff_p);
}

unsigned long get_duration(AnyBuff const &buff_p)
{
	unsigned long duration_l = 0;
    std::visit([&](auto && arg) { duration_l = arg._duration; }, buff_p);
	return duration_l;
}

const std::string & get_id(AnyBuff const &buff_p)
{
	std::string const * str_l = nullptr;
    std::visit([&](auto && arg) { str_l = &arg._id; }, buff_p);
	return *str_l;
}

bool is_no_op_decoy(TimedBuff const &buff_p)
{
	return ::is_zero(buff_p._offset) && ::is_zero(buff_p._coef);
}

bool is_no_op_decoy(ModifierBuff const &buff_p)
{
	return std::holds_alternative<NoModifier>(buff_p._attackMod);
}

bool is_no_op_decoy(ModifierAoEBuff const &buff_p)
{
	return ::is_zero(buff_p._deltaRatio) && ::is_zero(buff_p._deltaRange);
}

bool is_no_op(AnyBuff const &buff_p)
{
	bool no_op_l = false;
    std::visit([&](auto && arg) { no_op_l = is_no_op_decoy(arg); }, buff_p);
	return no_op_l;
}

}
