#include "GameLoop.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>
#include <thread>
#include <sstream>

// cuttlefish
#include "clicMode/AttackMoveClicMode.hh"
#include "clicMode/BuildClicMode.hh"
#include "clicMode/StandardClicMode.hh"
#include "cutscene/Dialog.hh"
#include "cutscene/DialogManager.hh"
#include "menu/Menu.hh"
#include "minimap/Minimap.hh"
#include "panel/DescPanel.hh"
#include "panel/OptionPanel.hh"
#include "panel/Panel.hh"
#include "panel/ProductionPanel.hh"
#include "panel/ResourcesPanel.hh"
#include "panel/StatsPanel.hh"
#include "sprite/Sprite.hh"
#include "sprite/SpriteLibrary.hh"
#include "text/Text.hh"
#include "text/WrappedText.hh"
#include "texture/Texture.hh"
#include "tilemap/Tilemap.hh"
#include "window/Window.hh"
#include "world/World.hh"

// octopus
#include "command/building/BuildingCancelCommand.hh"
#include "command/building/BuildingUnitCancelCommand.hh"
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "command/entity/EntityWaitCommand.hh"
#include "command/unit/UnitHarvestCommand.hh"
#include "controller/Controller.hh"
#include "library/Library.hh"
#include "logger/Logger.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/BuildingModel.hh"
#include "state/model/entity/EntityModel.hh"
#include "state/model/entity/UnitModel.hh"
#include "state/player/Player.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/Step.hh"

using octopus::to_double;

