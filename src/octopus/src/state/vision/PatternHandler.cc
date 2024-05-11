#include "PatternHandler.hh"

#include "VisionGrid.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/State.hh"
#include "state/player/Player.hh"
#include "utils/Box.hh"


namespace octopus
{

VisionPattern const &PatternHandler::getPattern(long lineOfSight_p) const
{
	// check cache
	auto && it_l = _patterns.find(lineOfSight_p);
	if(it_l != _patterns.end())
	{
		return it_l->second;
	}

	VisionPattern & pattern_l = _patterns[lineOfSight_p];

	// compute a box to avoid checking full map
	Box<long> box_l { - (lineOfSight_p+1),
					  lineOfSight_p+1,
					  - (lineOfSight_p+1),
					  lineOfSight_p+1
					};

	Fixed ref_l(lineOfSight_p*lineOfSight_p);
	// check distance on all nodes in the subbox
	for(long x = box_l._lowerX; x <= box_l._upperX; ++x)
	{
		for(long y = box_l._lowerY; y <= box_l._upperY; ++y)
		{
			if(square_length(Vector(x, y)) < ref_l)
			{
				pattern_l.push_back(std::make_pair(x, y));
			}
		}
	}

	return pattern_l;
}

VisionPattern const &PatternHandler::getMovementPattern(long lineOfSight_p, long dx, long dy) const
{
	// check cache
	if((dx == 0 && dy == 0)
	|| !_movingPatterns[lineOfSight_p][dx][dy].empty() )
	{
		return _movingPatterns[lineOfSight_p][dx][dy];
	}

	// get pattern for given los
	VisionPattern const & pattern_l = getPattern(lineOfSight_p);

	VisionPattern &movingPattern_l = _movingPatterns[lineOfSight_p][dx][dy];

	// list pattern for faster deletion
	std::list<std::pair<long, long> > listPattern_l(pattern_l.begin(), pattern_l.end());

	for(std::pair<long, long> const &pair_l : pattern_l)
	{
		long x = pair_l.first - dx;
		long y = pair_l.second - dy;

		listPattern_l.remove(std::make_pair(x, y));
	}
	movingPattern_l.insert(movingPattern_l.end(), listPattern_l.begin(), listPattern_l.end());

	return movingPattern_l;
}


} // octopus
