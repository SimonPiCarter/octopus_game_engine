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
#include "minimap/Minimap.hh"
#include "panel/DescPanel.hh"
#include "panel/DivinityPanel.hh"
#include "panel/Panel.hh"
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
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
#include "command/entity/EntityWaitCommand.hh"
#include "command/unit/UnitHarvestCommand.hh"
#include "command/player/PlayerChoseDivinityCommand.hh"
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

std::string resourceStr(octopus::Player const &player_p)
{
	std::stringstream ss_l;
	ss_l << "Food   : "<<stringify(std::abs(std::floor(1e-5+octopus::getResource(player_p, octopus::ResourceType::Food))))<<" | ";
	ss_l << "Steel  : "<<stringify(std::abs(std::floor(1e-5+octopus::getResource(player_p, octopus::ResourceType::Steel))))<<" | ";
	return ss_l.str();
}

std::string divAnchorStr(octopus::Player const &player_p)
{
	std::stringstream ss_l;
	ss_l << "anc | ";
	ss_l << "div1 : "<<stringify(int(octopus::getDivAnchor(player_p, octopus::DivinityType::Divinity_1)))<<" | ";
	return ss_l.str();
}

std::string divLvlStr(octopus::Player const &player_p)
{
	std::stringstream ss_l;
	ss_l << "lvl | ";
	ss_l << "div1 : "<<stringify(octopus::getDivLvl(player_p, octopus::DivinityType::Divinity_1))<<" | ";
	return ss_l.str();
}

/// @brief delete clicMode_p execpt if equal to stdClicMode_p
void cleanClicMode(ClicMode const * clicMode_p, ClicMode const * stdClicMode_p)
{
	if(stdClicMode_p != clicMode_p)
	{
		delete clicMode_p;
	}
}

