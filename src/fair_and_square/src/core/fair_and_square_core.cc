#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>
#include <thread>
#include <sstream>

// cuttlefish
#include "window/Window.hh"

#include "logger/Logger.hh"

// fas
#include "menu/MainMenu.hh"

#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1024

using namespace cuttlefish;

/// cf
/// http://lazyfoo.net/tutorials/SDL/index.php#The%20Viewport
///

int main( int argc, char* args[] )
{
	//octopus::Logger::enable_debug();
	cuttlefish::Window window_l;

	//Start up SDL and create window
	if( !window_l.init(SCREEN_WIDTH, SCREEN_HEIGHT, 0) )
	{
		printf( "Failed to initialize!\n" );
		return 1;
	}
	window_l.displayFps(true);

	fas::mainMenu(window_l);

	//Free resources and close SDL
	window_l.close();

	return 0;
}
