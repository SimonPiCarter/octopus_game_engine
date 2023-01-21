#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>

#include "sprite/Sprite.hh"
#include "texture/Texture.hh"
#include "window/Window.hh"
#include "world/World.hh"

#include "controller/Controller.hh"
#include "state/entity/Entity.hh"
#include "state/model/entity/EntityModel.hh"
#include "step/entity/spawn/EntitySpawnStep.hh"
#include "step/Step.hh"
#include "library/Library.hh"

#include "cases/Cases.hh"

#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

using namespace cuttlefish;

/// cf
/// http://lazyfoo.net/tutorials/SDL/index.php#The%20Viewport
///

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
			World world_l;

			octopus::Library lib_l;
			std::list<octopus::Steppable *> spawners_l = Case2(lib_l);

			octopus::Controller controller_l(spawners_l, 0.1);

			bool quit = false;
			double x = 0.;
			double y = 0.;
			double dX = 0.;
			double dY = 0.;
			double camSpeed_l = 200.;

			auto last_l = std::chrono::steady_clock::now();
			double elapsed_l = 0.;
			//Event handler
			SDL_Event e;
					   //While application is running
			while( !quit )
			{
				//Handle events on queue
				while( SDL_PollEvent( &e ) != 0 )
				{
					//User requests quit
					if( e.type == SDL_QUIT )
					{
						quit = true;
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

				// update controller
				controller_l.update(elapsed_l);
				controller_l.loop_body();

				// query a new state if available
				octopus::StateAndSteps stateAndSteps_l = controller_l.queryStateAndSteps();
				world_l.handleStep(window_l, stateAndSteps_l);

				//Render background texture to screen
				background_l->render(window_l.getRenderer(), 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH );

				auto cur_l = std::chrono::steady_clock::now();
				std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
				elapsed_l = elapsed_seconds_l.count();
				last_l = cur_l;

				world_l.display(window_l, elapsed_l);

				window_l.draw();
			}
		}
	}
	//Free resources and close SDL
	window_l.close();

	return 0;
}
