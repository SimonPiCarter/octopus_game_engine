#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>
#include <thread>
#include <sstream>

// cuttlefish
#include "clicMode/BuildClicMode.hh"
#include "clicMode/StandardClicMode.hh"
#include "logger/Logger.hh"
#include "panel/Panel.hh"
#include "panel/DivinityPanel.hh"
#include "sprite/Sprite.hh"
#include "sprite/SpriteLibrary.hh"
#include "text/Text.hh"
#include "text/WrappedText.hh"
#include "texture/Texture.hh"
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

#include "cases/Cases.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 820

using namespace cuttlefish;

/// cf
/// http://lazyfoo.net/tutorials/SDL/index.php#The%20Viewport
///

std::string resourceStr(octopus::Player const &player_p)
{
	std::stringstream ss_l;
	ss_l << "Food   : "<<stringify(octopus::getResource(player_p, octopus::ResourceType::Food))<<" | ";
	ss_l << "Steel  : "<<stringify(octopus::getResource(player_p, octopus::ResourceType::Steel))<<" | ";
	ss_l << "Ether  : "<<stringify(octopus::getResource(player_p, octopus::ResourceType::Ether))<<" | ";
	ss_l << "Anchor : "<<stringify(octopus::getResource(player_p, octopus::ResourceType::Anchor))<<" | ";
	return ss_l.str();
}

std::string divAnchorStr(octopus::Player const &player_p)
{
	std::stringstream ss_l;
	ss_l << "anc | ";
	ss_l << "div1 : "<<stringify(int(octopus::getDivAnchor(player_p, octopus::DivinityType::Divinity_1)))<<" | ";
	ss_l << "div2 : "<<stringify(int(octopus::getDivAnchor(player_p, octopus::DivinityType::Divinity_2)))<<" | ";
	ss_l << "div3 : "<<stringify(int(octopus::getDivAnchor(player_p, octopus::DivinityType::Divinity_3)))<<" | ";
	ss_l << "div4 : "<<stringify(int(octopus::getDivAnchor(player_p, octopus::DivinityType::Divinity_4)))<<" | ";
	ss_l << "div5 : "<<stringify(int(octopus::getDivAnchor(player_p, octopus::DivinityType::Divinity_5)))<<" | ";
	ss_l << "div6 : "<<stringify(int(octopus::getDivAnchor(player_p, octopus::DivinityType::Divinity_6)))<<" | ";
	return ss_l.str();
}

