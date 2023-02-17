#include <iostream>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include <chrono>
#include <thread>
#include <sstream>

// cuttlefish
#include "clicMode/BuildClicMode.hh"
#include "clicMode/StandardClicMode.hh"
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
	if( !window_l.init(SCREEN_WIDTH, SCREEN_HEIGHT) )
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

	SpriteLibrary spriteLib_l;
	spriteLib_l.registerSpriteTemplate("building", window_l.loadTexture("resources/building.png"), 1., 32, 32, 64, 64, {1, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	spriteLib_l.registerSpriteTemplate("building_shifted_0", window_l.loadTexture("resources/building.png"), 1., 0, 0, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	spriteLib_l.registerSpriteTemplate("building_shifted_64", window_l.loadTexture("resources/building.png"), 1., 64, 64, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);

	spriteLib_l.registerSpriteTemplate("building_2", window_l.loadTexture("resources/building.png"), 2., 32, 32, 64, 64, {1, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	spriteLib_l.registerSpriteTemplate("building_2_shifted_0", window_l.loadTexture("resources/building.png"), 2., 0, 0, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);
	spriteLib_l.registerSpriteTemplate("building_2_shifted_64", window_l.loadTexture("resources/building.png"), 2., 64, 64, 64, 64, {2, 2, 2, 2}, {0.25, 0.5, 0.5, 0.5}, 1);

	spriteLib_l.registerSpriteTemplate("tree", window_l.loadTexture("resources/tree.png"), 2., 29, 106, 64, 128, {1}, {1.}, -1);


	Sprite * ref_l = spriteLib_l.createSprite(0, "building", true);
	ref_l->setPosition(128, 128);
	Sprite * test_l = spriteLib_l.createSprite(0, "building", true);
	test_l->setPosition(256, 128);
	Sprite * tree_l = spriteLib_l.createSprite(0, "tree", true);
	tree_l->setPosition(256, 128);

	Sprite * ref0_l = spriteLib_l.createSprite(0, "building", true);
	ref0_l->setPosition(128, 256);
	Sprite * test0_l = spriteLib_l.createSprite(0, "building_shifted_0", true);
	test0_l->setPosition(256, 256);

	Sprite * ref64_l = spriteLib_l.createSprite(0, "building", true);
	ref64_l->setPosition(128, 512);
	Sprite * test64_l = spriteLib_l.createSprite(0, "building_shifted_64", true);
	test64_l->setPosition(256, 512);

	Sprite * ref2_l = spriteLib_l.createSprite(0, "building_2", true);
	ref2_l->setPosition(512+256, 128);
	Sprite * test2_l = spriteLib_l.createSprite(0, "building_2", true);
	test2_l->setPosition(512+256, 128);

	Sprite * ref2_0_l = spriteLib_l.createSprite(0, "building_2", true);
	ref2_0_l->setPosition(512+256, 300);
	Sprite * test2_0_l = spriteLib_l.createSprite(0, "building_2_shifted_0", true);
	test2_0_l->setPosition(512+256, 300);

	Sprite * ref2_64_l = spriteLib_l.createSprite(0, "building_2", true);
	ref2_64_l->setPosition(512+256, 512);
	Sprite * test2_64_l = spriteLib_l.createSprite(0, "building_2_shifted_64", true);
	test2_64_l->setPosition(512+256, 512);

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
		}
		window_l.clear();

		//Render background texture to screen
		background_l->render(window_l.getRenderer(), 0, 0, SCREEN_HEIGHT, SCREEN_WIDTH );



		auto cur_l = std::chrono::steady_clock::now();
		std::chrono::duration<double> elapsed_seconds_l = cur_l-last_l;
		elapsed_l = elapsed_seconds_l.count();
		last_l = cur_l;

		ref_l->update(elapsed_l);
		ref_l->render(window_l);
		test_l->update(elapsed_l);
		test_l->render(window_l);
		tree_l->update(elapsed_l);
		tree_l->render(window_l);
		ref0_l->update(elapsed_l);
		ref0_l->render(window_l);
		test0_l->update(elapsed_l);
		test0_l->render(window_l);
		ref64_l->update(elapsed_l);
		ref64_l->render(window_l);
		test64_l->update(elapsed_l);
		test64_l->render(window_l);

		ref2_l->update(elapsed_l);
		ref2_l->render(window_l);
		test2_l->update(elapsed_l);
		test2_l->render(window_l);
		ref2_0_l->update(elapsed_l);
		ref2_0_l->render(window_l);
		test2_0_l->update(elapsed_l);
		test2_0_l->render(window_l);
		ref2_64_l->update(elapsed_l);
		ref2_64_l->render(window_l);
		test2_64_l->update(elapsed_l);
		test2_64_l->render(window_l);

		window_l.draw();
	}

	//Free resources and close SDL
	window_l.close();

	return 0;
}
