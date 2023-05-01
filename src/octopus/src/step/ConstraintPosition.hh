#ifndef __ConstraintPosition__
#define __ConstraintPosition__

#include <list>
#include <vector>

#include "utils/Fixed.hh"
#include "utils/Vector.hh"

namespace octopus
{
/// @brief Object to add a constraint on X for any entity
/// any entity in the Y range (ray included) won't be able
/// to cross this constraint
struct ConstraintPositionX
{
    /// @brief the value of the constraint
    Fixed const x;
    /// @brief the min value of y where the constraint apply
    Fixed const minY;
    /// @brief the min value of y where the constraint apply
    Fixed const maxY;
    /// @brief set if the constraint force entity < of the given x
    bool const less;

    /// @brief check if the position violates the constraint
    bool apply(Vector const &pos_p) const;
};

/// @brief Object to add a constraint on Y for any entity
/// any entity in the Y range (ray included) won't be able
/// to cross this constraint
struct ConstraintPositionY
{
    /// @brief the value of the constraint
    Fixed const y;
    /// @brief the min value of y where the constraint apply
    Fixed const minX;
    /// @brief the min value of y where the constraint apply
    Fixed const maxX;
    /// @brief set if the constraint force entity < of the y given
    bool const less;

    /// @brief check if the position violates the constraint
    bool apply(Vector const &pos_p) const;
};

/// @brief class that stors all constraint for every player
class ConstraintPositionLibrary
{
public:
    std::list<ConstraintPositionX> &getXConstraintForPlayer(unsigned long const &player_p);
    std::list<ConstraintPositionX> const &getXConstraintForPlayer(unsigned long const &player_p) const;

    std::list<ConstraintPositionY> &getYConstraintForPlayer(unsigned long const &player_p);
    std::list<ConstraintPositionY> const &getYConstraintForPlayer(unsigned long const &player_p) const;
private:
    std::vector<std::list<ConstraintPositionX> > _xConstraintPerPlayer;
    std::vector<std::list<ConstraintPositionY> > _yConstraintPerPlayer;
};

class Step;
class State;

/// @brief will update all EntityMoveStep position to comply to the constraints
void updateStepFromConstraintPosition(Step &step_p, State const &state_p, ConstraintPositionLibrary const &libConstraint_p);

} // octopus

#endif
