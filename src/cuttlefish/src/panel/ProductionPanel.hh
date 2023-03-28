#ifndef __ProductionPanel__
#define __ProductionPanel__

#include <list>
#include <map>
#include "Panel.hh"
#include "sprite/Sprite.hh"
#include "text/SegmentedText.hh"
#include "world/Selection.hh"

#include "stats/ProductionPicture.hh"
#include "stats/SelectionPicture.hh"
#include "state/Handle.hh"

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

struct IndexProductionClic
{
	octopus::Handle _handle;
	unsigned long _idx;
	bool _inside;
};

/// @brief this class represent a ProductionPanel
/// It can display either
/// stats when only one entity is selected
/// selection when multiple entities are selected
class ProductionPanel
{
public:
	ProductionPanel(Window* window_p, int x, int y, Texture const *icons_p, Texture const *barBack_p, Texture const *barFill_p, int maxIcons_p, Selection & selection_p);
	~ProductionPanel();

	/// @brief refresh the production panel
	void refresh(Window &window_p, octopus::State const &state_p);

	/// @brief display ProductionPanel
	void render(Window &window_p);

	/// @brief add a sprite info for a given model
	void addSpriteInfo(std::string const &model_p, int state_p, int frame_p);

	IndexProductionClic getIndex(Window &window_p, int x, int y);
protected:
	/// @brief selection
	Selection & _selection;
	/// @brief key of last selection for fast compare
	SelectionKey _lastKey;

	/// @brief sprites
	std::vector<ProductionPicture *> _productionPictures;

	/// @brief this is used to store production index in case of cancel
	std::vector<unsigned long> _productionIndex;
	std::vector<octopus::Handle> _productionHandle;

	/// @brief map of sprite info (state/frame)
	/// for models
	std::map<std::string, SpriteInfo> _mapIcons;

	Texture const * const _icons;

	int _productionToDisplay {0};

	int const _x;
	int const _y;
};
} // namespace octopus


#endif
