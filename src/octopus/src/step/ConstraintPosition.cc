#include "ConstraintPosition.hh"

#include "state/State.hh"
#include "step/Step.hh"
#include "step/entity/EntityMoveStep.hh"

namespace octopus
{

bool ConstraintPositionX::apply(Vector const &pos_p) const
{
    if(pos_p.y <= maxY && pos_p.y >= minY)
    {
        if(less)
        {
            return pos_p.x > x;
        }
        return pos_p.x < x;
    }
    return false;
}

bool ConstraintPositionY::apply(Vector const &pos_p) const
{
    if(pos_p.x <= maxX && pos_p.x >= minX)
    {
        if(less)
        {
            return pos_p.y > y;
        }
        return pos_p.y < y;
    }
    return false;
}

std::list<ConstraintPositionX> &ConstraintPositionLibrary::getXConstraintForPlayer(unsigned long const &player_p)
{
    while(player_p >= _xConstraintPerPlayer.size())
    {
        _xConstraintPerPlayer.push_back({});
    }
    return _xConstraintPerPlayer[player_p];
}
std::list<ConstraintPositionX> const &ConstraintPositionLibrary::getXConstraintForPlayer(unsigned long const &player_p) const
{
    static std::list<ConstraintPositionX> empty_l = {};
    if(player_p >= _xConstraintPerPlayer.size())
    {
        return empty_l;
    }
    return _xConstraintPerPlayer.at(player_p);
}

std::list<ConstraintPositionY> &ConstraintPositionLibrary::getYConstraintForPlayer(unsigned long const &player_p)
{
    while(player_p >= _yConstraintPerPlayer.size())
    {
        _yConstraintPerPlayer.push_back({});
    }
    return _yConstraintPerPlayer[player_p];
}
std::list<ConstraintPositionY> const &ConstraintPositionLibrary::getYConstraintForPlayer(unsigned long const &player_p) const
{
    static std::list<ConstraintPositionY> empty_l = {};
    if(player_p >= _yConstraintPerPlayer.size())
    {
        return empty_l;
    }
    return _yConstraintPerPlayer.at(player_p);
}

void updateStepFromConstraintPosition(Step &step_p, State const &state_p, ConstraintPositionLibrary const &libConstraint_p)
{
    // for every move step
    for(EntityMoveStep *move_l : step_p.getEntityMoveStep())
    {
        Entity const *ent_l = state_p.getEntity(move_l->_handle);
        // get x constraint
        std::list<ConstraintPositionX> const &xConstraints_l = libConstraint_p.getXConstraintForPlayer(ent_l->_player);
        for(ConstraintPositionX const &cst_l : xConstraints_l)
        {
            Vector newPos_l = ent_l->_pos + move_l->_move;
            if(cst_l.apply(newPos_l))
            {
                move_l->_move.x = cst_l.x - ent_l->_pos.x;
            }
        }
        // then y constraint
        std::list<ConstraintPositionY> const &yConstraints_l = libConstraint_p.getYConstraintForPlayer(ent_l->_player);
        for(ConstraintPositionY const &cst_l : yConstraints_l)
        {
            Vector newPos_l = ent_l->_pos + move_l->_move;
            if(cst_l.apply(newPos_l))
            {
                move_l->_move.y = cst_l.y - ent_l->_pos.y;
            }
        }
    }
}

} // namespace octopus