namespace cuttlefish
{

/// @brief delete clicMode_p execpt if equal to stdClicMode_p
void cleanClicMode(ClicMode const * clicMode_p, ClicMode const * stdClicMode_p)
{
	if(stdClicMode_p != clicMode_p)
	{
		delete clicMode_p;
	}
}

void commandFromSpriteModel(SpriteModel const *spriteModel_p, octopus::State const &state_p, SpriteLibrary const &spriteLib_p,
	Selection const &selection_p, octopus::Controller &controller_p, ClicMode *&clicMode_r, ClicMode const & stdClicMode_p, World const &world_p)
{
	if(spriteModel_p)
	{
		if(spriteModel_p->unitModel)
		{
			SpriteEntity const * ent_l = getBestProductionBuilding(selection_p, state_p, spriteModel_p->unitModel);
			if(ent_l)
			{
				octopus::BuildingUnitProductionCommand * command_l = new octopus::BuildingUnitProductionCommand(
						ent_l->getHandle(),
						ent_l->getHandle(),
						*spriteModel_p->unitModel
					);
				command_l->setQueued(true);
				controller_p.commitCommandAsPlayer(command_l, world_p.getPlayer());
			}
		}
		if(spriteModel_p->buildingModel)
		{
			cleanClicMode(clicMode_r, &stdClicMode_p);
			clicMode_r = new BuildClicMode(*spriteModel_p->buildingModel, spriteLib_p);
		}
	}
}

void controllerLoop(octopus::Controller &controller_p, bool const &over_p, bool const &paused_p, long &ratio_p)
{
	using namespace std::chrono_literals;

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
	double full_l = 0.;
	long updated_l = 0;
	while(!over_p)
	{
		// if paused do not update controller
		if(!paused_p)
		{
			// update controller
			updated_l += controller_p.update(std::min(0.01, elapsed_l));
		}
		while(!controller_p.loop_body()) {}

		auto cur_l = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
		elapsed_l = elapsed_seconds_l.count();
		last_l = cur_l;

		full_l += elapsed_l;
		if(full_l > 1.)
		{
			ratio_p = updated_l;
			full_l = 0.;
			updated_l = 0;
		}
	}
}

void quit(bool &quit_p)
{
	quit_p = true;
}

void unpause(bool &menu_p)
{
	menu_p = false;
}

void GameLoop::runLoop(Window &window_p)
{
    Texture const * background_l = window_p.loadTexture("resources/background.png");

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	bool quit_l = false;
	bool paused_l = false;
	bool menuActive_l = false;
	// ratio in % of time used (higher the better, gives % of speed on runtime)
	long ratio_l = 100;

	std::thread controllerThread_l(controllerLoop, std::ref(_controller), std::ref(quit_l), std::ref(paused_l), std::ref(ratio_l));

	cuttlefish::Dialog * dialog_l = nullptr;

	cuttlefish::Menu menu_l(window_p.getWidth()/2, 100, 5);
	{
		Picture * button_l = new Picture(window_p.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
		Text * text_l = new Text(&window_p, {0, 0, 0}, 0, 0);
		text_l->setText("Reprendre");
		menu_l.addButton(button_l, text_l, std::bind(unpause, std::ref(menuActive_l)));
	}
	{
		Picture * button_l = new Picture(window_p.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
		Text * text_l = new Text(&window_p, {0, 0, 0}, 0, 0);
		text_l->setText("Quitter");
		menu_l.addButton(button_l, text_l, std::bind(quit, std::ref(quit_l)));
	}

	double x = 0.;
	double y = 0.;
	double dX = 0.;
	double dY = 0.;
	double camSpeed_l = 600.;

	bool descActive_l = false;

	// true if the minimap has been clicked
	bool minimapClicked_l = false;

	// true if a clic was started outside of UI elemnts
	bool clicStartedOnScreen_l = false;

	// Text for resource
	Text textSteps_l(&window_p, {0,0,0}, 150, 0);

	StandardClicMode standardClicMode_l;
	ClicMode * currentClicMode_l = &standardClicMode_l;

	Selection &selection_l = _world.getSelection();

	std::pair<int, int> initialClic_l {-1, -1};

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;

	bool over_l = false;
	bool draw_l = false;
	unsigned long winningTeam_l = 0;
	//Event handler
	SDL_Event e;
	//While application is running
	while( !quit_l )
	{
		// query a new state if available
		octopus::StateAndSteps stateAndSteps_l = _controller.queryStateAndSteps();
		octopus::State const &state_l = *stateAndSteps_l._state;
		_world.handleStep(window_p, _panel, _statsPanel, _optionPanel, stateAndSteps_l, _spriteLibrary);


		// quit loop if state is over
		if(state_l.isOver())
		{
			over_l = true;
			draw_l = !state_l.hasWinningTeam();
			winningTeam_l = state_l.getWinningTeam();
			quit_l = true;
		}

		// get dialog if any
		if(!dialog_l)
		{
			dialog_l = _world.getDialogManager().nextDialog(window_p);
		}

		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit_l
			if( e.type == SDL_QUIT )
			{
				quit_l = true;
			}
			if(menuActive_l)
			{
				menu_l.handleEvent(window_p, e);
			}

			if(dialog_l)
			{
				// end dialog
				if ((e.type == SDL_MOUSEBUTTONDOWN
					&& dialog_l->getBackground().isInside(window_p, e.button.x, e.button.y))
				|| e.type == SDL_KEYDOWN)
				{
					delete dialog_l;
					dialog_l = nullptr;
				}
			}

			if( e.type == SDL_MOUSEMOTION )
			{
				dX = 0;
				dY = 0;
				int margin_l = 5;
				if(e.button.x < margin_l)
				{
					dX = - camSpeed_l;
				}
				if(e.button.x > window_p.getWidth() - margin_l)
				{
					dX = camSpeed_l;
				}
				if(e.button.y < margin_l)
				{
					dY = -camSpeed_l;
				}
				if(e.button.y > window_p.getHeight() - margin_l)
				{
					dY = camSpeed_l;
				}
				SpriteModel const * spriteModel_l = _panel.getSpriteModel(window_p, e.button.x, e.button.y);

				if(spriteModel_l)
				{
					if(spriteModel_l->unitModel)
					{
						_descPanel.setText(getDesc(*spriteModel_l->unitModel));
					}
					if(spriteModel_l->buildingModel)
					{
						_descPanel.setText(getDesc(*spriteModel_l->buildingModel));
					}
					descActive_l = true;
				}
				else
				{
					descActive_l = false;
				}

				if(minimapClicked_l)
				{
					octopus::Vector pos_l = getCameraPosition(e.button.x, e.button.y, _minimap, window_p, state_l.getWorldSize());
					x = to_double(pos_l.x);
					y = to_double(pos_l.y);
				}
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if(_prodPanel.getIndex(window_p, e.button.x, e.button.y)._inside)
				{
					// NA just eat clic
				}
				else if(_minimap.isInside(e.button.x, e.button.y))
				{
					if(e.button.button == SDL_BUTTON_LEFT && !dynamic_cast<AttackMoveClicMode *>(currentClicMode_l))
					{
						minimapClicked_l = true;
						octopus::Vector pos_l = getCameraPosition(e.button.x, e.button.y, _minimap, window_p, state_l.getWorldSize());
						x = to_double(pos_l.x);
						y = to_double(pos_l.y);
					}
				}
				else if(((!_panel.getBackground()->isInside(window_p, e.button.x, e.button.y)
				&& !_statsPanel.getBackground()->isInside(window_p, e.button.x, e.button.y)) || !_panel.isActive())
				&& (!_optionPanel.getBackground()->isInside(window_p, e.button.x, e.button.y) || !_optionPanel.isActive()))
				{
					currentClicMode_l->handleMouseDown(e);
					clicStartedOnScreen_l = true;
				}
			}
			if (e.type == SDL_MOUSEBUTTONUP)
			{
				IndexProductionClic indexProd_l = _prodPanel.getIndex(window_p, e.button.x, e.button.y);
				if(indexProd_l._inside)
				{
					_controller.commitCommandAsPlayer(new octopus::BuildingUnitCancelCommand(indexProd_l._handle, indexProd_l._idx), _world.getPlayer());
				}
				else if(menuActive_l)
				{
					// NA (just avoid doing anything else)
				}
				else if(_optionPanel.isActive())
				{
					int option_l = _optionPanel.getOption(window_p, e.button.x, e.button.y);
					if(option_l >=0)
					{
						_controller.commitCommandAsPlayer(_optionPanel.newCommandFromOption(option_l), _world.getPlayer());
					}
				}
				else if(_minimap.isInside(e.button.x, e.button.y) && !clicStartedOnScreen_l)
				{
					if(e.button.button == SDL_BUTTON_RIGHT)
					{
						// handle move command using right clic handle of the standard clic mode
						octopus::Vector pos_l = getPosition(e.button.x, e.button.y, _minimap, window_p, state_l.getWorldSize());
						standardClicMode_l.handleRightClic(pos_l, selection_l, _world, _panel, window_p, state_l, _controller, nullptr);
					} else if(e.button.button == SDL_BUTTON_LEFT)
					{
						AttackMoveClicMode * atkClicMode_l = dynamic_cast<AttackMoveClicMode *>(currentClicMode_l);
						if(atkClicMode_l)
						{
							octopus::Vector pos_l = getPosition(e.button.x, e.button.y, _minimap, window_p, state_l.getWorldSize());
							atkClicMode_l->handleLeftClic(pos_l, selection_l, _world, _panel, window_p, state_l, _controller, nullptr);
							// release attack mode after
							cleanClicMode(currentClicMode_l, &standardClicMode_l);
							currentClicMode_l = &standardClicMode_l;
						}
					}
				}
				else
				{
					SpriteModel const * spriteModel_l = _panel.getSpriteModel(window_p, e.button.x, e.button.y);

					if(spriteModel_l)
					{
						if(e.button.button == SDL_BUTTON_LEFT)
						{
							commandFromSpriteModel(spriteModel_l, state_l, _spriteLibrary, selection_l, _controller, currentClicMode_l, standardClicMode_l, _world);
						}
					}
					//
					else if((_panel.getBackground()->isInside(window_p, e.button.x, e.button.y)
						 || _statsPanel.getBackground()->isInside(window_p, e.button.x, e.button.y))
						 && !clicStartedOnScreen_l
						 && _panel.isActive())
					{
						// NA (skip selection and move command)
					}
					else
					{
						if( currentClicMode_l->handleMouseUp(e, selection_l, _world, _panel, window_p, state_l, _controller) )
						{
							cleanClicMode(currentClicMode_l, &standardClicMode_l);
							currentClicMode_l = &standardClicMode_l;
						}
					}
				}
				minimapClicked_l = false;
				clicStartedOnScreen_l = false;
			}
			if( e.type == SDL_KEYDOWN)
			{
				/* Check the SDLKey values and move change the coords */
				switch( e.key.keysym.sym ){
					/// handle panel
					case SDLK_a:
					{
						SpriteModel const * spriteModel_l = _panel.getSpriteModelOnGrid(0, 0);
						commandFromSpriteModel(spriteModel_l, state_l, _spriteLibrary, selection_l, _controller,
							currentClicMode_l, standardClicMode_l, _world);
						break;
					}
					case SDLK_z:
					{
						SpriteModel const * spriteModel_l = _panel.getSpriteModelOnGrid(1, 0);
						commandFromSpriteModel(spriteModel_l, state_l, _spriteLibrary, selection_l, _controller,
							currentClicMode_l, standardClicMode_l, _world);
						break;
					}
					case SDLK_e:
					{
						SpriteModel const * spriteModel_l = _panel.getSpriteModelOnGrid(2, 0);
						commandFromSpriteModel(spriteModel_l, state_l, _spriteLibrary, selection_l, _controller,
							currentClicMode_l, standardClicMode_l, _world);
						break;
					}
					case SDLK_r:
					{
						SpriteModel const * spriteModel_l = _panel.getSpriteModelOnGrid(3, 0);
						commandFromSpriteModel(spriteModel_l, state_l, _spriteLibrary, selection_l, _controller,
							currentClicMode_l, standardClicMode_l, _world);
						break;
					}
					case SDLK_q:
					{
						SpriteModel const * spriteModel_l = _panel.getSpriteModelOnGrid(0, 1);
						commandFromSpriteModel(spriteModel_l, state_l, _spriteLibrary, selection_l, _controller,
							currentClicMode_l, standardClicMode_l, _world);
						break;
					}
					case SDLK_SPACE:
					{
						cleanClicMode(currentClicMode_l, &standardClicMode_l);
						currentClicMode_l = new AttackMoveClicMode();
						break;
					}
					case SDLK_s:
					{
						for(SpriteEntity * spriteEnt_l : selection_l._sprites)
						{
							if(state_l.getEntity(spriteEnt_l->getHandle())->_model._isBuilding)
							{
								octopus::BuildingCancelCommand * command_l = new octopus::BuildingCancelCommand(
										spriteEnt_l->getHandle()
									);
								_controller.commitCommandAsPlayer(command_l, _world.getPlayer());
							}
							else
							{
								octopus::EntityWaitCommand * command_l = new octopus::EntityWaitCommand(
										spriteEnt_l->getHandle(),
										spriteEnt_l->getHandle()
									);
								_controller.commitCommandAsPlayer(command_l, _world.getPlayer());
							}
						}
						break;
					}
					case SDLK_ESCAPE:
					{
						menuActive_l = !menuActive_l;
						break;
					}
					case SDLK_0:
					case SDLK_1:
					case SDLK_2:
					case SDLK_3:
					case SDLK_4:
					case SDLK_5:
					case SDLK_6:
					case SDLK_7:
					case SDLK_8:
					case SDLK_9:
						if(KMOD_SHIFT & SDL_GetModState())
						{
							_world.addSelection(e.key.keysym.sym-SDLK_0);
						}
						else if(KMOD_CTRL & SDL_GetModState())
						{
							_world.registerSelection(e.key.keysym.sym-SDLK_0);
						}
						else
						{
							_world.useSelection(e.key.keysym.sym-SDLK_0);
							_panel.refresh(_world.getSelection()._sprite, state_l);
						}
						break;
					default:
						break;
				}
			}
			currentClicMode_l->reset(e);
		}
		x += dX * elapsed_l;
		y += dY * elapsed_l;
		window_p.clampCamera(x, y);
		window_p.setCamera(x, y);
		window_p.clear();

		auto cur_l = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
		elapsed_l = elapsed_seconds_l.count();
		last_l = cur_l;

		_tilemap.render(window_p, elapsed_l);

		_world.display(window_p, state_l, elapsed_l);

		_world.getSelection().render(window_p);

		_minimap.render(state_l, _world, window_p);

		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		currentClicMode_l->display(window_p, elapsed_l, mouseX, mouseY);

		if(_panel.isActive())
		{
			_panel.render(window_p);
			_statsPanel.refresh(window_p, state_l);
			_statsPanel.render(window_p);
		}


		_prodPanel.refresh(window_p, state_l);
		_prodPanel.render(window_p);
		if(descActive_l)
		{
			_descPanel.render(window_p);
		}

		if(dialog_l)
		{
			dialog_l->display(window_p);
		}

		_optionPanel.refresh();
		paused_l = _optionPanel.isActive() || menuActive_l || dialog_l != nullptr;
		if(_optionPanel.isActive())
		{
			_optionPanel.render(window_p);
		}
		if(menuActive_l)
		{
			menu_l.display(elapsed_l, window_p);
		}

		_resPanel.refresh(window_p, state_l, _world.getPlayer());
		_resPanel.render(window_p);

		///
		/// Debug texts
		///
		octopus::Player const * player_l = state_l.getPlayer(_world.getPlayer());

		//Render background texture to screen
		background_l->render(window_p.getRenderer(), 0, 0, 25, window_p.getWidth()/3 );

		std::stringstream ss_l;
		ss_l << "step : "<<_controller.getOngoingStep()
		<<"     ratio : "<<int(ratio_l*_controller.getTimePerStep()*100.) << "%"
		<<"     entity count : "<<_world.getListSprite().size();
		textSteps_l.setText(ss_l.str());
		textSteps_l.display(window_p);

		window_p.draw();
	}

	if(!over_l)
	{
		std::cout<<"was not over"<<std::endl;
	}
	else if(draw_l)
	{
		std::cout<<"ended in a draw"<<std::endl;
	}
	else if (winningTeam_l == 0)
	{
		std::cout<<"won"<<std::endl;
	}
	else
	{
		std::cout<<"lost"<<std::endl;
	}

	controllerThread_l.join();

	streamMetrics(std::cout, _controller.getMetrics());
}

}
