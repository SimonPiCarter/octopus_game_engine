#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>
#include <thread>
#include <sstream>

// cuttlefish
#include "clicMode/BuildClicMode.hh"
#include "clicMode/StandardClicMode.hh"
#include "progressBar/ProgressBar.hh"
#include "panel/Panel.hh"
#include "panel/DivinityPanel.hh"
#include "sprite/Sprite.hh"
#include "sprite/SpriteLibrary.hh"
#include "text/Text.hh"
#include "text/WrappedText.hh"
#include "texture/Texture.hh"
#include "tilemap/Tilemap.hh"
#include "window/Window.hh"
#include "world/World.hh"

#include "cases/Cases.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 820

using namespace cuttlefish;

int main( int argc, char* args[] )
{
	cuttlefish::Window window_l;

	//Start up SDL and create window
	if( !window_l.init(SCREEN_WIDTH, SCREEN_HEIGHT, 50) )
	{
		printf( "Failed to initialize!\n" );
		return 1;
	}
	window_l.displayFps(true);
	Texture const * background_l = window_l.loadTexture("resources/wp3386769.jpg");
	//Load media
	if( !background_l )
	{
		printf( "Failed to load media!\n" );
		return 1;
	}

	bool quit_l = false;

	ProgressBar bar1_l(
		new Picture(window_l.loadTexture("resources/ally.png"), 1, 1, {1}, {1}),
		new Picture(window_l.loadTexture("resources/me.png"), 1, 1, {1}, {1}),
		240,
		40,
		2
	);
	bar1_l.setPosition(25, 100);

	ProgressBar bar2_l(
		new Picture(window_l.loadTexture("resources/me.png"), 1, 1, {1}, {1}),
		new Picture(window_l.loadTexture("resources/enemy.png"), 1, 1, {1}, {1}),
		240,
		40,
		2
	);
	bar2_l.setPosition(45, 200);

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
	//Event handler
	SDL_Event e;
	//While application is running
	while( !quit_l )
	{
		//Handle events on queue
		while( SDL_PollEvent( &e ) != 0 )
		{
			//User requests quit_l
			if( e.type == SDL_QUIT )
			{
				quit_l = true;
			}
			if( e.type == SDL_MOUSEMOTION )
			{
				bar1_l.setProgress(e.button.x*100/SCREEN_WIDTH);
				bar2_l.setProgress(e.button.y*100/SCREEN_HEIGHT);
			}
		}
		window_l.clear();

		//Render background texture to screen
		background_l->render(window_l.getRenderer(), 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH );

		bar1_l.update(elapsed_l);
		bar1_l.display(window_l);

		bar2_l.update(elapsed_l);
		bar2_l.display(window_l);

		auto cur_l = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
		elapsed_l = elapsed_seconds_l.count();
		last_l = cur_l;

		window_l.draw();
	}

	//Free resources and close SDL
	window_l.close();

	return 0;
}