std::string divLvlStr(octopus::Player const &player_p)
{
	std::stringstream ss_l;
	ss_l << "lvl | ";
	ss_l << "div1 : "<<stringify(octopus::getDivLvl(player_p, octopus::DivinityType::Divinity_1))<<" | ";
	ss_l << "div2 : "<<stringify(octopus::getDivLvl(player_p, octopus::DivinityType::Divinity_2))<<" | ";
	ss_l << "div3 : "<<stringify(octopus::getDivLvl(player_p, octopus::DivinityType::Divinity_3))<<" | ";
	ss_l << "div4 : "<<stringify(octopus::getDivLvl(player_p, octopus::DivinityType::Divinity_4))<<" | ";
	ss_l << "div5 : "<<stringify(octopus::getDivLvl(player_p, octopus::DivinityType::Divinity_5))<<" | ";
	ss_l << "div6 : "<<stringify(octopus::getDivLvl(player_p, octopus::DivinityType::Divinity_6))<<" | ";
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

void controllerLoop(octopus::Controller &controller_p, bool &over_p, bool &paused_p)
{
	//octopus::Logger::enable_debug();
	using namespace std::chrono_literals;

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
	while(!over_p)
	{
		// if paused to not update controller
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

int main( int argc, char* args[] )
{
	cuttlefish::Window window_l;

	//Start up SDL and create window
	if( !window_l.init(SCREEN_WIDTH, SCREEN_HEIGHT, 50) )
	{
		printf( "Failed to initialize!\n" );
	}
	else
	{
		window_l.displayFps(true);
		Texture const * background_l = window_l.loadTexture("resources/wp3386769.jpg");
		Texture const * circles_l = window_l.loadTexture("resources/circle.png");
		//Load media
		if( !background_l || !circles_l )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			bool quit_l = false;
			bool paused_l = false;

			World world_l;

			octopus::Library lib_l;
			std::list<octopus::Steppable *> spawners_l = Case4(lib_l);

			octopus::Controller controller_l(spawners_l, 0.01);

			std::thread controllerThread_l(controllerLoop, std::ref(controller_l), std::ref(quit_l), std::ref(paused_l));

			double x = 0.;
			double y = 0.;
			double dX = 0.;
			double dY = 0.;
			double camSpeed_l = 200.;

			Panel panel_l(&window_l, SCREEN_WIDTH-200, SCREEN_HEIGHT-200,
				window_l.loadTexture("resources/background.png"), window_l.loadTexture("resources/grid.png"), 3);
			panel_l.addSpriteInfo("unit", 2, 1);
			panel_l.addSpriteInfo("soldier", 2, 2);
			panel_l.addSpriteInfo("building", 1, 0);
			panel_l.addSpriteInfo("barrack", 0, 1);
			panel_l.addSpriteInfo("temple", 0, 4);

			DivinityPanel divPanel_l(&window_l, SCREEN_WIDTH-SCREEN_WIDTH/2-200, SCREEN_HEIGHT-SCREEN_HEIGHT/2-200,
				window_l.loadTexture("resources/background.png"), window_l.loadTexture("resources/grid.png"), 0);
			divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_1, 1, 3);
			divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_2, 1, 4);
			divPanel_l.addOptionInfo(octopus::DivinityType::Divinity_3, 2, 0);

			SpriteLibrary spriteLib_l;
			spriteLib_l.registerSpriteTemplate("resource", window_l.loadTexture("resources/square.png"), 2., 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);
			spriteLib_l.registerSpriteTemplate("building", window_l.loadTexture("resources/building.png"), 1., 32, 32, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
			spriteLib_l.registerSpriteTemplate("barrack", window_l.loadTexture("resources/building.png"), 1., 32, 32, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
			spriteLib_l.registerSpriteTemplate("temple", window_l.loadTexture("resources/building.png"), 1., 32, 32, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
			spriteLib_l.registerSpriteTemplate("unit", window_l.loadTexture("resources/circle.png"), 0.5, 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);
			spriteLib_l.registerSpriteTemplate("soldier", window_l.loadTexture("resources/circle.png"), 0.5, 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);

			// Text for resource
			Text textResource_l(&window_l, {0,0,0}, 300, 0);
			Text textDivLvl_l(&window_l, {0,0,0}, 200, 30);
			Text textDivAnchor_l(&window_l, {0,0,0}, 200, 60);
			Text textSteps_l(&window_l, {0,0,0}, 750, 0);

			WrappedText descText_l(&window_l, SCREEN_WIDTH-600, SCREEN_HEIGHT-400, 400);
			descText_l.setText("Lorem ipsum dolor sit amet, consectetur adipiscing $hp elit, sed do eiusmod tempor incididunt ut labore $fps et dolore magna aliqua. Ut enim ad minim veniam, quis nostrud exercitation ullamco laboris nisi ut aliquip ex ea commodo consequat. Duis aute irure dolor in reprehenderit in voluptate velit esse cillum dolore eu fugiat nulla pariatur. Excepteur sint occaecat cupidatat non proident, sunt in culpa qui officia deserunt mollit anim id est laborum.", {0,0,0});

			StandardClicMode standardClicMode_l;
			ClicMode * currentClicMode_l = &standardClicMode_l;

			Selection &selection_l = world_l.getSelection();

			std::pair<int, int> initialClic_l {-1, -1};

			auto last_l = std::chrono::steady_clock::now();
			double elapsed_l = 0.;
			//Event handler
			SDL_Event e;
			//While application is running
			while( !quit_l )
			{
				// query a new state if available
				octopus::StateAndSteps stateAndSteps_l = controller_l.queryStateAndSteps();
				octopus::State const &state_l = *stateAndSteps_l._state;
				world_l.handleStep(window_l, panel_l, divPanel_l, stateAndSteps_l, spriteLib_l);

				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit_l
					if( e.type == SDL_QUIT )
					{
						quit_l = true;
					}
					if (e.type == SDL_MOUSEBUTTONDOWN)
					{
						if(!panel_l.getBackground()->isInside(window_l, e.button.x, e.button.y))
						{
							currentClicMode_l->handleMouseDown(e);
						}
					}
					if (e.type == SDL_MOUSEBUTTONUP)
					{
						if(divPanel_l.isActive())
						{
							std::pair<bool, octopus::DivinityType> option_l = divPanel_l.getOption(window_l, e.button.x, e.button.y);
							if(option_l.first)
							{
								octopus::PlayerChoseDivinityCommand * command_l = new octopus::PlayerChoseDivinityCommand(0, option_l.second);
								controller_l.commitCommand(command_l);
								divPanel_l.popOptionLayer();
							}
						}
						else
						{
							SpriteModel const * spriteModel_l = panel_l.getSpriteModel(window_l, e.button.x, e.button.y);

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
							else if(panel_l.getBackground()->isInside(window_l, e.button.x, e.button.y))
							{
								// NA (skip selection and move command)
							}
							else
							{
								if( currentClicMode_l->handleMouseUp(e, selection_l, world_l, panel_l, window_l, state_l, controller_l) )
								{
									cleanClicMode(currentClicMode_l, &standardClicMode_l);
									currentClicMode_l = &standardClicMode_l;
								}
							}
						}
					}
					if( e.type == SDL_KEYDOWN)
					{
						/* Check the SDLKey values and move change the coords */
						switch( e.key.keysym.sym ){
							case SDLK_LEFT:
								dX = - camSpeed_l;
								break;
							case SDLK_RIGHT:
								dX = camSpeed_l;
								break;
							case SDLK_UP:
								dY = -camSpeed_l;
								break;
							case SDLK_DOWN:
								dY = camSpeed_l;
								break;

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
							default:
								break;
						}
					}
					if( e.type == SDL_KEYUP)
					{
						switch( e.key.keysym.sym ){
							case SDLK_LEFT:
								dX = 0;
								break;
							case SDLK_RIGHT:
								dX = 0;
								break;
							case SDLK_UP:
								dY = 0;
								break;
							case SDLK_DOWN:
								dY = 0;
								break;
							default:
								break;
						}
					}
				}
				x += dX * elapsed_l;
				y += dY * elapsed_l;
				window_l.setCamera(x, y);
				window_l.clear();

				//Render background texture to screen
				background_l->render(window_l.getRenderer(), 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH );


				auto cur_l = std::chrono::steady_clock::now();
				std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
				elapsed_l = elapsed_seconds_l.count();
				last_l = cur_l;

				world_l.display(window_l, elapsed_l);

				int mouseX, mouseY;
				SDL_GetMouseState(&mouseX, &mouseY);
				currentClicMode_l->display(window_l, elapsed_l, mouseX, mouseY);

				panel_l.render(window_l);

				octopus::Player const * player_l = state_l.getPlayer(0);

				divPanel_l.refresh();
				paused_l = divPanel_l.isActive();
				if(divPanel_l.isActive())
				{
					divPanel_l.render(window_l);
				}

				textResource_l.setText(resourceStr(*player_l));
				textResource_l.display(window_l);
				textDivLvl_l.setText(divLvlStr(*player_l));
				textDivLvl_l.display(window_l);
				textDivAnchor_l.setText(divAnchorStr(*player_l));
				textDivAnchor_l.display(window_l);

				std::stringstream ss_l;
				ss_l << stateAndSteps_l._steps.size()<<"/"<<controller_l.getOngoingStep();
				textSteps_l.setText(ss_l.str());
				textSteps_l.display(window_l);

				descText_l.updateText("hp", ss_l.str());
				descText_l.updateColor("hp", {155, 155, 155});
				descText_l.updateText("fps", "ldld");
				descText_l.updateColor("fps", {155, 0, 155});
				descText_l.display(window_l);

				window_l.draw();
			}

			controllerThread_l.join();

			streamMetrics(std::cout, controller_l.getMetrics());
		}
	}
	//Free resources and close SDL
	window_l.close();


	return 0;
}
