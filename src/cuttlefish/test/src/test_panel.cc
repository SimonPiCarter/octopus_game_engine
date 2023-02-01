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
#include "sprite/Sprite.hh"
#include "sprite/SpriteLibrary.hh"
#include "text/Text.hh"
#include "texture/Texture.hh"
#include "window/Window.hh"
#include "world/World.hh"

// octopus
#include "command/building/BuildingUnitProductionCommand.hh"
#include "command/entity/EntityMoveCommand.hh"
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

void controllerLoop(octopus::Controller &controller_p, bool &over_p)
{
	//octopus::Logger::enable_debug();
	using namespace std::chrono_literals;

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
	while(!over_p)
	{
		// update controller
		controller_p.update(elapsed_l);
		controller_p.loop_body();

		std::this_thread::sleep_for(1ms);

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
	if( !window_l.init(SCREEN_WIDTH, SCREEN_HEIGHT) )
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

			World world_l;

			octopus::Library lib_l;
			std::list<octopus::Steppable *> spawners_l = Case4(lib_l);

			octopus::Controller controller_l(spawners_l, 0.01);

			std::thread controllerThread_l(controllerLoop, std::ref(controller_l), std::ref(quit_l));

			double x = 0.;
			double y = 0.;
			double dX = 0.;
			double dY = 0.;
			double camSpeed_l = 200.;

			Panel panel_l(SCREEN_WIDTH-200, SCREEN_HEIGHT-200,
				window_l.loadTexture("resources/background.png"), window_l.loadTexture("resources/grid.png"), 3);
			panel_l.addSpriteInfo("unit", 2, 1);
			panel_l.addSpriteInfo("building", 1, 0);

			SpriteLibrary spriteLib_l;
			spriteLib_l.registerSpriteTemplate("resource", window_l.loadTexture("resources/square.png"), 2., 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);
			spriteLib_l.registerSpriteTemplate("building", window_l.loadTexture("resources/square.png"), 1., 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);
			spriteLib_l.registerSpriteTemplate("unit", window_l.loadTexture("resources/circle.png"), 1., 32, 32, 64, 64, {2, 2}, {0.25, 1}, 1);

			StandardClicMode standardClicMode_l;
			ClicMode * currentClicMode_l = &standardClicMode_l;

			Selection selection_l;

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
				world_l.handleStep(window_l, stateAndSteps_l, spriteLib_l);

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
						SpriteModel const * spriteModel_l = panel_l.getSpriteModel(window_l, e.button.x, e.button.y);

						if(spriteModel_l)
						{
							if(e.button.button == SDL_BUTTON_LEFT)
							{
								std::cout.clear();
								if(spriteModel_l->unitModel)
								{
									std::cout<<spriteModel_l->unitModel->_id<<std::endl;
								}
								if(spriteModel_l->buildingModel)
								{
									currentClicMode_l = new BuildClicMode(*spriteModel_l->buildingModel, spriteLib_l);
									std::cout<<spriteModel_l->buildingModel->_id<<std::endl;
								}
							}
						}
						else if(panel_l.getBackground()->isInside(window_l, e.button.x, e.button.y))
						{
							// NA (skip selection and move command)
						}
						else
						{
							if( currentClicMode_l->handleMouse(e, selection_l, world_l, panel_l, window_l, state_l, controller_l) )
							{
								delete currentClicMode_l;
								currentClicMode_l = &standardClicMode_l;
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
							case SDLK_p:
							{
								octopus::BuildingUnitProductionCommand * command_l =
									new octopus::BuildingUnitProductionCommand(0, 0, lib_l.getUnitModel("unit"));
								command_l->setQueued(true);
								controller_l.commitCommand(command_l);
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
				displayText(&window_l, resourceStr(*player_l), {0,0,0}, 300, 0);

				window_l.draw();
			}

			controllerThread_l.join();
		}
	}
	//Free resources and close SDL
	window_l.close();

	return 0;
}
