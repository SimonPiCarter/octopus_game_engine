#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>

#include "sprite/Sprite.hh"
#include "texture/Texture.hh"
#include "window/Window.hh"

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
		Texture const * background_l = window_l.loadTexture("resources/wp3386769.jpg");
		Texture const * circles_l = window_l.loadTexture("resources/circle.png");
		//Load media
		if( !background_l || !circles_l )
		{
			printf( "Failed to load media!\n" );
		}
		else
		{
			Sprite sprite_l(circles_l, 1., 32, 32, 64, 64, {2, 2}, {3., 0.25});
			sprite_l.setState(1);

			bool quit = false;
			double x = 0.;
			double y = 0.;
			double dX = 0.;
			double dY = 0.;
			double camSpeed_l = 200.;

			size_t frameCount_l = 0;
			double totalTime_l = 0.;
			double frameRate_l = 0;

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
							case SDLK_j:
								sprite_l.setState(0);
								break;
							case SDLK_k:
								sprite_l.setState(1);
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

				//Render background texture to screen
				background_l->render(window_l.getRenderer(), 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH );

				sprite_l.render(window_l, SCREEN_WIDTH/2, SCREEN_HEIGHT/2);

				window_l.draw();

				auto cur_l = std::chrono::steady_clock::now();
				std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
				elapsed_l = elapsed_seconds_l.count();
				sprite_l.update(elapsed_l);
				last_l = cur_l;

				++ frameCount_l;
				totalTime_l += elapsed_l;

				if(frameCount_l == 500)
				{
					frameRate_l = frameCount_l / totalTime_l;
					frameCount_l = 0;
					totalTime_l = 0.;
				}
			}
			std::cout<<"framerate = "<<frameRate_l<<std::endl;
		}
	}
	//Free resources and close SDL
	window_l.close();

	return 0;
}
