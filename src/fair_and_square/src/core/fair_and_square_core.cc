#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>
#include <thread>
#include <sstream>

#include "logger/Logger.hh"
// cuttlefish
#include "window/Window.hh"

#include "game/RunGame.hh"

#include "menu/Menu.hh"
#include "sprite/Picture.hh"
#include "text/Text.hh"
#include "texture/Texture.hh"

#define SCREEN_WIDTH 1600
#define SCREEN_HEIGHT 800

using namespace cuttlefish;

/// cf
/// http://lazyfoo.net/tutorials/SDL/index.php#The%20Viewport
///

void quit(bool &quit_p)
{
	quit_p = true;
}

void toogleFullScreen(cuttlefish::Window &window_p, bool &fullscreen_p)
{
	if(fullscreen_p)
	{
		window_p.setWindowed();
	}
	else
	{
		window_p.setWindowedFullScreeen();
	}
	fullscreen_p = !fullscreen_p;
}

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

	auto last_l = std::chrono::steady_clock::now();
	double elapsed_l = 0.;
	bool quit_l = false;
	bool fullscreen_l = false;


	cuttlefish::Menu menu_l(window_l.getWidth()/2, 20, 5);

	Picture octopus_l(window_l.loadTexture("resources/octopus.png"), 64, 64, {2}, {1});
	octopus_l.setDestination(window_l.getWidth()-150, window_l.getHeight()-150, 128, 128);

{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Play Wave Level (fast)");
	menu_l.addButton(button_l, text_l, std::bind(fas::runWave, std::ref(window_l), 0.1*60*100, 0, 150));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Play Wave Level (real)");
	menu_l.addButton(button_l, text_l, std::bind(fas::runWave, std::ref(window_l), 3*60*100, 1, 150));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Replay Wave Level");
	menu_l.addButton(button_l, text_l, std::bind(fas::replayWave, std::ref(window_l)));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Load Wave Level");
	menu_l.addButton(button_l, text_l, std::bind(fas::loadWave, std::ref(window_l)));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Play Arena Level (50)");
	menu_l.addButton(button_l, text_l, std::bind(fas::runArena, std::ref(window_l), 50));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Play Arena Level (500)");
	menu_l.addButton(button_l, text_l, std::bind(fas::runArena, std::ref(window_l), 500));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Replay Arena Level");
	menu_l.addButton(button_l, text_l, std::bind(fas::replayArena, std::ref(window_l)));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Load Arena Level");
	menu_l.addButton(button_l, text_l, std::bind(fas::loadArena, std::ref(window_l)));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Play Maze Level (50)");
	menu_l.addButton(button_l, text_l, std::bind(fas::runMaze, std::ref(window_l), 50));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Play Maze Level (500)");
	menu_l.addButton(button_l, text_l, std::bind(fas::runMaze, std::ref(window_l), 500));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("FullScreen (toggle)");
	menu_l.addButton(button_l, text_l, std::bind(toogleFullScreen, std::ref(window_l), std::ref(fullscreen_l)));
}
{
	Picture * button_l = new Picture(window_l.loadTexture("resources/button.png"), 256, 64, {1, 1 ,1}, {1, 1, 1});
	Text * text_l = new Text(&window_l, {0, 0, 0}, 0, 0);
	text_l->setText("Quit");
	menu_l.addButton(button_l, text_l, std::bind(quit, std::ref(quit_l)));
}

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

		octopus_l.update(elapsed_l);
		octopus_l.display(window_l);

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
