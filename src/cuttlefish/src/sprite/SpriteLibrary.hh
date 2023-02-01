#ifndef __SpriteLibrary__
#define __SpriteLibrary__

#include <unordered_map>
#include <string>
#include <vector>

#include "state/Handle.hh"

namespace cuttlefish
{
class Sprite;
class Texture;

/// @brief contains all information required to sprite loading
struct SpriteTemplate
{
	Texture const * texture;
	double scale;
	int logX;
	int logY;
	int width;
	int height;
	std::vector<int> nbFramesPerState;
	std::vector<double> timePerFramePerState;

	/// @brief blue print state (-1 if not applicable)
	int blueprint {-1};
};

/// @brief store sprites based on
class SpriteLibrary
{
public:
	void registerSpriteTemplate(std::string const &id_p, Texture const * texture_p, double scale_p, int logX_p, int logY_p, int width_p, int height_p,
			std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, int blueprintState_p);

	Sprite * createSprite(octopus::Handle const &handle_p, std::string const &id_p, bool absolute_p) const;

	int getBlueprintState(std::string const &id_p) const;

private:
	std::unordered_map<std::string, SpriteTemplate> _mapTemplates;
};

}

#endif
