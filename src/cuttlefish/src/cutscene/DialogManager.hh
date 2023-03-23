#ifndef __DialogManager__
#define __DialogManager__

#include <map>
#include <string>

#include "Dialog.hh"
#include "step/DialogStep.hh"

namespace cuttlefish
{
class Window;

/// @brief This class manages dialog that should be displayed
class DialogManager
{
public:
	virtual ~DialogManager();
	/// @brief return a new dialog based on the first step in the list of _steps
	/// @return nullptr if no dialog to spawn
	Dialog * nextDialog(Window &window_p);

	/// @brief add a dialog step in the list (at the back)
	void addDialogStep(DialogStep const *);
private:
	/// @brief list of dialogs in the order they must be displayed
	std::list<DialogStep const *> _steps;
};
} // namespace cuttlefish


#endif