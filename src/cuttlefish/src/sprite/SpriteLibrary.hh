#ifndef __SpriteLibrary__
#define __SpriteLibrary__

#include <unordered_map>
#include <string>
#include <vector>

#include "state/Handle.hh"

namespace cuttlefish
{
class Sprite;
class SpriteEntity;
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

	/// @brief dying state (-1 if not applicable)
	int dyingState {-1};

	/// @brief enable oiverriding sprite per player id
	std::unordered_map<unsigned long, Texture const *> mapTexturePerPlayer;
};

/// @brief store sprites based on
class SpriteLibrary
{
public:
	void registerSpriteTemplate(std::string const &id_p, Texture const * texture_p, double scale_p, int logX_p, int logY_p, int width_p, int height_p,
			std::vector<int> const &nbFramesPerState_p, std::vector<double> const &timePerFramePerState_p, int blueprintState_p);

	void registerAltTextureForTemplate(std::string const &id_p, unsigned long player_p, Texture const * texture_p);
	void addDyingState(std::string const &id_p, int state_p);

	Sprite * createSprite(std::string const &id_p, bool absolute_p) const;
	SpriteEntity * createSpriteEntity(octopus::Handle const &handle_p, std::string const &id_p, bool hpBar_p, unsigned long player_p) const;

	SpriteTemplate const & getSpriteTemplate(std::string const &id_p) const;

	int getBlueprintState(std::string const &id_p) const;

	void setHpBarBackground(Texture const * texture_p);
	void setHpBarFilling(Texture const * texture_p);

private:
	std::unordered_map<std::string, SpriteTemplate> _mapTemplates;

	Texture const * _hpBarBackground;
	Texture const * _hpBarFilling;
};

}

#endif
