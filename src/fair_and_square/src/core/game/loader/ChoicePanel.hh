#ifndef __ChoicePanel__
#define __ChoicePanel__

#include "panel/DivinityPanel.hh"

namespace fas
{

/// @brief this class represent a Divinity Panel
/// displaying the options for divinity
class ChoicePanel : public cuttlefish::DivinityPanel
{
public:
	ChoicePanel(int x, int y, cuttlefish::Texture const * background_p, cuttlefish::Texture const *icons_p, unsigned long player_p);

	virtual void addOptionLayer(octopus::PlayerAddOptionStep const *) override;
};
} // namespace octopus


#endif
