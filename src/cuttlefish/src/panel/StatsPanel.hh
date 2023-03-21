#ifndef __StatsPanel__
#define __StatsPanel__

#include <list>
#include <map>
#include "Panel.hh"
#include "sprite/Sprite.hh"
#include "text/SegmentedText.hh"
#include "world/Selection.hh"

#include "stats/ProductionPicture.hh"
#include "stats/SelectionPicture.hh"

namespace octopus
{
	class Entity;
	class State;
	struct UnitModel;
	struct BuildingModel;
} // namespace octopus


namespace cuttlefish
{
class Texture;
class Window;

/// @brief this class represent a StatsPanel
/// It can display either
/// stats when only one entity is selected
/// selection when multiple entities are selected
class StatsPanel
{
public:
	StatsPanel(Window* window_p, int x, int y, Texture const * background_p, Texture const *icons_p, Texture const *statsIcons_p, int iconsPerLine_p, Selection & selection_p);
	~StatsPanel();

	/// @brief refresh StatsPanel if necessary
	void refresh(Window &window_p, octopus::State const &state_p);

	/// @brief display StatsPanel
	void render(Window &window_p);

	/// @brief add a sprite info for a given model
	void addSpriteInfo(std::string const &model_p, int state_p, int frame_p);

	/// @brief return a sprite model combination if clicked
	/// @note return nullptr if no clic
	SpriteModel const * getSpriteModel(Window &window_p, int x, int y) const;

	/// @brief return a sprite model combination for grid position
	/// @note return nullptr if no match
	SpriteModel const * getSpriteModelOnGrid(int x, int y) const;

	Picture const * getBackground() const { return _background; }
protected:
	/// @brief selection
	Selection & _selection;
	/// @brief mono selection (only non null if mono selection)
	octopus::Entity const * _monoSelection {nullptr};
	/// @brief key of last selection for fast compare
	SelectionKey _lastKey;

	/// @brief sprites
	std::vector<SelectionPicture *> _selectionPictures;
	/// @brief sprite models used for grid coordinate access
	std::map<std::pair<int, int>, SpriteModel *> _grid;

	Picture * _background;

	/// @brief map of sprite info (state/frame)
	/// for models
	std::map<std::string, SpriteInfo> _mapIcons;

	Texture const * const _icons;

	int _x;
	int _y;
	int const _iconsPerLine;

	///
	/// Mono selection stats
	///
	Picture _monoIcon;
	Picture _damageIcon;
	Picture _armorIcon;
	Picture _attackSpeedIcon;
	Picture _speedIcon;

	SegmentedText _textHp;
	SegmentedText _textDamage;
	SegmentedText _textArmor;
	SegmentedText _textAttackSpeed;
	SegmentedText _textSpeed;

	ProgressBar _hpBar;

	SegmentedText _textQtyRes;
	SegmentedText _textResources;
};
} // namespace octopus


#endif
