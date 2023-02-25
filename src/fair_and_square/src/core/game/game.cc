#include "game.hh"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>
#include <thread>
#include <sstream>

// cuttlefish
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

#include "library/wave_level/WaveLevel.hh"

using namespace cuttlefish;

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

void commandFromSpriteModel(SpriteModel const *spriteModel_l, octopus::Library const &lib_l, SpriteLibrary const &spriteLib_l,
	Selection const &selection_l, octopus::Controller &controller_l, ClicMode *&clicMode_r, ClicMode const & stdClicMode_p)
{
	if(spriteModel_l)
	{
		if(spriteModel_l->unitModel)
		{
			octopus::BuildingUnitProductionCommand * command_l = new octopus::BuildingUnitProductionCommand(
					selection_l._sprite->getHandle(),
					selection_l._sprite->getHandle(),
					lib_l.getUnitModel(spriteModel_l->unitModel->_id)
				);
			command_l->setQueued(true);
			controller_l.commitCommand(command_l);
		}
		if(spriteModel_l->buildingModel)
		{
			cleanClicMode(clicMode_r, &stdClicMode_p);
			clicMode_r = new BuildClicMode(*spriteModel_l->buildingModel, spriteLib_l);
		}
	}
}

void controllerLoop(octopus::Controller &controller_p, bool const &over_p, bool const &paused_p)
{
	using namespace std::chrono_literals;

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
	while(!over_p)
	{
		// if paused do not update controller
		if(!paused_p)
		{
			// update controller
			controller_p.update(elapsed_l);
		}
		controller_p.loop_body();

		auto cur_l = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
		elapsed_l = elapsed_seconds_l.count();
		last_l = cur_l;
	}
}

