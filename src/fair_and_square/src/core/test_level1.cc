#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>
#include <thread>
#include <sstream>

// cuttlefish
#include "window/Window.hh"

#include "level1/level1.hh"

#include "menu/Menu.hh"
#include "sprite/Picture.hh"
#include "text/Text.hh"
#include "texture/Texture.hh"

#define SCREEN_WIDTH 1024
#define SCREEN_HEIGHT 820

using namespace cuttlefish;

/// cf
/// http://lazyfoo.net/tutorials/SDL/index.php#The%20Viewport
///

void quit(bool &quit_p)
{
	quit_p = true;
}

int main( int argc, char* args[] )
{
	cuttlefish::Window window_l;

	//Start up SDL and create window
	if( !window_l.init(SCREEN_WIDTH, SCREEN_HEIGHT, 0) )
	{
		printf( "Failed to initialize!\n" );
		return 1;
	}
	window_l.displayFps(true);

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
	bool quit_l = false;


	cuttlefish::Menu menu_l(SCREEN_WIDTH/2, 200, 5);

	Picture * button1_l = new Picture(window_l.loadTexture("resources/button.png"), 4., 128, 32, 256, 64, {1, 1 ,1}, {1, 1, 1}, true);
	Text * text1_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text1_l->setText("Play");
	menu_l.addButton(button1_l, text1_l, std::bind(runLevel1, std::ref(window_l)));

	Picture * button2_l = new Picture(window_l.loadTexture("resources/button.png"), 4., 128, 32, 256, 64, {1, 1 ,1}, {1, 1, 1}, true);
	Text * text2_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text2_l->setText("Quit");
	menu_l.addButton(button2_l, text2_l, std::bind(quit, std::ref(quit_l)));

	//Event handler
	SDL_Event e;

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

			menu_l.handleEvent(window_l, e);
		}

		window_l.clear();

		menu_l.display(elapsed_l, window_l);

		window_l.draw();

		auto cur_l = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
		elapsed_l = elapsed_seconds_l.count();
		last_l = cur_l;
	}
	//Free resources and close SDL
	window_l.close();

	return 0;
}
