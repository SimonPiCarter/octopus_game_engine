#ifndef __PatternHandler__
#define __PatternHandler__

#include <vector>
#include <unordered_map>

namespace octopus
{
typedef std::vector<std::pair<long, long> > VisionPattern;

/// @brief This class manages vision patterns based on line of sight and movement
class PatternHandler
{
public:
	/// @brief get pattern for the given line of sight
	VisionPattern const &getPattern(long lineOfSight_p) const;

	/// @brief get patterns for the given movement
	/// it only returns the patterns to increment (to get the pattern to decrement one need to call)
	/// this function with -dx and -dy where dx and dy is the movement of the entity
	VisionPattern const &getMovementPattern(long lineOfSight_p, long dx, long dy) const;
protected:
	/// @brief cache vision pattern based on line of sight
	mutable std::unordered_map<long, VisionPattern> _patterns;

	/// @brief cache vision pattern for movement
	/// indexing si done by : los, dx, dy
	mutable std::unordered_map<long, std::unordered_map<long, std::unordered_map<long, VisionPattern> > > _movingPatterns;
};

} // octopus

#endif