namespace cuttlefish
{

void runGame(Window &window_p)
{
	unsigned long gridSize_l = 5;
	unsigned long fullWorldSize_l = 50 * gridSize_l;

	window_p.setWorldSize(fullWorldSize_l);

    Texture const * background_l = window_p.loadTexture("resources/background.png");

	//SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1");
	bool quit_l = false;
	bool paused_l = false;

	World world_l;

	octopus::Library lib_l;
	std::list<octopus::Steppable *> spawners_l = WaveLevel(lib_l);

	octopus::Controller controller_l(spawners_l, 0.01, {}, gridSize_l);

	std::thread controllerThread_l(controllerLoop, std::ref(controller_l), std::ref(quit_l), std::ref(paused_l));

	double x = 0.;
	double y = 0.;
	double dX = 0.;
	double dY = 0.;
	double camSpeed_l = 400.;

	Panel panel_l(&window_p, window_p.getWidth()-260, window_p.getHeight()-260,
		window_p.loadTexture("resources/background.png"), window_p.loadTexture("resources/grid.png"), 4);

	panel_l.addSpriteInfo("worker", 2, 1);
	panel_l.addSpriteInfo("command_center", 1, 0);
	panel_l.addSpriteInfo("barrack", 0, 0);

	panel_l.addSpriteInfo("square", 1, 4);
	panel_l.addSpriteInfo("triangle", 2, 3);
	panel_l.addSpriteInfo("circle", 1, 3);

	StatsPanel statsPanel_l(&window_p, window_p.getWidth()-520, window_p.getHeight()-260,
		window_p.loadTexture("resources/background.png"), window_p.loadTexture("resources/grid.png"), 4, world_l.getSelection());

	// resources
	statsPanel_l.addSpriteInfo("resource_steel", 2, 4);
	statsPanel_l.addSpriteInfo("resource_food", 2, 4);

	statsPanel_l.addSpriteInfo("worker", 2, 1);
	statsPanel_l.addSpriteInfo("command_center", 1, 0);
	statsPanel_l.addSpriteInfo("barrack", 0, 0);

	statsPanel_l.addSpriteInfo("square", 1, 4);
	statsPanel_l.addSpriteInfo("triangle", 2, 3);
	statsPanel_l.addSpriteInfo("circle", 1, 3);

	DivinityPanel divPanel_l(&window_p, window_p.getWidth()-window_p.getWidth()/2-200, window_p.getHeight()-window_p.getHeight()/2-200,
		window_p.loadTexture("resources/background.png"), window_p.loadTexture("resources/grid.png"), 0);
	divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_1, 2, 0);
	divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_3, 1, 3);
	divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_4, 1, 4);
	divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_5, 2, 3);

	DescPanel descPanel_l(&window_p, window_p.getWidth()-260, window_p.getHeight()-520,
		window_p.loadTexture("resources/background.png"));
	descPanel_l.setText("ceci est un texte de test avec un parametre param $param");
	bool descActive_l = false;

	SpriteLibrary spriteLib_l;
	// tile map
	spriteLib_l.registerSpriteTemplate("tiles", window_p.loadTexture("resources/tiles.png"), 0.5, 0, 0, 64, 64, {1, 1}, {2,2}, 1);
	spriteLib_l.registerSpriteTemplate("details", window_p.loadTexture("resources/details.png"), 0.5, 0, 0, 64, 64, {1}, {2}, 1);

	// resources
	spriteLib_l.registerSpriteTemplate("resource_steel", window_p.loadTexture("resources/steel_prop1.png"), 2., 128, 128, 256, 256, {10, 10}, {0.2,0.2}, 1);
	spriteLib_l.registerSpriteTemplate("resource_food", window_p.loadTexture("resources/food_prop1.png"), 2., 64, 64, 128, 128, {2, 2}, {0.5, 1.}, 1);

	// neutral
	spriteLib_l.registerSpriteTemplate("command_center", window_p.loadTexture("resources/command_center.png"), 2., 32, 32, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	spriteLib_l.registerSpriteTemplate("barrack", window_p.loadTexture("resources/deposit.png"), 1., 32, 32, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	spriteLib_l.registerSpriteTemplate("worker", window_p.loadTexture("resources/worker.png"), 0.5, 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);

	spriteLib_l.registerSpriteTemplate("square", window_p.loadTexture("resources/soldier2_1.png"), 0.5, 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);
	spriteLib_l.registerSpriteTemplate("triangle", window_p.loadTexture("resources/soldier4_1.png"), 0.5, 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);
	spriteLib_l.registerSpriteTemplate("circle", window_p.loadTexture("resources/soldier1_1.png"), 0.5, 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);

	Tilemap tilemap_l(fullWorldSize_l, spriteLib_l, "tiles", "details");
	tilemap_l.generate();

	Minimap minimap_l(window_p, 0, window_p.getHeight()-window_p.getWidth()/4, window_p.getWidth()/4, window_p.getWidth()/4, tilemap_l, fullWorldSize_l, {"resources/me.png", "resources/enemy.png"});

	// true if the minimap has been clicked
	bool minimapClicked_l = false;

	// Text for resource
	Text textResource_l(&window_p, {0,0,0}, 300, 0);
	Text textDivLvl_l(&window_p, {0,0,0}, 550, 0);
	Text textDivAnchor_l(&window_p, {0,0,0}, 700, 0);
	Text textSteps_l(&window_p, {0,0,0}, 850, 0);

	StandardClicMode standardClicMode_l;
	ClicMode * currentClicMode_l = &standardClicMode_l;

	Selection &selection_l = world_l.getSelection();

	std::pair<int, int> initialClic_l {-1, -1};

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;

	bool draw_l = false;
	unsigned long winningTeam_l = 0;
	//Event handler
	SDL_Event e;
	//While application is running
	while( !quit_l )
	{
		// query a new state if available
		octopus::StateAndSteps stateAndSteps_l = controller_l.queryStateAndSteps();
		octopus::State const &state_l = *stateAndSteps_l._state;
		world_l.handleStep(window_p, panel_l, divPanel_l, stateAndSteps_l, spriteLib_l);

		// quit loop if state is over
		if(state_l.isOver())
		{
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
				SpriteModel const * spriteModel_l = panel_l.getSpriteModel(window_p, e.button.x, e.button.y);

				if(spriteModel_l)
				{
					if(spriteModel_l->unitModel)
					{
						descPanel_l.setText(getDesc(*spriteModel_l->unitModel));
					}
					if(spriteModel_l->buildingModel)
					{
						descPanel_l.setText(getDesc(*spriteModel_l->buildingModel));
					}
					descActive_l = true;
				}
				else
				{
					descActive_l = false;
				}

				if(minimapClicked_l)
				{
					octopus::Vector pos_l = getCameraPosition(e.button.x, e.button.y, minimap_l, window_p, state_l.getWorldSize());
					x = pos_l.x;
					y = pos_l.y;
				}
			}
			//User requests quit_l
			if( e.type == SDL_QUIT )
			{
				quit_l = true;
			}
			if (e.type == SDL_MOUSEBUTTONDOWN)
			{
				if(minimap_l.isInside(e.button.x, e.button.y))
				{
					octopus::Vector pos_l = getCameraPosition(e.button.x, e.button.y, minimap_l, window_p, state_l.getWorldSize());
					x = pos_l.x;
					y = pos_l.y;
					minimapClicked_l = true;
				}
				else if(!panel_l.getBackground()->isInside(window_p, e.button.x, e.button.y)
				&& !statsPanel_l.getBackground()->isInside(window_p, e.button.x, e.button.y)
				&& (!divPanel_l.getBackground()->isInside(window_p, e.button.x, e.button.y) || !divPanel_l.isActive()))
				{
					currentClicMode_l->handleMouseDown(e);
				}
			}
			if (e.type == SDL_MOUSEBUTTONUP)
			{

				if(divPanel_l.isActive())
				{
					std::pair<bool, octopus::DivinityType> option_l = divPanel_l.getOption(window_p, e.button.x, e.button.y);
					if(option_l.first)
					{
						octopus::PlayerChoseDivinityCommand * command_l = new octopus::PlayerChoseDivinityCommand(0, option_l.second);
						controller_l.commitCommand(command_l);
						divPanel_l.popOptionLayer();
					}
				}
				else if(!minimapClicked_l)
				{
					SpriteModel const * spriteModel_l = panel_l.getSpriteModel(window_p, e.button.x, e.button.y);

					if(spriteModel_l)
					{
						if(e.button.button == SDL_BUTTON_LEFT)
						{
							if(spriteModel_l->unitModel)
							{
								octopus::BuildingUnitProductionCommand * command_l = new octopus::BuildingUnitProductionCommand(
									selection_l._sprite->getHandle(),
									selection_l._sprite->getHandle(),
									lib_l.getUnitModel(spriteModel_l->unitModel->_id)
								);
								command_l->setQueued(true);
								controller_l.commitCommand(command_l);
							}
							if(spriteModel_l->buildingModel)
							{
								cleanClicMode(currentClicMode_l, &standardClicMode_l);
								currentClicMode_l = new BuildClicMode(*spriteModel_l->buildingModel, spriteLib_l);
							}
						}
					}
					// else if(panel_l.getBackground()->isInside(window_p, e.button.x, e.button.y)
					// 	 || statsPanel_l.getBackground()->isInside(window_p, e.button.x, e.button.y))
					// {
					// 	// NA (skip selection and move command)
					// }
					else
					{
						if( currentClicMode_l->handleMouseUp(e, selection_l, world_l, panel_l, window_p, state_l, controller_l) )
						{
							cleanClicMode(currentClicMode_l, &standardClicMode_l);
							currentClicMode_l = &standardClicMode_l;
						}
					}
				}
				minimapClicked_l = false;
			}
			if( e.type == SDL_KEYDOWN)
			{
				/* Check the SDLKey values and move change the coords */
				switch( e.key.keysym.sym ){
					/// handle panel
					case SDLK_a:
					{
						SpriteModel const * spriteModel_l = panel_l.getSpriteModelOnGrid(0, 0);
						commandFromSpriteModel(spriteModel_l, lib_l, spriteLib_l, selection_l, controller_l,
							currentClicMode_l, standardClicMode_l);
						break;
					}
					case SDLK_z:
					{
						SpriteModel const * spriteModel_l = panel_l.getSpriteModelOnGrid(1, 0);
						commandFromSpriteModel(spriteModel_l, lib_l, spriteLib_l, selection_l, controller_l,
							currentClicMode_l, standardClicMode_l);
						break;
					}
					case SDLK_e:
					{
						SpriteModel const * spriteModel_l = panel_l.getSpriteModelOnGrid(2, 0);
						commandFromSpriteModel(spriteModel_l, lib_l, spriteLib_l, selection_l, controller_l,
							currentClicMode_l, standardClicMode_l);
						break;
					}
					case SDLK_q:
					{
						SpriteModel const * spriteModel_l = panel_l.getSpriteModelOnGrid(0, 1);
						commandFromSpriteModel(spriteModel_l, lib_l, spriteLib_l, selection_l, controller_l,
							currentClicMode_l, standardClicMode_l);
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
							world_l.addSelection(e.key.keysym.sym-SDLK_0);
						}
						else if(KMOD_CTRL & SDL_GetModState())
						{
							world_l.registerSelection(e.key.keysym.sym-SDLK_0);
						}
						else
						{
							world_l.useSelection(e.key.keysym.sym-SDLK_0);
							panel_l.refresh(world_l.getSelection()._sprite, state_l);
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

		tilemap_l.render(window_p, elapsed_l);

		world_l.display(window_p, elapsed_l);

		world_l.getSelection().render(window_p);

		minimap_l.render(state_l, world_l, window_p);

		int mouseX, mouseY;
		SDL_GetMouseState(&mouseX, &mouseY);
		currentClicMode_l->display(window_p, elapsed_l, mouseX, mouseY);

		panel_l.render(window_p);
		statsPanel_l.refresh(window_p, state_l);
		statsPanel_l.render(window_p);
		if(descActive_l)
		{
			descPanel_l.render(window_p);
		}

		divPanel_l.refresh();
		paused_l = divPanel_l.isActive();
		if(divPanel_l.isActive())
		{
			divPanel_l.render(window_p);
		}

		///
		/// Debug texts
		///
		octopus::Player const * player_l = state_l.getPlayer(0);

		//Render background texture to screen
		background_l->render(window_p.getRenderer(), 0, 0, 30, window_p.getWidth() );

		textResource_l.setText(resourceStr(*player_l));
		textResource_l.display(window_p);
		textDivLvl_l.setText(divLvlStr(*player_l));
		textDivLvl_l.display(window_p);
		textDivAnchor_l.setText(divAnchorStr(*player_l));
		textDivAnchor_l.display(window_p);

		std::stringstream ss_l;
		ss_l << stateAndSteps_l._steps.size()<<"/"<<controller_l.getOngoingStep();
		textSteps_l.setText(ss_l.str());
		textSteps_l.display(window_p);

		window_p.draw();
	}

	if(draw_l)
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

	streamMetrics(std::cout, controller_l.getMetrics());
}

} // namespace cuttlefish