#ifndef __ResourceLoader__
#define __ResourceLoader__

#include "minimap/Minimap.hh"
#include "panel/DescPanel.hh"
#include "panel/DivinityPanel.hh"
#include "panel/Panel.hh"
#include "panel/ProductionPanel.hh"
#include "panel/ResourcesPanel.hh"
#include "panel/StatsPanel.hh"
#include "sprite/SpriteLibrary.hh"
#include "tilemap/Tilemap.hh"

#include "ChoicePanel.hh"

namespace fas
{

/// @brief Loading all resource on instanciation
class ResourceLoader
{
public:
	ResourceLoader(cuttlefish::Window &window_p, unsigned long fullWorldSize_p, cuttlefish::World &world_p);

	cuttlefish::Panel _panel;
	cuttlefish::ProductionPanel _prodPanel;
	cuttlefish::ResourcesPanel _resourcesPanel;
	cuttlefish::StatsPanel _statsPanel;
	fas::ChoicePanel _choicePanel;
	cuttlefish::DescPanel _descPanel;
	cuttlefish::SpriteLibrary _spriteLib;
	cuttlefish::Tilemap _tilemap;
	cuttlefish::Minimap _minimap;
};

} // namespace fas


#endif