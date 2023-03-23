#include "DialogManager.hh"

#include "window/Window.hh"

namespace cuttlefish
{

DialogManager::~DialogManager()
{}

Dialog * DialogManager::nextDialog(Window &window_p)
{
    if(_steps.empty())
    {
        return nullptr;
    }
    DialogStep const *step_l = _steps.front();
    Dialog * dialog_l = new Dialog(&window_p, 295, window_p.getHeight()-250,
        Picture(window_p.loadTexture("resources/fair_and_square/panel.png"), 276, 276, {1}, {1}),
        step_l->_name,
        step_l->_text,
        step_l->_picture);
    _steps.pop_front();
    return dialog_l;
}

void DialogManager::addDialogStep(DialogStep const *step_p)
{
    _steps.push_back(step_p);
}

}