void commandFromSpriteModel(SpriteModel const *spriteModel_p, octopus::State const &state_p, SpriteLibrary const &spriteLib_p,
	Selection const &selection_p, octopus::Controller &controller_p, ClicMode *&clicMode_r, ClicMode const & stdClicMode_p)
{
	if(spriteModel_p)
	{
		if(spriteModel_p->unitModel)
		{
			SpriteEntity const * ent_l = getBestProductionBuilding(selection_p, state_p, spriteModel_p->unitModel);
			octopus::BuildingUnitProductionCommand * command_l = new octopus::BuildingUnitProductionCommand(
					ent_l->getHandle(),
					ent_l->getHandle(),
					*spriteModel_p->unitModel
				);
			command_l->setQueued(true);
			controller_p.commitCommand(command_l);
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

void GameLoop::runLoop(Window &window_p)
{
    Texture const * background_l = window_p.loadTexture("resources/background.png");

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	bool quit_l = false;
	bool paused_l = false;
	// ratio in % of time used (higher the better, gives % of speed on runtime)
	long ratio_l = 100;

	std::thread controllerThread_l(controllerLoop, std::ref(_controller), std::ref(quit_l), std::ref(paused_l), std::ref(ratio_l));

	double x = 0.;
	double y = 0.;
	double dX = 0.;
	double dY = 0.;
	double camSpeed_l = 400.;

	bool descActive_l = false;

	// true if the minimap has been clicked
	bool minimapClicked_l = false;

	// true if a clic was started outside of UI elemnts
	bool clicStartedOnScreen_l = false;

	// Text for resource
	Text textResource_l(&window_p, {0,0,0}, 250, 0);
	Text textDivLvl_l(&window_p, {0,0,0}, 500, 0);
	Text textDivAnchor_l(&window_p, {0,0,0}, 650, 0);
	Text textSteps_l(&window_p, {0,0,0}, 0, 20);

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
		_world.handleStep(window_p, _panel, _statsPanel, _divinityPanel, stateAndSteps_l, _spriteLibrary);

		// quit loop if state is over
		if(state_l.isOver())
		{
			over_l = true;
			draw_l = !state_l.hasWinningTeam();
			winningTeam_l = state_l.getWinningTeam();
			quit_l = true;
		}

		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
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
			//User requests quit_l
			if( e.type == SDL_QUIT )
			{
				quit_l = true;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if(_minimap.isInside(e.button.x, e.button.y))
				{
					if(e.button.button == SDL_BUTTON_LEFT)
					{
						minimapClicked_l = true;
						octopus::Vector pos_l = getCameraPosition(e.button.x, e.button.y, _minimap, window_p, state_l.getWorldSize());
						x = to_double(pos_l.x);
						y = to_double(pos_l.y);
					}
				}
				else if(!_panel.getBackground()->isInside(window_p, e.button.x, e.button.y)
				&& !_statsPanel.getBackground()->isInside(window_p, e.button.x, e.button.y)
				&& (!_divinityPanel.getBackground()->isInside(window_p, e.button.x, e.button.y) || !_divinityPanel.isActive()))
				{
					currentClicMode_l->handleMouseDown(e);
					clicStartedOnScreen_l = true;
				}
			}
			if (e.type == SDL_MOUSEBUTTONUP)
			{
				if(_divinityPanel.isActive())
				{
					std::pair<bool, octopus::DivinityType> option_l = _divinityPanel.getOption(window_p, e.button.x, e.button.y);
					if(option_l.first)
					{
						octopus::PlayerChoseDivinityCommand * command_l = new octopus::PlayerChoseDivinityCommand(0, option_l.second);
						_controller.commitCommand(command_l);
						_divinityPanel.popOptionLayer();
					}
				}
				else if(_minimap.isInside(e.button.x, e.button.y) && !clicStartedOnScreen_l)
				{
					if(e.button.button == SDL_BUTTON_RIGHT)
					{
						// handle move command using right clic handle of the standard clic mode
						octopus::Vector pos_l = getPosition(e.button.x, e.button.y, _minimap, window_p, state_l.getWorldSize());
						standardClicMode_l.handleRightClic(pos_l, selection_l, _world, _panel, window_p, state_l, _controller, nullptr);
					}
				}
				else
				{
					SpriteModel const * spriteModel_l = _panel.getSpriteModel(window_p, e.button.x, e.button.y);

					if(spriteModel_l)
					{
						if(e.button.button == SDL_BUTTON_LEFT)
						{
							commandFromSpriteModel(spriteModel_l, state_l, _spriteLibrary, selection_l, _controller, currentClicMode_l, standardClicMode_l);
						}
					}
					//
					else if((_panel.getBackground()->isInside(window_p, e.button.x, e.button.y)
						 || _statsPanel.getBackground()->isInside(window_p, e.button.x, e.button.y))
						 && !clicStartedOnScreen_l)
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
							currentClicMode_l, standardClicMode_l);
						break;
					}
					case SDLK_z:
					{
						SpriteModel const * spriteModel_l = _panel.getSpriteModelOnGrid(1, 0);
						commandFromSpriteModel(spriteModel_l, state_l, _spriteLibrary, selection_l, _controller,
							currentClicMode_l, standardClicMode_l);
						break;
					}
					case SDLK_e:
					{
						SpriteModel const * spriteModel_l = _panel.getSpriteModelOnGrid(2, 0);
						commandFromSpriteModel(spriteModel_l, state_l, _spriteLibrary, selection_l, _controller,
							currentClicMode_l, standardClicMode_l);
						break;
					}
					case SDLK_q:
					{
						SpriteModel const * spriteModel_l = _panel.getSpriteModelOnGrid(0, 1);
						commandFromSpriteModel(spriteModel_l, state_l, _spriteLibrary, selection_l, _controller,
							currentClicMode_l, standardClicMode_l);
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
							octopus::EntityWaitCommand * command_l = new octopus::EntityWaitCommand(
									spriteEnt_l->getHandle(),
									spriteEnt_l->getHandle()
								);
							_controller.commitCommand(command_l);
						}
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

		_panel.render(window_p);
		_statsPanel.refresh(window_p, state_l);
		_statsPanel.render(window_p);
		if(descActive_l)
		{
			_descPanel.render(window_p);
		}

		_divinityPanel.refresh();
		paused_l = _divinityPanel.isActive();
		if(_divinityPanel.isActive())
		{
			_divinityPanel.render(window_p);
		}

		///
		/// Debug texts
		///
		octopus::Player const * player_l = state_l.getPlayer(_world.getPlayer());

		//Render background texture to screen
		background_l->render(window_p.getRenderer(), 0, 0, 50, window_p.getWidth() );

		textResource_l.setText(resourceStr(*player_l));
		textResource_l.display(window_p);
		textDivLvl_l.setText(divLvlStr(*player_l));
		textDivLvl_l.display(window_p);
		textDivAnchor_l.setText(divAnchorStr(*player_l));
		textDivAnchor_l.display(window_p);

		std::stringstream ss_l;
		ss_l << stateAndSteps_l._steps.size()<<"/"<<_controller.getOngoingStep()
		<<"     ratio : "<<ratio_l << "%"
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